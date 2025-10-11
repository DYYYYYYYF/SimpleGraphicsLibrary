#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <functional>

#include "PlatformMoudleAPI.h"

// 前向声明
class Event;

// 事件回调函数类型
using EventCallbackFn = std::function<void(Event&)>;

// WindowImpl 抽象基类 - 只在实现文件中使用，不暴露给用户
class ENGINE_PLATFORM_API WindowImpl {
public:
	virtual ~WindowImpl() = default;

	// 纯虚函数接口 - 所有平台都必须实现
	virtual bool Create() = 0;
	virtual void Destroy() = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;

	virtual void SetTitle(const std::string& title) = 0;
	virtual void SetSize(uint32_t width, uint32_t height) = 0;
	virtual void SetPosition(int x, int y) = 0;
	virtual void SetResizable(bool resizable) = 0;

	virtual void ProcessMessages() = 0;
	virtual bool ShouldClose() const = 0;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual int GetX() const = 0;
	virtual int GetY() const = 0;
	virtual const std::string& GetTitle() const = 0;
	virtual bool IsVisible() const = 0;
	virtual bool IsResizable() const = 0;

	virtual void* GetNativeHandle() const = 0;

	// 事件系统集成
	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
	virtual void SetEventDispatchMode(bool useGlobalQueue, bool useDirectCallback = true) = 0;
};

// 平台特定工厂函数声明 - 在各自的平台文件中实现
#ifdef _WIN32
std::unique_ptr<WindowImpl> CreateWindowsWindow(const std::string& title, uint32_t width, uint32_t height);
#elif defined(__linux__)
std::unique_ptr<WindowImpl> CreateLinuxWindow(const std::string& title, uint32_t width, uint32_t height);
#elif defined(__APPLE__)
std::unique_ptr<WindowImpl> CreateMacWindow(const std::string& title, uint32_t width, uint32_t height);
#endif