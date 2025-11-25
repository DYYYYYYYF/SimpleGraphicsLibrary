#include "CameraActor.h"

CameraActor::CameraActor(const std::string& name) : Actor(name) {
	CameraComponent_ = CreateComponent<CameraComponent>(this, "CameraComponent");
	if (!CameraComponent_) { return; }
}

void CameraActor::BeginPlay() {

}

void CameraActor::Tick(float DeltaTime) {
	(void)DeltaTime;
}
