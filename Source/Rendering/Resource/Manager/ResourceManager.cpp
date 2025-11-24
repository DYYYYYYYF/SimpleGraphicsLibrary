#include "ResourceManager.h"
#include "Rendering/Renderer/Renderer.h"
#include "Platform/File/JsonObject.h"
#include "Resource/IShader.h"
#include "Loader/MaterialLoader.h"
#include <Logger.hpp>
#include "Loader/MeshLoader.h"
#include "Loader/ShaderLoader.h"

ResourceManager& ResourceManager::Instance() {
	static ResourceManager GlobalResourceSys;
	return GlobalResourceSys;
}

bool ResourceManager::Initialize() {

	return true;
}


void ResourceManager::Shutdown() {
	MeshNameMap_.clear();
	MaterialNameMap_.clear();
	ShaderNameMap_.clear();
	TextureNameMap_.clear();

	for (auto& Res : Resources_) {
		if (Res.second) {
			Res.second.reset();
		}
	}
	Resources_.clear();

	LOG_INFO << "Resource system shutdown.";
}

std::shared_ptr<IResource> ResourceManager::LoadResource(ResourceType Type, const std::string& filename) {
	switch (Type)
	{
	case ResourceType::eMesh:
		return LoadMeshResource(filename);
		break;
	case ResourceType::eMaterial:
		return LoadMaterialResource(filename);
		break;
	case ResourceType::eShader:
		return LoadShaderResource(filename);
		break;
	case ResourceType::eTexture:
		return LoadTextureResource(filename);
		break;
	}

	return nullptr;
}

std::shared_ptr<IResource> ResourceManager::Acquire(ResourceType Type, const std::string& Name) {
	uint64_t ResourceID = 0xFF;

	switch (Type)
	{
	case ResourceType::eMesh: {
		if (MaterialNameMap_.find(Name) == MaterialNameMap_.end()) {
			LOG_WARN << "Can not find mesh resource '" << Name << "', return nullptr";
			return nullptr;
		}
		else {
			ResourceID = MeshNameMap_[Name];
		}
	} break;
	case ResourceType::eMaterial:
		if (MaterialNameMap_.find(Name) == MaterialNameMap_.end()) {
			LOG_WARN << "Can not find material resource '" << Name << "', return nullptr";
			return nullptr;
		}
		else {
			ResourceID = MaterialNameMap_[Name];
		}
	break;
	case ResourceType::eShader:
		break;
	case ResourceType::eTexture:
		break;
	default:
		LOG_ERROR << "ResourceSystem::Acquire - Acquire resource failed!";
		return nullptr;
	}

	if (ResourceID == 0xFF || Resources_.find(ResourceID) == Resources_.end()) {
		return nullptr;
	}

	return Resources_[ResourceID];
}

void ResourceManager::Release(uint64_t ID) {
	if (Resources_.find(ID) == Resources_.end()) {
		return;
	}

	long UseCount = Resources_[ID].use_count();
	LOG_INFO << "Resource '" << Resources_[ID]->GetName() << "' current reference count: " << UseCount;
	if (UseCount == 1) {
		Resources_[ID].reset();
		Resources_.erase(ID);
		LOG_WARN << "Unloading the resource cause the reference count has reached 0!";
	}
}

void ResourceManager::GenerateBuiltinMesh(){

}

void ResourceManager::GenerateBuiltinMaterial(){

}

void ResourceManager::GenerateBuiltinShader(){

}

void ResourceManager::GenerateBuiltinTexture() {

}

std::shared_ptr<IResource> ResourceManager::LoadMeshResource(const std::string& filename) {
	File MeshSrc(MESH_CONFIG_PATH + filename);
	if (!MeshSrc.IsExist()) {
		return nullptr;
	}

	JsonObject Content = JsonObject(MeshSrc.ReadBytes());
	const std::string& Name = Content.Get("Name").GetString();
	if (MeshNameMap_.find(Name) != MeshNameMap_.end()) {
		LOG_WARN << "Resource mesh '" << Name << "' already exist.";
		return nullptr;
	}

	// MeshAsset
	const std::string& MeshAsset = Content.Get("MeshAsset").GetString();
	if (!MeshLoader::Load(MeshAsset)) {
		LOG_ERROR << "Load mesh asset '" << Name << "' failed.";
		return nullptr;
	}

	std::shared_ptr<IMesh> Mesh = Renderer::Instance()->CreateMesh(filename);
	if (!Mesh || !Mesh->IsValid()) {
		return nullptr;
	}

	uint64_t ID = Mesh->GetID();
	MeshNameMap_[Name] = ID;
	Resources_[ID] = Mesh;
	return Mesh;
}

std::shared_ptr<IResource> ResourceManager::LoadMaterialResource(const std::string& filename) {
	File MaterialSrc(MATERIAL_CONFIG_PATH + filename);
	if (!MaterialSrc.IsExist()) {
		return nullptr;
	}

	JsonObject Content = JsonObject(MaterialSrc.ReadBytes());
	const std::string& Name = Content.Get("Name").GetString();
	if (MaterialNameMap_.find(Name) != MaterialNameMap_.end()) {
		LOG_WARN << "Resource material '" << Name << "' already exist.";
		return nullptr;
	}

	MaterialDesc Desc;
	if (!MaterialLoader::Load(filename, Desc)) {
		// TODO: 失败时使用 Built-in Material
		LOG_WARN << "Load material '" << filename << "' failed! Use built-in material.";
	}

	std::shared_ptr<IMaterial> Material = Renderer::Instance()->CreateMaterial(Desc);
	if (!Material || !Material->IsValid()) {
		LOG_WARN << "TODO: Need use default material.";
		LOG_ERROR << "Load material asset failed!";
	}

	uint64_t ID = Material->GetID();
	MaterialNameMap_[Name] = ID;
	Resources_[ID] = Material;
	return Material;
}

std::shared_ptr<IResource> ResourceManager::LoadShaderResource(const std::string& filename) {
	File ShaderAsset(MATERIAL_CONFIG_PATH + filename);
	if (!ShaderAsset.IsExist()) {
		return nullptr;
	}

	JsonObject Content = JsonObject(ShaderAsset.ReadBytes());
	const std::string& Name = Content.Get("Name").GetString();
	if (ShaderNameMap_.find(Name) != ShaderNameMap_.end()) {
		LOG_WARN << "Resource shader '" << Name << "' already exist.";
		return nullptr;
	}

	ShaderDesc Desc;
	if (!ShaderLoader::Load(filename, Desc)) {
		// TODO: 失败时使用 Built-in Shader
		LOG_WARN << "Load shader '" << filename << "' failed! Use built-in shader.";
	}

	std::shared_ptr<IShader> Shader = Renderer::Instance()->CreateShader(Desc);
	if (!Shader || !Shader->IsValid()) {
		return nullptr;
	}

	uint64_t ID = Shader->GetID();
	MaterialNameMap_[Name] = ID;
	Resources_[ID] = Shader;
	return Shader;
}

std::shared_ptr<IResource> ResourceManager::LoadTextureResource(const std::string& filename) {

	return nullptr;
}
