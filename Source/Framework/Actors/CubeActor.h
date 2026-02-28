#pragma once

#include "MeshActor.h"

class ACubeActor : public AMeshActor {
public:
	ENGINE_FRAMEWORK_API ACubeActor(const std::string& name);

public:
	ENGINE_FRAMEWORK_API virtual void BeginPlay() override;
	ENGINE_FRAMEWORK_API virtual void Tick(float DeltaTime) override;

};