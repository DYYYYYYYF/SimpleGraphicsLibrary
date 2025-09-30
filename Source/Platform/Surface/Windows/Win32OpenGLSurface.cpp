// Graphics/OpenGL/Win32/Win32OpenGLSurface.cpp
#include "Win32OpenGLSurface.h"
#include <glad/glad.h>
#include <iostream>

namespace Engine {
	namespace Platform {

		Win32OpenGLSurface::Win32OpenGLSurface(HWND hwnd, HDC hdc, uint32_t width, uint32_t height)
			: m_Hwnd(hwnd)
			, m_Hdc(hdc)
			, m_GLContext(nullptr)
			, m_Width(width)
			, m_Height(height)
		{
		}

		Win32OpenGLSurface::~Win32OpenGLSurface() {
			Shutdown();
		}

		bool Win32OpenGLSurface::Initialize() {
			// 设置像素格式
			PIXELFORMATDESCRIPTOR pfd = {};
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 24;
			pfd.cStencilBits = 8;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int pixelFormat = ChoosePixelFormat(m_Hdc, &pfd);
			if (!pixelFormat) {
				std::cerr << "Failed to choose pixel format" << std::endl;
				return false;
			}

			if (!SetPixelFormat(m_Hdc, pixelFormat, &pfd)) {
				std::cerr << "Failed to set pixel format" << std::endl;
				return false;
			}

			// 创建临时上下文
			HGLRC tempContext = wglCreateContext(m_Hdc);
			if (!tempContext) {
				std::cerr << "Failed to create temporary OpenGL context" << std::endl;
				return false;
			}

			wglMakeCurrent(m_Hdc, tempContext);

			// 加载GLAD
			if (!gladLoadGL()) {
				std::cerr << "Failed to initialize GLAD" << std::endl;
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(tempContext);
				return false;
			}

			// 创建现代OpenGL上下文（4.5）
			typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
				(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

			HGLRC modernContext = nullptr;
			if (wglCreateContextAttribsARB) {
				const int attribs[] = {
					0x2091, 4,              // WGL_CONTEXT_MAJOR_VERSION_ARB
					0x2092, 5,              // WGL_CONTEXT_MINOR_VERSION_ARB
					0x9126, 0x00000001,     // WGL_CONTEXT_PROFILE_MASK_ARB, CORE
					0
				};

				modernContext = wglCreateContextAttribsARB(m_Hdc, nullptr, attribs);
			}

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tempContext);

			if (!modernContext) {
				std::cerr << "Failed to create modern OpenGL context" << std::endl;
				return false;
			}

			m_GLContext = modernContext;
			wglMakeCurrent(m_Hdc, m_GLContext);

			// 设置VSync
			typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int);
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
				(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			if (wglSwapIntervalEXT) {
				wglSwapIntervalEXT(1);
			}

			std::cout << "Win32 OpenGL Surface initialized" << std::endl;
			return true;
		}

		void Win32OpenGLSurface::Shutdown() {
			if (m_GLContext) {
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(m_GLContext);
				m_GLContext = nullptr;
			}
		}

		void Win32OpenGLSurface::MakeCurrent() {
			if (m_GLContext) {
				wglMakeCurrent(m_Hdc, m_GLContext);
			}
		}

		void Win32OpenGLSurface::SwapBuffers() {
			if (m_Hdc) {
				::SwapBuffers(m_Hdc);
			}
		}

	} // namespace Graphics
} // namespace Engine

#endif // _WIN32