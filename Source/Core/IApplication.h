#pragma once

#include <string>
#include "Engine/Scene.h"

class IApplication {
public:
	virtual ~IApplication() = default;

public:
	virtual bool Initialize() = 0;
	virtual void InitScene(Scene& Sce) = 0;
	virtual void Tick(float DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;
	virtual const std::string& GetName() { return AppName_; }

protected:
	std::string AppName_;
};