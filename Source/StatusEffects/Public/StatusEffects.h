// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "UE5Coro.h"
#include "Modules/ModuleManager.h"

using namespace UE5Coro;

class FStatusEffectsModule final : public IModuleInterface
{
public:

	//~IModuleInterface Interface Begin
	virtual void StartupModule() override;
	
	virtual void ShutdownModule() override;
	//~IModuleInterface Interface End
};
