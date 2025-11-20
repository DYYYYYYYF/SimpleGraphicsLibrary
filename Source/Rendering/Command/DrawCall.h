#pragma once

#include "Core/BaseMath.h"
#include <memory>

enum class CommandType : uint8_t {
	eDraw,               // 绘制几何体
	eDrawIndexed,        // 绘制索引几何体
	eDrawInstanced,      // 实例化绘制
	eSetPipeline,        // 设置渲染管线
	eSetVertexBuffer,    // 设置顶点缓冲
	eSetIndexBuffer,     // 设置索引缓冲
	eSetViewport,        // 设置视口
	eSetScissor,         // 设置裁剪矩形
	eClear,  // 清除渲染目标
	eBeginRenderPass,    // 开始渲染通道
	eEndRenderPass,      // 结束渲染通道
	eSetUniform,         // 设置Uniform
	eBindTexture,        // 绑定纹理
};

struct DrawCall {
	// 绘制参数
	uint32_t vertexCount = 0;
	uint32_t instanceCount = 1;
	uint32_t firstVertex = 0;
	uint32_t firstInstance = 0;

	// 索引绘制参数
	uint32_t indexCount = 0;
	uint32_t firstIndex = 0;
	int32_t vertexOffset = 0;

	// 资源引用（轻量级句柄）
	struct {
		void* mesh;           // Mesh指针或句柄
		void* material;       // Material指针或句柄
		void* pipeline;       // Pipeline状态对象
	} resources;

	// 变换矩阵
	FMatrix4 modelMatrix;

	// 排序键（用于状态排序）
	uint64_t sortKey = 0;

	// 生成排序键（用于减少状态切换）
	void GenerateSortKey(float depth = 0.0f) {
		// 排序键组成：
		// [63-48]: Material ID (16位)
		// [47-32]: Pipeline ID (16位)
		// [31-16]: Mesh ID (16位)
		// [15-0]:  深度值 (16位)

		uint64_t materialID = reinterpret_cast<uintptr_t>(resources.material) & 0xFFFF;
		uint64_t pipelineID = reinterpret_cast<uintptr_t>(resources.pipeline) & 0xFFFF;
		uint64_t meshID = reinterpret_cast<uintptr_t>(resources.mesh) & 0xFFFF;
		uint16_t depthBits = static_cast<uint16_t>(depth * 65535.0f);

		sortKey = (materialID << 48) | (pipelineID << 32) | (meshID << 16) | depthBits;
	}
};