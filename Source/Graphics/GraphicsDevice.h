// Graphics/GraphicsDevice.h
#pragma once
#include "GraphicsEnums.h"
#include "GraphicsResource.h"
#include <memory>

namespace Engine {
	namespace Graphics {

		// 前向声明
		class Shader;
		class Texture;
		class VertexBuffer;
		class IndexBuffer;
		class RenderTarget;
		class Pipeline;

		struct GraphicsDeviceDesc {
			GraphicsBackend backend = GraphicsBackend::OpenGL;
			void* windowHandle = nullptr;
			uint32_t width = 1280;
			uint32_t height = 720;
			bool vsync = true;
			bool debugMode = false;
		};

		struct Color {
			float r, g, b, a;

			Color() : r(0), g(0), b(0), a(1) {}
			Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
		};

		struct Viewport {
			float x, y;
			float width, height;
			float minDepth, maxDepth;

			Viewport() : x(0), y(0), width(0), height(0), minDepth(0), maxDepth(1) {}
			Viewport(float x, float y, float w, float h)
				: x(x), y(y), width(w), height(h), minDepth(0), maxDepth(1) {}
		};

		// GraphicsDevice - RHI的核心接口
		class GraphicsDevice {
		public:
			virtual ~GraphicsDevice() = default;

			// 设备管理
			virtual bool Initialize(void* windowHandle, uint32_t width, uint32_t height) = 0;
			virtual void Shutdown() = 0;

			// 静态工厂方法
			static GraphicsDevice* Create(GraphicsBackend backend);
			static GraphicsDevice* GetInstance() { return s_Instance; }

		protected:
			static GraphicsDevice* s_Instance;
		};

		// 类型别名
		using GraphicsDevicePtr = std::shared_ptr<GraphicsDevice>;

	} // namespace Graphics
} // namespace Engine