#pragma once

#include "BaseComponent.h"
#include "Core/BaseMath.h"

class TransformComponent : public BaseComponent {
public:
	TransformComponent();
	TransformComponent(Actor* Owner, const std::string& Name);
	virtual ~TransformComponent();

public:
	// 获取函数
	ENGINE_FRAMEWORK_API const FMatrix4& GetModelMatrix() const;
	ENGINE_FRAMEWORK_API const FVector3& GetPosition() const { return Position_; }
	ENGINE_FRAMEWORK_API void SetPosition(const FVector3& Position);
	ENGINE_FRAMEWORK_API const FVector3& GetRotation() const { return Rotation_; }
	ENGINE_FRAMEWORK_API void SetRotation(const FVector3& Rotation);
	ENGINE_FRAMEWORK_API const FVector3& GetScale() const { return Scale_; }
	ENGINE_FRAMEWORK_API void SetScale(const FVector3& Scale);

	// 绕任意轴旋转（角度制）
	ENGINE_FRAMEWORK_API void Rotate(const FVector3& Axis, float AngleDegrees);
	// 绕任意轴旋转（弧度制）
	ENGINE_FRAMEWORK_API void RotateRad(const FVector3& Axis, float AngleRadians);
	// 绕世界坐标系的轴旋转
	ENGINE_FRAMEWORK_API void RotateX(float AngleDegrees);
	ENGINE_FRAMEWORK_API void RotateY(float AngleDegrees);
	ENGINE_FRAMEWORK_API void RotateZ(float AngleDegrees);
	// 绕局部坐标系的轴旋转
	ENGINE_FRAMEWORK_API void RotateLocalX(float AngleDegrees);
	ENGINE_FRAMEWORK_API void RotateLocalY(float AngleDegrees);
	ENGINE_FRAMEWORK_API void RotateLocalZ(float AngleDegrees);
	// 重置旋转
	ENGINE_FRAMEWORK_API void ResetRotation();
	// 从旋转矩阵提取欧拉角（ZYX 顺序）
	ENGINE_FRAMEWORK_API FMatrix4 CalculateRotationMatrix(const FVector3& Euler) const;

private:
	FMatrix4 CalculateModelMatrix(const FVector3& Position, const FVector3& Rotation, const FVector3& Scale) const ;
	FVector3 ExtractEulerAnglesFromMatrix(const Eigen::Matrix3f& RotMatrix);

private:
	FVector3 Position_{ 0.0f, 0.0f, 0.0f };
	FVector3 Rotation_{ 0.0f, 0.0f, 0.0f };  // 欧拉角或四元数
	FVector3 Scale_{ 1.0f, 1.0f, 1.0f };

	// 缓存模型矩阵
	mutable FMatrix4 ModelMatrix_;
	mutable bool IsDirty_ = true;
};