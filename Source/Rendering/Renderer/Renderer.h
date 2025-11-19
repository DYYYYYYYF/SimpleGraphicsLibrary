#pragma once

#include "RenderModuleAPI.h"
#include "Graphics/GraphicsAPI.h"

#include <string>
#include <memory>

class Window;
class IGraphicsDevice;
class IMesh;
class IMaterial;

class Renderer {
public:
	ENGINE_RENDERING_API Renderer();
	ENGINE_RENDERING_API virtual ~Renderer();

	ENGINE_RENDERING_API static Renderer* Instance();

public:
	ENGINE_RENDERING_API virtual bool Initialize(Window* Win, BackendAPI Type);
	ENGINE_RENDERING_API virtual void Draw();
	ENGINE_RENDERING_API virtual void Destroy();

public:
	ENGINE_RENDERING_API std::shared_ptr<IMesh> CreateMesh(const std::string& Mesh);
	ENGINE_RENDERING_API std::shared_ptr<IMaterial> CreateMaterial(const std::string& Mesh);

protected:
	std::unique_ptr<IGraphicsDevice> GraphicsDevice_;
	static Renderer* GlobalRenderer;

};