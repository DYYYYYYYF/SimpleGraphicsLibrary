// Graphics/OpenGL/OpenGLDevice.h
#pragma once
#include "GraphicsDevice.h"
#include <glad.h>

namespace Engine {
	namespace Graphics {

		class OpenGLDevice : public GraphicsDevice {
		public:
			OpenGLDevice();
			virtual ~OpenGLDevice();

			// 实现基类接口
			bool Initialize(void* windowHandle, uint32_t width, uint32_t height) override;
			void Shutdown() override;

			// 渲染控制
			void BeginFrame();
			void EndFrame();
			void Clear(const Color& color, float depth = 1.0f, uint8_t stencil = 0);
			void SetViewport(const Viewport& viewport);
			void Present();

			// 绘制命令
			void Draw(uint32_t vertexCount, uint32_t startVertex = 0);
			void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0, uint32_t baseVertex = 0);
			void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount);
			void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount);

			// 状态管理
			void SetDepthTest(bool enable);
			void SetBlend(bool enable);
			void SetCullFace(bool enable, bool cullBack = true);
			void SetWireframe(bool enable);

			// 资源绑定
			void BindShader(GLuint program);
			void BindVertexArray(GLuint vao);
			void BindTexture(GLuint texture, uint32_t slot = 0);
			void BindFramebuffer(GLuint fbo);

			// 获取信息
			uint32_t GetWidth() const { return m_Width; }
			uint32_t GetHeight() const { return m_Height; }

		private:
			void LogGLInfo();
			void SetupDebugOutput();

			void* m_WindowHandle;
			uint32_t m_Width;
			uint32_t m_Height;
			bool m_DebugMode;

			// 状态缓存
			GLuint m_BoundVAO;
			GLuint m_BoundProgram;
			GLuint m_BoundFBO;
			GLuint m_BoundTextures[16];
		};

	} // namespace Graphics
} // namespace Engine