#include "CubeActor.h"

ACubeActor::ACubeActor(const std::string& name) : AMeshActor(name) {}

void ACubeActor::BeginPlay() {
	MeshComponent_->LoadFromFile("/Builtin/Builtin.json");
}

void ACubeActor::Tick(float DeltaTime) {
	(void)DeltaTime;
}