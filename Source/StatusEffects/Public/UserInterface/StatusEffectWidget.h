// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "StatusEffects.h"
#include "StatusEffectWidget.generated.h"

struct FStatusEffectOverrideInfo;
struct FStatusEffectRequest;
struct FGameplayEffectRemovalInfo;
class UStatusEffectComponent;
struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
class UStatusEffectListView;

UCLASS(Abstract)
class STATUSEFFECTS_API UStatusEffectWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	UStatusEffectWidget();

	//~UUserWidget Interface Begin
	virtual void NativeOnInitialized() override;
	//~UUserWidget Interface End
	
protected:

	UFUNCTION()
	virtual void StatusEffectAdd(UAbilitySystemComponent* Target, UAbilitySystemComponent* Source, const FStatusEffectRequest& RequestedEffect);

	UFUNCTION()
	virtual void StatusEffectOverride(const FStatusEffectOverrideInfo& OverrideInfo);

	UFUNCTION()
	virtual void StatusEffectRemove(UAbilitySystemComponent* Target, const FStatusEffectRequest& RequestedEffect);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusEffectListView> List;

private:

	TCoroutine<> Refresh(const FForceLatentCoroutine ForceLatent = {});
};
