#pragma once

#include "GraphicsAPI.h"

class Window;

class IGraphicsDevice {
public:
	virtual bool Initialize(Window* Win, BackendAPI Type) = 0;
	virtual void Draw() =0;
	virtual void Destroy() = 0;

public:
	BackendAPI GetBackendAPI() { return BackendAPI_; }

protected:
	BackendAPI BackendAPI_;
};