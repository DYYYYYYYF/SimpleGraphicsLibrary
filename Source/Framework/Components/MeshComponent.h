#pragma once

#include "BaseComponent.h"
#include <memory>

class IMesh;
class IMaterial;

class MeshComponent : public BaseComponent {
public:
	MeshComponent();
	MeshComponent(Actor* Owner, const std::string& Name);

public:
	ENGINE_FRAMEWORK_API void Draw();

	bool LoadFromFile(const std::string& File);

private:
	std::shared_ptr<IMesh> Meshes_;
	std::shared_ptr<IMaterial> Materials_;


};