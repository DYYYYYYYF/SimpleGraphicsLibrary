#include "Editor.h"
#include "Logger.hpp"

#include "Source/RotateCube.h"
#include <Framework/Actors/CameraActor.h>

std::shared_ptr<RotateCubeActor> Model = nullptr;
std::shared_ptr<CameraActor> Camera = nullptr;

bool Editor::Initialize()
{
	AppName_ = "Editor";

	Model = std::make_shared<RotateCubeActor>("Model");
	if (!Model) {
		LOG_ERROR << "Create model failed.";
	}

	Camera = std::make_shared<CameraActor>("Camera");
	if (!Camera) {
		LOG_ERROR << "Create camera failed.";
	}

	Camera->SetActorLocation(FVector(0, 0, -10));

	LOG_INFO << "Editor init successfully.";
	return true;
}

void Editor::InitScene(Scene& Sce) {
	Model->BeginPlay();

	Sce.AddToScene(Model);
	Sce.AddToScene(Camera);
}

void Editor::Tick(float DeltaTime)
{
	Model->Tick(DeltaTime);
}

void Editor::Render()
{
	
}

void Editor::Shutdown()
{
	Model.reset();
	Camera.reset();
	LOG_INFO << "Editor shutdown.";
}

