// DynamicLibrary_Win32.cpp
#ifdef _WIN32

#include "DLL/DynamicLibrary.h"
#include <windows.h>

DynamicLibrary::~DynamicLibrary() {
	Unload();
}

bool DynamicLibrary::Load(const std::string& path) {
	if (m_Handle) {
		Unload();
	}

	m_Handle = LoadLibraryA(path.c_str());

	if (!m_Handle) {
		// 这个使用全局 :: 即使用<windows.h>中的方法而非类内方法
		DWORD error = ::GetLastError();
		char buffer[256];
		FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			error,
			0,
			buffer,
			sizeof(buffer),
			nullptr
		);
		m_LastError = buffer;
		return false;
	}

	m_LastError.clear();
	return true;
}

void DynamicLibrary::Unload() {
	if (m_Handle) {
		FreeLibrary(static_cast<HMODULE>(m_Handle));
		m_Handle = nullptr;
	}
}

void* DynamicLibrary::GetFunction(const std::string& name) {
	if (!m_Handle) {
		m_LastError = "Library not loaded";
		return nullptr;
	}

	void* func = GetProcAddress(static_cast<HMODULE>(m_Handle), name.c_str());

	if (!func) {
		m_LastError = "Function not found: " + name;
	}

	return func;
}

#endif // _WIN32