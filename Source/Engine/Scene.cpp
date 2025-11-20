#include "Scene.h"
#include "Framework/Actors/Actor.h"

void Scene::AddToScene(std::shared_ptr<Actor> Act) { 
	AllActrors_.push_back(Act); 
}

std::vector<std::shared_ptr<Actor>> Scene::GetAllActors() const { 
	return AllActrors_; 
}
