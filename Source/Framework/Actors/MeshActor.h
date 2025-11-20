#include "Actor.h"
#include "Framework/Components/MeshComponent.h"

class MeshActor : public Actor {
public:
	ENGINE_FRAMEWORK_API MeshActor(const std::string& name);

public:
	ENGINE_FRAMEWORK_API virtual void BeginPlay() override;
	ENGINE_FRAMEWORK_API virtual void Tick(float DeltaTime) override;

private:
	MeshComponent* MeshComponent_;
};