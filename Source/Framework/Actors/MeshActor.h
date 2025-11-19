#include "Actor.h"

class MeshComponent;

class MeshActor : public Actor {
public:
	ENGINE_FRAMEWORK_API MeshActor(const std::string& name);

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	std::unique_ptr<MeshComponent> MeshComponent_;
};