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

	eventQueue_.push(std::move(event));
}

void EventManager::DispatchEvent(Event& event) {
	if (loggingEnabled_) {
		LogEvent(event);
	}

	DispatchToListeners(event);
}

void EventManager::ProcessEvents() {
	// ��������е������¼�
	while (!eventQueue_.empty()) {
		auto event = std::move(eventQueue_.front());
		eventQueue_.pop();

		if (event) {
			DispatchToListeners(*event);
		}
	}
}

void EventManager::ClearEvents() {
	// ����¼�����
	std::queue<std::unique_ptr<Event>> empty;
	eventQueue_.swap(empty);
}

void EventManager::UnsubscribeAll() {
	listeners_.clear();
}

void EventManager::DispatchToListeners(Event& event) {
	// ��ȡ�¼����Ͷ�Ӧ�ļ�����
	auto eventType = std::type_index(typeid(event));
	auto it = listeners_.find(eventType);

	if (it != listeners_.end()) {
		// �������м�����
		for (auto& listener : it->second) {
			if (listener && listener->OnEvent(event)) {
				// ����������������¼������Ϊ�Ѵ���
				event.SetHandled(true);
				break;
			}
		}
	}

	// ����Ƿ���ͨ�ü���������������¼����ͣ�
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
	// ��ȡ��ǰʱ��
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;

	// ��ʽ��ʱ���
	std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S");
	std::cout << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";

	// ����¼���Ϣ
	std::cout << "EVENT: " << event.ToString();

	// ��ʾ�¼����
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