#include "CameraActor.h"

CameraActor::CameraActor(const std::string& name) : AActor(name) {
	CameraComponent_ = CreateComponent<UCameraComponent>(this, "CameraComponent");
	if (!CameraComponent_) { return; }
}

void CameraActor::BeginPlay() {

}

void CameraActor::Tick(float DeltaTime) {
	(void)DeltaTime;
}
