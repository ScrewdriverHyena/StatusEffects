// Copyright No Bright Shadows. All Rights Reserved.

#include "StatusEffectBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Algo/AnyOf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "System/StatusEffectComponent.h"
#include "System/StatusEffectObserver.h"

UStatusEffectBlueprintLibrary::UStatusEffectBlueprintLibrary() = default;

UStatusEffectObserver* UStatusEffectBlueprintLibrary::GetObserverByLocalPlayer(const ULocalPlayer* LocalPlayer)
{
	return LocalPlayer ? LocalPlayer->GetSubsystem<UStatusEffectObserver>() : nullptr;
}

UStatusEffectObserver* UStatusEffectBlueprintLibrary::GetObserverByAbilityTarget(const UAbilitySystemComponent* Target)
{
	if (Target)
	{
		if (const APawn* Pawn = Target->GetOwner<APawn>(); Pawn && Pawn->IsLocallyControlled())
		{
			if (const APlayerController* PC = Pawn->GetController<APlayerController>())
			{
				return GetObserverByLocalPlayer(PC->GetLocalPlayer());
			}
		}
	}

	return nullptr;
}

FGrantStatusEffectResult UStatusEffectBlueprintLibrary::GrantEffect(FStatusEffectRequest Request, UAbilitySystemComponent* Target,
	UAbilitySystemComponent* Source)
{
	return GrantEffects(TArray<FStatusEffectRequest>{MoveTemp(Request)}, Target, Source);
}

FGrantStatusEffectResult UStatusEffectBlueprintLibrary::GrantEffects(const TArray<FStatusEffectRequest>& Requests,
	UAbilitySystemComponent* Target, UAbilitySystemComponent* Source)
{
	const APawn* Pawn = Target->GetOwner<APawn>();

	FGrantStatusEffectResult Result;

	if (!Source)
	{
		Source = Target;
	}
	
	if (!Pawn || !Pawn->HasNetOwner())
	{
		return Result;
	}

	UStatusEffectObserver* Observer = GetObserverByAbilityTarget(Target);

	const FActiveGameplayEffectsContainer& ActiveEffects = Target->GetActiveGameplayEffects();

	for (const FStatusEffectRequest& Request : Requests)
	{
		if (!Request.EffectClass)
		{
			ensureAlwaysMsgf(false, TEXT("GrantEffects failed: Requested EffectClass is null."));
			
			continue;
		}
		
		const UStatusEffectComponent* Data = GetDefault<UGameplayEffect>(Request.EffectClass)->FindComponent<UStatusEffectComponent>();

		bool bContinue = false;

		for (const FActiveGameplayEffect& Effect : &ActiveEffects)
		{
			if (const UStatusEffectComponent* ActiveEffectStatusEffectData = Effect.Spec.Def->FindComponent<UStatusEffectComponent>();
				!ActiveEffectStatusEffectData || !Data)
			{
				continue;
			}

			if (Effect.Spec.Def->GetPrimaryAssetId() == Request.EffectClass->GetPrimaryAssetId())
			{
				if (Effect.Spec.Duration > FGameplayEffectConstants::INSTANT_APPLICATION)
				{
					FActiveGameplayEffect& EffectMutable = const_cast<FActiveGameplayEffect&>(Effect);
					FActiveGameplayEffectsContainer& EffectsMutable = const_cast<FActiveGameplayEffectsContainer&>(ActiveEffects);

					const float TimeLeft = Effect.GetTimeRemaining(EffectsMutable.GetWorldTime());

					EffectMutable.Spec.SetDuration(Request.Duration.Get(FGameplayEffectConstants::INFINITE_DURATION),
						false);
					EffectMutable.StartServerWorldTime = EffectsMutable.GetServerWorldTime();
					EffectMutable.CachedStartServerWorldTime = EffectMutable.StartServerWorldTime;
					EffectMutable.StartWorldTime = EffectsMutable.GetWorldTime();
					EffectsMutable.MarkItemDirty(EffectMutable);
					EffectsMutable.CheckDuration(EffectMutable.Handle);

					EffectMutable.EventSet.OnTimeChanged.Broadcast(EffectMutable.Handle,
						EffectMutable.StartWorldTime, EffectMutable.GetDuration());
					
					Target->OnGameplayEffectDurationChange(EffectMutable);

					Result.OverridenEffects.Add(Request);

					UE_LOG(LogStatusEffects, Verbose, TEXT("GrantEffects: Overriding StatusEffect %s."), *Request.EffectClass->GetName());

					Observer->OnStatusEffectOverrideDelegate.Broadcast(FStatusEffectOverrideInfo{Source, Target, Request.EffectClass,
						1, Request.StackCount, {TimeLeft}, Request.Duration});

					bContinue = true;
					
					break;
				}

				bContinue = Effect.Spec.Def->StackingType == EGameplayEffectStackingType::None;
				
				break;
			}
		}

		if (bContinue)
		{
			continue;
		}

		if (Request.Chance >= 1.0 || (FMath::Rand() & RAND_MAX) / static_cast<double>(RAND_MAX) <= Request.Chance)
		{
			FGameplayEffectSpecHandle SpecHandle = Source->MakeOutgoingSpec(Request.EffectClass, 1.0f,
				Target->MakeEffectContext());

			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

			if (SpecHandle.IsValid())
			{
				Spec->SetDuration(Request.Duration.Get(FGameplayEffectConstants::INFINITE_DURATION),
					false);

				for (int i = 0; i < Request.StackCount; i++)
				{
					Source->ApplyGameplayEffectSpecToTarget(*Spec, Target);

					if (GetDefault<UGameplayEffect>(Request.EffectClass)->StackingType == EGameplayEffectStackingType::None)
					{
						break;
					}
				}

				UE_LOG(LogStatusEffects, Verbose, TEXT("GrantEffect: Granted StatusEffect %s."), *Request.EffectClass->GetName());

				Observer->OnStatusEffectGrantedDelegate.Broadcast(Target, Source, Request);
				
				Result.GrantedEffects.Add(Request);
			}
		}
	}

	return Result;
}

