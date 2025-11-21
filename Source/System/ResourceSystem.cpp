#include "ResourceSystem.h"
#include "Rendering/Renderer/Renderer.h"
#include <Logger.hpp>

ResourceSystem& ResourceSystem::Instance() {
	static ResourceSystem GlobalResourceSys;
	return GlobalResourceSys;
}

bool ResourceSystem::Initialize(){
	
	return true;
}


void ResourceSystem::Shutdown(){
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

std::shared_ptr<IResource> ResourceSystem::Acquire(ResourceType Type, const std::string& Name) {
	uint64_t ResourceID = 0xFF;

	switch (Type)
	{
	case ResourceType::eMesh: {
		if (MeshNameMap_.find(Name) == MeshNameMap_.end()) {
			std::shared_ptr<IMesh> Mesh = Renderer::Instance()->CreateMesh(Name);
			if (!Mesh || !Mesh->IsValid()) {
				return nullptr;
			}

			ResourceID = Mesh->GetID();
			Resources_[ResourceID] = Mesh;
		}
		else {
			ResourceID = MeshNameMap_[Name];
		}
	} break;
	case ResourceType::eMaterial:
		if (MaterialNameMap_.find(Name) == MaterialNameMap_.end()) {
			std::shared_ptr<IMaterial> Material = Renderer::Instance()->CreateMaterial(Name);
			if (!Material || !Material->IsValid()) {
				return nullptr;
			}

			ResourceID = Material->GetID();
			Resources_[ResourceID] = Material;
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

void ResourceSystem::Release(uint64_t ID) {
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