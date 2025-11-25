#pragma once

#include "Actor.h"
#include "Framework/Components/CameraComponent.h"

class CameraActor : public Actor {
public:
	ENGINE_FRAMEWORK_API CameraActor(const std::string& name);

public:
	ENGINE_FRAMEWORK_API virtual void BeginPlay() override;
	ENGINE_FRAMEWORK_API virtual void Tick(float DeltaTime) override;

private:
	CameraComponent* CameraComponent_;
};