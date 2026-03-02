#include "InputManager.h"

// ============================================================
//  每帧生命周期
// ============================================================

void AInputManager::BeginFrame() {
	MouseDeltaX_ = 0.0f;
	MouseDeltaY_ = 0.0f;
	ScrollDeltaX_ = 0.0f;
	ScrollDeltaY_ = 0.0f;
}

void AInputManager::EndFrame() {
	LastMouseX_ = MouseX_;
	LastMouseY_ = MouseY_;
}

// ============================================================
//  平台层回调
// ============================================================

void AInputManager::OnKeyPressed(KeyCode key, const ModifierKeys& mods, bool isRepeat) {
	HeldKeys_.insert(static_cast<uint16_t>(key));

	AEventManager::Instance().PostEvent(
		std::make_unique<KeyPressedEvent>(key, mods, isRepeat)
	);
}

void AInputManager::OnKeyReleased(KeyCode key, const ModifierKeys& mods) {
	HeldKeys_.erase(static_cast<uint16_t>(key));

	AEventManager::Instance().PostEvent(
		std::make_unique<KeyReleasedEvent>(key, mods)
	);
}

void AInputManager::OnMouseMoved(float x, float y) {
	if (FirstMouse_) {
		LastMouseX_ = x;
		LastMouseY_ = y;
		FirstMouse_ = false;
	}

	// 帧内累积（一帧可能触发多次原始回调）
	MouseDeltaX_ += x - LastMouseX_;
	MouseDeltaY_ += y - LastMouseY_;
	LastMouseX_ = x;
	LastMouseY_ = y;
	MouseX_ = x;
	MouseY_ = y;

	// MouseMovedEvent 只携带绝对坐标（与 Event.h 定义一致）
	AEventManager::Instance().PostEvent(
		std::make_unique<MouseMovedEvent>(x, y)
	);
}

void AInputManager::OnMouseScrolled(float offsetX, float offsetY) {
	ScrollDeltaX_ += offsetX;
	ScrollDeltaY_ += offsetY;

	AEventManager::Instance().PostEvent(
		std::make_unique<MouseScrolledEvent>(offsetX, offsetY)
	);
}

void AInputManager::OnMouseButtonPressed(MouseButton button, const ModifierKeys& mods) {
	HeldMouseButtons_.insert(static_cast<uint8_t>(button));

	AEventManager::Instance().PostEvent(
		std::make_unique<MouseButtonPressedEvent>(button, mods)
	);
}

void AInputManager::OnMouseButtonReleased(MouseButton button, const ModifierKeys& mods) {
	HeldMouseButtons_.erase(static_cast<uint8_t>(button));

	AEventManager::Instance().PostEvent(
		std::make_unique<MouseButtonReleasedEvent>(button, mods)
	);
}

// ============================================================
//  状态查询
// ============================================================

bool AInputManager::IsKeyHeld(KeyCode key) const {
	return HeldKeys_.count(static_cast<uint16_t>(key)) > 0;
}

bool AInputManager::IsMouseButtonHeld(MouseButton button) const {
	return HeldMouseButtons_.count(static_cast<uint8_t>(button)) > 0;
}

// ============================================================
//  BuildInputState —— 备用轮询接口，直接读取持续状态快照
// ============================================================
FInputState AInputManager::BuildInputState() const {
	FInputState state;

	state.W = IsKeyHeld(KeyCode::W);
	state.S = IsKeyHeld(KeyCode::S);
	state.A = IsKeyHeld(KeyCode::A);
	state.D = IsKeyHeld(KeyCode::D);
	state.Q = IsKeyHeld(KeyCode::Q);
	state.E = IsKeyHeld(KeyCode::E);

	state.ShiftHeld = IsKeyHeld(KeyCode::LeftShift) || IsKeyHeld(KeyCode::RightShift);

	state.RightMouseHeld = IsMouseButtonHeld(MouseButton::Right);
	state.MiddleMouseHeld = IsMouseButtonHeld(MouseButton::Middle);

	state.MouseDeltaX = MouseDeltaX_;
	state.MouseDeltaY = MouseDeltaY_;
	state.ScrollDelta = ScrollDeltaY_;

	return state;
}