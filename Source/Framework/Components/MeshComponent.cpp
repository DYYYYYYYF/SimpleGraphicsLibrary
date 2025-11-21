#include "MeshComponent.h"

#include "Rendering/Renderer/Renderer.h"
#include "Rendering/Resource/IMesh.h"
#include "Rendering/Resource/IMaterial.h"
#include "Platform/File/JsonObject.h"
#include "System/ResourceSystem.h"

MeshComponent::MeshComponent() : BaseComponent() {}
MeshComponent::MeshComponent(Actor* Owner, const std::string& Name) : BaseComponent(Owner, Name) {}
MeshComponent::~MeshComponent() {
	ResourceSystem& RS = ResourceSystem::Instance();
	if (Meshes_) {
		RS.Release(Meshes_->GetID());
		Meshes_.reset();
	}
	if (Materials_) {
		RS.Release(Materials_->GetID());
		Materials_.reset();
	}
}

void MeshComponent::Draw(CommandList& CmdList) {
	Actor* Owner = GetOwner();
	if (!Owner) {
		return;
	}

	if (!Materials_) {
		return;
	}

	if (!GetMesh() || !GetMaterial()) {
		return;
	}

	CmdList.DrawIndexed(GetMesh().get(), GetMaterial().get(), FMatrix4::Identity(), 6);
}

bool MeshComponent::LoadFromFile(const std::string& FilePath) {
	File MeshSrc("../Assets/Meshes" + FilePath);
	if (!MeshSrc.IsExist()) {
		return false;
	}
	
	JsonObject Content = MeshSrc.ReadBytes();
	ResourceSystem& RS = ResourceSystem::Instance();

	// 加载Mesh
	std::string MeshAsset = Content.Get("MeshAsset").GetString();
	Meshes_ = RS.Acquire(ResourceType::eMesh, MeshAsset);
	if (!Meshes_) {
		return false;
	}

	// 加载材质
	std::string MaterialAsset = Content.Get("MaterialAsset").GetString();
	Materials_ = RS.Acquire(ResourceType::eMaterial, MaterialAsset);
	if (!Materials_) {
		return false;
	}

	return true;
}