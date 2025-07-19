// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FMaaassParticleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
