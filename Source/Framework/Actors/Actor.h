#pragma once

#include "BaseObject.h"
#include "Framework/Components/Component.h"

#include <vector>
#include <string>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

class Actor : public BaseObject {
public:
	ENGINE_FRAMEWORK_API Actor();
	ENGINE_FRAMEWORK_API Actor(const std::string& Name);

	// 虚函数
public:
	ENGINE_FRAMEWORK_API virtual void BeginPlay() {};
	ENGINE_FRAMEWORK_API virtual void Tick(float DeltaTime);

	// -------------------------- 通用函数 ------------------------------------
public:
	ENGINE_FRAMEWORK_API void AddChild(std::unique_ptr<Actor> Child);
	ENGINE_FRAMEWORK_API Actor* GetParent() const;
	ENGINE_FRAMEWORK_API const std::vector<std::unique_ptr<Actor>>& GetChildren() const;
	ENGINE_FRAMEWORK_API void SetActive(bool Active);
	ENGINE_FRAMEWORK_API bool IsActive() const;
	ENGINE_FRAMEWORK_API const std::string& GetName() const;


	template<typename T, typename... Args>
	T* AddComponent(Args&&... args) {
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
	T* GetComponent() const {
		auto it = Components_.find(typeid(T));
		if (it != Components_.end()) {
			return static_cast<T*>(it->second.get());
		}
		return nullptr;
	}

	template<typename T>
	void RemoveComponent() {
		auto it = Components_.find(typeid(T));
		if (it != Components_.end()) {
			it->second->OnDetach();
			Components_.erase(it);
		}
	}

	template<typename T>
	bool HasComponent() const {
		return Components_.find(typeid(T)) != Components_.end();
	}


private:
	std::string Name_;
	bool Active_ = true;

	// 组件存储（按类型索引）
	std::unordered_map<std::type_index, std::unique_ptr<Component>> Components_;

	// 必备组件（每个Actor都有）
	//TransformComponent* m_Transform;

	// 层次结构
	Actor* Parent_ = nullptr;
	std::vector<std::unique_ptr<Actor>> Children_;

};