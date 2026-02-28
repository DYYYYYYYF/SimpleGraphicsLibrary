#pragma once

#include "Actor.h"
#include "Framework/Components/MeshComponent.h"

class AMeshActor : public AActor {
public:
	ENGINE_FRAMEWORK_API AMeshActor(const std::string& name);

public:
	ENGINE_FRAMEWORK_API virtual void BeginPlay() override;
	ENGINE_FRAMEWORK_API virtual void Tick(float DeltaTime) override;

protected:
	UMeshComponent* MeshComponent_;
};