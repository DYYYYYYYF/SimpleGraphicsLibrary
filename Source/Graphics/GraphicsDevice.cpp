#include "GraphicsDevice.h"

#ifdef ENGINE_OPENGL_ENABLED
#include "Backend/OpenGL/OpenGLDevice.h"
#endif

#ifdef ENGINE_DX12_ENABLED
#include "Backend/D3D12/D3D12Device.h"
#endif

#ifdef ENGINE_VULKAN_ENABLED
#include "Backend/Vulkan/VulkanDevice.h"
#endif

namespace Engine {
	namespace Graphics {

		GraphicsDevice* GraphicsDevice::s_Instance = nullptr;

		GraphicsDevice* GraphicsDevice::Create(GraphicsBackend backend) {
			if (s_Instance) {
				delete s_Instance;
			}

			switch (backend) {
#ifdef ENGINE_OPENGL_ENABLED
			case GraphicsBackend::OpenGL:
				s_Instance = new OpenGLDevice();
				break;
#endif

#ifdef ENGINE_DX12_ENABLED
			case GraphicsBackend::DirectX12:
				s_Instance = new D3D12Device();
				break;
#endif

#ifdef ENGINE_VULKAN_ENABLED
			case GraphicsBackend::Vulkan:
				s_Instance = new VulkanDevice();
				break;
#endif

			default:
				// 返回nullptr或抛出异常
				return nullptr;
			}

			return s_Instance;
		}

	} // namespace Graphics
} // namespace Engine