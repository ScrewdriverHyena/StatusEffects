// Copyright No Bright Shadows. All Rights Reserved.


#include "Interactable/StatusEffectOverlapComponent.h"

#include "AbilitySystemInterface.h"
#include "StatusEffectBlueprintLibrary.h"

UStatusEffectOverlapComponent::UStatusEffectOverlapComponent() = default;

void UStatusEffectOverlapComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner<AActor>()->OnActorBeginOverlap.AddDynamic(this, &UStatusEffectOverlapComponent::OnOwnerBeginOverlap);
}

void UStatusEffectOverlapComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* Owner = GetOwner<AActor>(); IsValid(Owner))
	{
		Owner->OnActorBeginOverlap.RemoveDynamic(this, &UStatusEffectOverlapComponent::OnOwnerBeginOverlap);
	}
	
	Super::EndPlay(EndPlayReason);
}

void UStatusEffectOverlapComponent::OnOwnerBeginOverlap(AActor* ThisActor, AActor* OtherActor)
{
	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		UStatusEffectBlueprintLibrary::GrantEffects(EffectsGrantedOnEntry, AbilitySystemInterface->GetAbilitySystemComponent(),
			ThisActor->Implements<UAbilitySystemInterface>() ? Cast<IAbilitySystemInterface>(ThisActor)->GetAbilitySystemComponent()
			: nullptr);
	}
}

void UStatusEffectOverlapComponent::OnOwnerEndOverlap(AActor* ThisActor, AActor* OtherActor)
{
	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (bRemoveEffectsOnExit)
		{
			const TArray<FStatusEffectRequest>& ToRemove = !bDontRemoveTemporaryEffectsOnExit
				? EffectsGrantedOnEntry
				: EffectsGrantedOnEntry.FilterByPredicate([]
					(const FStatusEffectRequest& Request) -> bool
				{
					return !Request.Duration.IsSet();
				});
			
			UStatusEffectBlueprintLibrary::RemoveEffects(ToRemove, AbilitySystemInterface->GetAbilitySystemComponent());
		}

		UStatusEffectBlueprintLibrary::GrantEffects(EffectsGrantedOnExit, AbilitySystemInterface->GetAbilitySystemComponent(),
			ThisActor->Implements<UAbilitySystemInterface>() ? Cast<IAbilitySystemInterface>(ThisActor)->GetAbilitySystemComponent()
			: nullptr);
	}
}


