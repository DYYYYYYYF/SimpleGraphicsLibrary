#pragma once

#include "RenderModuleAPI.h"
#include "Graphics/GraphicsAPI.h"

class Window;
class IGraphicsDevice;

class ENGINE_RENDERING_API Renderer {
public:
	Renderer();
	virtual ~Renderer();

public:
	virtual bool Initialize(Window* Win, BackendAPI Type);
	virtual void Draw();
	virtual void Destroy();

protected:
	IGraphicsDevice* GraphicsDevice_;

};