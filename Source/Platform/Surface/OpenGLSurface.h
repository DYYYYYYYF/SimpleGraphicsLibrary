// Graphics/OpenGL/OpenGLSurface.h
#pragma once
#include "RenderSurface.h"

namespace Engine {
	namespace Platform {

		class OpenGLSurface : public Surface {
		public:
			virtual ~OpenGLSurface() = default;

			bool Initialize() override = 0;
			void Shutdown() override = 0;
			void MakeCurrent() override = 0;
			void SwapBuffers() override = 0;

			void* GetNativeHandle() const override = 0;
			GraphicsBackend GetBackend() const override { return GraphicsBackend::OpenGL; }

			uint32_t GetWidth() const override = 0;
			uint32_t GetHeight() const override = 0;
		};

	} 
} 