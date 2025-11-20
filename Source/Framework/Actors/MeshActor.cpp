#include "MeshActor.h"

MeshActor::MeshActor(const std::string& name) : Actor(name){
	MeshComponent_ = AddComponent<MeshComponent>(this, "MeshComponent");
	if (!MeshComponent_) { return; }

	

}

void MeshActor::BeginPlay() {
	MeshComponent_->LoadFromFile("");
}

void MeshActor::Tick(float DeltaTime) {
	
}