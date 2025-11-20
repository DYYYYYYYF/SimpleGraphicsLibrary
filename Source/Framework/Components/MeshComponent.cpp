#include "MeshComponent.h"

#include "Rendering/Renderer/Renderer.h"
#include "Rendering/Resource/IMesh.h"
#include "Rendering/Resource/IMaterial.h"
#include "Platform/File/JsonObject.h"

MeshComponent::MeshComponent() : BaseComponent() {}
MeshComponent::MeshComponent(Actor* Owner, const std::string& Name) : BaseComponent(Owner, Name) {}

void MeshComponent::Draw(CommandList& CmdList) {
	Actor* Owner = GetOwner();
	if (!Owner) {
		return;
	}

	if (!Materials_) {
		return;
	}

	CmdList.DrawIndexed(Meshes_.get(), Materials_.get(), FMatrix4::Identity(), 6);
}

bool MeshComponent::LoadFromFile(const std::string& FilePath) {
	File MeshSrc("../Assets/Meshes" + FilePath);
	if (!MeshSrc.IsExist()) {
		return false;
	}
	
	JsonObject Content = MeshSrc.ReadBytes();
	std::string MeshAsset = Content.Get("MeshAsset").GetString();
	std::string MaterialAsset = Content.Get("MaterialAsset").GetString();

	Meshes_ =  Renderer::Instance()->CreateMesh(MeshAsset);
	if (!Meshes_) {
		return false;
	}

	Materials_ = Renderer::Instance()->CreateMaterial(MaterialAsset);
	if (!Materials_) {
		return false;
	}

	return true;
}