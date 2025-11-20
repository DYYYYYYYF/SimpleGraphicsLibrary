#pragma once

#include "EngineModuleAPI.h"
#include <vector>
#include <memory>

class Actor;

class Scene {
public:
	ENGINE_ENGINE_API void AddToScene(std::shared_ptr<Actor> Act);
	ENGINE_ENGINE_API std::vector<std::shared_ptr<Actor>> GetAllActors() const;

private:
	std::vector<std::shared_ptr<Actor>> AllActrors_;

};