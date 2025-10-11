#include "Core/EventManager.h"
#include <iostream>
#include <chrono>
#include <iomanip>

void EventManager::PostEvent(std::unique_ptr<Event> event) {
	if (!event) {
		return;
	}

	if (loggingEnabled_) {
		LogEvent(*event);
	}

	MutexGuard Lock(Mutex_);
	eventQueue_.push(std::move(event));
}

void EventManager::DispatchEvent(Event& event) {
	if (loggingEnabled_) {
		LogEvent(event);
	}

	DispatchToListeners(event);
}

void EventManager::ProcessEvents() {
	// 处理队列中的所有事件
	while (!eventQueue_.empty()) {
		MutexGuard Lock(Mutex_);
		auto event = std::move(eventQueue_.front());
		eventQueue_.pop();

		if (event) {
			DispatchToListeners(*event);
		}
	}
}

void EventManager::ClearEvents() {
	// 清空事件队列
	std::queue<std::unique_ptr<Event>> empty;
	MutexGuard Lock(Mutex_);
	eventQueue_.swap(empty);
}

void EventManager::UnsubscribeAll() {
	listeners_.clear();
}

void EventManager::DispatchToListeners(Event& event) {
	// 获取事件类型对应的监听器
	auto eventType = std::type_index(typeid(event));
	auto it = listeners_.find(eventType);

	if (it != listeners_.end()) {
		// 遍历所有监听器
		for (auto& listener : it->second) {
			if (listener && listener->OnEvent(event)) {
				// 如果监听器处理了事件，标记为已处理
				event.SetHandled(true);
				break;
			}
		}
	}

	// 检查是否有通用监听器（针对所有事件类型）
	auto allEventsIt = listeners_.find(std::type_index(typeid(Event)));
	if (allEventsIt != listeners_.end()) {
		for (auto& listener : allEventsIt->second) {
			if (listener && listener->OnEvent(event)) {
				event.SetHandled(true);
				break;
			}
		}
	}
}

void EventManager::LogEvent(const Event& event) {
	// 获取当前时间
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;

	// 格式化时间戳
	std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S");
	std::cout << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";

	// 输出事件信息
	std::cout << "EVENT: " << event.ToString();

	// 显示事件类别
	std::cout << " [";
	bool first = true;
	uint32_t categories = event.GetCategoryFlags();

	if (categories & static_cast<uint32_t>(EventCategory::Application)) {
		if (!first) std::cout << "|";
		std::cout << "App";
		first = false;
	}
	if (categories & static_cast<uint32_t>(EventCategory::Input)) {
		if (!first) std::cout << "|";
		std::cout << "Input";
		first = false;
	}
	if (categories & static_cast<uint32_t>(EventCategory::Keyboard)) {
		if (!first) std::cout << "|";
		std::cout << "Keyboard";
		first = false;
	}
	if (categories & static_cast<uint32_t>(EventCategory::Mouse)) {
		if (!first) std::cout << "|";
		std::cout << "Mouse";
		first = false;
	}
	if (categories & static_cast<uint32_t>(EventCategory::Window)) {
		if (!first) std::cout << "|";
		std::cout << "Window";
		first = false;
	}

	std::cout << "]" << std::endl;
}