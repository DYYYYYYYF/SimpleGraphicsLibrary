#pragma once

#include "Actor.h"
#include "Framework/Components/CameraComponent.h"
#include "Core/EventManager.h"
#include "Core/InputManager.h"

class ACameraActor : public AActor, public EventHandler {
public:
	ENGINE_FRAMEWORK_API ACameraActor(const std::string& name);
	ENGINE_FRAMEWORK_API virtual ~ACameraActor();

public:
	ENGINE_FRAMEWORK_API virtual void BeginPlay() override;
	ENGINE_FRAMEWORK_API virtual void Tick(float DeltaTime) override;

	ENGINE_FRAMEWORK_API FMatrix4        GetViewMatrix()       const;
	ENGINE_FRAMEWORK_API const FMatrix4& GetProjectionMatrix() const;

	ENGINE_FRAMEWORK_API void            SetPosition(const FVector3& Pos) { Position_ = Pos; }
	ENGINE_FRAMEWORK_API const FVector3& GetPosition()                const { return Position_; }

	ENGINE_FRAMEWORK_API UCameraComponent* GetCameraComponent() const { return CameraComponent_; }

	ENGINE_FRAMEWORK_API void UnsubscribeCamera();

private:
	// 把 Position_ 写回 TransformComponent，供 Engine::Render() 读取
	void SyncPositionToTransform();

	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnKeyReleased(KeyReleasedEvent& e);
	bool OnMouseMoved(MouseMovedEvent& e);
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

private:
	UCameraComponent* CameraComponent_ = nullptr;

	FVector3    Position_{ 0.0f, 0.0f, 10.0f };
	FInputState FrameInput_;

	float LastMouseX_ = 0.0f;
	float LastMouseY_ = 0.0f;
	bool  FirstMouse_ = true;
	bool  Subscribed_ = false;
};