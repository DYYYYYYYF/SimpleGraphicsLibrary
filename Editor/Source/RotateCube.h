#pragma once

#include "Framework/Actors/MeshActor.h"

class RotateCubeActor : public MeshActor {
public:
	RotateCubeActor(const std::string& Name) : MeshActor(Name) {}

public:
	void Tick(float DeltaTime) override;

};