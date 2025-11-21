#include "Editor.h"
#include "Logger.hpp"

#include <Framework/Actors/MeshActor.h>

std::shared_ptr<MeshActor> Model = nullptr;

bool Editor::Initialize()
{
	AppName_ = "Editor";

	Model = std::make_shared<MeshActor>("Model");
	if (!Model) {
		LOG_ERROR << "Create model failed.";
	}

	LOG_INFO << "Editor init successfully.";
	return true;
}

void Editor::InitScene(Scene& Sce) {
	Model->BeginPlay();

	Sce.AddToScene(Model);
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
	LOG_INFO << "Editor shutdown.";
}

