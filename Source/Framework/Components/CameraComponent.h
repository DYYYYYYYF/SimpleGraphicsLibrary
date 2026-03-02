#pragma once

#include "BaseComponent.h"
#include "Core/BaseMath.h"

struct FInputState;

class UCameraComponent : public UBaseComponent {
public:
	enum class ProjectionType { Perspective, Orthographic };

	enum class ControlMode {
		LookAt,        // 看向固定目标点
		FreeRotation,  // FPS 自由旋转
		Orbit          // 围绕目标点轨道旋转
	};

public:
	UCameraComponent();
	UCameraComponent(AActor* Owner, const std::string& Name);
	virtual ~UCameraComponent();

	// --------------------------------------------------------
	//  每帧驱动：处理输入，更新内部状态，输出新的相机位置
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API void ProcessInput(const FInputState& Input, float DeltaTime,
		FVector3& InOutCameraPos);

	// --------------------------------------------------------
	//  矩阵获取
	//  GetViewMatrix 只需要 CameraPos，内部用 Yaw/Pitch 算出朝向
	//  不再依赖 Target_ 世界坐标（FreeRotation/Orbit 各自维护）
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API FMatrix4        GetViewMatrix(const FVector3& CameraPos) const;
	ENGINE_FRAMEWORK_API const FMatrix4& GetProjectionMatrix() const;

	// --------------------------------------------------------
	//  投影设置
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API void SetAspect(float Aspect);
	ENGINE_FRAMEWORK_API void SetAspect(float Width, float Height);
	ENGINE_FRAMEWORK_API void SetProjectionType(ProjectionType Type);

	// --------------------------------------------------------
	//  控制模式
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API void        SetControlMode(ControlMode Mode);
	ENGINE_FRAMEWORK_API ControlMode GetControlMode() const { return ControlMode_; }

	// --------------------------------------------------------
	//  参数设置
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API void SetTarget(const FVector3& Target);   // LookAt / Orbit 的目标点
	ENGINE_FRAMEWORK_API void SetUp(const FVector3& Up);
	ENGINE_FRAMEWORK_API void SetMoveSpeed(float Speed) { MoveSpeed_ = Speed; }
	ENGINE_FRAMEWORK_API void SetRotateSpeed(float Speed) { RotateSpeed_ = Speed; }
	ENGINE_FRAMEWORK_API void SetZoomSpeed(float Speed) { ZoomSpeed_ = Speed; }
	ENGINE_FRAMEWORK_API void SetOrbitDistance(float Dist);
	ENGINE_FRAMEWORK_API void SetPitchClamp(float MinDeg, float MaxDeg);
	ENGINE_FRAMEWORK_API void SetYaw(float Yaw) { Yaw_ = Yaw;   IsViewDirty_ = true; }
	ENGINE_FRAMEWORK_API void SetPitch(float Pitch) { Pitch_ = Pitch; ClampPitch(); IsViewDirty_ = true; }

	// --------------------------------------------------------
	//  状态读取
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API float           GetYaw()           const { return Yaw_; }
	ENGINE_FRAMEWORK_API float           GetPitch()         const { return Pitch_; }
	ENGINE_FRAMEWORK_API float           GetOrbitDistance() const { return OrbitDistance_; }
	ENGINE_FRAMEWORK_API const FVector3& GetTarget()        const { return Target_; }

	// --------------------------------------------------------
	//  低级操作（供外部直接调用）
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API void AddLocalOffset(const FVector3& Delta);
	ENGINE_FRAMEWORK_API void AddWorldOffset(const FVector3& Delta);
	ENGINE_FRAMEWORK_API void AddRelativeRotation(float YawDegree, float PitchDegree);
	ENGINE_FRAMEWORK_API void RotateAroundTarget(float YawDegree, float PitchDegree, float Distance);
	ENGINE_FRAMEWORK_API void AddZoom(float Delta);

	// --------------------------------------------------------
	//  方向向量（公开，供外部查询）
	// --------------------------------------------------------
	ENGINE_FRAMEWORK_API FVector3 GetForwardVector() const;
	ENGINE_FRAMEWORK_API FVector3 GetRightVector()   const;
	ENGINE_FRAMEWORK_API FVector3 GetUpVector()      const;  // 本地 Up（非世界 Up）

private:
	void ProcessLookAt(const FInputState& Input, float DeltaTime, FVector3& InOutPos);
	void ProcessFreeRotation(const FInputState& Input, float DeltaTime, FVector3& InOutPos);
	void ProcessOrbit(const FInputState& Input, float DeltaTime, FVector3& InOutPos);

	FMatrix4 CreateViewMatrix(const FVector3& Eye, const FVector3& Center, const FVector3& Up) const;
	FMatrix4 CreatePerspectiveMatrix(float FovY, float Aspect, float Near, float Far) const;
	FMatrix4 CreateOrthographicMatrix(float Left, float Right, float Bottom, float Top, float Near, float Far) const;

	void ClampPitch();

private:
	// ---- 朝向：用 Yaw/Pitch 表达，是唯一真相来源 ----
	float Yaw_ = 0.0f;   // 水平旋转（度），0 → 看向 -Z
	float Pitch_ = 0.0f;   // 垂直旋转（度），正值 → 抬头
	float PitchMin_ = -89.0f;
	float PitchMax_ = 89.0f;

	// ---- LookAt / Orbit 的世界目标点 ----
	FVector3 Target_{ 0.0f, 0.0f, 0.0f };

	// ---- 世界 Up 轴 ----
	FVector3 WorldUp_{ 0.0f, 1.0f, 0.0f };

	// ---- Orbit 距离 ----
	float OrbitDistance_ = 10.0f;
	float OrbitDistanceMin_ = 0.5f;
	float OrbitDistanceMax_ = 500.0f;

	// ---- 控制参数 ----
	ControlMode ControlMode_ = ControlMode::Orbit;
	float MoveSpeed_ = 5.0f;
	float RotateSpeed_ = 0.15f;
	float ZoomSpeed_ = 2.0f;

	// ---- 投影参数 ----
	ProjectionType ProjectionType_ = ProjectionType::Perspective;
	float Fov_ = 45.0f;
	float Aspect_ = 16.0f / 9.0f;
	float NearPlane_ = 0.1f;
	float FarPlane_ = 1000.0f;
	float Left_ = -10.0f, Right_ = 10.0f;
	float Bottom_ = -10.0f, Top_ = 10.0f;

	// ---- 缓存矩阵 ----
	mutable FMatrix4 ViewMatrix_;
	mutable FMatrix4 ProjectionMatrix_;
	mutable bool IsViewDirty_ = true;
	mutable bool IsProjectionDirty_ = true;
};

// ============================================================
//  FInputState
// ============================================================
struct FInputState {
	bool W = false, S = false;
	bool A = false, D = false;
	bool Q = false, E = false;

	float MouseDeltaX = 0.0f;
	float MouseDeltaY = 0.0f;
	float ScrollDelta = 0.0f;

	bool RightMouseHeld = false;
	bool MiddleMouseHeld = false;
	bool ShiftHeld = false;
};