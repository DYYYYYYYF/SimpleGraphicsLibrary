#pragma once
#include "GraphicsEnums.h"
#include <memory>
#include <string>

namespace Engine {
	namespace Graphics {

		// 资源基类
		class GraphicsResource {
		public:
			virtual ~GraphicsResource() = default;

			uint32_t GetID() const { return m_ResourceID; }
			bool IsValid() const { return m_ResourceID != 0; }

		protected:
			GraphicsResource() : m_ResourceID(0) {}
			uint32_t m_ResourceID;
		};

	} // namespace Graphics
} // namespace Engine