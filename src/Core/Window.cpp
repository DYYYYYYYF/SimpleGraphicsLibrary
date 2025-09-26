#include "Core/Window.h"
#include "WindowImpl.h"  // 包含内部实现头文件
#include <iostream>
#include <stdexcept>

// 平台特定的工厂函数实现 - 选择正确的平台实现
std::unique_ptr<WindowImpl> CreatePlatformWindow(const std::string& title, uint32_t width, uint32_t height) {
#ifdef _WIN32
	return CreateWindowsWindow(title, width, height);
#elif defined(__linux__)
	return CreateLinuxWindow(title, width, height);
#elif defined(__APPLE__)
	return CreateMacWindow(title, width, height);
#else
#error "Unsupported platform"
#endif
}

// Window类实现
Window::Window() : Window("Default Window", 800, 600) {
}

Window::Window(const std::string& title, uint32_t width, uint32_t height)
	: Instance(CreatePlatformWindow(title, width, height)) {
	if (!Instance) {
		throw std::runtime_error("Failed to create platform window implementation");
	}
}

Window::~Window() {
	if (Instance) {
		Instance->Destroy();
	}
}

// 移动构造函数
Window::Window(Window&& other) noexcept : Instance(std::move(other.Instance)) {
}

// 移动赋值运算符
Window& Window::operator=(Window&& other) noexcept {
	if (this != &other) {
		Instance = std::move(other.Instance);
	}
	return *this;
}

// 窗口操作函数 - 转发到平台特定实现
bool Window::Create() {
	return Instance ? Instance->Create() : false;
}

void Window::Destroy() {
	if (Instance) {
		Instance->Destroy();
	}
}

void Window::Show() {
	if (Instance) {
		Instance->Show();
	}
}

void Window::Hide() {
	if (Instance) {
		Instance->Hide();
	}
}

void Window::SetTitle(const std::string& title) {
	if (Instance) {
		Instance->SetTitle(title);
	}
}

void Window::SetSize(uint32_t width, uint32_t height) {
	if (Instance) {
		Instance->SetSize(width, height);
	}
}

void Window::SetPosition(int x, int y) {
	if (Instance) {
		Instance->SetPosition(x, y);
	}
}

void Window::SetResizable(bool resizable) {
	if (Instance) {
		Instance->SetResizable(resizable);
	}
}

void Window::ProcessMessages() {
	if (Instance) {
		Instance->ProcessMessages();
	}
}

bool Window::ShouldClose() const {
	return Instance ? Instance->ShouldClose() : true;
}

// 属性获取函数
uint32_t Window::GetWidth() const {
	return Instance ? Instance->GetWidth() : 0;
}

uint32_t Window::GetHeight() const {
	return Instance ? Instance->GetHeight() : 0;
}

int Window::GetX() const {
	return Instance ? Instance->GetX() : 0;
}

int Window::GetY() const {
	return Instance ? Instance->GetY() : 0;
}

const std::string& Window::GetTitle() const {
	static const std::string empty;
	return Instance ? Instance->GetTitle() : empty;
}

bool Window::IsVisible() const {
	return Instance ? Instance->IsVisible() : false;
}

bool Window::IsResizable() const {
	return Instance ? Instance->IsResizable() : false;
}

void* Window::GetNativeHandle() const {
	return Instance ? Instance->GetNativeHandle() : nullptr;
}