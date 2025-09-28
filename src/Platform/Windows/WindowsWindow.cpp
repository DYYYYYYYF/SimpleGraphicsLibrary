#ifdef _WIN32

#include "../../Core/WindowImpl.h"  // 修正包含路径
#include "Core/Event.h"       // 包含事件定义
#include "Core/EventManager.h" // 包含事件管理器
#include <iostream>
#include <stdexcept>
#include <unordered_map>

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

// 静态成员定义
const wchar_t* WindowsWindowImpl::WINDOW_CLASS_NAME = L"SimpleGraphicsLibraryWindow";
bool WindowsWindowImpl::classRegistered_ = false;
std::unordered_map<HWND, WindowsWindowImpl*> WindowsWindowImpl::windowMap_;

// 构造函数
WindowsWindowImpl::WindowsWindowImpl(const std::string& title, uint32_t width, uint32_t height)
	: title_(title), width_(width), height_(height), x_(CW_USEDEFAULT), y_(CW_USEDEFAULT)
	, shouldClose_(false), isVisible_(false), isResizable_(true)
	, hwnd_(nullptr), hdc_(nullptr), hInstance_(GetModuleHandle(nullptr))
	, useGlobalEventQueue_(true), useDirectCallback_(true) {  // 默认同时使用两种方式
}

// 析构函数
WindowsWindowImpl::~WindowsWindowImpl() {
	Destroy();
}

// 创建窗口
bool WindowsWindowImpl::Create() {
	if (hwnd_) {
		std::cout << "Window already created" << std::endl;
		return true;
	}

	// 注册窗口类
	if (!RegisterWindowClass()) {
		std::cerr << "Failed to register window class" << std::endl;
		return false;
	}

	// 计算窗口大小（包含边框）
	RECT windowRect = { 0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_) };
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	if (!isResizable_) {
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}

	AdjustWindowRect(&windowRect, windowStyle, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// 转换标题为宽字符
	std::wstring wTitle = StringToWString(title_);

	// 创建窗口
	hwnd_ = CreateWindowExW(
		0,                          // 扩展样式
		WINDOW_CLASS_NAME,          // 窗口类名
		wTitle.c_str(),            // 窗口标题
		windowStyle,               // 窗口样式
		x_,                        // X位置
		y_,                        // Y位置
		windowWidth,               // 宽度
		windowHeight,              // 高度
		nullptr,                   // 父窗口
		nullptr,                   // 菜单
		hInstance_,                // 实例句柄
		this                       // 传递this指针
	);

	if (!hwnd_) {
		DWORD error = GetLastError();
		std::cerr << "Failed to create window. Error: " << error << std::endl;
		return false;
	}

	// 获取设备上下文
	hdc_ = GetDC(hwnd_);
	if (!hdc_) {
		std::cerr << "Failed to get device context" << std::endl;
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
		return false;
	}

	// 将窗口句柄和实例关联
	windowMap_[hwnd_] = this;

	std::cout << "Window created successfully: " << title_ << " (" << width_ << "x" << height_ << ")" << std::endl;
	return true;
}

// 销毁窗口
void WindowsWindowImpl::Destroy() {
	if (hwnd_) {
		// 从映射中移除
		windowMap_.erase(hwnd_);

		// 释放设备上下文
		if (hdc_) {
			ReleaseDC(hwnd_, hdc_);
			hdc_ = nullptr;
		}

		// 销毁窗口
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;

		std::cout << "Window destroyed: " << title_ << std::endl;
	}
}

// 显示窗口
void WindowsWindowImpl::Show() {
	if (hwnd_) {
		ShowWindow(hwnd_, SW_SHOW);
		UpdateWindow(hwnd_);
		isVisible_ = true;
		std::cout << "Window shown: " << title_ << std::endl;
	}
}

// 隐藏窗口
void WindowsWindowImpl::Hide() {
	if (hwnd_) {
		ShowWindow(hwnd_, SW_HIDE);
		isVisible_ = false;
		std::cout << "Window hidden: " << title_ << std::endl;
	}
}

// 设置窗口标题
void WindowsWindowImpl::SetTitle(const std::string& title) {
	title_ = title;
	if (hwnd_) {
		std::wstring wTitle = StringToWString(title);
		SetWindowTextW(hwnd_, wTitle.c_str());
	}
}

