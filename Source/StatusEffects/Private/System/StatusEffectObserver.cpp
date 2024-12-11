// Copyright No Bright Shadows. All Rights Reserved.

#include "System/StatusEffectObserver.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "System/StatusEffectComponent.h"

DEFINE_LOG_CATEGORY(LogStatusEffects);

UStatusEffectObserver::UStatusEffectObserver() = default;

void UStatusEffectObserver::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	(void)AsyncInitialize();
}

void UStatusEffectObserver::Deinitialize()
{
	Super::Deinitialize();
}

bool UStatusEffectObserver::ShouldCreateSubsystem(UObject* Outer) const
{
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(GetClass(), ChildClasses, false);

	return Super::ShouldCreateSubsystem(Outer) && ChildClasses.Num() == 0;
}

void UStatusEffectObserver::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);
}

FOnStatusEffectGranted& UStatusEffectObserver::OnStatusEffectGranted() const
{
	return OnStatusEffectGrantedDelegate;
}

FOnStatusEffectOverride& UStatusEffectObserver::OnStatusEffectOverride() const
{
	return OnStatusEffectOverrideDelegate;
}

FOnStatusEffectRevoked& UStatusEffectObserver::OnStatusEffectRevoked() const
{
	return OnStatusEffectRevokedDelegate;
}

FOnStatusEffectAdded& UStatusEffectObserver::OnStatusEffectAdded() const
{
	return OnStatusEffectAddedDelegate;
}

FOnStatusEffectRemoved& UStatusEffectObserver::OnStatusEffectRemoved() const
{
	return OnStatusEffectRemovedDelegate;
}

FOnStatusEffectAddedOverride& UStatusEffectObserver::OnStatusEffectAddedOverride() const
{
	return OnStatusEffectAddedOverrideDelegate;
}

FOnStatusEffectRemovedOverride& UStatusEffectObserver::OnStatusEffectRemovedOverride() const
{
	return OnStatusEffectRemovedOverrideDelegate;
}

EObserverOverrideResult UStatusEffectObserver::OnStatusEffectAddedBP_Implementation(UAbilitySystemComponent* ASC, FGameplayEffectSpec Spec,
	const FActiveGameplayEffectHandle& Handle, const UStatusEffectComponent* StatusEffectData)
{
	return EObserverOverrideResult::Override;
}

EObserverOverrideResult UStatusEffectObserver::OnStatusEffectRemovedBP_Implementation(UAbilitySystemComponent* ASC,
	const FGameplayEffectRemovalInfo& RemovalInfo, const UStatusEffectComponent* StatusEffectData)
{
	return EObserverOverrideResult::Override;
}

FOnStatusEffectAddedOverride& UStatusEffectObserver::OverrideStatusEffectAdded()
{
	return OnStatusEffectAddedOverrideDelegate;
}

FOnStatusEffectRemovedOverride& UStatusEffectObserver::OverrideStatusEffectRemoved()
{
	return OnStatusEffectRemovedOverrideDelegate;
}

TCoroutine<> UStatusEffectObserver::AsyncInitialize(const FForceLatentCoroutine ForceLatent)
{
	APawn* Pawn = co_await WaitForValidPawn(ForceLatent);

	check(Pawn->Implements<UAbilitySystemInterface>() && Pawn->GetController<APlayerController>());

	Observe(Cast<IAbilitySystemInterface>(Pawn));
}

TCoroutine<APawn*> UStatusEffectObserver::WaitForValidPawn(const FForceLatentCoroutine)
{
	APawn** ReturnPawn = nullptr;
	
	co_await Latent::Until(std::function([this, &ReturnPawn]
		() -> bool
	{
		if (const ULocalPlayer* LP = GetLocalPlayer(); LP && GetWorld() && GetWorld()->IsInitialized())
		{
			if (const APlayerController* PC = LP->GetPlayerController(GetWorld()))
			{
				if (APawn* Pawn = PC->GetPawn(); Pawn && Pawn->Implements<UAbilitySystemInterface>())
				{
					if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Pawn))
					{
						*ReturnPawn = Pawn;
						
						return !!AbilitySystemInterface->GetAbilitySystemComponent();
					}
				}
			}
		}

		return false;
	}));

	co_return !!ReturnPawn ? *ReturnPawn : nullptr;
}

