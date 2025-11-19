#include "Editor.h"
#include "Logger.hpp"

#include <Framework/Actors/MeshActor.h>

MeshActor* Model = nullptr;

bool Editor::Initialize()
{
	AppName_ = "Editor";

	Model = new MeshActor("Model");
	if (!Model) {
		LOG_ERROR << "Create model failed.";
	}

	LOG_INFO << "Editor init successfully.";
	return true;
}

void Editor::InitScene() {
	Model->BeginPlay();
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
	LOG_INFO << "Editor shutdown.";
}