// 设置窗口大小
void WindowsWindowImpl::SetSize(uint32_t width, uint32_t height) {
	width_ = width;
	height_ = height;

	if (hwnd_) {
		// 计算包含边框的窗口大小
		RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		DWORD windowStyle = static_cast<DWORD>(GetWindowLong(hwnd_, GWL_STYLE));
		AdjustWindowRect(&windowRect, windowStyle, FALSE);

		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		SetWindowPos(hwnd_, nullptr, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOZORDER);
	}
}

// 设置窗口位置
void WindowsWindowImpl::SetPosition(int x, int y) {
	x_ = x;
	y_ = y;

	if (hwnd_) {
		SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
}

// 设置是否可调整大小
void WindowsWindowImpl::SetResizable(bool resizable) {
	isResizable_ = resizable;

	if (hwnd_) {
		DWORD currentStyle = static_cast<DWORD>(GetWindowLong(hwnd_, GWL_STYLE));
		DWORD newStyle;

		if (resizable) {
			newStyle = WS_OVERLAPPEDWINDOW;
		}
		else {
			newStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		}

		SetWindowLong(hwnd_, GWL_STYLE, static_cast<LONG>(newStyle));
		SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	}
}

// 事件系统集成
void WindowsWindowImpl::SetEventCallback(const EventCallbackFn& callback) {
	eventCallback_ = callback;
}

// 设置事件分发模式
void WindowsWindowImpl::SetEventDispatchMode(bool useGlobalQueue, bool useDirectCallback) {
	useGlobalEventQueue_ = useGlobalQueue;
	useDirectCallback_ = useDirectCallback;
}

// 处理消息
void WindowsWindowImpl::ProcessMessages() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

// 事件分发辅助函数 - 修改为同时支持两种分发方式
void WindowsWindowImpl::DispatchEvent(Event& event) {
	// 直接回调 (立即处理)
	if (useDirectCallback_ && eventCallback_) {
		eventCallback_(event);
	}

	// 放入全局事件队列 (延迟处理)
	if (useGlobalEventQueue_) {
		// 创建事件的副本并放入队列
		std::unique_ptr<Event> eventCopy = CreateEventCopy(event);
		if (eventCopy) {
			EventManager::Instance().PostEvent(std::move(eventCopy));
		}
	}
}

// 创建事件副本的辅助函数
std::unique_ptr<Event> WindowsWindowImpl::CreateEventCopy(const Event& event) {
	switch (event.GetEventType()) {
	case EventType::WindowClose:
		return std::make_unique<WindowCloseEvent>();

	case EventType::WindowResize: {
		const auto& e = static_cast<const WindowResizeEvent&>(event);
		return std::make_unique<WindowResizeEvent>(e.GetWidth(), e.GetHeight());
	}

	case EventType::WindowMove: {
		const auto& e = static_cast<const WindowMoveEvent&>(event);
		return std::make_unique<WindowMoveEvent>(e.GetX(), e.GetY());
	}

	case EventType::WindowFocus:
		return std::make_unique<WindowFocusEvent>();

	case EventType::WindowLostFocus:
		return std::make_unique<WindowLostFocusEvent>();

	case EventType::KeyPressed: {
		const auto& e = static_cast<const KeyPressedEvent&>(event);
		return std::make_unique<KeyPressedEvent>(e.GetKeyCode(), e.GetModifiers(), e.IsRepeat());
	}

	case EventType::KeyReleased: {
		const auto& e = static_cast<const KeyReleasedEvent&>(event);
		return std::make_unique<KeyReleasedEvent>(e.GetKeyCode(), e.GetModifiers());
	}

	case EventType::CharInput: {
		const auto& e = static_cast<const CharInputEvent&>(event);
		return std::make_unique<CharInputEvent>(e.GetCharacter());
	}

	case EventType::MouseButtonPressed: {
		const auto& e = static_cast<const MouseButtonPressedEvent&>(event);
		return std::make_unique<MouseButtonPressedEvent>(e.GetMouseButton(), e.GetModifiers());
	}

	case EventType::MouseButtonReleased: {
		const auto& e = static_cast<const MouseButtonReleasedEvent&>(event);
		return std::make_unique<MouseButtonReleasedEvent>(e.GetMouseButton(), e.GetModifiers());
	}

	case EventType::MouseMoved: {
		const auto& e = static_cast<const MouseMovedEvent&>(event);
		return std::make_unique<MouseMovedEvent>(e.GetX(), e.GetY());
	}

	case EventType::MouseScrolled: {
		const auto& e = static_cast<const MouseScrolledEvent&>(event);
		return std::make_unique<MouseScrolledEvent>(e.GetXOffset(), e.GetYOffset());
	}

	default:
		return nullptr;
	}
}

// 虚拟键码转换为我们的KeyCode
KeyCode WindowsWindowImpl::VirtualKeyToKeyCode(WPARAM vkCode) {
	switch (vkCode) {
		// 字母键
	case 'A': return KeyCode::A; case 'B': return KeyCode::B; case 'C': return KeyCode::C;
	case 'D': return KeyCode::D; case 'E': return KeyCode::E; case 'F': return KeyCode::F;
	case 'G': return KeyCode::G; case 'H': return KeyCode::H; case 'I': return KeyCode::I;
	case 'J': return KeyCode::J; case 'K': return KeyCode::K; case 'L': return KeyCode::L;
	case 'M': return KeyCode::M; case 'N': return KeyCode::N; case 'O': return KeyCode::O;
	case 'P': return KeyCode::P; case 'Q': return KeyCode::Q; case 'R': return KeyCode::R;
	case 'S': return KeyCode::S; case 'T': return KeyCode::T; case 'U': return KeyCode::U;
	case 'V': return KeyCode::V; case 'W': return KeyCode::W; case 'X': return KeyCode::X;
	case 'Y': return KeyCode::Y; case 'Z': return KeyCode::Z;

		// 数字键
	case '0': return KeyCode::D0; case '1': return KeyCode::D1; case '2': return KeyCode::D2;
	case '3': return KeyCode::D3; case '4': return KeyCode::D4; case '5': return KeyCode::D5;
	case '6': return KeyCode::D6; case '7': return KeyCode::D7; case '8': return KeyCode::D8;
	case '9': return KeyCode::D9;

		// 功能键
	case VK_F1: return KeyCode::F1;   case VK_F2: return KeyCode::F2;
	case VK_F3: return KeyCode::F3;   case VK_F4: return KeyCode::F4;
	case VK_F5: return KeyCode::F5;   case VK_F6: return KeyCode::F6;
	case VK_F7: return KeyCode::F7;   case VK_F8: return KeyCode::F8;
	case VK_F9: return KeyCode::F9;   case VK_F10: return KeyCode::F10;
	case VK_F11: return KeyCode::F11; case VK_F12: return KeyCode::F12;

		// 特殊键
	case VK_SPACE: return KeyCode::Space;
	case VK_RETURN: return KeyCode::Enter;
	case VK_ESCAPE: return KeyCode::Escape;
	case VK_TAB: return KeyCode::Tab;
	case VK_BACK: return KeyCode::Backspace;
	case VK_DELETE: return KeyCode::Delete;
	case VK_INSERT: return KeyCode::Insert;
	case VK_HOME: return KeyCode::Home;
	case VK_END: return KeyCode::End;
	case VK_PRIOR: return KeyCode::PageUp;
	case VK_NEXT: return KeyCode::PageDown;

		// 箭头键
	case VK_LEFT: return KeyCode::Left;
	case VK_UP: return KeyCode::Up;
	case VK_RIGHT: return KeyCode::Right;
	case VK_DOWN: return KeyCode::Down;

		// 修饰键
	case VK_LSHIFT: return KeyCode::LeftShift;
	case VK_RSHIFT: return KeyCode::RightShift;
	case VK_LCONTROL: return KeyCode::LeftControl;
	case VK_RCONTROL: return KeyCode::RightControl;
	case VK_LMENU: return KeyCode::LeftAlt;
	case VK_RMENU: return KeyCode::RightAlt;

		// 小键盘
	case VK_NUMPAD0: return KeyCode::NumPad0; case VK_NUMPAD1: return KeyCode::NumPad1;
	case VK_NUMPAD2: return KeyCode::NumPad2; case VK_NUMPAD3: return KeyCode::NumPad3;
	case VK_NUMPAD4: return KeyCode::NumPad4; case VK_NUMPAD5: return KeyCode::NumPad5;
	case VK_NUMPAD6: return KeyCode::NumPad6; case VK_NUMPAD7: return KeyCode::NumPad7;
	case VK_NUMPAD8: return KeyCode::NumPad8; case VK_NUMPAD9: return KeyCode::NumPad9;

		// 其他键
	case VK_CAPITAL: return KeyCode::CapsLock;
	case VK_NUMLOCK: return KeyCode::NumLock;
	case VK_SCROLL: return KeyCode::ScrollLock;
	case VK_SNAPSHOT: return KeyCode::PrintScreen;
	case VK_PAUSE: return KeyCode::Pause;

	default: return KeyCode::A; // 默认返回A键，实际应用中可以定义Unknown键
	}
}

// 鼠标按钮转换
MouseButton WindowsWindowImpl::VirtualButtonToMouseButton(UINT message, WPARAM wParam) {
	switch (message) {
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		return MouseButton::Left;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		return MouseButton::Right;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		return MouseButton::Middle;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		return (HIWORD(wParam) == XBUTTON1) ? MouseButton::Button4 : MouseButton::Button5;
	default:
		return MouseButton::Left;
	}
}

// 获取当前修饰键状态
ModifierKeys WindowsWindowImpl::GetCurrentModifiers() {
	ModifierKeys modifiers;
	modifiers.shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	modifiers.control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	modifiers.alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	modifiers.super = (GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0;
	return modifiers;
}

// 属性获取函数
bool WindowsWindowImpl::ShouldClose() const {
	return shouldClose_;
}

uint32_t WindowsWindowImpl::GetWidth() const {
	if (hwnd_) {
		RECT clientRect;
		GetClientRect(hwnd_, &clientRect);
		return static_cast<uint32_t>(clientRect.right - clientRect.left);
	}
	return width_;
}

uint32_t WindowsWindowImpl::GetHeight() const {
	if (hwnd_) {
		RECT clientRect;
		GetClientRect(hwnd_, &clientRect);
		return static_cast<uint32_t>(clientRect.bottom - clientRect.top);
	}
	return height_;
}

int WindowsWindowImpl::GetX() const {
	if (hwnd_) {
		RECT windowRect;
		GetWindowRect(hwnd_, &windowRect);
		return windowRect.left;
	}
	return x_;
}

int WindowsWindowImpl::GetY() const {
	if (hwnd_) {
		RECT windowRect;
		GetWindowRect(hwnd_, &windowRect);
		return windowRect.top;
	}
	return y_;
}

const std::string& WindowsWindowImpl::GetTitle() const {
	return title_;
}

bool WindowsWindowImpl::IsVisible() const {
	if (hwnd_) {
		return IsWindowVisible(hwnd_) != FALSE;
	}
	return isVisible_;
}

bool WindowsWindowImpl::IsResizable() const {
	return isResizable_;
}

void* WindowsWindowImpl::GetNativeHandle() const {
	return static_cast<void*>(hwnd_);
}

// 静态窗口过程
LRESULT CALLBACK WindowsWindowImpl::StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WindowsWindowImpl* window = nullptr;

	if (uMsg == WM_NCCREATE) {
		// 窗口创建时，从lParam获取this指针
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = static_cast<WindowsWindowImpl*>(createStruct->lpCreateParams);
		windowMap_[hwnd] = window;
	}
	else {
		// 从映射中查找窗口实例
		auto it = windowMap_.find(hwnd);
		if (it != windowMap_.end()) {
			window = it->second;
		}
	}

	if (window) {
		return window->WindowProc(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 实例窗口过程
LRESULT WindowsWindowImpl::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE: {
		shouldClose_ = true;
		WindowCloseEvent event;
		DispatchEvent(event);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE: {
		uint32_t newWidth = LOWORD(lParam);
		uint32_t newHeight = HIWORD(lParam);

		if (newWidth != width_ || newHeight != height_) {
			width_ = newWidth;
			height_ = newHeight;

			WindowResizeEvent event(width_, height_);
			DispatchEvent(event);
		}
		return 0;
	}

	case WM_MOVE: {
		int newX = static_cast<int>(static_cast<short>(LOWORD(lParam)));
		int newY = static_cast<int>(static_cast<short>(HIWORD(lParam)));

		if (newX != x_ || newY != y_) {
			x_ = newX;
			y_ = newY;

			WindowMoveEvent event(x_, y_);
			DispatchEvent(event);
		}
		return 0;
	}

	case WM_SETFOCUS: {
		WindowFocusEvent event;
		DispatchEvent(event);
		return 0;
	}

	case WM_KILLFOCUS: {
		WindowLostFocusEvent event;
		DispatchEvent(event);
		return 0;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		KeyCode keyCode = VirtualKeyToKeyCode(wParam);
		ModifierKeys modifiers = GetCurrentModifiers();
		bool isRepeat = (lParam & 0x40000000) != 0;

		KeyPressedEvent event(keyCode, modifiers, isRepeat);
		DispatchEvent(event);
		return 0;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP: {
		KeyCode keyCode = VirtualKeyToKeyCode(wParam);
		ModifierKeys modifiers = GetCurrentModifiers();

		KeyReleasedEvent event(keyCode, modifiers);
		DispatchEvent(event);
		return 0;
	}

	case WM_CHAR: {
		uint32_t character = static_cast<uint32_t>(wParam);

		// 过滤掉控制字符
		if (character >= 32 && character != 127) {
			CharInputEvent event(character);
			DispatchEvent(event);
		}
		return 0;
	}

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN: {
		MouseButton button = VirtualButtonToMouseButton(uMsg, wParam);
		ModifierKeys modifiers = GetCurrentModifiers();

		MouseButtonPressedEvent event(button, modifiers);
		DispatchEvent(event);

		// 捕获鼠标以接收鼠标释放事件
		SetCapture(hwnd);
		return (uMsg == WM_XBUTTONDOWN) ? TRUE : 0;
	}

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP: {
		MouseButton button = VirtualButtonToMouseButton(uMsg, wParam);
		ModifierKeys modifiers = GetCurrentModifiers();

		MouseButtonReleasedEvent event(button, modifiers);
		DispatchEvent(event);

		// 释放鼠标捕获
		ReleaseCapture();
		return (uMsg == WM_XBUTTONUP) ? TRUE : 0;
	}

	case WM_MOUSEMOVE: {
		float x = static_cast<float>(static_cast<short>(LOWORD(lParam)));
		float y = static_cast<float>(static_cast<short>(HIWORD(lParam)));

		MouseMovedEvent event(x, y);
		DispatchEvent(event);
		return 0;
	}

	case WM_MOUSEWHEEL: {
		float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;

		MouseScrolledEvent event(0.0f, delta);
		DispatchEvent(event);
		return 0;
	}

	case WM_MOUSEHWHEEL: {
		float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;

		MouseScrolledEvent event(delta, 0.0f);
		DispatchEvent(event);
		return 0;
	}

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// 简单的绘制示例 - 填充背景色
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		HBRUSH brush = CreateSolidBrush(RGB(50, 50, 100));
		FillRect(hdc, &clientRect, brush);
		DeleteObject(brush);

		EndPaint(hwnd, &ps);
		return 0;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

// 注册窗口类
bool WindowsWindowImpl::RegisterWindowClass() {
	if (classRegistered_) {
		return true;
	}

	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = StaticWindowProc;
	wc.hInstance = hInstance_;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassExW(&wc)) {
		DWORD error = GetLastError();
		if (error != ERROR_CLASS_ALREADY_EXISTS) {
			std::cerr << "Failed to register window class. Error: " << error << std::endl;
			return false;
		}
	}

	classRegistered_ = true;
	return true;
}

// 字符串转换辅助函数
std::wstring WindowsWindowImpl::StringToWString(const std::string& str) {
	if (str.empty()) return std::wstring();

	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::wstring result(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
	return result;
}

std::string WindowsWindowImpl::WStringToString(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();

	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string result(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
	return result;
}

// 实现Windows平台特定的工厂函数
std::unique_ptr<WindowImpl> CreateWindowsWindow(const std::string& title, uint32_t width, uint32_t height) {
	return std::make_unique<WindowsWindowImpl>(title, width, height);
}

#endif // _WIN32