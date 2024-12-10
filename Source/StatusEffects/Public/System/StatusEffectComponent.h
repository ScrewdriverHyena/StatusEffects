// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData_TextOnly.h"
#include "StatusEffectComponent.generated.h"

enum class EStatusEffectType : uint8;

/**
 * @brief	Supplies the necessary data to classify a Gameplay Effect as a Status Effect.
 *
 * @note	Inherit from this class if one would like to add more information to their Status Effects.
 */
UCLASS(DisplayName = "Status Effect Data Component")
class STATUSEFFECTS_API UStatusEffectComponent : public UGameplayEffectUIData_TextOnly
{
	GENERATED_BODY()

public:

	UStatusEffectComponent();

	/**
	 * @brief	The name of the status effect.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display Information")
	FText DisplayName;

	/**
	 * @brief	Icon to be used on the Status Effect HUD.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display Information")
	TObjectPtr<UTexture2D> Icon;

	/**
	 * @brief	Optional mesh which may be used to create a 3D representation of the status effect.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display Information")
	TObjectPtr<UStaticMesh> Mesh;

	/**
	 * @brief	Whether the Status Effect is Neutral, a buff (Positive), or a debuff (Negative).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Information")
	EStatusEffectType Type;

	/**
	 * @brief	Color associated with this Status Effect.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Information")
	TOptional<FColor> OverrideTypeColor;

	/**
	 * @brief	Additional flags for the Status Effect.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Information", meta = (Bitmask, BitmaskEnum = "EStatusEffectFlags"))
	uint8 Flags;

	/**
	 * @brief	Override pickup sound for this Status Effect. Enable but leave blank to force no pickup sound.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Information")
	TOptional<USoundWave*> OverridePickupSound;

	/**
	 * @brief	Override removal sound for this Status Effect. Enable but leave blank to force no removal sound.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Information")
	TOptional<USoundWave*> OverrideRemoveSound;

	/**
	 * @brief	Sound played on first pickup. Leave blank to play regular/override pickup sound.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Information")
	TObjectPtr<USoundWave> FirstTimePickupSound;
};
