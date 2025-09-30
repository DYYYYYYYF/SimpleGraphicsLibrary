// Graphics/OpenGL/OpenGLDevice.cpp
#include "OpenGLDevice.h"
#include <iostream>
#include <cstring>

namespace Engine {
	namespace Graphics {

		OpenGLDevice::OpenGLDevice()
			: m_WindowHandle(nullptr)
			, m_Width(0)
			, m_Height(0)
			, m_DebugMode(false)
			, m_BoundVAO(0)
			, m_BoundProgram(0)
			, m_BoundFBO(0)
		{
			memset(m_BoundTextures, 0, sizeof(m_BoundTextures));
		}

		OpenGLDevice::~OpenGLDevice() {
			Shutdown();
		}

		bool OpenGLDevice::Initialize(void* windowHandle, uint32_t width, uint32_t height) {
			m_WindowHandle = windowHandle;
			m_Width = width;
			m_Height = height;

			// OpenGL上下文已由窗口创建，这里只需要加载函数
			if (!gladLoadGL()) {
				std::cerr << "GLAD failed to load OpenGL functions" << std::endl;
				return false;
			}

			// 检查OpenGL版本
			if (GLVersion.major < 4) {
				std::cerr << "OpenGL 4.0 or higher required" << std::endl;
				return false;
			}

			// 打印OpenGL信息
			LogGLInfo();

			// 设置调试输出
			if (m_DebugMode) {
				SetupDebugOutput();
			}

			// 设置默认OpenGL状态
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			// 设置视口
			Viewport viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
			SetViewport(viewport);

			return true;
		}

		void OpenGLDevice::LogGLInfo() {
			std::cout << "=== OpenGL Information ===" << std::endl;
			std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
			std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
			std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
			std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
			std::cout << "GLAD Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;
			std::cout << "=========================" << std::endl;
		}

		void OpenGLDevice::SetupDebugOutput() {
			if (GLAD_GL_ARB_debug_output || GLAD_GL_KHR_debug) {
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

				glDebugMessageCallback([](GLenum source, GLenum type, GLuint id,
					GLenum severity, GLsizei length,
					const GLchar* message, const void* userParam) {
						const char* severityStr = "UNKNOWN";
						switch (severity) {
						case GL_DEBUG_SEVERITY_HIGH:         severityStr = "HIGH"; break;
						case GL_DEBUG_SEVERITY_MEDIUM:       severityStr = "MEDIUM"; break;
						case GL_DEBUG_SEVERITY_LOW:          severityStr = "LOW"; break;
						case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "INFO"; break;
						}

						if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
							std::cerr << "[OpenGL " << severityStr << "] " << message << std::endl;
						}
					}, nullptr);

				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
					GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
			}
		}

		void OpenGLDevice::Shutdown() {
			// OpenGL上下文由窗口管理，这里只清理OpenGL资源
			m_WindowHandle = nullptr;
		}

		void OpenGLDevice::BeginFrame() {
			// 可以在这里重置状态
		}

		void OpenGLDevice::EndFrame() {
			glFlush();
		}

		void OpenGLDevice::Clear(const Color& color, float depth, uint8_t stencil) {
			glClearColor(color.r, color.g, color.b, color.a);
			glClearDepth(depth);
			glClearStencil(stencil);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void OpenGLDevice::SetViewport(const Viewport& viewport) {
			glViewport(static_cast<GLint>(viewport.x),
				static_cast<GLint>(viewport.y),
				static_cast<GLsizei>(viewport.width),
				static_cast<GLsizei>(viewport.height));
			glDepthRange(viewport.minDepth, viewport.maxDepth);
		}

		void OpenGLDevice::Present() {
			// 实际的Present由窗口实现调用SwapBuffers
		}

		void OpenGLDevice::Draw(uint32_t vertexCount, uint32_t startVertex) {
			glDrawArrays(GL_TRIANGLES, startVertex, vertexCount);
		}

		void OpenGLDevice::DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) {
			if (baseVertex == 0) {
				glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT,
					(void*)(startIndex * sizeof(uint32_t)));
			}
			else {
				glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT,
					(void*)(startIndex * sizeof(uint32_t)), baseVertex);
			}
		}

		void OpenGLDevice::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount) {
			glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, instanceCount);
		}

		void OpenGLDevice::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount) {
			glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
		}

		void OpenGLDevice::SetDepthTest(bool enable) {
			if (enable) {
				glEnable(GL_DEPTH_TEST);
			}
			else {
				glDisable(GL_DEPTH_TEST);
			}
		}

		void OpenGLDevice::SetBlend(bool enable) {
			if (enable) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else {
				glDisable(GL_BLEND);
			}
		}

		void OpenGLDevice::SetCullFace(bool enable, bool cullBack) {
			if (enable) {
				glEnable(GL_CULL_FACE);
				glCullFace(cullBack ? GL_BACK : GL_FRONT);
			}
			else {
				glDisable(GL_CULL_FACE);
			}
		}

		void OpenGLDevice::SetWireframe(bool enable) {
			glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
		}

		void OpenGLDevice::BindShader(GLuint program) {
			if (m_BoundProgram != program) {
				glUseProgram(program);
				m_BoundProgram = program;
			}
		}

		void OpenGLDevice::BindVertexArray(GLuint vao) {
			if (m_BoundVAO != vao) {
				glBindVertexArray(vao);
				m_BoundVAO = vao;
			}
		}

		void OpenGLDevice::BindTexture(GLuint texture, uint32_t slot) {
			if (slot < 16 && m_BoundTextures[slot] != texture) {
				glActiveTexture(GL_TEXTURE0 + slot);
				glBindTexture(GL_TEXTURE_2D, texture);
				m_BoundTextures[slot] = texture;
			}
		}

		void OpenGLDevice::BindFramebuffer(GLuint fbo) {
			if (m_BoundFBO != fbo) {
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				m_BoundFBO = fbo;
			}
		}

	} // namespace Graphics
} // namespace Engine