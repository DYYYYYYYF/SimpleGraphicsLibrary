#include "TransformComponent.h"

TransformComponent::TransformComponent() : BaseComponent() {}
TransformComponent::TransformComponent(Actor* Owner, const std::string& Name) : BaseComponent(Owner, Name) {}
TransformComponent::~TransformComponent() {}

const FMatrix4& TransformComponent::GetModelMatrix() const {
	if (IsDirty_) {
		// 计算模型矩阵
		ModelMatrix_ = CalculateModelMatrix(Position_, Rotation_, Scale_);
		IsDirty_ = false;
	}
	return ModelMatrix_;
}

void TransformComponent::SetPosition(const FVector3& Position) {
	Position_ = Position;
	IsDirty_ = true;
}

void TransformComponent::SetRotation(const FVector3& Rotation) {
	Rotation_ = Rotation;
	IsDirty_ = true;
}

void TransformComponent::SetScale(const FVector3& Scale) {
	Scale_ = Scale;
	IsDirty_ = true;
}

void TransformComponent::Rotate(const FVector3& Axis, float AngleDegrees) {
	RotateRad(Axis, AngleDegrees * M_PI / 180.0f);
}

void TransformComponent::RotateRad(const FVector3& Axis, float AngleRadians) {
	// 使用 Eigen 的 AngleAxis
	AngleAxis Rotation(AngleRadians, Axis.normalized());

	// 获取当前旋转矩阵
	FMatrix4 CurrentRotation = CalculateRotationMatrix(Rotation_);

	// 应用旋转（世界空间）
	FMatrix3 RotMatrix3 = Rotation.matrix() * CurrentRotation.block<3, 3>(0, 0);

	// 提取欧拉角
	FVector3 EulerAngles = ExtractEulerAnglesFromMatrix(RotMatrix3);  // ZYX 顺序

	Rotation_.x() = EulerAngles.z() * 180.0f / M_PI;
	Rotation_.y() = EulerAngles.y() * 180.0f / M_PI;
	Rotation_.z() = EulerAngles.x() * 180.0f / M_PI;

	IsDirty_ = true;
}

void TransformComponent::RotateX(float AngleDegrees) {
	Rotation_.x() += AngleDegrees;
	IsDirty_ = true;
}

void TransformComponent::RotateY(float AngleDegrees) {
	Rotation_.y() += AngleDegrees;
	IsDirty_ = true;
}

void TransformComponent::RotateZ(float AngleDegrees) {
	Rotation_.z() += AngleDegrees;
	IsDirty_ = true;
}

void TransformComponent::RotateLocalX(float AngleDegrees) {
	Rotate(GetModelMatrix().block<3, 1>(0, 0), AngleDegrees);  // 局部 X 轴
}

void TransformComponent::RotateLocalY(float AngleDegrees) {
	Rotate(GetModelMatrix().block<3, 1>(0, 1), AngleDegrees);  // 局部 Y 轴
}

void TransformComponent::RotateLocalZ(float AngleDegrees) {
	Rotate(GetModelMatrix().block<3, 1>(0, 2), AngleDegrees);  // 局部 Z 轴
}

void TransformComponent::ResetRotation() {
	Rotation_ = FVector3(0.0f, 0.0f, 0.0f);
	IsDirty_ = true;
}

// 从欧拉角计算旋转矩阵
FMatrix4 TransformComponent::CalculateRotationMatrix(const FVector3& Euler) const {
	float rx = Euler.x() * M_PI / 180.0f;
	float ry = Euler.y() * M_PI / 180.0f;
	float rz = Euler.z() * M_PI / 180.0f;

	AngleAxis rotX(rx, Eigen::Vector3f::UnitX());
	AngleAxis rotY(ry, Eigen::Vector3f::UnitY());
	AngleAxis rotZ(rz, Eigen::Vector3f::UnitZ());

	FMatrix4 RotMatrix = FMatrix4::Identity();
	RotMatrix.block<3, 3>(0, 0) = (rotZ * rotY * rotX).matrix();

	return RotMatrix;
}

FMatrix4 TransformComponent::CalculateModelMatrix(const FVector3& Position, const FVector3& Rotation, const FVector3& Scale) const {
	FMatrix4 ModelMatrix = FMatrix4::Identity();

	// 缩放
	FMatrix4 ScaleMatrix = FMatrix4::Identity();
	ScaleMatrix(0, 0) = Scale.x();
	ScaleMatrix(1, 1) = Scale.y();
	ScaleMatrix(2, 2) = Scale.z();

	// 旋转 (绕 X, Y, Z 轴)
	AngleAxis RotX(Rotation.x() * M_PI / 180.0f, Eigen::Vector3f::UnitX());
	AngleAxis RotY(Rotation.y() * M_PI / 180.0f, Eigen::Vector3f::UnitY());
	AngleAxis RotZ(Rotation.z() * M_PI / 180.0f, Eigen::Vector3f::UnitZ());
	FMatrix4 RotMatrix = FMatrix4::Identity();
	RotMatrix.block<3, 3>(0, 0) = (RotZ * RotY * RotX).matrix();

	// 平移
	FMatrix4 TransMatrix = FMatrix4::Identity();
	TransMatrix.block<3, 1>(0, 3) = Position;

	// 组合: T * R * S
	ModelMatrix = TransMatrix * RotMatrix * ScaleMatrix;
	return ModelMatrix;
}

FVector3 TransformComponent::ExtractEulerAnglesFromMatrix(const Eigen::Matrix3f& RotMatrix) {
	FVector3 euler;
	float sy = std::sqrt(RotMatrix(0, 0) * RotMatrix(0, 0) +
		RotMatrix(1, 0) * RotMatrix(1, 0));

	bool singular = sy < 1e-6f; // 万向锁检测

	if (!singular) {
		euler.x() = std::atan2(RotMatrix(2, 1), RotMatrix(2, 2));  // Roll (X)
		euler.y() = std::atan2(-RotMatrix(2, 0), sy);              // Pitch (Y)
		euler.z() = std::atan2(RotMatrix(1, 0), RotMatrix(0, 0));  // Yaw (Z)
	}
	else {
		// 万向锁情况
		euler.x() = std::atan2(-RotMatrix(1, 2), RotMatrix(1, 1));
		euler.y() = std::atan2(-RotMatrix(2, 0), sy);
		euler.z() = 0.0f;
	}

	// 转换为角度
	euler.x() *= 180.0f / M_PI;
	euler.y() *= 180.0f / M_PI;
	euler.z() *= 180.0f / M_PI;

	return euler;
}