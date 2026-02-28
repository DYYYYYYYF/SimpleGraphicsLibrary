#pragma once

#include "Component.h"
#include <string>

class AActor;

class UBaseComponent : public UComponent {
public:
	UBaseComponent() = default;

public:
	ENGINE_FRAMEWORK_API UBaseComponent(AActor* Owner, const std::string& Name) : Name_(Name) { SetOwner(Owner); }

	ENGINE_FRAMEWORK_API virtual void OnAttach() override {}
	ENGINE_FRAMEWORK_API virtual void OnDetach() override {}
	ENGINE_FRAMEWORK_API virtual void OnEnable() override {}
	ENGINE_FRAMEWORK_API virtual void OnDisable() override {}

	ENGINE_FRAMEWORK_API virtual void Tick(float DeltaTime) { (void)DeltaTime; };

	ENGINE_FRAMEWORK_API AActor* GetOwner() const { return Owner_; }
	ENGINE_FRAMEWORK_API void SetOwner(AActor* Owner) { Owner_ = Owner; }

	ENGINE_FRAMEWORK_API bool IsEnabled() const { return IsEnabled_; }
	ENGINE_FRAMEWORK_API void SetEnabled(bool Enabled) {
		if (IsEnabled_ != Enabled) {
			IsEnabled_ = Enabled;
			Enabled ? OnEnable() : OnDisable();
		}
	}

protected:
	std::string Name_;
	bool IsEnabled_ = true;

private:
	AActor* Owner_ = nullptr;

};