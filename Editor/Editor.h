#pragma once

#include "Core/IApplication.h"

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

class Editor : public IApplication {
public:
	Editor() {}
	virtual ~Editor() {}

public:
	EXPORT_API virtual bool Initialize() override;
	EXPORT_API virtual void InitScene(Scene& Sce) override;
	EXPORT_API virtual void Tick(float DeltaTime) override;
	EXPORT_API virtual void Render() override;
	EXPORT_API virtual void Shutdown() override;
};

extern "C" {
	EXPORT_API IApplication* CreateApplication() {
		return new Editor();
	}

	EXPORT_API void DestroyApplication(IApplication* app) {
		delete app;
	}
}
