#pragma once
#include "../OpenGLSurface.h"

#ifdef _WIN32
#include <windows.h>

namespace Engine {
	namespace Platform {

		class Win32OpenGLSurface : public OpenGLSurface {
		public:
			Win32OpenGLSurface(HWND hwnd, HDC hdc, uint32_t width, uint32_t height);
			~Win32OpenGLSurface() override;

			bool Initialize() override;
			void Shutdown() override;
			void MakeCurrent() override;
			void SwapBuffers() override;

			void* GetNativeHandle() const override { return m_GLContext; }
			uint32_t GetWidth() const override { return m_Width; }
			uint32_t GetHeight() const override { return m_Height; }

		private:
			HWND m_Hwnd;
			HDC m_Hdc;
			HGLRC m_GLContext;
			uint32_t m_Width;
			uint32_t m_Height;
		};

	} // namespace Graphics
} // namespace Engine