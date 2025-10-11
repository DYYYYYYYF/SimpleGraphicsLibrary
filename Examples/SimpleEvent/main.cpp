#include "Platform/Window/Window.h"
#include "Core/Event.h"
#include "Core/EventManager.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

class EventDemoApp : public EventHandler {
public:
	EventDemoApp() : running_(false), window_("事件系统演示", 800, 600) {}

	bool Initialize() {
		std::cout << "=== 事件系统演示程序 ===" << std::endl;
		std::cout << "初始化窗口和事件系统..." << std::endl;

		// 创建并显示窗口
		if (!window_.Create()) {
			std::cerr << "窗口创建失败!" << std::endl;
			return false;
		}

		// 设置窗口事件回调
		window_.SetEventCallback([this](Event& event) {
			OnWindowEvent(event);
			});

		// 注册全局事件监听器
		SetupEventListeners();

		window_.Show();
		std::cout << "窗口创建成功!" << std::endl;

		running_ = true;
		return true;
	}

	void Run() {
		std::cout << "\n=== 事件处理开始 ===" << std::endl;
		std::cout << "控制说明:" << std::endl;
		std::cout << "  ESC     - 退出程序" << std::endl;
		std::cout << "  SPACE   - 切换事件日志" << std::endl;
		std::cout << "  R       - 重置窗口位置" << std::endl;
		std::cout << "  F1      - 显示帮助信息" << std::endl;
		std::cout << "  鼠标    - 移动和点击测试" << std::endl;
		std::cout << "  其他键  - 键盘事件测试" << std::endl;
		std::cout << "=========================" << std::endl;

		auto& eventManager = EventManager::Instance();
		eventManager.SetLogging(true);

		auto lastTime = std::chrono::steady_clock::now();
		int frameCount = 0;

		while (running_ && !window_.ShouldClose()) {
			// 处理窗口消息
			window_.ProcessMessages();

			// 处理全局事件队列
			eventManager.ProcessEvents();

			// FPS计算和显示
			auto currentTime = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);

			if (elapsed.count() >= 5) {
				float fps = frameCount / static_cast<float>(elapsed.count());
				std::cout << "\n[INFO] 窗口状态: " << window_.GetWidth() << "x" << window_.GetHeight()
					<< " @ (" << window_.GetX() << "," << window_.GetY() << ")"
					<< " | FPS: " << std::fixed << std::setprecision(1) << fps << std::endl;
				frameCount = 0;
				lastTime = currentTime;
			}

			frameCount++;

			// 限制帧率
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		std::cout << "\n=== 程序退出 ===" << std::endl;
	}

