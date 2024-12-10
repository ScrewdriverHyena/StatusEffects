// Copyright No Bright Shadows. All Rights Reserved.

#include "UserInterface/StatusEffectListView.h"
#include "Misc/DataValidation.h"
#include "UserInterface/StatusEffectListViewEntry.h"

#define LOCTEXT_NAMESPACE "StatusEffects"

UStatusEffectListView::UStatusEffectListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
EDataValidationResult UStatusEffectListView::IsDataValid(FDataValidationContext& Context) const
{
	if (!EntryWidgetClass->IsChildOf(UStatusEffectListViewEntry::StaticClass()))
	{
		Context.AddError(LOCTEXT("InvalidEntryWidgetClass", "EntryWidgetClass must be a UStatusEffectListViewEntry"));

		Super::IsDataValid(Context);

		return EDataValidationResult::Invalid;
	}
	
	return Super::IsDataValid(Context);
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE