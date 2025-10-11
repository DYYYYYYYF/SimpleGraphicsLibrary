#ifdef __APPLE__

#include "DLL/DynamicLibrary.h"
#include <dlfcn.h>

DynamicLibrary::~DynamicLibrary() {
	Unload();
}

bool DynamicLibrary::Load(const std::string& path) {
	if (m_Handle) {
		Unload();
	}

	// RTLD_LAZY: 延迟绑定
	// RTLD_NOW: 立即解析所有符号
	m_Handle = dlopen(path.c_str(), RTLD_NOW);

	if (!m_Handle) {
		m_LastError = dlerror();
		return false;
	}

	m_LastError.clear();
	return true;
}

void DynamicLibrary::Unload() {
	if (m_Handle) {
		dlclose(m_Handle);
		m_Handle = nullptr;
	}
}

void* DynamicLibrary::GetFunction(const std::string& name) {
	if (!m_Handle) {
		m_LastError = "Library not loaded";
		return nullptr;
	}

	// 清除之前的错误
	dlerror();

	void* func = dlsym(m_Handle, name.c_str());

	const char* error = dlerror();
	if (error) {
		m_LastError = error;
		return nullptr;
	}

	return func;
}

#endif