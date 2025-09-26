#pragma once
#include "Event.h"
#include <functional>
#include <vector>
#include <queue>
#include <memory>
#include <unordered_map>
#include <typeindex>

// �¼�����������
class EventListener {
public:
	virtual ~EventListener() = default;
	virtual bool OnEvent(Event& event) = 0;
};

// ����ʽ�¼�������
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

// �¼��ַ��� - ���ڴ���ͬ���͵��¼�
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

// ���¼�������
class EventManager {
public:
	EventManager() = default;
	~EventManager() = default;

	// ���ÿ����͸�ֵ
	EventManager(const EventManager&) = delete;
	EventManager& operator=(const EventManager&) = delete;

	// ����ģʽ
	static EventManager& Instance() {
		static EventManager instance;
		return instance;
	}

	// �����¼�
	void PostEvent(std::unique_ptr<Event> event);

	// �����ַ��¼�����������У�
	void DispatchEvent(Event& event);

	// �����¼������е������¼�
	void ProcessEvents();

	// ����¼�����
	void ClearEvents();

	// ע���¼�������
	template<typename EventType>
	void Subscribe(std::function<bool(EventType&)> callback) {
		auto listener = std::make_shared<FunctionEventListener<EventType>>(callback);
		listeners_[std::type_index(typeid(EventType))].push_back(listener);
	}

	// ע���¼������� (lambda�򻯰汾)
	template<typename EventType, typename Func>
	void On(Func&& func) {
		Subscribe<EventType>(std::forward<Func>(func));
	}

	// �Ƴ��ض����͵����м�����
	template<typename EventType>
	void Unsubscribe() {
		listeners_[std::type_index(typeid(EventType))].clear();
	}

	// �Ƴ����м�����
	void UnsubscribeAll();

	// ��ȡ�������¼�����
	size_t GetEventCount() const { return eventQueue_.size(); }

	// ����Ƿ����ض����͵��¼��ڶ�����
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

	// ����/�����¼���־
	void SetLogging(bool enabled) { loggingEnabled_ = enabled; }
	bool IsLoggingEnabled() const { return loggingEnabled_; }

private:
	// �ַ��¼���������
	void DispatchToListeners(Event& event);

	// ��¼�¼���־
	void LogEvent(const Event& event);

private:
	std::queue<std::unique_ptr<Event>> eventQueue_;
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<EventListener>>> listeners_;
	bool loggingEnabled_ = false;
};

// ������ - ���¼�������ע��
#define EVENT_BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// �¼����������� - ���Ա��̳��������¼��������
class EventHandler {
public:
	virtual ~EventHandler() = default;

protected:
	// �������� - ע���¼�����
	template<typename EventType>
	void Subscribe(std::function<bool(EventType&)> callback) {
		EventManager::Instance().Subscribe<EventType>(callback);
	}

	template<typename EventType, typename Func>
	void On(Func&& func) {
		EventManager::Instance().On<EventType>(std::forward<Func>(func));
	}
};