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

	// Input
	RegisterInputCallbacks();

	// Event
	AEventManager& eventManager = AEventManager::Instance();
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

		// 清零帧增量（MouseDelta / ScrollDelta）
		AInputManager::Instance().BeginFrame();

		// 处理窗口消息
		Window_->ProcessMessages();
		// 处理全局事件队列
		AEventManager::Instance().ProcessEvents();

		Tick(Frc.GetDeltaTime());

		// 备份鼠标位置（供 InputManager 下帧计算 delta）
		AInputManager::Instance().EndFrame();

		// 渲染
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
	std::vector<std::shared_ptr<AActor>> AllActors = Scene_->GetAllActors();

	// 先摄像机
	for (auto& Act : AllActors) {
		ACameraActor* Camera = DynamicCast<ACameraActor>(Act).get();
		if (Camera) {
			const FMatrix4& ViewMatrix = Camera->GetViewMatrix();
			const FMatrix4& ProjMatrix = Camera->GetProjectionMatrix();

			CmdList.SetViewProjection(ViewMatrix, ProjMatrix);
			break;
		}
	}

	// 后网格
	for (auto& Act : AllActors) {
		UMeshComponent* MeshComp = Act.get()->GetComponent<UMeshComponent>();
		if (MeshComp) {
			MeshComp->Draw(CmdList);
		}
	}

	CoreRenderer->DrawScene(CmdList);
	CoreRenderer->EndCommand(CmdList);
}

void Engine::RegisterInputCallbacks() {
	Window_->SetEventCallback([](Event& e) {
		AInputManager& IM = AInputManager::Instance();

		// ---- 键盘 ----
		if (e.GetEventType() == EventType::KeyPressed) {
			auto& ke = static_cast<KeyPressedEvent&>(e);
			IM.OnKeyPressed(ke.GetKeyCode(), ke.GetModifiers(), ke.IsRepeat());
		}
		else if (e.GetEventType() == EventType::KeyReleased) {
			auto& ke = static_cast<KeyReleasedEvent&>(e);
			IM.OnKeyReleased(ke.GetKeyCode(), ke.GetModifiers());
		}

		// ---- 鼠标移动 ----
		else if (e.GetEventType() == EventType::MouseMoved) {
			auto& me = static_cast<MouseMovedEvent&>(e);
			IM.OnMouseMoved(me.GetX(), me.GetY());
		}

		// ---- 鼠标滚轮 ----
		else if (e.GetEventType() == EventType::MouseScrolled) {
			auto& me = static_cast<MouseScrolledEvent&>(e);
			IM.OnMouseScrolled(me.GetXOffset(), me.GetYOffset());
		}

		// ---- 鼠标按键 ----
		else if (e.GetEventType() == EventType::MouseButtonPressed) {
			auto& me = static_cast<MouseButtonPressedEvent&>(e);
			IM.OnMouseButtonPressed(me.GetMouseButton(), me.GetModifiers());
		}
		else if (e.GetEventType() == EventType::MouseButtonReleased) {
			auto& me = static_cast<MouseButtonReleasedEvent&>(e);
			IM.OnMouseButtonReleased(me.GetMouseButton(), me.GetModifiers());
		}

		// InputManager::On*() 内部已经 PostEvent 到 EventManager，
		// 非 Input 事件（窗口 Resize 等）需要单独入队
		else {
			// 窗口事件直接转发（Resize、Close 等）
			// 注意：这里无法 move unique_ptr，用 DispatchEvent 直接分发更合适
			AEventManager::Instance().DispatchEvent(e);
		}

	});
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

	AEventManager::Instance().UnsubscribeAll();

	if (Window_) {
		Window_->Destroy();
	}

	if (Application_)
	{
		Application_->Shutdown();
	}

	LOG_INFO << "Engine shutdown.";
}


