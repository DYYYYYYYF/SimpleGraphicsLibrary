#include "RotateCube.h"

void ARotateCubeActor::Tick(float DeltaTime) {
	RotateDegress(Axis::Y, DeltaTime * 30.0f);
}