#pragma once
#include "Event.h"
#include <functional>
#include <vector>
#include <queue>
#include <memory>
#include <unordered_map>
#include <typeindex>

// 事件监听器基类
class EventListener {
public:
	virtual ~EventListener() = default;
	virtual bool OnEvent(Event& event) = 0;
};

// 函数式事件监听器
template<typename T>
class FunctionEventListener : public EventListener {
public:
	using EventFunction = std::function<bool(T&)>;

	FunctionEventListener(EventFunction func) : function_(func) {}

	bool OnEvent(Event& event) override {
		if (event.GetEventType() == T::GetStaticType()) {
			return function_(static_cast<T&>(event));
		}
		return false;
	}

private:
	EventFunction function_;
};

// 事件分发器 - 用于处理不同类型的事件
class EventDispatcher {
public:
	EventDispatcher(Event& event) : event_(event) {}

	template<typename T, typename F>
	bool Dispatch(const F& func) {
		if (event_.GetEventType() == T::GetStaticType()) {
			event_.SetHandled(func(static_cast<T&>(event_)));
			return true;
		}
		return false;
	}

private:
	Event& event_;
};

// 主事件管理器
class EventManager {
public:
	EventManager() = default;
	~EventManager() = default;

	// 禁用拷贝和赋值
	EventManager(const EventManager&) = delete;
	EventManager& operator=(const EventManager&) = delete;

	// 单例模式
	static EventManager& Instance() {
		static EventManager instance;
		return instance;
	}

	// 发布事件
	void PostEvent(std::unique_ptr<Event> event);

	// 立即分发事件（不加入队列）
	void DispatchEvent(Event& event);

	// 处理事件队列中的所有事件
	void ProcessEvents();

	// 清空事件队列
	void ClearEvents();

	// 注册事件监听器
	template<typename EventType>
	void Subscribe(std::function<bool(EventType&)> callback) {
		auto listener = std::make_shared<FunctionEventListener<EventType>>(callback);
		listeners_[std::type_index(typeid(EventType))].push_back(listener);
	}

	// 注册事件监听器 (lambda简化版本)
	template<typename EventType, typename Func>
	void On(Func&& func) {
		Subscribe<EventType>(std::forward<Func>(func));
	}

	// 移除特定类型的所有监听器
	template<typename EventType>
	void Unsubscribe() {
		listeners_[std::type_index(typeid(EventType))].clear();
	}

	// 移除所有监听器
	void UnsubscribeAll();

	// 获取队列中事件数量
	size_t GetEventCount() const { return eventQueue_.size(); }

	// 检查是否有特定类型的事件在队列中
	template<typename EventType>
	bool HasEvent() const {
		std::queue<std::unique_ptr<Event>> tempQueue = eventQueue_;
		while (!tempQueue.empty()) {
			if (tempQueue.front()->GetEventType() == EventType::GetStaticType()) {
				return true;
			}
			tempQueue.pop();
		}
		return false;
	}

	// 启用/禁用事件日志
	void SetLogging(bool enabled) { loggingEnabled_ = enabled; }
	bool IsLoggingEnabled() const { return loggingEnabled_; }

private:
	// 分发事件给监听器
	void DispatchToListeners(Event& event);

	// 记录事件日志
	void LogEvent(const Event& event);

private:
	std::queue<std::unique_ptr<Event>> eventQueue_;
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<EventListener>>> listeners_;
	bool loggingEnabled_ = false;
};

// 便利宏 - 简化事件监听器注册
#define EVENT_BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// 事件处理器基类 - 可以被继承来创建事件处理对象
class EventHandler {
public:
	virtual ~EventHandler() = default;

protected:
	// 便利函数 - 注册事件监听
	template<typename EventType>
	void Subscribe(std::function<bool(EventType&)> callback) {
		EventManager::Instance().Subscribe<EventType>(callback);
	}

	template<typename EventType, typename Func>
	void On(Func&& func) {
		EventManager::Instance().On<EventType>(std::forward<Func>(func));
	}
};