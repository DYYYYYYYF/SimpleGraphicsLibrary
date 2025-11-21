#include "Renderer.h"

#include "Graphics/Backend/OpenGL/GLDevice.h"
#include "Rendering/Resource/IMesh.h"
#include "Command/CommandList.h"
#include "Framework/Components/MeshComponent.h"
#include "Framework/Actors/Actor.h"

#include <Logger.hpp>

Renderer::Renderer() {
	GraphicsDevice_ = nullptr;
}

Renderer::~Renderer() {
	
}

Renderer* Renderer::GlobalRenderer = nullptr;
Renderer* Renderer::Instance() {
	if (!GlobalRenderer) {
		return nullptr;
	}

	return GlobalRenderer;
}

bool Renderer::Initialize(Window* Win, BackendAPI Type) {
	switch (Type)
	{
#ifdef ENGINE_OPENGL_ENABLED
	case BackendAPI::eOpenGL:
	{
		LOG_INFO << "Backend API Type: OpenGL.";
		GraphicsDevice_ = std::make_unique<GLDevice>();
		if (!GraphicsDevice_ || !GraphicsDevice_->Initialize(Win)) {
			LOG_ERROR << "Create graphics device failed!";
			return false;
		}
	} break;
#endif
	default:
	{
		LOG_ERROR << "Create graphics device failed! Must select a type for backend api type";

	} return false;
	}

	GlobalRenderer = this;
	return true;
}

void Renderer::BeginCommand(CommandList& CmdList) {
	CmdList.Begin();

	// TODO: 拆分清除指令
	CmdList.Clear(FVector4(0.2f, 0.3f, 0.3f, 1.0f));

}

void Renderer::Draw() {
	CommandList CmdList;
	CmdList.Begin();

	// 清除指令
	CmdList.Clear(FVector4(0.2f, 0.3f, 0.3f, 1.0f));



	GraphicsDevice_->ExecuteCommandList(CmdList);
}

void Renderer::DrawScene(CommandList& CmdList) {
	GraphicsDevice_->ExecuteCommandList(CmdList);
}

void Renderer::EndCommand(CommandList& CmdList) {
	CmdList.End();
}

void Renderer::Destroy() {
	if (GraphicsDevice_) {
		GraphicsDevice_->Destroy();
	}

	LOG_INFO << "Renderer destroyed.";
}

std::shared_ptr<IMesh> Renderer::CreateMesh(const std::string& AssetPath) {
	return GraphicsDevice_->CreateMesh(AssetPath);
}

std::shared_ptr<IMaterial> Renderer::CreateMaterial(const std::string& AssetPath) {
	std::shared_ptr<IMaterial> NewMaterial = GraphicsDevice_->CreateMaterial(AssetPath);
	AllMaterials.push_back(NewMaterial);
	return NewMaterial;
}
