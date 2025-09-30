// Graphics/Buffer.h
#pragma once
#include "GraphicsResource.h"

namespace Engine {
	namespace Graphics {

		// Buffer - RHI缓冲区基类
		class Buffer : public GraphicsResource {
		public:
			virtual ~Buffer() = default;

			virtual uint32_t GetSize() const = 0;
			virtual BufferUsage GetUsage() const = 0;

			// 数据操作
			virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
			virtual void* Map() = 0;
			virtual void Unmap() = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

		// VertexBuffer - 顶点缓冲
		class VertexBuffer : public Buffer {
		public:
			virtual ~VertexBuffer() = default;
		};

		// IndexBuffer - 索引缓冲
		class IndexBuffer : public Buffer {
		public:
			virtual ~IndexBuffer() = default;

			virtual uint32_t GetIndexCount() const = 0;
			virtual IndexType GetIndexType() const = 0;
		};

		using BufferPtr = std::shared_ptr<Buffer>;
		using VertexBufferPtr = std::shared_ptr<VertexBuffer>;
		using IndexBufferPtr = std::shared_ptr<IndexBuffer>;

	} // namespace Graphics
} // namespace Engine