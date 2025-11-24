#include "MeshComponent.h"

#include "Rendering/Renderer/Renderer.h"
#include "Rendering/Resource/IMesh.h"
#include "Rendering/Resource/IMaterial.h"
#include "Platform/File/JsonObject.h"
#include "Rendering/Resource/Manager/ResourceManager.h"

#ifndef DynmicCast
#define DynmicCast std::dynamic_pointer_cast
#endif

MeshComponent::MeshComponent() : BaseComponent() {}
MeshComponent::MeshComponent(Actor* Owner, const std::string& Name) : BaseComponent(Owner, Name) {}
MeshComponent::~MeshComponent() {
	ResourceManager& RS = ResourceManager::Instance();
	if (Meshes_) {
		RS.Release(Meshes_->GetID());
		Meshes_.reset();
	}
}

void MeshComponent::Draw(CommandList& CmdList) {
	Actor* Owner = GetOwner();
	if (!Owner) {
		return;
	}

	IMesh* CurrentMesh = GetMesh().get();
	if (!CurrentMesh) {
		return;
	}

	CurrentMesh->Draw(CmdList);
}

bool MeshComponent::LoadFromFile(const std::string& FilePath) {
	
	// 加载资产
	ResourceManager& RS = ResourceManager::Instance();
	Meshes_ = DynmicCast<IMesh>(RS.LoadResource(ResourceType::eMesh, FilePath));
	if (!Meshes_) {
		return false;
	}

	return true;
}