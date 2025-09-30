// Graphics/Texture.h
#pragma once
#include "GraphicsResource.h"

namespace Engine {
	namespace Graphics {

		// Texture - RHI纹理接口
		class Texture : public GraphicsResource {
		public:
			virtual ~Texture() = default;

			// 纹理信息
			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;
			virtual TextureFormat GetFormat() const = 0;

			// 数据操作
			virtual void SetData(const void* data, uint32_t size) = 0;
			virtual void GetData(void* data, uint32_t size) = 0;

			// 绑定（立即执行）
			virtual void Bind(uint32_t slot = 0) = 0;
			virtual void Unbind() = 0;

			// 纹理参数
			virtual void SetFilter(TextureFilter minFilter, TextureFilter magFilter) = 0;
			virtual void SetWrap(TextureWrap wrapS, TextureWrap wrapT) = 0;
		};

		using TexturePtr = std::shared_ptr<Texture>;

	} // namespace Graphics
} // namespace Engine