#pragma once

#include "EngineModuleAPI.h"
#include <vector>
#include <string>
#include <memory>

class Actor;

class Scene {
public:
	ENGINE_ENGINE_API bool Initialize();

public:
	ENGINE_ENGINE_API void AddToScene(std::shared_ptr<Actor> Act);
	ENGINE_ENGINE_API std::vector<std::shared_ptr<Actor>> GetAllActors() const;
	ENGINE_ENGINE_API void Clear();

	ENGINE_ENGINE_API const std::string& GetName() const { return Name_; }

private:
	std::string Name_;
	std::vector<std::shared_ptr<Actor>> AllActrors_;

};