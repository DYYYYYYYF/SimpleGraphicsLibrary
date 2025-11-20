#include "MeshActor.h"

MeshActor::MeshActor(const std::string& name) : Actor(name){
	MeshComponent_ = CreateComponent<MeshComponent>(this, "MeshComponent");
	if (!MeshComponent_) { return; }

	

}

void MeshActor::BeginPlay() {
	MeshComponent_->LoadFromFile("/Builtin/Builtin.json");
}

void MeshActor::Tick(float DeltaTime) {
	(void)DeltaTime;
}