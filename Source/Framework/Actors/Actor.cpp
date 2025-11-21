#include "Actor.h"
#include "Framework/Components/BaseComponent.h"

Actor::Actor() {
	Name_ = "";
}

Actor::Actor(const std::string& Name) : Name_(Name) {
	// 添加必备的Transform组件
	//m_Transform = AddComponent<TransformComponent>();
}

Actor::~Actor() {
	for (auto& Child : Children_) {
		Child.reset();
	}

	for (auto& Comp : Components_) {
		Comp.second.reset();
	}
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