	void Shutdown() {
		running_ = false;
		EventManager::Instance().UnsubscribeAll();
		window_.Destroy();
	}

private:
	void OnWindowEvent(Event& event) {
		// 使用事件分发器处理窗口事件
		EventDispatcher dispatcher(event);

		// 分发不同类型的事件到对应的处理函数
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
			e.GetName();
			std::cout << "[WINDOW] 接收到关闭事件，程序将退出" << std::endl;
			running_ = false;
			return true;
			});

		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
			std::cout << "[WINDOW] 窗口大小改变: " << e.GetWidth() << "x" << e.GetHeight() << std::endl;
			return false; // 不阻止其他监听器处理
			});

		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) {
			return OnKeyPressed(e);
			});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) {
			return OnMouseButtonPressed(e);
			});

		dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& e) {
			return OnMouseMoved(e);
			});
	}

	void SetupEventListeners() {
		auto& eventManager = EventManager::Instance();

		// 注册键盘事件监听器
		eventManager.On<KeyReleasedEvent>([this](KeyReleasedEvent& event) {
			OnKeyReleased(event);
			return false;
			});

		// 注册字符输入事件监听器
		eventManager.On<CharInputEvent>([this](CharInputEvent& event) {
			char ch = static_cast<char>(event.GetCharacter());
			if (ch >= 32 && ch <= 126) { // 可打印字符
				std::cout << "[INPUT] 字符输入: '" << ch << "' (ASCII: " << event.GetCharacter() << ")" << std::endl;
			}
			return false;
			});

		// 注册鼠标滚轮事件监听器
		eventManager.On<MouseScrolledEvent>([this](MouseScrolledEvent& event) {
			std::cout << "[MOUSE] 滚轮滚动: X=" << event.GetXOffset()
				<< ", Y=" << event.GetYOffset() << std::endl;
			return false;
			});

		// 注册窗口焦点事件监听器
		eventManager.On<WindowFocusEvent>([this](WindowFocusEvent& event) {
			event.GetName();
			std::cout << "[WINDOW] 窗口获得焦点" << std::endl;
			return false;
			});

		eventManager.On<WindowLostFocusEvent>([this](WindowLostFocusEvent& event) {
			event.GetName();
			std::cout << "[WINDOW] 窗口失去焦点" << std::endl;
			return false;
			});
	}

	bool OnKeyPressed(KeyPressedEvent& event) {
		KeyCode key = event.GetKeyCode();
		const ModifierKeys& mods = event.GetModifiers();

		// 处理特殊按键
		switch (key) {
		case KeyCode::Escape:
			std::cout << "[KEY] ESC键按下 - 退出程序" << std::endl;
			running_ = false;
			return true;

		case KeyCode::Space:
			std::cout << "[KEY] 空格键按下 - 切换事件日志" << std::endl;
			EventManager::Instance().SetLogging(!EventManager::Instance().IsLoggingEnabled());
			return true;

		case KeyCode::R:
			if (mods.control) {
				std::cout << "[KEY] Ctrl+R - 重置窗口位置和大小" << std::endl;
				window_.SetPosition(100, 100);
				window_.SetSize(800, 600);
			}
			else {
				std::cout << "[KEY] R键按下 - 重置窗口位置" << std::endl;
				window_.SetPosition(200, 200);
			}
			return true;

		case KeyCode::F1:
			ShowHelp();
			return true;

		case KeyCode::Enter:
			std::cout << "[KEY] 回车键按下";
			if (mods.alt) {
				std::cout << " (全屏切换功能未实现)";
			}
			std::cout << std::endl;
			return true;

		default:
			// 显示修饰键状态
			std::string modStr = "";
			if (mods.control) modStr += "Ctrl+";
			if (mods.shift) modStr += "Shift+";
			if (mods.alt) modStr += "Alt+";
			if (mods.super) modStr += "Win+";

			std::cout << "[KEY] 按键按下: " << modStr << "KeyCode(" << static_cast<int>(key) << ")";
			if (event.IsRepeat()) {
				std::cout << " (重复)";
			}
			std::cout << std::endl;
			break;
		}

		return false; // 允许其他监听器处理
	}

	void OnKeyReleased(KeyReleasedEvent& event) {
		// 只记录特殊键的释放
		KeyCode key = event.GetKeyCode();
		if (key == KeyCode::LeftShift || key == KeyCode::RightShift ||
			key == KeyCode::LeftControl || key == KeyCode::RightControl ||
			key == KeyCode::LeftAlt || key == KeyCode::RightAlt) {
			std::cout << "[KEY] 修饰键释放: KeyCode(" << static_cast<int>(key) << ")" << std::endl;
		}
	}

	bool OnMouseButtonPressed(MouseButtonPressedEvent& event) {
		std::string buttonName;
		switch (event.GetMouseButton()) {
		case MouseButton::Left: buttonName = "左键"; break;
		case MouseButton::Right: buttonName = "右键"; break;
		case MouseButton::Middle: buttonName = "中键"; break;
		case MouseButton::Button4: buttonName = "侧键4"; break;
		case MouseButton::Button5: buttonName = "侧键5"; break;
		}

		const ModifierKeys& mods = event.GetModifiers();
		std::string modStr = "";
		if (mods.control) modStr += "Ctrl+";
		if (mods.shift) modStr += "Shift+";
		if (mods.alt) modStr += "Alt+";

		std::cout << "[MOUSE] 鼠标" << buttonName << "按下";
		if (!modStr.empty()) {
			std::cout << " (" << modStr.substr(0, modStr.length() - 1) << ")";
		}
		std::cout << std::endl;

		// 右键点击显示菜单信息
		if (event.GetMouseButton() == MouseButton::Right) {
			std::cout << "[MENU] 右键菜单 (未实现)" << std::endl;
		}

		return false;
	}

	bool OnMouseMoved(MouseMovedEvent& event) {
		static auto lastMoveTime = std::chrono::steady_clock::now();
		static int moveCount = 0;

		auto currentTime = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastMoveTime);

		moveCount++;

		// 每500ms或每50次移动显示一次位置信息
		if (elapsed.count() >= 500 || moveCount >= 50) {
			std::cout << "[MOUSE] 位置: (" << std::fixed << std::setprecision(1)
				<< event.GetX() << ", " << event.GetY() << ")" << std::endl;
			lastMoveTime = currentTime;
			moveCount = 0;
		}

		return false;
	}

	void ShowHelp() {
		std::cout << "\n=== 帮助信息 ===" << std::endl;
		std::cout << "这是一个事件系统演示程序，展示了以下功能:" << std::endl;
		std::cout << "1. 窗口事件处理 (关闭、调整大小、移动、焦点)" << std::endl;
		std::cout << "2. 键盘事件处理 (按键按下/释放、字符输入)" << std::endl;
		std::cout << "3. 鼠标事件处理 (按钮点击、移动、滚轮)" << std::endl;
		std::cout << "4. 事件分发和监听器系统" << std::endl;
		std::cout << "5. 修饰键状态检测 (Ctrl, Shift, Alt, Win)" << std::endl;
		std::cout << "\n当前窗口信息:" << std::endl;
		std::cout << "  大小: " << window_.GetWidth() << "x" << window_.GetHeight() << std::endl;
		std::cout << "  位置: (" << window_.GetX() << ", " << window_.GetY() << ")" << std::endl;
		std::cout << "  可见: " << (window_.IsVisible() ? "是" : "否") << std::endl;
		std::cout << "  可调整: " << (window_.IsResizable() ? "是" : "否") << std::endl;
		std::cout << "=================" << std::endl;
	}

private:
	bool running_;
	Window window_;
};

int main() {
	try {
		EventDemoApp app;

		if (!app.Initialize()) {
			std::cerr << "应用初始化失败!" << std::endl;
			return -1;
		}

		app.Run();
		app.Shutdown();

	}
	catch (const std::exception& e) {
		std::cerr << "异常: " << e.what() << std::endl;
		return -1;
	}

	std::cout << "程序正常退出" << std::endl;
	return 0;
}