#pragma once

#include "Framework/Actors/CubeActor.h"

class ARotateCubeActor : public ACubeActor {
public:
	ARotateCubeActor(const std::string& Name) : ACubeActor(Name) {}

public:
	void Tick(float DeltaTime) override;

};