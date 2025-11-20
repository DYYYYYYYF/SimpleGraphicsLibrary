#include "MeshComponent.h"

#include "Rendering/Renderer/Renderer.h"
#include "Rendering/Resource/IMesh.h"
#include "Rendering/Resource/IMaterial.h"

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

bool MeshComponent::LoadFromFile(const std::string& File) {
	Meshes_ =  Renderer::Instance()->CreateMesh("");
	if (!Meshes_) {
		return false;
	}

	Materials_ = Renderer::Instance()->CreateMaterial("");
	if (!Materials_) {
		return false;
	}

	return true;
}