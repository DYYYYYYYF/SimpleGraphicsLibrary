#include "Actor.h"
#include "Framework/Components/BaseComponent.h"

Actor::Actor() {
	Name_ = "";
}

Actor::Actor(const std::string& Name) : Name_(Name) {
	// 添加必备的Transform组件
<<<<<<< Updated upstream
	//m_Transform = AddComponent<TransformComponent>();
=======
	TransformComponent_ = CreateComponent<TransformComponent>(this, "TransformComponent");
	if (!TransformComponent_) {
		return;
	}

}

Actor::~Actor() {
	for (auto& Child : Children_) {
		Child.reset();
	}

	for (auto& Comp : Components_) {
		Comp.second.reset();
	}
}

FVector3 Actor::GetActorLocation() const {
	if (!TransformComponent_) return FVector3();
	return TransformComponent_->GetPosition();
}

void Actor::SetActorLocation(const FVector3& Location) {
	if (!TransformComponent_) return ;
	TransformComponent_->SetPosition(Location);
}

FVector3 Actor::GetActorRotation() const {
	if (!TransformComponent_) return FVector3();
	return TransformComponent_->GetRotationEuler();
}

void Actor::SetActorRotation(const FVector3& Rotation) {
	if (!TransformComponent_) return;
	TransformComponent_->SetRotationEuler(Rotation);
}

FVector3 Actor::GetActorScale() const {
	if (!TransformComponent_) return FVector3();
	return TransformComponent_->GetScale();
}

void Actor::SetActorScale(const FVector3& Scale) {
	if (!TransformComponent_) return;
	TransformComponent_->SetScale(Scale);
>>>>>>> Stashed changes
}

void Actor::AddChild(std::unique_ptr<Actor> child) {
	child->Parent_ = this;
	Children_.push_back(std::move(child));
}

Actor* Actor::GetParent() const { return Parent_; }
const std::vector<std::unique_ptr<Actor>>& Actor::GetChildren() const { return Children_; }

void Actor::Tick(float DeltaTime) {
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

void Actor::SetActive(bool Active) { Active_ = Active; }
bool Actor::IsActive() const { return Active_; }

const std::string& Actor::GetName() const { return Name_; }
