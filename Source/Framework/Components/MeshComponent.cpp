#include "MeshComponent.h"

#include "Rendering/Renderer/Renderer.h"
#include "Rendering/Resource/IMesh.h"
#include "Rendering/Resource/IMaterial.h"
#include "Platform/File/JsonObject.h"
#include "Rendering/Resource/Manager/ResourceManager.h"
#include "Framework/Actors/Actor.h"

#ifndef DynmicCast
#define DynmicCast std::dynamic_pointer_cast
#endif

MeshComponent::MeshComponent() : BaseComponent() {}
MeshComponent::MeshComponent(Actor* Owner, const std::string& Name) : BaseComponent(Owner, Name) {}
MeshComponent::~MeshComponent() {
	ResourceManager& RS = ResourceManager::Instance();
	if (MeshAsset_) {
		RS.Release(MeshAsset_->GetID());
		MeshAsset_.reset();
	}
}

void MeshComponent::Draw(CommandList& CmdList) {
	Actor* Owner = GetOwner();
	if (!Owner || !MeshAsset_) {
		return;
	}

	FMatrix4 ModelMatrix = FMatrix4::Identity();
	TransformComponent* TransformComp = Owner->GetComponent<TransformComponent>();
	if (TransformComp) ModelMatrix = TransformComp->GetModelMatrix();

	const std::vector<SubMeshDesc>& SubMeshes = MeshAsset_->GetSubMeshes();
	for (const SubMeshDesc& SubMesh : SubMeshes) {
		IMaterial* Material = MeshAsset_->GetMaterial(SubMesh.MaterialIndex).get();
		if (!Material) {
			continue;
		}

		CmdList.DrawIndexed(MeshAsset_.get(), Material, ModelMatrix, SubMesh.IndexCount, SubMesh.BaseIndex);
	}
}

bool MeshComponent::LoadFromFile(const std::string& FilePath) {
	
	// 加载资产
	ResourceManager& RS = ResourceManager::Instance();
	MeshAsset_ = DynmicCast<IMesh>(RS.LoadResource(ResourceType::eMesh, FilePath));
	if (!MeshAsset_) {
		return false;
	}

	return true;
}