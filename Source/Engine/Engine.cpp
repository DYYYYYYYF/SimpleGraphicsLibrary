#include "Engine.h"
#include "Logger.hpp"

#include "Core/IApplication.h"
#include "Core/EventManager.h"
#include "Platform/Window/Window.h"

Engine::Engine() {
	Window_ = nullptr;
	Running_ = false;
	Application_ = nullptr;
}

Engine& Engine::GetInstance() {
	static Engine Engine_;
	return Engine_;
}

bool Engine::Initialize(IApplication* app) {
    // Logger
    Log::Logger* Logger = Log::Logger::getInstance();
    Logger->open("Engine");
    Logger->setMaxSize(1024 * 1024);
    Logger->SetMode(Log::Logger::LogMode::eMode_Only_Message);

	// Application
	if (!app) {
		LOG_ERROR << "Engine create failed. Application pointer is null!";
		return false;
	}

	if (!app->Initialize()) {
		LOG_ERROR << "Application init failed!";
		return false;
	}

	Application_ = app;

	// Window
	Window_ = new Window(app->GetName(), 1200, 720);
	if (!Window_ || !Window_->Create()) {
		LOG_ERROR << "Window init failed!";
	}

	Running_ = true;
	Window_->Show();

	// Event
	EventManager& eventManager = EventManager::Instance();
	eventManager.SetLogging(false);
	eventManager.Subscribe<KeyPressedEvent>([this](KeyPressedEvent& e) {
		if (e.GetKeyCode() == KeyCode::Escape)
		{
			Running_ = false;
		}
		return false;
	});

	// TODO: Managers  Renderer  Subsystems Window

	LOG_INFO << "Engine create successfully.";
	return true;
}

void Engine::Run() {
	EventManager& eventManager = EventManager::Instance();
	while (Running_ && !Window_->ShouldClose()) {
		// 处理窗口消息
		Window_->ProcessMessages();
		// 处理全局事件队列
		eventManager.ProcessEvents();

		// Application tick
		Application_->Tick();

		// TODO: Render
	}
}

void Engine::Shutdown() {
	EventManager::Instance().UnsubscribeAll();

	if (Window_) {
		Window_->Destroy();
	}

	if (Application_)
	{
		Application_->Shutdown();
	}

	LOG_INFO << "Engine shutdown.";
}


