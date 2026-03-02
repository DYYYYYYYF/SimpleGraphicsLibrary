#include "CameraComponent.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline float ToRad(float deg) { return deg * static_cast<float>(M_PI) / 180.0f; }
static inline float Clamp(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

UCameraComponent::UCameraComponent() : UBaseComponent() {}
UCameraComponent::UCameraComponent(AActor* Owner, const std::string& Name)
	: UBaseComponent(Owner, Name) {
}
UCameraComponent::~UCameraComponent() {}

void UCameraComponent::SetControlMode(ControlMode Mode) { ControlMode_ = Mode; IsViewDirty_ = true; }
void UCameraComponent::SetTarget(const FVector3& T) { Target_ = T;    IsViewDirty_ = true; }
void UCameraComponent::SetUp(const FVector3& Up) { WorldUp_ = Up;   IsViewDirty_ = true; }

void UCameraComponent::SetOrbitDistance(float Dist) {
	OrbitDistance_ = Clamp(Dist, OrbitDistanceMin_, OrbitDistanceMax_);
	IsViewDirty_ = true;
}
void UCameraComponent::SetPitchClamp(float MinDeg, float MaxDeg) {
	PitchMin_ = MinDeg; PitchMax_ = MaxDeg; ClampPitch();
}
void UCameraComponent::SetAspect(float A) { Aspect_ = A;         IsProjectionDirty_ = true; }
void UCameraComponent::SetAspect(float W, float H) { Aspect_ = W / H;     IsProjectionDirty_ = true; }
void UCameraComponent::SetProjectionType(ProjectionType T) {
	if (T != ProjectionType_) { ProjectionType_ = T; IsProjectionDirty_ = true; }
}

// ============================================================
//  方向向量
//
//  约定（右手系，Y 朝上）：
//    Yaw=0, Pitch=0  →  Forward = (0, 0, -1)  即看向 -Z
//    Yaw 顺时针增大（从上往下看），Yaw=90 → Forward = (+1, 0, 0)
//    Pitch 正值抬头，Pitch=90 → Forward = (0, +1, 0)
//
//  Forward:
//    X =  cos(pitch) * sin(yaw)
//    Y =  sin(pitch)
//    Z = -cos(pitch) * cos(yaw)
//
//  Right = normalize(Forward × WorldUp)
//  （展开后与 Pitch 无关，Right 始终水平，符合 FPS 直觉）
//    X =  cos(yaw)
//    Y =  0
//    Z =  sin(yaw)
//
//  LocalUp = Right × Forward
//  （真实的相机上方向，随 Pitch 倾斜）
// ============================================================
FVector3 UCameraComponent::GetForwardVector() const {
	const float yr = ToRad(Yaw_);
	const float pr = ToRad(Pitch_);
	return FVector3(
		std::cos(pr) * std::sin(yr),
		std::sin(pr),
		-std::cos(pr) * std::cos(yr)
	).normalized();
}

FVector3 UCameraComponent::GetRightVector() const {
	const float yr = ToRad(Yaw_);
	return FVector3(std::cos(yr), 0.0f, std::sin(yr)).normalized();
}

FVector3 UCameraComponent::GetUpVector() const {
	// 本地 Up = Right × Forward（随 Pitch 倾斜的真实相机上方向）
	FVector3 f = GetForwardVector();
	FVector3 r = GetRightVector();
	return r.cross(f).normalized();   // 注意：不是 f×r
}

FMatrix4 UCameraComponent::GetViewMatrix(const FVector3& CameraPos) const {
	if (IsViewDirty_) {
		FVector3 center;
		if (ControlMode_ == ControlMode::LookAt) {
			// LookAt 模式：看向固定世界点，从 Target_ 反推 Yaw/Pitch 无意义，直接用 Target_
			center = Target_;
		}
		else {
			// FreeRotation / Orbit：用 Yaw/Pitch 算出的前向量确定 Center
			// 这样 Eye 换了位置也能保证朝向正确
			center = CameraPos + GetForwardVector();
		}
		ViewMatrix_ = CreateViewMatrix(CameraPos, center, WorldUp_);
		IsViewDirty_ = false;
	}
	return ViewMatrix_;
}

const FMatrix4& UCameraComponent::GetProjectionMatrix() const {
	if (IsProjectionDirty_) {
		if (ProjectionType_ == ProjectionType::Perspective) {
			ProjectionMatrix_ = CreatePerspectiveMatrix(Fov_, Aspect_, NearPlane_, FarPlane_);
		}
		else {
			ProjectionMatrix_ = CreateOrthographicMatrix(Left_, Right_, Bottom_, Top_, NearPlane_, FarPlane_);
		}
		IsProjectionDirty_ = false;
	}
	return ProjectionMatrix_;
}

void UCameraComponent::ProcessInput(const FInputState& Input, float DeltaTime,
	FVector3& InOutCameraPos) {
	switch (ControlMode_) {
	case ControlMode::LookAt:       ProcessLookAt(Input, DeltaTime, InOutCameraPos); break;
	case ControlMode::FreeRotation: ProcessFreeRotation(Input, DeltaTime, InOutCameraPos); break;
	case ControlMode::Orbit:        ProcessOrbit(Input, DeltaTime, InOutCameraPos); break;
	}
}

void UCameraComponent::ProcessLookAt(const FInputState& Input, float DeltaTime,
	FVector3& InOutPos) {
	float speed = MoveSpeed_ * (Input.ShiftHeld ? 3.0f : 1.0f);

	FVector3 toTarget = Target_ - InOutPos;
	FVector3 flatForward = FVector3(toTarget.x(), 0.0f, toTarget.z()).normalized();
	FVector3 right = FVector3(flatForward.z(), 0.0f, -flatForward.x());

	FVector3 moveDelta(0, 0, 0);
	if (Input.W) moveDelta = moveDelta + flatForward * (speed * DeltaTime);
	if (Input.S) moveDelta = moveDelta + flatForward * (-speed * DeltaTime);
	if (Input.D) moveDelta = moveDelta + right * (speed * DeltaTime);
	if (Input.A) moveDelta = moveDelta + right * (-speed * DeltaTime);

	if (Input.MiddleMouseHeld) {
		float panSpeed = speed * 0.01f;
		moveDelta = moveDelta + right * (-Input.MouseDeltaX * panSpeed);
		moveDelta = moveDelta + flatForward * (Input.MouseDeltaY * panSpeed);
	}

	InOutPos = InOutPos + moveDelta;
	Target_ = Target_ + moveDelta;

	if (std::abs(Input.ScrollDelta) > 1e-4f) {
		FVector3 dir = (InOutPos - Target_).normalized();
		float    dist = Clamp((InOutPos - Target_).norm() - Input.ScrollDelta * ZoomSpeed_,
			OrbitDistanceMin_, OrbitDistanceMax_);
		InOutPos = Target_ + dir * dist;
	}

	IsViewDirty_ = true;
}

void UCameraComponent::ProcessFreeRotation(const FInputState& Input, float DeltaTime,
	FVector3& InOutPos) {
	bool dirty = false;

	// --- 鼠标旋转（只在按住右键时响应）---
	if (Input.RightMouseHeld &&
		(std::abs(Input.MouseDeltaX) > 1e-4f || std::abs(Input.MouseDeltaY) > 1e-4f)) {
		Yaw_ += Input.MouseDeltaX * RotateSpeed_;
		Pitch_ -= Input.MouseDeltaY * RotateSpeed_;  // 鼠标下移 → Pitch 减小 → 低头
		ClampPitch();
		dirty = true;
	}

	// --- 键盘移动 ---
	// 先取方向向量（旋转已经更新，此时方向是最新的）
	const FVector3 forward = GetForwardVector();
	const FVector3 right = GetRightVector();
	// QE 升降用世界 Up，而不是本地 Up，防止 Pitch 很大时升降方向偏转
	const FVector3 worldUp = WorldUp_;

	float speed = MoveSpeed_ * (Input.ShiftHeld ? 3.0f : 1.0f) * DeltaTime;

	FVector3 moveDelta(0, 0, 0);
	if (Input.W) moveDelta = moveDelta + forward * speed;
	if (Input.S) moveDelta = moveDelta + forward * -speed;
	if (Input.D) moveDelta = moveDelta + right * speed;
	if (Input.A) moveDelta = moveDelta + right * -speed;
	if (Input.E) moveDelta = moveDelta + worldUp * speed;
	if (Input.Q) moveDelta = moveDelta + worldUp * -speed;

	if (moveDelta.squaredNorm() > 1e-8f) {
		InOutPos = InOutPos + moveDelta;
		dirty = true;
	}

	if (dirty) IsViewDirty_ = true;
}

void UCameraComponent::ProcessOrbit(const FInputState& Input, float DeltaTime,
	FVector3& InOutPos) {
	bool dirty = false;

	// --- 旋转 ---
	if (Input.RightMouseHeld &&
		(std::abs(Input.MouseDeltaX) > 1e-4f || std::abs(Input.MouseDeltaY) > 1e-4f)) {
		Yaw_ += Input.MouseDeltaX * RotateSpeed_;
		Pitch_ -= Input.MouseDeltaY * RotateSpeed_;
		ClampPitch();
		dirty = true;
	}

	// --- 滚轮缩放 ---
	if (std::abs(Input.ScrollDelta) > 1e-4f) {
		OrbitDistance_ = Clamp(OrbitDistance_ - Input.ScrollDelta * ZoomSpeed_,
			OrbitDistanceMin_, OrbitDistanceMax_);
		dirty = true;
	}

	// --- WASD / 中键平移 Target_ ---
	float panSpeed = MoveSpeed_ * (Input.ShiftHeld ? 3.0f : 1.0f) * DeltaTime;

	const float yr = ToRad(Yaw_);
	FVector3 flatForward(-std::sin(yr), 0.0f, -std::cos(yr));
	FVector3 flatRight(std::cos(yr), 0.0f, -std::sin(yr));

	FVector3 panDelta(0, 0, 0);
	if (Input.W) panDelta = panDelta + flatForward * panSpeed;
	if (Input.S) panDelta = panDelta + flatForward * -panSpeed;
	if (Input.D) panDelta = panDelta + flatRight * panSpeed;
	if (Input.A) panDelta = panDelta + flatRight * -panSpeed;

	if (Input.MiddleMouseHeld) {
		float mPan = MoveSpeed_ * 0.01f;
		panDelta = panDelta + flatRight * (-Input.MouseDeltaX * mPan);
		panDelta = panDelta + flatForward * (Input.MouseDeltaY * mPan);
	}

	if (panDelta.squaredNorm() > 1e-8f) {
		Target_ = Target_ + panDelta;
		dirty = true;
	}

	// --- 球坐标 → 相机世界位置 ---
	// Orbit 的 Yaw/Pitch 描述的是"从目标点看向相机"的方向的反向
	// 即相机在目标点的"背面"偏移
	const float pr = ToRad(Pitch_);
	const float cosP = std::cos(pr);
	FVector3 offset(
		OrbitDistance_ * cosP * std::sin(yr),
		OrbitDistance_ * std::sin(pr),
		OrbitDistance_ * cosP * std::cos(yr)
	);

	InOutPos = Target_ + offset;
	if (dirty) IsViewDirty_ = true;
}

void UCameraComponent::AddLocalOffset(const FVector3& Delta) {
	// Delta: (right, up, forward) 本地空间
	FVector3 world = GetRightVector() * Delta.x()
		+ GetUpVector() * Delta.y()
		+ GetForwardVector() * Delta.z();
	Target_ = Target_ + world;
	IsViewDirty_ = true;
}

void UCameraComponent::AddWorldOffset(const FVector3& Delta) {
	Target_ = Target_ + Delta;
	IsViewDirty_ = true;
}

void UCameraComponent::AddRelativeRotation(float YawDeg, float PitchDeg) {
	Yaw_ += YawDeg;
	Pitch_ += PitchDeg;
	ClampPitch();
	IsViewDirty_ = true;
}

void UCameraComponent::RotateAroundTarget(float YawDeg, float PitchDeg, float Distance) {
	Yaw_ += YawDeg;
	Pitch_ += PitchDeg;
	OrbitDistance_ = Clamp(Distance, OrbitDistanceMin_, OrbitDistanceMax_);
	ClampPitch();
	IsViewDirty_ = true;
}

void UCameraComponent::AddZoom(float Delta) {
	OrbitDistance_ = Clamp(OrbitDistance_ - Delta * ZoomSpeed_,
		OrbitDistanceMin_, OrbitDistanceMax_);
	IsViewDirty_ = true;
}

void UCameraComponent::ClampPitch() {
	Pitch_ = Clamp(Pitch_, PitchMin_, PitchMax_);
}

FMatrix4 UCameraComponent::CreateViewMatrix(const FVector3& Eye,
	const FVector3& Center,
	const FVector3& Up) const {
	FVector3 f = (Center - Eye).normalized();     // forward
	FVector3 s = f.cross(Up).normalized();         // right
	FVector3 u = s.cross(f);                       // real up

	FMatrix4 view = FMatrix4::Identity();
	view(0, 0) = s.x(); view(0, 1) = s.y(); view(0, 2) = s.z(); view(0, 3) = -s.dot(Eye);
	view(1, 0) = u.x(); view(1, 1) = u.y(); view(1, 2) = u.z(); view(1, 3) = -u.dot(Eye);
	view(2, 0) = -f.x(); view(2, 1) = -f.y(); view(2, 2) = -f.z(); view(2, 3) = f.dot(Eye);
	return view;
}

FMatrix4 UCameraComponent::CreatePerspectiveMatrix(float FovY, float Aspect,
	float NearPlane, float FarPlane) const {
	float t = std::tan(ToRad(FovY) * 0.5f);
	FMatrix4 proj = FMatrix4::Zero();
	proj(0, 0) = 1.0f / (Aspect * t);
	proj(1, 1) = 1.0f / t;
	proj(2, 2) = -(FarPlane + NearPlane) / (FarPlane - NearPlane);
	proj(2, 3) = -(2.0f * FarPlane * NearPlane) / (FarPlane - NearPlane);
	proj(3, 2) = -1.0f;
	return proj;
}

FMatrix4 UCameraComponent::CreateOrthographicMatrix(float Left, float Right,
	float Bottom, float Top,
	float NearPlane, float FarPlane) const {
	FMatrix4 proj = FMatrix4::Identity();
	proj(0, 0) = 2.0f / (Right - Left);
	proj(1, 1) = 2.0f / (Top - Bottom);
	proj(2, 2) = -2.0f / (FarPlane - NearPlane);
	proj(0, 3) = -(Right + Left) / (Right - Left);
	proj(1, 3) = -(Top + Bottom) / (Top - Bottom);
	proj(2, 3) = -(FarPlane + NearPlane) / (FarPlane - NearPlane);
	return proj;
}