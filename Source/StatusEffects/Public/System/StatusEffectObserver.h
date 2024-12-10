// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffects.h"
#include "StatusEffectTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StatusEffectObserver.generated.h"

class UStatusEffectBlueprintLibrary;
struct FGameplayEffectRemovalInfo;
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;
class UStatusEffectComponent;
class UAbilitySystemComponent;
class IAbilitySystemInterface;

DECLARE_LOG_CATEGORY_EXTERN(LogStatusEffects, Log, All);

DECLARE_DELEGATE_RetVal_FourParams(EObserverOverrideResult, FOnStatusEffectAddedOverride, UAbilitySystemComponent*, FGameplayEffectSpec&,
	const FActiveGameplayEffectHandle&, const UStatusEffectComponent*);
DECLARE_DELEGATE_RetVal_ThreeParams(EObserverOverrideResult, FOnStatusEffectRemovedOverride, UAbilitySystemComponent*,
	const FGameplayEffectRemovalInfo&, const UStatusEffectComponent*);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnStatusEffectAdded, UAbilitySystemComponent*, const FGameplayEffectSpec&, const FActiveGameplayEffectHandle&,
	const UStatusEffectComponent*);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnStatusEffectRemoved, UAbilitySystemComponent*, const FGameplayEffectRemovalInfo&,
	const UStatusEffectComponent*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatusEffectGranted, UAbilitySystemComponent*, Target, UAbilitySystemComponent*, Source,
	const FStatusEffectRequest&, RequestedEffect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectOverride, const FStatusEffectOverrideInfo&, OverrideInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatusEffectRevoked, UAbilitySystemComponent*, Target, const FStatusEffectRequest&, RequestedEffect);

UCLASS()
class STATUSEFFECTS_API UStatusEffectObserver : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:

	UStatusEffectObserver();

	//~USubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~USubsystem Interface End

	//~ULocalPlayerSubsystem Interface Begin
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;
	//~ULocalPlayerSubsystem Interface End

	FOnStatusEffectGranted& OnStatusEffectGranted() const;

	FOnStatusEffectOverride& OnStatusEffectOverride() const;

	FOnStatusEffectRevoked& OnStatusEffectRevoked() const;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName = "On Status Effect Added")
	EObserverOverrideResult OnStatusEffectAddedBP(UAbilitySystemComponent* ASC, FGameplayEffectSpec Spec,
		const FActiveGameplayEffectHandle& Handle, const UStatusEffectComponent* StatusEffectData);

	UFUNCTION(BlueprintNativeEvent, DisplayName = "On Status Effect Removed")
	EObserverOverrideResult OnStatusEffectRemovedBP(UAbilitySystemComponent* ASC,
		const FGameplayEffectRemovalInfo& RemovalInfo, const UStatusEffectComponent* StatusEffectData);

	FOnStatusEffectAddedOverride& OverrideStatusEffectAdded();

	FOnStatusEffectRemovedOverride& OverrideStatusEffectRemoved();

protected:

	virtual TCoroutine<> AsyncInitialize(const FForceLatentCoroutine ForceLatent = {});

	virtual TCoroutine<APawn*> WaitForValidPawn(const FForceLatentCoroutine ForceLatent = {});

	virtual TCoroutine<> Observe(const IAbilitySystemInterface* Pawn);

	virtual void OnStatusEffectAdded(UAbilitySystemComponent* ASC, FGameplayEffectSpec Spec, const FActiveGameplayEffectHandle& Handle,
		const UStatusEffectComponent* StatusEffectData);

	virtual void OnStatusEffectRemoved(UAbilitySystemComponent* ASC, const FGameplayEffectRemovalInfo& RemovalInfo,
		const UStatusEffectComponent* StatusEffectData);

private:

	FOnStatusEffectAddedOverride OnStatusEffectAddedOverrideDelegate;

	FOnStatusEffectRemovedOverride OnStatusEffectRemovedOverrideDelegate;

	FOnStatusEffectAdded OnStatusEffectAddedDelegate;

	FOnStatusEffectRemoved OnStatusEffectRemovedDelegate;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	mutable FOnStatusEffectGranted OnStatusEffectGrantedDelegate;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	mutable FOnStatusEffectOverride OnStatusEffectOverrideDelegate;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	mutable FOnStatusEffectRevoked OnStatusEffectRevokedDelegate;

	friend UStatusEffectBlueprintLibrary;
};