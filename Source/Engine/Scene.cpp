#include "Scene.h"
#include "Framework/Actors/Actor.h"

#include <Logger.hpp>

bool Scene::Initialize() {
	Name_ = "Default";

	LOG_INFO << "Scene '" << Name_ << "' init successful.";
	return true;
}

void Scene::AddToScene(std::shared_ptr<Actor> Act) { 
	AllActrors_.push_back(Act); 
}

std::vector<std::shared_ptr<Actor>> Scene::GetAllActors() const { 
	return AllActrors_; 
}

void Scene::Clear() {
	for (auto& Act : AllActrors_) {
		if (Act) {
			Act.reset();
		}
	}
}
