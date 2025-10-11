#pragma once
#ifdef _WIN32

#include "Window/WindowImpl.h"
#include "Core/Event.h"     

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

class WindowsWindowImpl : public WindowImpl {
public:
	WindowsWindowImpl(const std::string& title, uint32_t width, uint32_t height);
	~WindowsWindowImpl() override;

	// WindowImpl接口实现
	bool Create() override;
	void Destroy() override;
	void Show() override;
	void Hide() override;

	void SetTitle(const std::string& title) override;
	void SetSize(uint32_t width, uint32_t height) override;
	void SetPosition(int x, int y) override;
	void SetResizable(bool resizable) override;

	void ProcessMessages() override;
	bool ShouldClose() const override;

	uint32_t GetWidth() const override;
	uint32_t GetHeight() const override;
	int GetX() const override;
	int GetY() const override;
	const std::string& GetTitle() const override;
	bool IsVisible() const override;
	bool IsResizable() const override;

	void* GetNativeHandle() const override;

	// 事件系统集成
	void SetEventCallback(const EventCallbackFn& callback) override;
	void SetEventDispatchMode(bool useGlobalQueue, bool useDirectCallback = true) override;

private:
	// Windows API相关
	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool RegisterWindowClass();
	void UnregisterWindowClass();
	std::wstring StringToWString(const std::string& str);
	std::string WStringToString(const std::wstring& wstr);

	// 事件转换辅助函数
	KeyCode VirtualKeyToKeyCode(WPARAM vkCode);
	MouseButton VirtualButtonToMouseButton(UINT message, WPARAM wParam);
	ModifierKeys GetCurrentModifiers();
	void DispatchEvent(Event& event);
	std::unique_ptr<Event> CreateEventCopy(const Event& event);  // 创建事件副本

private:
	// 窗口属性
	std::string title_;
	uint32_t width_;
	uint32_t height_;
	int x_;
	int y_;
	bool shouldClose_;
	bool isVisible_;
	bool isResizable_;

	// Windows特定成员
	HWND hwnd_;
	HDC hdc_;
	HINSTANCE hInstance_;

	// 事件系统
	EventCallbackFn eventCallback_;
	bool useGlobalEventQueue_;
	bool useDirectCallback_;

	// 静态成员
	static const wchar_t* WINDOW_CLASS_NAME;
	static bool classRegistered_;
	static std::unordered_map<HWND, WindowsWindowImpl*> windowMap_;
};

#endif