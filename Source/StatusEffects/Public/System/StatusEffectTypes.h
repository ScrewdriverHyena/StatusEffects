// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "StatusEffectTypes.generated.h"

class UStatusEffectComponent;
enum class EGameplayEffectDurationType : uint8;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EStatusEffectType : uint8
{
	Neutral,
	Positive,
	Negative
};

UENUM(BlueprintType, meta = (BitFlags))
enum class EStatusEffectFlags : uint8
{
	None	= 0,
	Super	= 1 << 0
};

UENUM(BlueprintType)
enum class EObserverOverrideResult : uint8
{
	Ignored,
	Handled,
	Override,
	Supercede
};

ENUM_CLASS_FLAGS(EStatusEffectFlags);

USTRUCT(BlueprintType)
struct STATUSEFFECTS_API FStatusEffectRequest
{
	GENERATED_BODY()

	FStatusEffectRequest();

	explicit FStatusEffectRequest(TSubclassOf<UGameplayEffect> InEffectClass, const int32 InStackCount = 1,
		TOptional<double> InDuration = TOptional<double>(), const double InChance = 1.0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance", meta = (UIMin = 1, ClampMin = 1))
	int32 StackCount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance", meta = (UIMin = 0.0, ClampMin = 0.0))
	TOptional<double> Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance", meta = (UIMin = 0.0, ClampMin = 0.0, UIMax = 1.0, ClampMax = 1.0))
	double Chance;
};

USTRUCT(BlueprintType)
struct STATUSEFFECTS_API FGrantStatusEffectResult
{
	GENERATED_BODY()

	FGrantStatusEffectResult();

	explicit FGrantStatusEffectResult(TArray<FStatusEffectRequest> InGrantedEffects, TArray<FStatusEffectRequest> InOverridenEffects = {});

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bWasSuccessful : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FStatusEffectRequest> GrantedEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FStatusEffectRequest> OverridenEffects;
};

USTRUCT(BlueprintType)
struct STATUSEFFECTS_API FRemoveStatusEffectResult
{
	GENERATED_BODY()

	FRemoveStatusEffectResult();

	explicit FRemoveStatusEffectResult(TArray<FStatusEffectRequest> InRemovedEffects);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bWasSuccessful : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FStatusEffectRequest> RemovedEffects;
};

USTRUCT(BlueprintType)
struct STATUSEFFECTS_API FStatusEffectOverrideInfo
{
	GENERATED_BODY()

	FStatusEffectOverrideInfo();

	explicit FStatusEffectOverrideInfo(UAbilitySystemComponent* InSource, UAbilitySystemComponent* InTarget,
		TSubclassOf<UGameplayEffect> InEffectClass, const int32 InOldStacks, const int32 InNewStacks, TOptional<double> InOldDuration,
		TOptional<double> InNewDuration);

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> Source;

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 OldStacks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NewStacks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TOptional<double> OldDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TOptional<double> NewDuration;
};

UCLASS()
class STATUSEFFECTS_API UStatusEffectUIObject : public UObject
{
	GENERATED_BODY()

public:

	UStatusEffectUIObject();

	UPROPERTY(Transient)
	TObjectPtr<const UStatusEffectComponent> Data;

	double Percentage;

	double TimeRemaining;

	int32 StackCount;
};