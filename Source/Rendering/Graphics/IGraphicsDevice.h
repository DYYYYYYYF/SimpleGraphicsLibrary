#pragma once

#include "GraphicsAPI.h"
#include <memory>
#include <string>

class Window;
class IMesh;
class IMaterial;
class IShader;
class ITexture;
class CommandList;

class IGraphicsDevice {
public:
	virtual bool Initialize(Window* Win) = 0;
	virtual void ExecuteCommandList(const CommandList& cmdList) =0;
	virtual void MakeCurrent() = 0;
	virtual void SwapBuffers() = 0;
	virtual void Destroy() = 0;

public:
	virtual std::shared_ptr<IMesh> CreateMesh(const struct MeshDesc& AssetDesc) = 0;
	virtual std::shared_ptr<IMaterial> CreateMaterial(const struct MaterialDesc& AssetPath) = 0;
	virtual std::shared_ptr<IShader> CreateShader(const struct ShaderDesc& AssetDesc) = 0;
	virtual std::shared_ptr<ITexture> CreateTexture(const std::string& AssetPath) = 0;

public:
	BackendAPI GetBackendAPI() { return BackendAPI_; }

protected:
	BackendAPI BackendAPI_;
};