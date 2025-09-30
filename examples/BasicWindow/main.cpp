#include "Platform/Window/Window.h"
#include <iostream>
#include <chrono>
#include <thread>

int TestSingleWindows();
int TestMultipleWindows();

int main() {
	// 可选：测试单/多窗口功能
	TestSingleWindows();
	//TestMultipleWindows();

	return 0;
}

int TestSingleWindows() {
	try {
		std::cout << "=== 跨平台窗口系统测试 ===" << std::endl;

		// 创建窗口
		Window window("Simple Graphics Library - Test Window", 800, 600);

		// 创建窗口
		if (!window.Create()) {
			std::cerr << "Failed to create window!" << std::endl;
			return -1;
		}

		// 显示窗口
		window.Show();

		std::cout << "Window created and shown successfully!" << std::endl;
		std::cout << "Window info:" << std::endl;
		std::cout << "  Title: " << window.GetTitle() << std::endl;
		std::cout << "  Size: " << window.GetWidth() << "x" << window.GetHeight() << std::endl;
		std::cout << "  Position: (" << window.GetX() << ", " << window.GetY() << ")" << std::endl;
		std::cout << "  Visible: " << (window.IsVisible() ? "Yes" : "No") << std::endl;
		std::cout << "  Resizable: " << (window.IsResizable() ? "Yes" : "No") << std::endl;

		std::cout << "\n=== 开始消息循环 ===" << std::endl;
		std::cout << "关闭窗口来退出程序" << std::endl;

		// 主消息循环
		auto lastTime = std::chrono::steady_clock::now();
		int frameCount = 0;

		while (!window.ShouldClose()) {
			// 处理窗口消息
			window.ProcessMessages();

			// FPS计算
			auto currentTime = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);

			if (elapsed.count() >= 1) {
				std::cout << "FPS: " << frameCount << " | Window size: "
					<< window.GetWidth() << "x" << window.GetHeight() << std::endl;
				frameCount = 0;
				lastTime = currentTime;
			}

			frameCount++;

			// 限制FPS到60
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		std::cout << "\n=== 清理资源 ===" << std::endl;
		window.Destroy();

	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return -1;
	}

	std::cout << "Program finished successfully!" << std::endl;
	return 0;
}

int TestMultipleWindows() {
	std::cout << "\n=== 多窗口测试 ===" << std::endl;

	try {
		Window window1("窗口 1", 600, 400);
		Window window2("窗口 2", 400, 300);

		window1.Create();
		window2.Create();

		window1.SetPosition(100, 100);
		window2.SetPosition(400, 200);

		window1.Show();
		window2.Show();

		// 运行多窗口消息循环
		while (!window1.ShouldClose() && !window2.ShouldClose()) {
			window1.ProcessMessages();
			window2.ProcessMessages();

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Multi-window test failed: " << e.what() << std::endl;
	}

	return 0;
}