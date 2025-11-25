#pragma once

#include "RenderModuleAPI.h"
#include "Resource/IResource.h"

#include <memory>
#include <unordered_map>

#define BUILTIN_RECTANGLE_MESH "BuiltinRectangle"
#define BUILTIN_PBR_SHADER "BuiltinShader"
#define BUILTIN_PBR_MATERIAL "BuiltinMaterial"


class ResourceManager {
public:
	ENGINE_RENDERING_API static ResourceManager& Instance();

public:
	ENGINE_RENDERING_API virtual bool Initialize();
	ENGINE_RENDERING_API virtual void Shutdown();

	ENGINE_RENDERING_API std::shared_ptr<IResource> LoadResource(ResourceType Type, const std::string& filename);
	ENGINE_RENDERING_API std::shared_ptr<IResource> LoadResourceFromDescriptor(ResourceType Type, IResourceDesc* Desc);

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