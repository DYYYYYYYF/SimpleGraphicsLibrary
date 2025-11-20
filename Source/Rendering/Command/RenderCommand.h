#pragma once

#include "DrawCall.h"
#include "Resource/IMesh.h"
#include "Resource/IMaterial.h"

class RenderCommand {
public:
	CommandType Type_;

	RenderCommand(CommandType t) : Type_(t) {}
	virtual ~RenderCommand() = default;
};

// 绘制指令
class DrawIndexedCommand : public RenderCommand {
public:
	DrawCall DrawCall_;

	DrawIndexedCommand(const DrawCall& dc)
		: RenderCommand(CommandType::eDrawIndexed), DrawCall_(dc) {}

	uint32_t IndexCount_;      // 所有API都需要
	uint32_t InstanceCount_;   // 实例化渲染
	uint32_t FirstIndex_;      // 索引偏移
	int32_t VertexOffset_;     // 顶点偏移（Vulkan需要）
	uint32_t FirstInstance_;   // 实例偏移
};

// 设置视口命令
class SetViewportCommand : public RenderCommand {
public:
	int x, y, width, height;

	SetViewportCommand(int x, int y, int w, int h)
		: RenderCommand(CommandType::eSetViewport),
		x(x), y(y), width(w), height(h) {}
};

// 清除命令
class ClearCommand : public RenderCommand {
public:
	FVector4 Color_;
	bool ClearColor_;
	bool ClearDepth_;
	float DepthValue_;

	ClearCommand(const FVector4& c, bool ccolor = true, bool depth = true, float dv = 1.0f)
		: RenderCommand(CommandType::eClear),
		Color_(c), ClearColor_(ccolor), ClearDepth_(depth), DepthValue_(dv) {}
};