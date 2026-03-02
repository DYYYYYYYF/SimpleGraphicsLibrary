#pragma once

#include "Core/EventManager.h"  
#include "Event.h"             
#include "Framework/Components/CameraComponent.h" 

#include <unordered_set>

// ============================================================
//  AInputManager
//
//  职责：
//    1. 接收来自平台层（GLFW/Win32/SDL）的原始输入回调
//    2. 构造对应 Event 并通过 EventManager::PostEvent 入队
//    3. 维护当前帧键鼠持续状态 + 帧增量，供 BuildInputState() 使用
//
//  每帧调用顺序：
//    1. InputManager::BeginFrame()         —— 重置帧增量
//    2. ... 平台回调触发 On*() ...
//    3. EventManager::ProcessEvents()      —— 分发队列事件（驱动 CameraActor 回调）
//    4. InputManager::EndFrame()           —— 备份鼠标位置供下帧计算 delta
// ============================================================
class AInputManager {
public:
	ENGINE_CORE_API static AInputManager& Instance() {
		static AInputManager instance;
		return instance;
	}

	AInputManager(const AInputManager&) = delete;
	AInputManager& operator=(const AInputManager&) = delete;

	// --------------------------------------------------------
	//  每帧生命周期
	// --------------------------------------------------------
	ENGINE_CORE_API void BeginFrame();
	ENGINE_CORE_API void EndFrame();

	// --------------------------------------------------------
	//  平台层回调（由窗口系统/GLFW 等直接调用）
	//  ModifierKeys 由平台层从系统修饰键状态构造后传入
	// --------------------------------------------------------
	ENGINE_CORE_API void OnKeyPressed(KeyCode key, const ModifierKeys& mods, bool isRepeat = false);
	ENGINE_CORE_API void OnKeyReleased(KeyCode key, const ModifierKeys& mods);

	ENGINE_CORE_API void OnMouseMoved(float x, float y);
	ENGINE_CORE_API void OnMouseScrolled(float offsetX, float offsetY);
	ENGINE_CORE_API void OnMouseButtonPressed(MouseButton button, const ModifierKeys& mods);
	ENGINE_CORE_API void OnMouseButtonReleased(MouseButton button, const ModifierKeys& mods);

	// --------------------------------------------------------
	//  状态查询（供非事件驱动的系统轮询）
	// --------------------------------------------------------
	ENGINE_CORE_API bool  IsKeyHeld(KeyCode     key)    const;
	ENGINE_CORE_API bool  IsMouseButtonHeld(MouseButton button) const;
	ENGINE_CORE_API float GetMouseX()       const { return MouseX_; }
	ENGINE_CORE_API float GetMouseY()       const { return MouseY_; }
	ENGINE_CORE_API float GetMouseDeltaX()  const { return MouseDeltaX_; }
	ENGINE_CORE_API float GetMouseDeltaY()  const { return MouseDeltaY_; }
	ENGINE_CORE_API float GetScrollDeltaX() const { return ScrollDeltaX_; }
	ENGINE_CORE_API float GetScrollDeltaY() const { return ScrollDeltaY_; }

	// --------------------------------------------------------
	//  生成当前帧 FInputState（作为备用轮询接口；
	//  事件驱动路径下由 CameraActor 的回调直接维护 FrameInput_）
	// --------------------------------------------------------
	ENGINE_CORE_API FInputState BuildInputState() const;

private:
	AInputManager() = default;

	// 持续按键集合（使用 uint16_t 对应 KeyCode 底层类型）
	std::unordered_set<uint16_t> HeldKeys_;
	// 持续鼠标按键集合
	std::unordered_set<uint8_t>  HeldMouseButtons_;

	// 鼠标绝对位置
	float MouseX_ = 0.0f;
	float MouseY_ = 0.0f;

	// 上一帧末尾位置（EndFrame 更新）
	float LastMouseX_ = 0.0f;
	float LastMouseY_ = 0.0f;
	bool  FirstMouse_ = true;   // 防止首帧跳变

	// 帧增量（BeginFrame 清零，OnMouseMoved 累积）
	float MouseDeltaX_ = 0.0f;
	float MouseDeltaY_ = 0.0f;

	// 滚轮帧增量（BeginFrame 清零，OnMouseScrolled 累积）
	float ScrollDeltaX_ = 0.0f;
	float ScrollDeltaY_ = 0.0f;
};