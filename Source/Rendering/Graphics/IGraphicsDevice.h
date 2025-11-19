#pragma once

#include "GraphicsAPI.h"
#include <memory>

class Window;
class IMesh;
class IMaterial;

class IGraphicsDevice {
public:
	virtual bool Initialize(Window* Win) = 0;
	virtual void Draw() =0;
	virtual void MakeCurrent() = 0;
	virtual void SwapBuffers() = 0;
	virtual void Destroy() = 0;

public:
	virtual std::shared_ptr<IMesh> CreateMesh() = 0;
	virtual std::shared_ptr<IMaterial> CreateMaterial() = 0;

public:
	BackendAPI GetBackendAPI() { return BackendAPI_; }

protected:
	BackendAPI BackendAPI_;
};