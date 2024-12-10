// Copyright No Bright Shadows. All Rights Reserved.

#include "UserInterface/StatusEffectWidget.h"
#include "System/StatusEffectComponent.h"
#include "AbilitySystemInterface.h"
#include "StatusEffectBlueprintLibrary.h"
#include "System/StatusEffectObserver.h"
#include "UserInterface/StatusEffectListView.h"

UStatusEffectWidget::UStatusEffectWidget() = default;

void UStatusEffectWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const ULocalPlayer* Owner = GetOwningLocalPlayer();
	check(IsValid(Owner));
	
	const UStatusEffectObserver* Observer = UStatusEffectBlueprintLibrary::GetObserverByLocalPlayer(Owner);
	check(IsValid(Observer));

	Observer->OnStatusEffectGranted().AddUniqueDynamic(this, &UStatusEffectWidget::StatusEffectAdd);
	Observer->OnStatusEffectOverride().AddUniqueDynamic(this, &UStatusEffectWidget::StatusEffectOverride);
	Observer->OnStatusEffectRevoked().AddUniqueDynamic(this, &UStatusEffectWidget::StatusEffectRemove);
}

void UStatusEffectWidget::StatusEffectAdd(UAbilitySystemComponent*, UAbilitySystemComponent*, const FStatusEffectRequest&)
{
	Refresh();
}

void UStatusEffectWidget::StatusEffectOverride(const FStatusEffectOverrideInfo&)
{
	Refresh();
}

void UStatusEffectWidget::StatusEffectRemove(UAbilitySystemComponent*, const FStatusEffectRequest&)
{
	Refresh();
}

TCoroutine<> UStatusEffectWidget::Refresh(const FForceLatentCoroutine)
{
	List->ClearListItems();

	co_await Latent::NextTick();

	const ULocalPlayer* Owner = GetOwningLocalPlayer();
	check(IsValid(Owner));

	const APlayerController* PC = Owner->GetPlayerController(GetWorld());
	check(IsValid(PC));

	const APawn* Pawn = PC->GetPawn();
	check(IsValid(Pawn));

	const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!AbilitySystemInterface)
	{
		co_return;
	}

	co_await Latent::Until(std::function([AbilitySystemInterface]
		() -> bool
	{
		return !!AbilitySystemInterface->GetAbilitySystemComponent();
	}));
	
	const UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	check(IsValid(ASC));

	TMap<const UStatusEffectComponent*, int32> Stacks;
	for (const FActiveGameplayEffect& AGE : &ASC->GetActiveGameplayEffects())
	{
		if (AGE.GetDuration() != FGameplayEffectConstants::INFINITE_DURATION)
		{
			continue;
		}
		
		const UStatusEffectComponent* Data = AGE.Spec.Def->FindComponent<UStatusEffectComponent>();
		if (!Data)
		{
			continue;
		}

		if (Stacks.Contains(Data))
		{
			++Stacks[Data];

			continue;
		}

		Stacks.Add(Data, 1);
	}

	for (const FActiveGameplayEffect& AGE : &ASC->GetActiveGameplayEffects())
	{
		const UStatusEffectComponent* Data = AGE.Spec.Def->FindComponent<UStatusEffectComponent>();
		if (!Data)
		{
			continue;
		}

		const double TimeRemaining = AGE.GetTimeRemaining(ASC->GetActiveGameplayEffects().GetWorldTime());

		UStatusEffectUIObject* Item = NewObject<UStatusEffectUIObject>(this);
		Item->Data = Data;
		Item->Percentage = TimeRemaining / AGE.Spec.GetDuration();
		Item->TimeRemaining = TimeRemaining;
		Item->StackCount = Stacks.Contains(Data) ? Stacks[Data] : 1;

		List->AddItem(Item);
	}
}
