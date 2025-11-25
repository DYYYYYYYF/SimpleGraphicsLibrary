#pragma once

#include "BaseComponent.h"
#include <memory>
#include "Rendering/Command/CommandList.h"

class IMesh;
class IMaterial;

class MeshComponent : public BaseComponent {
public:
	MeshComponent();
	MeshComponent(Actor* Owner, const std::string& Name);
	virtual ~MeshComponent();

public:
	ENGINE_FRAMEWORK_API void Draw(CommandList& CmdList);

	ENGINE_FRAMEWORK_API bool LoadFromFile(const std::string& FilePath);

	ENGINE_FRAMEWORK_API std::shared_ptr<IMesh> GetMesh() { return DynamicCast<IMesh>(MeshAsset_); }
	ENGINE_FRAMEWORK_API void SetMesh(std::shared_ptr<IResource> Mesh) {
		if (MeshAsset_) MeshAsset_.reset();
		MeshAsset_ = DynamicCast<IMesh>(MeshAsset_); 
	}

private:
	std::shared_ptr<IMesh> MeshAsset_;

};