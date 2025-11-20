#pragma once

#include "RenderCommand.h"

class CommandList {
public:
	CommandList() = default;

	// 禁止拷贝，允许移动
	CommandList(const CommandList&) = delete;
	CommandList& operator=(const CommandList&) = delete;
	CommandList(CommandList&&) = default;
	CommandList& operator=(CommandList&&) = default;

	// 添加绘制调用
	void Draw(const DrawCall& drawCall) {
		if (!IsRecording_) return;

		DrawCalls_.push_back(drawCall);
		Commands_.push_back(std::make_unique<DrawIndexedCommand>(drawCall));
		IsSorted_ = false;
	}

	// 添加索引绘制
	void DrawIndexed(IMesh* Mesh, IMaterial* Material,
		const FMatrix& modelMatrix,
		uint32_t indexCount, uint32_t firstIndex = 0) {
		DrawCall dc;
		dc.indexCount = indexCount;
		dc.firstIndex = firstIndex;
		dc.instanceCount = 1;
		dc.resources.mesh = Mesh;
		dc.resources.material = Material;
		dc.modelMatrix = modelMatrix;
		dc.GenerateSortKey();

		Draw(dc);
	}

	// 添加实例化绘制
	void DrawInstanced(IMesh* Mesh, IMaterial* Material,
		const std::vector<FMatrix4>& instanceMatrices) {
		if (!IsRecording_) return;

		// 批量添加实例
		for (const auto& matrix : instanceMatrices) {
			DrawCall dc;
			dc.indexCount = Mesh->GetIndexCount();
			dc.instanceCount = 1;
			dc.resources.mesh = Mesh;
			dc.resources.material = Material;
			dc.modelMatrix = matrix;
			dc.GenerateSortKey();

			DrawCalls_.push_back(dc);
		}
	}

	// 设置视口
	void SetViewport(int x, int y, int width, int height) {
		if (!IsRecording_) return;
		Commands_.push_back(std::make_unique<SetViewportCommand>(x, y, width, height));
	}

	// 清除渲染目标
	void Clear(const FVector4& Color, bool ClearColor = true,
		bool clearDepth = true, float depth = 1.0f) {
		if (!IsRecording_) return;
		Commands_.push_back(std::make_unique<ClearCommand>(Color, ClearColor, clearDepth, depth));
	}

	// 开始记录
	void Begin() {
		Commands_.clear();
		DrawCalls_.clear();
		IsRecording_ = true;
		IsSorted_ = false;
	}

	// 结束记录
	void End() {
		IsRecording_ = false;
	}

	// 重置（保留内存）
	void Reset() {
		Commands_.clear();
		DrawCalls_.clear();
		IsRecording_ = true;
		IsSorted_ = false;
	}

	// 排序DrawCall（减少状态切换）
	void Sort() {
		if (IsSorted_) return;

		// 按sortKey排序（Material > Pipeline > Mesh > Depth）
		std::sort(DrawCalls_.begin(), DrawCalls_.end(),
			[](const DrawCall& a, const DrawCall& b) {
				return a.sortKey < b.sortKey;
			});

		// 重建命令列表（仅包含DrawCommand）
		Commands_.clear();
		for (const auto& dc : DrawCalls_) {
			Commands_.push_back(std::make_unique<DrawIndexedCommand>(dc));
		}

		IsSorted_ = true;
	}

	// 获取DrawCall数量
	size_t GetDrawCallCount() const {
		return DrawCalls_.size();
	}

	// 获取DrawCall列表（用于统计/调试）
	const std::vector<DrawCall>& GetDrawCalls() const {
		return DrawCalls_;
	}

	const std::vector<std::unique_ptr<RenderCommand>>& GetCommands() const { return Commands_; }

private:
	std::vector<std::unique_ptr<RenderCommand>> Commands_;
	std::vector<DrawCall> DrawCalls_;  // 专门存储DrawCall用于批处理

	bool IsSorted_ = false;
	bool IsRecording_ = true;

};
