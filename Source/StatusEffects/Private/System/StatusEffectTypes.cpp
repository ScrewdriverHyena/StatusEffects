// Copyright No Bright Shadows. All Rights Reserved.

#include "System/StatusEffectTypes.h"

FStatusEffectRequest::FStatusEffectRequest() = default;

FStatusEffectRequest::FStatusEffectRequest(TSubclassOf<UGameplayEffect> InEffectClass, const int32 InStackCount, TOptional<double> InDuration,
	const double InChance)
	: EffectClass(MoveTemp(InEffectClass))
	, StackCount(InStackCount)
	, Duration(MoveTemp(InDuration))
	, Chance(InChance)
{
}

FGrantStatusEffectResult::FGrantStatusEffectResult() = default;

FGrantStatusEffectResult::FGrantStatusEffectResult(TArray<FStatusEffectRequest> InGrantedEffects,
	TArray<FStatusEffectRequest> InOverridenEffects)
	: GrantedEffects(MoveTemp(InGrantedEffects))
	, OverridenEffects(MoveTemp(InOverridenEffects))
{
	if (GrantedEffects.Num())
	{
		bWasSuccessful = true;
	}
}

FRemoveStatusEffectResult::FRemoveStatusEffectResult() = default;

FRemoveStatusEffectResult::FRemoveStatusEffectResult(TArray<FStatusEffectRequest> InRemovedEffects)
	: RemovedEffects(MoveTemp(InRemovedEffects))
{
	if (RemovedEffects.Num())
	{
		bWasSuccessful = true;
	}
}

FStatusEffectOverrideInfo::FStatusEffectOverrideInfo() = default;

FStatusEffectOverrideInfo::FStatusEffectOverrideInfo(UAbilitySystemComponent* InSource, UAbilitySystemComponent* InTarget,
	TSubclassOf<UGameplayEffect> InEffectClass, const int32 InOldStacks, const int32 InNewStacks, TOptional<double> InOldDuration,
	TOptional<double> InNewDuration)
	: Source(InSource)
	, Target(InTarget)
	, EffectClass(MoveTemp(InEffectClass))
	, OldStacks(InOldStacks)
	, NewStacks(InNewStacks)
	, OldDuration(MoveTemp(InOldDuration))
	, NewDuration(MoveTemp(InNewDuration))
{
}

UStatusEffectUIObject::UStatusEffectUIObject()
	: Percentage(1.0)
	, TimeRemaining(0.0)
	, StackCount(1)
{
}

