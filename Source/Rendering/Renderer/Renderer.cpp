#include "Renderer.h"

#include "Logger.hpp"
#include "Graphics/Backend/OpenGL/GLDevice.h"
#include "Rendering/Resource/IMesh.h"

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
	case BackendAPI::eOpenGL:
	{
		LOG_INFO << "Backend API Type: OpenGL.";
		GraphicsDevice_ = std::make_unique<GLDevice>();
		if (!GraphicsDevice_ || !GraphicsDevice_->Initialize(Win)) {
			LOG_ERROR << "Create graphics device failed!";
			return false;
		}
	} break;
	default:
	{
		LOG_ERROR << "Create graphics device failed! Must select a type for backend api type";

	} return false;
	}

	GlobalRenderer = this;
	return true;
}

void Renderer::Draw() {
	GraphicsDevice_->Draw();
}

void Renderer::Destroy() {
	if (GraphicsDevice_) {
		GraphicsDevice_->Destroy();
	}
}

std::shared_ptr<IMesh> Renderer::CreateMesh(const std::string& Mesh) {
	std::shared_ptr<IMesh> NewMesh = GraphicsDevice_->CreateMesh();
	return NewMesh;
}

std::shared_ptr<IMaterial> Renderer::CreateMaterial(const std::string& Mesh) {
	std::shared_ptr<IMaterial> NewMaterial = GraphicsDevice_->CreateMaterial();
	return NewMaterial;
}
