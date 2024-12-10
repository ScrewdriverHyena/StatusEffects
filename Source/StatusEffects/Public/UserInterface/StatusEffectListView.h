// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "StatusEffectListView.generated.h"

UCLASS()
class STATUSEFFECTS_API UStatusEffectListView : public UCommonListView
{
	GENERATED_BODY()

public:

	explicit UStatusEffectListView(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	//~UObject Interface Begin
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	//~UObject Interface End
#endif // WITH_EDITOR
};
