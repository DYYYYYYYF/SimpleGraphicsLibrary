#include "Editor.h"
#include "Logger.hpp"

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

extern "C" {
	EXPORT_API IApplication* CreateApplication() {
		return new Editor();
	}

	EXPORT_API void DestroyApplication(IApplication* app) {
		delete app;
	}
}

bool Editor::Initialize()
{
	AppName_ = "Editor";

	LOG_INFO << "Editor init successfully.";
	return true;
}

void Editor::Tick()
{

}

void Editor::Render()
{
	
}

void Editor::Shutdown()
{
	LOG_INFO << "Editor shutdown.";
}

