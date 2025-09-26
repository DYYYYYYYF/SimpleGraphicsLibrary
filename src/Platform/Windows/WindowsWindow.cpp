#ifdef _WIN32

#include "../../Core/WindowImpl.h"
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

private:
	// Windows API相关
	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool RegisterWindowClass();
	void UnregisterWindowClass();
	std::wstring StringToWString(const std::string& str);
	std::string WStringToString(const std::wstring& wstr);

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
	, hwnd_(nullptr), hdc_(nullptr), hInstance_(GetModuleHandle(nullptr)) {
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

// 处理消息
void WindowsWindowImpl::ProcessMessages() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
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
	case WM_CLOSE:
		shouldClose_ = true;
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		width_ = LOWORD(lParam);
		height_ = HIWORD(lParam);
		std::cout << "Window resized to: " << width_ << "x" << height_ << std::endl;
		return 0;

	case WM_MOVE:
		x_ = static_cast<int>(static_cast<short>(LOWORD(lParam)));
		y_ = static_cast<int>(static_cast<short>(HIWORD(lParam)));
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// 简单的绘制示例 - 填充背景色
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
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