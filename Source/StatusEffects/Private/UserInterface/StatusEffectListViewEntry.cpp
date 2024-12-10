// Copyright No Bright Shadows. All Rights Reserved.

#include "UserInterface/StatusEffectListViewEntry.h"
#include "CommonLazyImage.h"
#include "CommonNumericTextBlock.h"
#include "Components/ProgressBar.h"
#include "System/StatusEffectComponent.h"
#include "System/StatusEffectTypes.h"


UStatusEffectListViewEntry::UStatusEffectListViewEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bUseEffectIconForProgressBarBrush(false)
	, Timeline(FVoidCoroutine())
{
}

void UStatusEffectListViewEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	const UStatusEffectUIObject* UIObject = Cast<UStatusEffectUIObject>(ListItemObject);
	if (!UIObject || !UIObject->Data.Get())
	{
		return;
	}

	Setup(UIObject);
}

void UStatusEffectListViewEntry::NativeOnEntryReleased()
{
	if (Timeline.IsValid())
	{
		Timeline.Cancel();
	}
}

TCoroutine<> UStatusEffectListViewEntry::Setup(const UStatusEffectUIObject* UIObject, const FForceLatentCoroutine ForceLatent)
{
	Icon->SetBrushFromTexture(UIObject->Data->Icon.Get());
	Count->InterpolateToValue(UIObject->StackCount);
	ProgressBar->SetPercent(UIObject->Percentage);

	if (bUseEffectIconForProgressBarBrush)
	{
		FProgressBarStyle& Style = const_cast<FProgressBarStyle&>(ProgressBar->GetWidgetStyle());
		Style.BackgroundImage.SetResourceObject(UIObject->Data->Icon.Get());
		Style.BackgroundImage.TintColor = ProgressBarBackgroundTint;
		Style.FillImage.SetResourceObject(UIObject->Data->Icon.Get());
	}

	if (UIObject->TimeRemaining > 0.0)
	{
		Timeline = FVoidCoroutine(Latent::Timeline(this, UIObject->Percentage, 0.0, UIObject->TimeRemaining,
			std::function([this]
				(const double Alpha)
		{
			ProgressBar->SetPercent(static_cast<float>(Alpha));
		})));

		co_await Timeline;
	}
}
