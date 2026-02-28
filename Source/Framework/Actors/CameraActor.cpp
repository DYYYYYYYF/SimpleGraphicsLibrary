#include "CameraActor.h"

ACameraActor::ACameraActor(const std::string& name) : AActor(name) {
	CameraComponent_ = CreateComponent<UCameraComponent>(this, "CameraComponent");
	if (!CameraComponent_) { return; }
}

void ACameraActor::BeginPlay() {

}

void ACameraActor::Tick(float DeltaTime) {
	(void)DeltaTime;
}
