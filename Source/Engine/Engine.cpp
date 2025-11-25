#include "Engine.h"
#include "Logger.hpp"

#include "Core/IApplication.h"
#include "Core/EventManager.h"
#include "Platform/Window/Window.h"

#include "Rendering/Renderer/Renderer.h"
#include "Scene.h"
#include "Framework/Actors/CameraActor.h"
#include "Framework/Components/MeshComponent.h"
#include "Rendering/Resource/Manager/ResourceManager.h"
#include "FrameRateController.h"

Engine::Engine() {
	Window_ = nullptr;
	Running_ = false;
	Application_ = nullptr;
	Scene_ = nullptr;
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
		LOG_ERROR << "Renderer init failed!";
		return false;
	}

	ResourceManager& ResourceSys = ResourceManager::Instance();
	if (!ResourceSys.Initialize()) {
		LOG_ERROR << "ResourceSystem init failed!";
		return false;
	}

	// TODO: Managers  Renderer  Subsystems Window

	// 创建场景
	Scene_ = new Scene();
	if (!Scene_ || !Scene_->Initialize()) {
		LOG_ERROR << "Scene init failed!";
		return false;
	}

	LOG_INFO << "Engine create successfully.";
	return true;
}

void Engine::Run() {
	// 加载场景
	Application_->InitScene(*Scene_);
	// 帧率控制
	FrameRateController Frc(144, 60);

	// Tick
	while (Running_ && !Window_->ShouldClose()) {
		Frc.BeginFrame();
		// 固定逻辑帧（物理 / AI）
		if (Frc.ShouldRunFixedUpdate())
		{
			FixedTick(Frc.GetFixedDeltaTime());
		}

		// 处理窗口消息
		Window_->ProcessMessages();
		// 处理全局事件队列
		EventManager::Instance().ProcessEvents();

		Tick(Frc.GetDeltaTime());

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
		}
	}

	// 后网格
	for (auto& Act : AllActors) {
		MeshComponent* MeshComp = Act.get()->GetComponent<MeshComponent>();
		if (MeshComp) {
			MeshComp->Draw(CmdList);
		}
	}

	CoreRenderer->DrawScene(CmdList);
	CoreRenderer->EndCommand(CmdList);
}

void Engine::Shutdown() {
	Scene_->Clear();

	ResourceManager& ResourceSys = ResourceManager::Instance();
	ResourceSys.Shutdown();

	if (CoreRenderer) {
		CoreRenderer->Destroy();
		delete(CoreRenderer);
		CoreRenderer = nullptr;
	}

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


