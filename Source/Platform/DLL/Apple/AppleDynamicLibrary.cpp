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

	// RTLD_LAZY: �ӳٰ�
	// RTLD_NOW: �����������з���
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

	// ���֮ǰ�Ĵ���
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