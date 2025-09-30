// Graphics/RenderSurface.h
#pragma once
#include "Graphics/GraphicsEnums.h"
#include <memory>

namespace Engine {
	namespace Platform {

		// 渲染表面抽象接口
		class Surface {
		public:
			virtual ~Surface() = default;

			virtual bool Initialize() = 0;
			virtual void Shutdown() = 0;
			virtual void MakeCurrent() = 0;
			virtual void SwapBuffers() = 0;

			virtual void* GetNativeHandle() const = 0;
			virtual Graphics::GraphicsBackend GetBackend() const = 0;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;
		};

		using SurfacePtr = std::shared_ptr<Surface>;

	} // namespace Platform
} // namespace Engine