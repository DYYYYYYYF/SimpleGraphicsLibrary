#include "MeshActor.h"

AMeshActor::AMeshActor(const std::string& name) : AActor(name){
	MeshComponent_ = CreateComponent<UMeshComponent>(this, "MeshComponent");
	if (!MeshComponent_) { return; }

}

void AMeshActor::BeginPlay() {}

void AMeshActor::Tick(float DeltaTime) {
	(void)DeltaTime;
}