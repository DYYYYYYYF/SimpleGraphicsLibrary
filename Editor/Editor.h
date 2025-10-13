#pragma once

#include "Core/IApplication.h"

class Editor : public IApplication {
public:
	Editor() {}
	virtual ~Editor() {}

public:
	virtual bool Initialize() override;
	virtual void Tick() override;
	virtual void Render() override;
	virtual void Shutdown() override;
};
