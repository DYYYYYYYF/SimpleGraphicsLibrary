#include "TransformComponent.h"

TransformComponent::TransformComponent() : BaseComponent() {}
TransformComponent::TransformComponent(Actor* Owner, const std::string& Name) : BaseComponent(Owner, Name){}
TransformComponent::~TransformComponent() {}

void TransformComponent::SetPosition(const FVector3& p) {
	Position_ = p;
	IsDirty_ = true;
}

void TransformComponent::SetRotationQuat(const FQuaternion& q) {
	Rotation_ = q.normalized();
	IsDirty_ = true;
}

void TransformComponent::SetRotationEuler(const FVector3& eulerDeg) {
	// Degrees → Radians → Quaternion
	FVector3 rad = eulerDeg * (M_PI / 180.0f);
	Rotation_ = FQuaternion(Eigen::AngleAxisf(rad.z(), FVector3::UnitZ()) *
		AngleAxis(rad.y(), FVector3::UnitY()) *
		AngleAxis(rad.x(), FVector3::UnitX()));
	Rotation_.normalize();
	IsDirty_ = true;
}

FVector3 TransformComponent::GetRotationEuler() const {
	// Quaternion → Euler
	Eigen::Vector3f rad = Rotation_.toRotationMatrix().canonicalEulerAngles(0, 1, 2); // XYZ
	return rad * (180.0f / M_PI);
}

// 世界旋转（不受物体朝向影响）
void TransformComponent::RotateWorld(const FVector3& axis, float deg) {
	float rad = deg * (M_PI / 180.0f);
	FQuaternion dq(AngleAxis(rad, axis.normalized()));
	Rotation_ = (dq * Rotation_).normalized();
	IsDirty_ = true;
}

// 局部旋转（与当前朝向对齐）
void TransformComponent::RotateLocal(const FVector3& axis, float deg) {
	float rad = deg * (M_PI / 180.0f);
	// ⭐ 局部轴 = Rotation 对应的旋转矩阵的方向
	FVector3 localAxis = Rotation_ * axis.normalized();
	FQuaternion dq(AngleAxis(rad, localAxis));
	Rotation_ = (dq * Rotation_).normalized();
	IsDirty_ = true;
}

// World
void TransformComponent::RotateX(float deg) { RotateWorld(FVector3::UnitX(), deg); }
void TransformComponent::RotateY(float deg) { RotateWorld(FVector3::UnitY(), deg); }
void TransformComponent::RotateZ(float deg) { RotateWorld(FVector3::UnitZ(), deg); }

// Local
void TransformComponent::RotateLocalX(float deg) { RotateLocal(FVector3::UnitX(), deg); }
void TransformComponent::RotateLocalY(float deg) { RotateLocal(FVector3::UnitY(), deg); }
void TransformComponent::RotateLocalZ(float deg) { RotateLocal(FVector3::UnitZ(), deg); }

void TransformComponent::SetScale(const FVector3& s) {
	Scale_ = s;
	IsDirty_ = true;
}

const FMatrix4& TransformComponent::GetModelMatrix() const {
	if (IsDirty_) UpdateModelMatrix();
	return ModelMatrix_;
}

void TransformComponent::UpdateModelMatrix() const {
	ModelMatrix_ = FMatrix4::Identity();

	// Scale
	ModelMatrix_.block<3, 3>(0, 0) = Rotation_.toRotationMatrix();
	ModelMatrix_.block<3, 3>(0, 0) *= Scale_.asDiagonal();

	// Translation
	ModelMatrix_.block<3, 1>(0, 3) = Position_;

	IsDirty_ = false;
}
