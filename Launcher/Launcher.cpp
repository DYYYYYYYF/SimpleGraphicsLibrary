#include "Editor.h"
#include "Logger.hpp"
#include "Engine/Engine.h"
#include "Platform/DLL/DynamicLibrary.h"

int main(int argc, char** argv) {
	bool isEditor = false;
	if (argc > 1) {
		// ����������
		for (int i = 1; i < argc; ++i) {
			if (strcmp(argv[i], "-editor") == 0) {
				isEditor = true;
			}
		}
	}
	else {
		std::cout << "Select launcher mode: \n1. Editor \n2. Game \n";
		int mode = -1; 
		std::cin >> mode;
		if (mode == 1) {
			isEditor = true;
		}
	}
	
	// ����ƽ̨ѡ��DLL����
#ifdef _WIN32
	const char* libName = isEditor ? "Editor.dll" : "Game.dll";
#elif __APPLE__
	const char* libName = isEditor ? "libEditor.dylib" : "libGame.dylib";
#endif

	// ���ض�̬��
	DynamicLibrary appLibrary;
	if (!appLibrary.Load(libName)) {
		LOG_ERROR << "Failed to load library: " << appLibrary.GetLastError();
		return -1;
	}

	// ��ȡ��������
	typedef IApplication* (*CreateAppFunc)();
	CreateAppFunc createApp = appLibrary.GetFunction<CreateAppFunc>("CreateApplication");

	if (!createApp) {
		LOG_ERROR << "Failed to get CreateApplication: " << appLibrary.GetLastError();
		return -1;
	}

	// ����Ӧ�ó���
	IApplication* app = createApp();
	if (!app) {
		LOG_ERROR << "Failed to create application";
		return -1;
	}

	// ��������
	Engine& engine = Engine::GetInstance();
	if (engine.Initialize(app)) {
		engine.Run();
	}
	engine.Shutdown();

	// ��ȡ���ٺ�������ѡ������ȫ��
	typedef void (*DestroyAppFunc)(IApplication*);
	DestroyAppFunc destroyApp = appLibrary.GetFunction<DestroyAppFunc>("DestroyApplication");

	if (destroyApp) {
		destroyApp(app);
	}
	else {
		delete app;
	}

	return 0;
}