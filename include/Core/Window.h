#pragma once

#include <string>
#include <cstdint>
#include <memory>

class WindowImpl;

class Window {
public:
	Window();
	Window(const std::string& title, uint32_t width, uint32_t height);
	~Window();

	// 禁用拷贝构造和赋值
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	// 允许移动构造和赋值
	Window(Window&& other) noexcept;
	Window& operator=(Window&& other) noexcept;

public:
	// 窗口生命周期管理
	bool Create();
	void Destroy();
	void Show();
	void Hide();

	// 窗口属性设置
	void SetTitle(const std::string& title);
	void SetSize(uint32_t width, uint32_t height);
	void SetPosition(int x, int y);
	void SetResizable(bool resizable);

	// 消息处理和事件循环
	void ProcessMessages();
	bool ShouldClose() const;

	// 属性获取
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	int GetX() const;
	int GetY() const;
	const std::string& GetTitle() const;
	bool IsVisible() const;
	bool IsResizable() const;

	// 平台特定句柄获取（返回void*以保持平台无关性）
	void* GetNativeHandle() const;

protected:
	std::unique_ptr<WindowImpl> Instance;
};