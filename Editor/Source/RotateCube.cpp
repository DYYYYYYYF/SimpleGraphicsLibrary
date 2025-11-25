#include "RotateCube.h"

void RotateCubeActor::Tick(float DeltaTime) {
	RotateDegress(Axis::Y, DeltaTime * 30.0f);
}