#pragma once

#include "BaseComponent.h"
#include "Core/BaseMath.h"

class TransformComponent : public BaseComponent{
public:
	TransformComponent();
	TransformComponent(Actor* Owner, const std::string& Name);
	~TransformComponent();

public:
	// Position
	ENGINE_FRAMEWORK_API void SetPosition(const FVector3& p);
	ENGINE_FRAMEWORK_API const FVector3& GetPosition() const { return Position_; }

	// Rotation (Euler API 对外一致)
	ENGINE_FRAMEWORK_API void SetRotationEuler(const FVector3& eulerDeg);
	ENGINE_FRAMEWORK_API FVector3 GetRotationEuler() const;

	// Direct quaternion
	ENGINE_FRAMEWORK_API void SetRotationQuat(const FQuaternion& q);
	ENGINE_FRAMEWORK_API const FQuaternion& GetRotationQuat() const { return Rotation_; }

	// Rotation operations
	ENGINE_FRAMEWORK_API void RotateWorld(const FVector3& axis, float deg);
	ENGINE_FRAMEWORK_API void RotateLocal(const FVector3& axis, float deg);

	ENGINE_FRAMEWORK_API void RotateLocalX(float deg);
	ENGINE_FRAMEWORK_API void RotateLocalY(float deg);
	ENGINE_FRAMEWORK_API void RotateLocalZ(float deg);

	ENGINE_FRAMEWORK_API void RotateX(float deg);  // world
	ENGINE_FRAMEWORK_API void RotateY(float deg);
	ENGINE_FRAMEWORK_API void RotateZ(float deg);

	ENGINE_FRAMEWORK_API void SetScale(const FVector3& s);
	ENGINE_FRAMEWORK_API const FVector3& GetScale() const { return Scale_; }

	// Matrix
	ENGINE_FRAMEWORK_API const FMatrix4& GetModelMatrix() const;

private:
	void UpdateModelMatrix() const;

private:
	FVector3 Position_ = FVector3::Zero();
	FVector3 Scale_ = FVector3(1, 1, 1);
	FQuaternion Rotation_ = FQuaternion::Identity();

	mutable FMatrix4 ModelMatrix_ = FMatrix4::Identity();
	mutable bool IsDirty_ = true;
};