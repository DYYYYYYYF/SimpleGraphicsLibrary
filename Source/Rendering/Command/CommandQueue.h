#pragma once

#include "CommandList.h"
#include "Core/Mutex.h"

class CommandQueue {
private:
	std::vector<std::unique_ptr<CommandList>> CommandLists_;
	Mutex Mutex_;

public:
	// 提交命令列表
	void Submit(std::unique_ptr<CommandList> commandList) {
		MutexGuard lock(Mutex_);
		CommandLists_.push_back(std::move(commandList));
	}

	// 获取统计信息
	size_t GetTotalDrawCalls() const {
		size_t total = 0;
		for (const auto& cmdList : CommandLists_) {
			total += cmdList->GetDrawCallCount();
		}
		return total;
	}
};