FRemoveStatusEffectResult UStatusEffectBlueprintLibrary::RemoveEffect(FStatusEffectRequest Request, UAbilitySystemComponent* Target)
{
	return RemoveEffects(TArray<FStatusEffectRequest>{MoveTemp(Request)}, Target);
}

FRemoveStatusEffectResult UStatusEffectBlueprintLibrary::RemoveEffects(const TArray<FStatusEffectRequest>& Requests,
	UAbilitySystemComponent* Target)
{
	const AActor* Owner = Target->GetOwner<AActor>();

	FRemoveStatusEffectResult Result;

	if (!Owner)
	{
		return Result;
	}
	
	for (const FStatusEffectRequest& Request : Requests)
	{
		if (!Request.EffectClass)
		{
			ensureAlwaysMsgf(false, TEXT("RemoveEffects failed: Requested EffectClass is null."));
			
			continue;
		}

		for (const FActiveGameplayEffect& Effect : &Target->GetActiveGameplayEffects())
		{
			if (Effect.Spec.Def->GetPrimaryAssetId() == Request.EffectClass->GetPrimaryAssetId()
				&& ((Request.Duration.IsSet() && Effect.Spec.Duration > 0.0f)
					|| !Request.Duration.IsSet() && Effect.Spec.Duration < 0.0f))
			{
				Target->RemoveActiveGameplayEffect(Effect.Handle, Request.StackCount);

				Result.RemovedEffects.Add(Request);

				UE_LOG(LogStatusEffects, Verbose, TEXT("RemoveEffects: Removed StatusEffect %s."), *Request.EffectClass->GetName());
				
				GetObserverByAbilityTarget(Target)->OnStatusEffectRevokedDelegate.Broadcast(Target, Request);
			}
		}
	}

	return Result;
}

bool UStatusEffectBlueprintLibrary::HasEffectAny(const UAbilitySystemComponent* Target, const TSubclassOf<UGameplayEffect> EffectClass)
{
	return Algo::AnyOf(&Target->GetActiveGameplayEffects(), [EffectClass]
		(const FActiveGameplayEffect& Effect) -> bool
	{
		return Effect.Spec.Def->GetPrimaryAssetId() == EffectClass->GetPrimaryAssetId();
	});
}

bool UStatusEffectBlueprintLibrary::HasEffectExact(const UAbilitySystemComponent* Target, const TSubclassOf<UGameplayEffect> EffectClass,
	const EGameplayEffectDurationType DurationType, const int32 Stacks)
{
	return Algo::AnyOf(&Target->GetActiveGameplayEffects(), [EffectClass, DurationType, Stacks]
		(const FActiveGameplayEffect& Effect) -> bool
	{
		return Effect.Spec.Def->GetPrimaryAssetId() == EffectClass->GetPrimaryAssetId()
			&& Effect.Spec.GetDuration() == (DurationType == EGameplayEffectDurationType::Infinite
				? FGameplayEffectConstants::INFINITE_DURATION : Effect.Spec.GetDuration())
			&& Effect.Spec.GetStackCount() == Stacks;
	});
}

TSubclassOf<UGameplayEffect> UStatusEffectBlueprintLibrary::FindEffectByName(const FString EffectName)
{
	FARFilter Filter = FARFilter();
	Filter.ClassPaths = { UBlueprint::StaticClass()->GetClassPathName() };
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssets(Filter, AssetData, true);

	for (const FAssetData& Data : AssetData)
	{
		if (const UBlueprint* BP = Cast<UBlueprint>(Data.GetAsset()))
		{
			if (UClass* Class = BP->GeneratedClass; Class && Class->IsChildOf(UGameplayEffect::StaticClass()))
			{
				if (const UStatusEffectComponent* DataComponent = GetDefault<UGameplayEffect>(Class)->FindComponent<UStatusEffectComponent>();
					DataComponent && DataComponent->DisplayName.ToString() == EffectName)
				{
					UE_LOG(LogStatusEffects, Verbose, TEXT("FindEffectByName: Found StatusEffect %s."), *EffectName);
					
					return Class;
				}
			}
		}
	}

	return nullptr;
}

FGrantStatusEffectResult UStatusEffectBlueprintLibrary::GrantEffectByName(const FString EffectName, UAbilitySystemComponent* Target,
	const double Duration, const int32 StackCount, UAbilitySystemComponent* Source)
{
	const TSubclassOf<UGameplayEffect> FoundClass = FindEffectByName(EffectName);
	return FoundClass ? GrantEffect(FStatusEffectRequest{FoundClass, StackCount, TOptional{Duration}, 1.0}, Target, Source)
		: FGrantStatusEffectResult();
}
