#include "Renderer.h"

#include "Logger.hpp"
#include "Graphics/Backend/OpenGL/OpenGLDevice.h"

Renderer::Renderer() {
	GraphicsDevice_ = nullptr;
}

Renderer::~Renderer() {

}

bool Renderer::Initialize(Window* Win) {

	GraphicsDevice_ = new OpenGLDevice();
	if (!GraphicsDevice_ || !GraphicsDevice_->Initialize(Win, BackendAPI::OpenGL)) {
		LOG_ERROR << "Create graphics device failed!";
		return false;
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
