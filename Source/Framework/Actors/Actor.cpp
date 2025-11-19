#include "Actor.h"
#include "Framework/Components/BaseComponent.h"

Actor::Actor() {
	Name_ = "";
}

Actor::Actor(const std::string& Name) : Name_(Name) {
	// 添加必备的Transform组件
	//m_Transform = AddComponent<TransformComponent>();
}

template<typename T, typename... Args>
T* Actor::AddComponent(Args&&... args) {
	static_assert(std::is_base_of<Component, T>::value,
		"T must derive from Component");

	auto component = std::make_unique<T>(std::forward<Args>(args)...);
	T* ptr = component.get();

	component->SetOwner(this);
	Components_[typeid(T)] = std::move(component);

	ptr->OnAttach();
	return ptr;
}

template<typename T>
T* Actor::GetComponent() const {
	auto it = Components_.find(typeid(T));
	if (it != Components_.end()) {
		return static_cast<T*>(it->second.get());
	}
	return nullptr;
}

template<typename T>
void Actor::RemoveComponent() {
	auto it = Components_.find(typeid(T));
	if (it != Components_.end()) {
		it->second->OnDetach();
		Components_.erase(it);
	}
}

template<typename T>
bool Actor::HasComponent() const {
	return Components_.find(typeid(T)) != Components_.end();
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