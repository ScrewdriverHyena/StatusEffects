// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "System/StatusEffectTypes.h"
#include "StatusEffectOverlapComponent.generated.h"

UCLASS()
class STATUSEFFECTS_API UStatusEffectOverlapComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	TArray<FStatusEffectRequest> EffectsGrantedOnEntry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	TArray<FStatusEffectRequest> EffectsGrantedOnExit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	uint8 bRemoveEffectsOnExit : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects", meta = (EditCondition = "bRemoveEffectsOnExit"))
	uint8 bDontRemoveTemporaryEffectsOnExit : 1;

	UStatusEffectOverlapComponent();

protected:

	//~AActor Interface Begin
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~AActor Interface End

	UFUNCTION()
	virtual void OnOwnerBeginOverlap(AActor* ThisActor, AActor* OtherActor);

	UFUNCTION()
	virtual void OnOwnerEndOverlap(AActor* ThisActor, AActor* OtherActor);
};
