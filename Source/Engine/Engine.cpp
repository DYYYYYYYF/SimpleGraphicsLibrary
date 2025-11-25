#include "Engine.h"
#include "Logger.hpp"

#include "Core/IApplication.h"
#include "Core/EventManager.h"
#include "Platform/Window/Window.h"

#include "Rendering/Renderer/Renderer.h"
#include "Scene.h"
#include "Framework/Actors/Actor.h"
#include "Framework/Components/MeshComponent.h"

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
    Logger->open("Engine", std::ios_base::ate);
    Logger->setMaxSize(1024 * 1024);
    Logger->SetMode(Log::Logger::LogMode::eMode_Complex);

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

	// Rendering
	CoreRenderer = new Renderer();
	if (!CoreRenderer || !CoreRenderer->Initialize(Window_, BackendAPI::eOpenGL)) {
		return false;
	}

	// TODO: Managers  Renderer  Subsystems Window

	LOG_INFO << "Engine create successfully.";
	return true;
}

Scene Scene_;
void Engine::Run() {
	Application_->InitScene(Scene_);

	EventManager& eventManager = EventManager::Instance();
	while (Running_ && !Window_->ShouldClose()) {
		// 处理窗口消息
		Window_->ProcessMessages();
		// 处理全局事件队列
		eventManager.ProcessEvents();

		// Application tick
		Application_->Tick(0.01f);

<<<<<<< Updated upstream
		CommandList CmdList;
		CoreRenderer->BeginCommand(CmdList);
		std::vector<std::shared_ptr<Actor>> AllActors = Scene_.GetAllActors();
		for (auto& Act : AllActors) {
			MeshComponent* MeshComp = Act.get()->GetComponent<MeshComponent>();
			if (MeshComp) {
				MeshComp->Draw(CmdList);
			}
=======
		Render();

		Frc.EndFrame();
	}
}

void Engine::FixedTick(float DeltaTime) {
	(void)DeltaTime;
}

void Engine::Tick(float DeltaTime) {
	// Application tick
	Application_->Tick(DeltaTime);
}

void Engine::Render() {
	CommandList CmdList;
	CoreRenderer->BeginCommand(CmdList);
	std::vector<std::shared_ptr<Actor>> AllActors = Scene_->GetAllActors();

	// 先摄像机
	for (auto& Act : AllActors) {
		CameraActor* Camera = DynamicCast<CameraActor>(Act).get();
		if (Camera) {
			TransformComponent* TransformComp = Camera->GetComponent<TransformComponent>();
			if (!TransformComp) continue;
			const FVector3& Location = TransformComp->GetPosition();

			CameraComponent* CameraComp = Camera->GetComponent<CameraComponent>();
			if (!CameraComp) continue;
			const FMatrix4& ViewMatrix = CameraComp->GetViewMatrix(Location);
			const FMatrix4& ProjMatrix = CameraComp->GetProjectionMatrix();

			CmdList.SetViewProjection(ViewMatrix, ProjMatrix);
			break;
>>>>>>> Stashed changes
		}

		CoreRenderer->DrawScene(CmdList);
		CoreRenderer->EndCommand(CmdList);
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


