#include "Editor.h"
#include "Logger.hpp"

#include "Core/EventManager.h"
#include "Source/RotateCube.h"
#include <Framework/Actors/CameraActor.h>

std::shared_ptr<ACubeActor> Model = nullptr;
std::shared_ptr<ACameraActor> Camera = nullptr;

bool Editor::Initialize()
{
	// 基础设置
	AppName_ = "Editor";

	// 模型
	Model = std::make_shared<ACubeActor>("CubeModel");
	if (!Model) {
		LOG_ERROR << "Create model failed.";
	}

	// 摄像机
	Camera = std::make_shared<ACameraActor>("DefaultCamera");
	if (!Camera) {
		LOG_ERROR << "Create camera failed.";
	}
	Camera->SetActorLocation(FVector(0, 0, -10));

	LOG_INFO << "Editor init successfully.";
	return true;
}

void Editor::InitScene(Scene& Sce) {
	Model->BeginPlay();
	Camera->BeginPlay();

	Sce.AddToScene(Model);
	Sce.AddToScene(Camera);
}

void Editor::Tick(float DeltaTime)
{
	Model->Tick(DeltaTime);
	Camera->Tick(DeltaTime);
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

