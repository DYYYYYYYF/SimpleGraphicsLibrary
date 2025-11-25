#include "RotateCube.h"

void RotateCubeActor::Tick(float DeltaTime) {
	TransformComponent_->RotateLocalY(DeltaTime);
}