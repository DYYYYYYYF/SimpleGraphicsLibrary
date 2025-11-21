#pragma once

#include "ISystem.h"

class EngineSystem : public ISystem {
public:
	virtual bool Initialize() override { return true; }
	virtual void Shutdown() override {}
};