TCoroutine<> UStatusEffectObserver::Observe(const IAbilitySystemInterface* Pawn)
{
	UAbilitySystemComponent* ASC = Pawn->GetAbilitySystemComponent();
	const TWeakObjectPtr<UAbilitySystemComponent> WeakASC = ASC;

	if (!ASC)
	{
		co_return;
	}

	FActiveGameplayEffectHandle Handle;
	FGameplayEffectSpec Spec;

	TCoroutine<> WaitForAGE = []
		(const TLatentContext<ThisClass>, UAbilitySystemComponent* ThisASC, FActiveGameplayEffectHandle& OutHandle,
			FGameplayEffectSpec& OutSpec) -> TCoroutine<>
	{
		auto [_1, InnerSpec, InnerHandle] = co_await ThisASC->OnActiveGameplayEffectAddedDelegateToSelf;

		OutHandle = InnerHandle;
		OutSpec = InnerSpec;
	}(TLatentContext<ThisClass>(this), ASC, Handle, Spec);

	TCoroutine<> WaitForFailCondition = []
		(const TLatentContext<ThisClass> This, const APawn* ThisPawn) -> TCoroutine<>
	{
		co_await Latent::Until(std::function([This, ThisPawn]
			() -> bool
		{
			return !IsValid(ThisPawn)
				|| !ThisPawn->GetController<APlayerController>()
				|| This->GetLocalPlayer() != ThisPawn->GetController<APlayerController>()->GetLocalPlayer();
		}));
	}(TLatentContext<ThisClass>(this), Cast<APawn>(Pawn));

	if (!co_await Race(MoveTemp(WaitForFailCondition), MoveTemp(WaitForAGE)) || !WeakASC.IsValid())
	{
		co_await FSelfCancellation();
	}
	
	Observe(Pawn);

	if (!Handle.IsValid() || FMath::IsNearlyZero(Spec.Duration) || !Spec.Def.Get() || !Spec.Def->FindComponent<UStatusEffectComponent>())
	{
		co_return;
	}

	OnStatusEffectAdded(ASC, Spec, Handle, Spec.Def->FindComponent<UStatusEffectComponent>());

	FOnActiveGameplayEffectRemoved_Info* RemovalDelegate = WeakASC.Pin()->OnGameplayEffectRemoved_InfoDelegate(Handle);

	if (!RemovalDelegate)
	{
		co_await FSelfCancellation();
	}

	TTuple<const FGameplayEffectRemovalInfo&> RemovalInfo_Tuple = co_await *RemovalDelegate;
	
	OnStatusEffectRemoved(ASC, RemovalInfo_Tuple.Get<const FGameplayEffectRemovalInfo&>(),
		Spec.Def->FindComponent<UStatusEffectComponent>());
}

void UStatusEffectObserver::OnStatusEffectAdded(UAbilitySystemComponent* ASC, FGameplayEffectSpec Spec,
	const FActiveGameplayEffectHandle& Handle, const UStatusEffectComponent* StatusEffectData)
{
	const FGameplayEffectSpec CopySpec = Spec;
	
	if (OnStatusEffectAddedOverrideDelegate.IsBound())
	{
		switch (OnStatusEffectAddedOverrideDelegate.Execute(ASC, Spec, Handle, StatusEffectData))
		{
		case EObserverOverrideResult::Supercede:
			[[fallthrough]];
		case EObserverOverrideResult::Ignored:
			return;
		case EObserverOverrideResult::Handled:
			{
				Spec = CopySpec;
				[[fallthrough]];
			}
		case EObserverOverrideResult::Override:
			break;
		}
	}

	switch (OnStatusEffectAddedBP(ASC, Spec, Handle, StatusEffectData))
	{
	case EObserverOverrideResult::Supercede:
		[[fallthrough]];
	case EObserverOverrideResult::Ignored:
		return;
	case EObserverOverrideResult::Handled:
		{
			Spec = CopySpec;
			[[fallthrough]];
		}
	case EObserverOverrideResult::Override:
		break;
	}

	OnStatusEffectAddedDelegate.Broadcast(ASC, Spec, Handle, StatusEffectData);
}

void UStatusEffectObserver::OnStatusEffectRemoved(UAbilitySystemComponent* ASC, const FGameplayEffectRemovalInfo& RemovalInfo,
	const UStatusEffectComponent* StatusEffectData)
{
	if (OnStatusEffectRemovedOverrideDelegate.IsBound())
	{
		switch (OnStatusEffectRemovedOverrideDelegate.Execute(ASC, RemovalInfo, StatusEffectData))
		{
		case EObserverOverrideResult::Supercede:
			[[fallthrough]];
		case EObserverOverrideResult::Ignored:
			return;
		case EObserverOverrideResult::Handled:
			[[fallthrough]];
		case EObserverOverrideResult::Override:
			break;
		}
	}

	switch (OnStatusEffectRemovedBP(ASC, RemovalInfo, StatusEffectData))
	{
	case EObserverOverrideResult::Supercede:
		[[fallthrough]];
	case EObserverOverrideResult::Ignored:
		return;
	case EObserverOverrideResult::Handled:
		[[fallthrough]];
	case EObserverOverrideResult::Override:
		break;
	}

	OnStatusEffectRemovedDelegate.Broadcast(ASC, RemovalInfo, StatusEffectData);
}
