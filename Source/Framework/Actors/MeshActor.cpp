#include "MeshActor.h"
#include "Framework/Components/MeshComponent.h"

MeshActor::MeshActor(const std::string& name) : Actor(name){
	MeshComponent_ = std::make_unique<MeshComponent>(this, "MeshComponent");
	if (!MeshComponent_) { return; }

}

void MeshActor::BeginPlay() {
	MeshComponent_->LoadFromFile("");
}

void MeshActor::Tick(float DeltaTime) {
	MeshComponent_->Draw();
}