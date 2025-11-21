#pragma once

#include "RenderModuleAPI.h"
#include "Resource/IResource.h"

#include <memory>
#include <unordered_map>

#ifndef MESH_CONFIG_PATH
#define MESH_CONFIG_PATH "../Assets/Meshes"
#endif
#ifndef MATERIAL_CONFIG_PATH
#define MATERIAL_CONFIG_PATH "../Assets/Materials"
#endif
#ifndef SHADER_CONFIG_PATH
#define SHADER_CONFIG_PATH "../Assets/Shaders/Configs"
#endif
#ifndef SHADER_ASSET_PATH
#define SHADER_ASSET_PATH "../Assets/Shaders/Sources"
#endif
#ifndef TEXTURE_ASSET_PATH
#define TEXTURE_ASSET_PATH "../Assets/Textures"
#endif

#ifndef DynamicCast
#define DynamicCast std::dynamic_pointer_cast
#endif

class ResourceManager {
public:
	ENGINE_RENDERING_API static ResourceManager& Instance();

public:
	ENGINE_RENDERING_API virtual bool Initialize();
	ENGINE_RENDERING_API virtual void Shutdown();

	ENGINE_RENDERING_API std::shared_ptr<IResource> LoadResource(ResourceType Type, const std::string& filename);

	ENGINE_RENDERING_API std::shared_ptr<IResource> Acquire(ResourceType Type, const std::string& Name);
	ENGINE_RENDERING_API std::shared_ptr<IResource> Acquire(uint64_t ID);
	ENGINE_RENDERING_API void Release(uint64_t ID);

private:
	void GenerateBuiltinMesh();
	void GenerateBuiltinMaterial();
	void GenerateBuiltinShader();
	void GenerateBuiltinTexture();

	std::shared_ptr<IResource> LoadMeshResource(const std::string& filename);
	std::shared_ptr<IResource> LoadMaterialResource(const std::string& filename);
	std::shared_ptr<IResource> LoadShaderResource(const std::string& filename);
	std::shared_ptr<IResource> LoadTextureResource(const std::string& filename);

public:
	std::unordered_map<uint64_t, std::shared_ptr<IResource>> Resources_;

	std::unordered_map<std::string, uint64_t> MeshNameMap_;
	std::unordered_map<std::string, uint64_t> MaterialNameMap_;
	std::unordered_map<std::string, uint64_t> ShaderNameMap_;
	std::unordered_map<std::string, uint64_t> TextureNameMap_;

};