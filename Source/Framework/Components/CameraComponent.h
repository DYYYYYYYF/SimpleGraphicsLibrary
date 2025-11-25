#pragma once

#include "BaseComponent.h"
#include "Core/BaseMath.h"

class CameraComponent : public BaseComponent {
public:
	enum class ProjectionType { Perspective, Orthographic };
	enum class ControlMode {
		LookAt,        // 看向某点
		FreeRotation,  // 自由旋转（FPS风格）
		Orbit          // 轨道旋转
	};

public:
	CameraComponent();
	CameraComponent(Actor* Owner, const std::string& Name);
	virtual ~CameraComponent();

public:
	// 获取视图矩阵（需要 Transform 组件的 position）
	ENGINE_FRAMEWORK_API FMatrix4 GetViewMatrix(const FVector3& CameraPos) const;

	// 获取投影矩阵
	ENGINE_FRAMEWORK_API const FMatrix4& GetProjectionMatrix() const;

	ENGINE_FRAMEWORK_API void SetAspect(float Aspect);
	ENGINE_FRAMEWORK_API void SetAspect(float Width, float Height);
	ENGINE_FRAMEWORK_API void SetProjectionType(ProjectionType Type);

private:
	// 生成视图矩阵 (View Matrix) - LookAt
	FMatrix4 CreateViewMatrix(const FVector3& Eye, const FVector3& Center, const FVector3& Up) const;
	// 生成透视投影矩阵 (Perspective Projection)
	FMatrix4 CreatePerspectiveMatrix(float FovY, float Aspect, float NearPlane, float FarPlane) const;
	// 生成正交投影矩阵 (Orthographic Projection)
	FMatrix4 CreateOrthographicMatrix(float Left, float Right, float Bottom, float Top, float NearPlane, float FarPlane) const;

private:
	// 视图参数
	FVector3 Target_{ 0.0f, 0.0f, 0.0f };
	FVector3 Up_{ 0.0f, 1.0f, 0.0f };

	// 投影参数
	ProjectionType ProjectionType_ = ProjectionType::Perspective;

	// 透视投影参数
	float Fov_ = 45.0f;
	float Aspect_ = 16.0f / 9.0f;
	float NearPlane_ = 0.1f;
	float FarPlane_ = 100.0f;

	// 正交投影参数
	float Left_ = -10.0f, right = 10.0f;
	float Bottom_ = -10.0f, top = 10.0f;

	// 缓存矩阵
	mutable FMatrix4 ViewMatrix_;
	mutable FMatrix4 ProjectionMatrix_;
	mutable bool IsViewDirty_ = true;
	mutable bool IsProjectionDirty_ = true;
};