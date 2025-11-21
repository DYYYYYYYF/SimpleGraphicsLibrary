#pragma once

#include "EngineModuleAPI.h"

class Window;
class IApplication;
class Renderer;
class Scene;

class Engine {
public:
	ENGINE_ENGINE_API Engine();
	ENGINE_ENGINE_API virtual ~Engine() = default;

	ENGINE_ENGINE_API static Engine& GetInstance();

public:
	ENGINE_ENGINE_API bool Initialize(IApplication* app);
	ENGINE_ENGINE_API void Run();
	ENGINE_ENGINE_API void Shutdown();

protected:
	Window* Window_;

	IApplication* Application_;
	bool Running_;

	Renderer* CoreRenderer;

	Scene* Scene_;
};
