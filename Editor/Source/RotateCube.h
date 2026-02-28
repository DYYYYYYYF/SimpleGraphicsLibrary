#pragma once

#include "Framework/Actors/CubeActor.h"

class RotateCubeActor : public ACubeActor {
public:
	RotateCubeActor(const std::string& Name) : ACubeActor(Name) {}

public:
	void Tick(float DeltaTime) override;

};