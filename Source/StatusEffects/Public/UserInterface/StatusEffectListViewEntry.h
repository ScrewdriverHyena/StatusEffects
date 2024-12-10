// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "StatusEffects.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "StatusEffectListViewEntry.generated.h"

class UStatusEffectUIObject;
class UCommonNumericTextBlock;
class UProgressBar;
class UCommonLazyImage;

UCLASS(Abstract)
class STATUSEFFECTS_API UStatusEffectListViewEntry : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Status Effects|User Interface")
	uint8 bUseEffectIconForProgressBarBrush : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Status Effects|User Interface", meta = (EditCondition = "bUseEffectIconForProgressBarBrush"))
	FSlateColor ProgressBarBackgroundTint = FSlateColor(FColor::White);

	explicit UStatusEffectListViewEntry(const FObjectInitializer& ObjectInitializer);

	//~IUserObjectListEntry Interface Begin
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	virtual void NativeOnEntryReleased() override;
	//~IUserObjectListEntry Interface End

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonNumericTextBlock> Count;

private:

	TCoroutine<> Setup(const UStatusEffectUIObject* UIObject, const FForceLatentCoroutine ForceLatent = {});

	FVoidCoroutine Timeline;
};
