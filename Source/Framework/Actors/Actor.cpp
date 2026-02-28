#include "Actor.h"
#include "Framework/Components/BaseComponent.h"

AActor::AActor() {
	Name_ = "UnnamedActor";
}

AActor::AActor(const std::string& Name) : Name_(Name) {
	// 添加必备的Transform组件
	TransformComponent_ = CreateComponent<UTransformComponent>(this, "TransformComponent");
	if (!TransformComponent_) {
		return;
	}

}

AActor::~AActor() {
	for (auto& Child : Children_) {
		Child.reset();
	}

	for (auto& Comp : Components_) {
		Comp.second.reset();
	}
}

FVector3 AActor::GetActorLocation() const {
	if (!TransformComponent_) return FVector3();
	return TransformComponent_->GetPosition();
}

void AActor::SetActorLocation(const FVector3& Location) {
	if (!TransformComponent_) return ;
	TransformComponent_->SetPosition(Location);
}

FVector3 AActor::GetActorRotation() const {
	if (!TransformComponent_) return FVector3();
	return TransformComponent_->GetRotationEuler();
}

void AActor::SetActorRotation(const FVector3& Rotation) {
	if (!TransformComponent_) return;
	TransformComponent_->SetRotationEuler(Rotation);
}

FVector3 AActor::GetActorScale() const {
	if (!TransformComponent_) return FVector3();
	return TransformComponent_->GetScale();
}

void AActor::SetActorScale(const FVector3& Scale) {
	if (!TransformComponent_) return;
	TransformComponent_->SetScale(Scale);
}

void AActor::RotateDegress(const FVector3& Axis, float Angle) {
	if (!TransformComponent_) return;
	TransformComponent_->RotateLocal(Axis, Angle);
}

void AActor::AddChild(std::unique_ptr<AActor> child) {
	child->Parent_ = this;
	Children_.push_back(std::move(child));
}

AActor* AActor::GetParent() const { return Parent_; }
const std::vector<std::unique_ptr<AActor>>& AActor::GetChildren() const { return Children_; }

void AActor::Tick(float DeltaTime) {
	if (!Active_) return;

	// 更新所有组件
	for (auto& [type, component] : Components_) {
		if (component->IsEnabled()) {
			component->Tick(DeltaTime);
		}
	}

	// 更新子节点
	for (auto& child : Children_) {
		child->Tick(DeltaTime);
	}
}

void AActor::SetActive(bool Active) { Active_ = Active; }
bool AActor::IsActive() const { return Active_; }

const std::string& AActor::GetName() const { return Name_; }
