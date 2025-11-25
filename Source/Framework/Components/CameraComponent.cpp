#include "CameraComponent.h"

CameraComponent::CameraComponent() : BaseComponent(){}
CameraComponent::CameraComponent(Actor* Owner, const std::string& Name) : BaseComponent(Owner, Name) {}
CameraComponent::~CameraComponent() {}

FMatrix4 CameraComponent::GetViewMatrix(const FVector3& CameraPos) const {
	if (IsViewDirty_) {
		ViewMatrix_ = CreateViewMatrix(CameraPos, Target_, Up_);
		IsViewDirty_ = false;
	}
	return ViewMatrix_;
}

// 获取投影矩阵
const FMatrix4& CameraComponent::GetProjectionMatrix() const {
	if (IsProjectionDirty_) {
		if (ProjectionType_ == ProjectionType::Perspective) {
			ProjectionMatrix_ = CreatePerspectiveMatrix(Fov_, Aspect_, NearPlane_, FarPlane_);
		}
		else {
			ProjectionMatrix_ = CreateOrthographicMatrix(Left_, right, Bottom_, top, NearPlane_, FarPlane_);
		}
		IsProjectionDirty_ = false;
	}
	return ProjectionMatrix_;
}

void CameraComponent::SetAspect(float Aspect) {
	Aspect_ = Aspect;
	IsProjectionDirty_ = true;
}

void CameraComponent::SetAspect(float Width, float Height) {
	Aspect_ = Width / Height;
	IsProjectionDirty_ = true;
}

void CameraComponent::SetProjectionType(ProjectionType Type) {
	if (Type != ProjectionType_) {
		ProjectionType_ = Type;
	}
}

FMatrix4 CameraComponent::CreateViewMatrix(const FVector3& Eye, const FVector3& Center, const FVector3& Up) const {
	FVector3 f = (Center - Eye).normalized();  // forward
	FVector3 s = f.cross(Up).normalized();     // right
	FVector3 u = s.cross(f);                   // up

	FMatrix4 view = FMatrix4::Identity();

	view(0, 0) = s.x();
	view(0, 1) = s.y();
	view(0, 2) = s.z();
	view(0, 3) = -s.dot(Eye);

	view(1, 0) = u.x();
	view(1, 1) = u.y();
	view(1, 2) = u.z();
	view(1, 3) = -u.dot(Eye);

	view(2, 0) = -f.x();
	view(2, 1) = -f.y();
	view(2, 2) = -f.z();
	view(2, 3) = f.dot(Eye);

	return view;
}

FMatrix4 CameraComponent::CreatePerspectiveMatrix(float FovY, float Aspect, float NearPlane, float FarPlane) const {
	float tanHalfFovy = tan(FovY * M_PI / 360.0f);

	FMatrix4 proj = FMatrix4::Zero();

	proj(0, 0) = 1.0f / (Aspect * tanHalfFovy);
	proj(1, 1) = 1.0f / tanHalfFovy;
	proj(2, 2) = -(FarPlane + NearPlane) / (FarPlane - NearPlane);
	proj(2, 3) = -(2.0f * FarPlane * NearPlane) / (FarPlane - NearPlane);
	proj(3, 2) = -1.0f;

	return proj;
}

FMatrix4 CameraComponent::CreateOrthographicMatrix(float Left, float Right, 
	float Bottom, float Top, float NearPlane, float FarPlane) const {
	FMatrix4 proj = FMatrix4::Identity();

	proj(0, 0) = 2.0f / (Right - Left);
	proj(1, 1) = 2.0f / (Top - Bottom);
	proj(2, 2) = -2.0f / (FarPlane - NearPlane);

	proj(0, 3) = -(Right + Left) / (Right - Left);
	proj(1, 3) = -(Top + Bottom) / (Top - Bottom);
	proj(2, 3) = -(FarPlane + NearPlane) / (FarPlane - NearPlane);

	return proj;
}
