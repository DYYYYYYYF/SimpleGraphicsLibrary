#include "CameraActor.h"

// ============================================================
//  构造 / 析构
// ============================================================
ACameraActor::ACameraActor(const std::string& name) : AActor(name) {
	CameraComponent_ = CreateComponent<UCameraComponent>(this, "CameraComponent");
	if (!CameraComponent_) { return; }

	CameraComponent_->SetControlMode(UCameraComponent::ControlMode::FreeRotation);
	CameraComponent_->SetOrbitDistance(10.0f);
	CameraComponent_->SetTarget({ 0.0f, 0.0f, 0.0f });
}

ACameraActor::~ACameraActor() {
	UnsubscribeCamera();
}

// ============================================================
//  BeginPlay —— 订阅所有 Input 事件
// ============================================================
void ACameraActor::BeginPlay() {
	if (Subscribed_) { return; }

	On<KeyPressedEvent>(EVENT_BIND_FN(OnKeyPressed));
	On<KeyReleasedEvent>(EVENT_BIND_FN(OnKeyReleased));
	On<MouseMovedEvent>(EVENT_BIND_FN(OnMouseMoved));
	On<MouseScrolledEvent>(EVENT_BIND_FN(OnMouseScrolled));
	On<MouseButtonPressedEvent>(EVENT_BIND_FN(OnMouseButtonPressed));
	On<MouseButtonReleasedEvent>(EVENT_BIND_FN(OnMouseButtonReleased));

	Subscribed_ = true;

	// 同步初始位置到 TransformComponent（Engine::Render 从这里读）
	SyncPositionToTransform();
}

void ACameraActor::Tick(float DeltaTime) {
	if (!CameraComponent_) { return; }

	CameraComponent_->ProcessInput(FrameInput_, DeltaTime, Position_);

	// 关键：把最新 Position_ 写回 TransformComponent
	// Engine::Render() 从 TransformComponent::GetPosition() 读取
	SyncPositionToTransform();

	// 重置帧增量，持续按键 bool 保留
	FrameInput_.MouseDeltaX = 0.0f;
	FrameInput_.MouseDeltaY = 0.0f;
	FrameInput_.ScrollDelta = 0.0f;
}

// ============================================================
//  SyncPositionToTransform
// ============================================================
void ACameraActor::SyncPositionToTransform() {
	UTransformComponent* TC = GetComponent<UTransformComponent>();
	if (TC) {
		TC->SetPosition(Position_);
	}
}

void ACameraActor::UnsubscribeCamera() {
	if (!Subscribed_) { return; }

	AEventManager::Instance().Unsubscribe<KeyPressedEvent>();
	AEventManager::Instance().Unsubscribe<KeyReleasedEvent>();
	AEventManager::Instance().Unsubscribe<MouseMovedEvent>();
	AEventManager::Instance().Unsubscribe<MouseScrolledEvent>();
	AEventManager::Instance().Unsubscribe<MouseButtonPressedEvent>();
	AEventManager::Instance().Unsubscribe<MouseButtonReleasedEvent>();

	Subscribed_ = false;
}

FMatrix4 ACameraActor::GetViewMatrix() const {
	if (!CameraComponent_) { return FMatrix4::Identity(); }
	return CameraComponent_->GetViewMatrix(Position_);
}

const FMatrix4& ACameraActor::GetProjectionMatrix() const {
	return CameraComponent_->GetProjectionMatrix();
}

bool ACameraActor::OnKeyPressed(KeyPressedEvent& e) {
	FrameInput_.ShiftHeld = e.GetModifiers().shift;
	switch (e.GetKeyCode()) {
	case KeyCode::W: FrameInput_.W = true; break;
	case KeyCode::S: FrameInput_.S = true; break;
	case KeyCode::A: FrameInput_.A = true; break;
	case KeyCode::D: FrameInput_.D = true; break;
	case KeyCode::Q: FrameInput_.Q = true; break;
	case KeyCode::E: FrameInput_.E = true; break;
	default: break;
	}
	return false;
}

bool ACameraActor::OnKeyReleased(KeyReleasedEvent& e) {
	FrameInput_.ShiftHeld = e.GetModifiers().shift;
	switch (e.GetKeyCode()) {
	case KeyCode::W: FrameInput_.W = false; break;
	case KeyCode::S: FrameInput_.S = false; break;
	case KeyCode::A: FrameInput_.A = false; break;
	case KeyCode::D: FrameInput_.D = false; break;
	case KeyCode::Q: FrameInput_.Q = false; break;
	case KeyCode::E: FrameInput_.E = false; break;
	default: break;
	}
	return false;
}

bool ACameraActor::OnMouseMoved(MouseMovedEvent& e) {
	float x = e.GetX();
	float y = e.GetY();

	if (FirstMouse_) {
		LastMouseX_ = x;
		LastMouseY_ = y;
		FirstMouse_ = false;
		return false;  // 首帧不累积，防止跳变
	}

	FrameInput_.MouseDeltaX += x - LastMouseX_;
	FrameInput_.MouseDeltaY += y - LastMouseY_;
	LastMouseX_ = x;
	LastMouseY_ = y;
	return false;
}

bool ACameraActor::OnMouseScrolled(MouseScrolledEvent& e) {
	FrameInput_.ScrollDelta += e.GetYOffset();
	return false;
}

bool ACameraActor::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
	switch (e.GetMouseButton()) {
	case MouseButton::Right:  FrameInput_.RightMouseHeld = true; break;
	case MouseButton::Middle: FrameInput_.MiddleMouseHeld = true; break;
	default: break;
	}
	return false;
}

bool ACameraActor::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
	switch (e.GetMouseButton()) {
	case MouseButton::Right:  FrameInput_.RightMouseHeld = false; break;
	case MouseButton::Middle: FrameInput_.MiddleMouseHeld = false; break;
	default: break;
	}
	return false;
}