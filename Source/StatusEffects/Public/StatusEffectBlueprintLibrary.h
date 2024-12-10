// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "System/StatusEffectTypes.h"
#include "StatusEffectBlueprintLibrary.generated.h"

class UAbilitySystemComponent;
class UStatusEffectObserver;

UCLASS()
class STATUSEFFECTS_API UStatusEffectBlueprintLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UStatusEffectBlueprintLibrary();

	UFUNCTION(BlueprintPure, Category = "Status Effects")
	static UStatusEffectObserver* GetObserverByLocalPlayer(const ULocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintPure, Category = "Status Effects")
	static UStatusEffectObserver* GetObserverByAbilityTarget(const UAbilitySystemComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	static FGrantStatusEffectResult GrantEffect(FStatusEffectRequest Request, UAbilitySystemComponent* Target,
		UAbilitySystemComponent* Source = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	static FGrantStatusEffectResult GrantEffects(const TArray<FStatusEffectRequest>& Requests,
		UAbilitySystemComponent* Target, UAbilitySystemComponent* Source = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Status Effects", meta = (DisplayName = "Revoke Effect"))
	static FRemoveStatusEffectResult RemoveEffect(FStatusEffectRequest Request, UAbilitySystemComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Status Effects", meta = (DisplayName = "Revoke Effects"))
	static FRemoveStatusEffectResult RemoveEffects(const TArray<FStatusEffectRequest>& Requests, UAbilitySystemComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Status Effects", meta = (DisplayName = "Has Effect (Any)"))
	static bool HasEffectAny(const UAbilitySystemComponent* Target, const TSubclassOf<UGameplayEffect> EffectClass);

	UFUNCTION(BlueprintCallable, Category = "Status Effects", meta = (DisplayName = "Has Effect (Exact)"))
	static bool HasEffectExact(const UAbilitySystemComponent* Target, const TSubclassOf<UGameplayEffect> EffectClass,
		const EGameplayEffectDurationType DurationType = EGameplayEffectDurationType::Infinite, const int32 Stacks = 1);

	UFUNCTION(BlueprintCallable, Category = "Status Effects", meta = (AdvancedDisplay = "2", DevelopmentOnly))
	static TSubclassOf<UGameplayEffect> FindEffectByName(const FString EffectName);
	
	UFUNCTION(BlueprintCallable, Category = "Status Effects", meta = (AdvancedDisplay = "2", DevelopmentOnly))
	static FGrantStatusEffectResult GrantEffectByName(const FString EffectName, UAbilitySystemComponent* Target, const double Duration = 1.0,
		const int32 StackCount = 1, UAbilitySystemComponent* Source = nullptr);
};
