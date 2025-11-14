#include "Renderer.h"

#include "Logger.hpp"
#include "Graphics/Backend/OpenGL/OpenGLDevice.h"

Renderer::Renderer() {
	GraphicsDevice_ = nullptr;
}

Renderer::~Renderer() {

}

bool Renderer::Initialize(Window* Win, BackendAPI Type) {
	switch (Type)
	{
	case BackendAPI::eOpenGL:
	{
		LOG_INFO << "Backend API Type: OpenGL.";
		GraphicsDevice_ = new OpenGLDevice();
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
