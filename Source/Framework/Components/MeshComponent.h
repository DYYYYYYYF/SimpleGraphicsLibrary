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

	ENGINE_FRAMEWORK_API std::shared_ptr<IMesh> GetMesh() { return std::dynamic_pointer_cast<IMesh>(Meshes_); }
	ENGINE_FRAMEWORK_API std::shared_ptr<IMaterial> GetMaterial() { return std::dynamic_pointer_cast<IMaterial>(Materials_); }

private:
	std::shared_ptr<IResource> Meshes_;
	std::shared_ptr<IResource> Materials_;


};