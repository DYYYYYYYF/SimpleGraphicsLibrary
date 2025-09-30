#pragma once
#include "CommandBuffer.h"
#include "Material.h"
#include <memory>

namespace Engine {
	namespace Components {

		// 渲染器基类 - 类似Unity的Renderer组件
		class Renderer {
		public:
			Renderer();
			virtual ~Renderer();

			// 启用/禁用
			void SetEnabled(bool enabled) { m_Enabled = enabled; }
			bool IsEnabled() const { return m_Enabled; }

			// 材质
			void SetMaterial(Rendering::MaterialPtr material) { m_Material = material; }
			Rendering::MaterialPtr GetMaterial() const { return m_Material; }

			// 渲染层级
			void SetRenderQueue(int32_t queue) { m_RenderQueue = queue; }
			int32_t GetRenderQueue() const { return m_RenderQueue; }

			// 渲染方法（子类实现）
			virtual void Render(Rendering::CommandBuffer& cmd) = 0;

			// 获取边界（用于剔除）
			virtual void GetBounds(float& minX, float& minY, float& minZ,
				float& maxX, float& maxY, float& maxZ) const = 0;

		protected:
			bool m_Enabled;
			Rendering::MaterialPtr m_Material;
			int32_t m_RenderQueue;
		};

		using RendererPtr = std::shared_ptr<Renderer>;

	} // namespace Components
} // namespace Engine