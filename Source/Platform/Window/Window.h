#pragma once

#include "Core/Event.h"
#include "PlatformMoudleAPI.h"

#include <string>
#include <cstdint>
#include <memory>

class WindowImpl;

class Window {
public:
	ENGINE_PLATFORM_API Window();
	ENGINE_PLATFORM_API Window(const std::string& title, uint32_t width, uint32_t height);
	ENGINE_PLATFORM_API ~Window();

	// 禁用拷贝构造和赋值
	ENGINE_PLATFORM_API Window(const Window&) = delete;
	ENGINE_PLATFORM_API Window& operator=(const Window&) = delete;

	// 允许移动构造和赋值
	ENGINE_PLATFORM_API Window(Window&& other) noexcept;
	ENGINE_PLATFORM_API Window& operator=(Window&& other) noexcept;

public:
	// 窗口生命周期管理
	ENGINE_PLATFORM_API bool Create();
	ENGINE_PLATFORM_API void Destroy();
	ENGINE_PLATFORM_API void Show();
	ENGINE_PLATFORM_API void Hide();

	// 窗口属性设置
	ENGINE_PLATFORM_API void SetTitle(const std::string& title);
	ENGINE_PLATFORM_API void SetSize(uint32_t width, uint32_t height);
	ENGINE_PLATFORM_API void SetPosition(int x, int y);
	ENGINE_PLATFORM_API void SetResizable(bool resizable);

	// 消息处理和事件循环
	ENGINE_PLATFORM_API void ProcessMessages();
	ENGINE_PLATFORM_API bool ShouldClose() const;

	// 事件系统集成
	ENGINE_PLATFORM_API void SetEventCallback(const EventCallbackFn& callback);

	// 属性获取
	ENGINE_PLATFORM_API uint32_t GetWidth() const;
	ENGINE_PLATFORM_API uint32_t GetHeight() const;
	ENGINE_PLATFORM_API int GetX() const;
	ENGINE_PLATFORM_API int GetY() const;
	ENGINE_PLATFORM_API const std::string& GetTitle() const;
	ENGINE_PLATFORM_API bool IsVisible() const;
	ENGINE_PLATFORM_API bool IsResizable() const;

	// 平台特定句柄获取（返回void*以保持平台无关性）
	ENGINE_PLATFORM_API void* GetNativeHandle() const;

protected:
	std::unique_ptr<WindowImpl> Instance;
};