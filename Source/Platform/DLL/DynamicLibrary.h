#pragma once

#include "../PlatformMoudleAPI.h"

#include <string>

class DynamicLibrary {
public:
	ENGINE_PLATFORM_API DynamicLibrary() = default;
	ENGINE_PLATFORM_API virtual ~DynamicLibrary();

	// ��ֹ����
	DynamicLibrary(const DynamicLibrary&) = delete;
	DynamicLibrary& operator=(const DynamicLibrary&) = delete;

	// ���ض�̬��
	ENGINE_PLATFORM_API bool Load(const std::string& path);
	// ж�ض�̬��
	ENGINE_PLATFORM_API void Unload();
	// ��ȡ����ָ��
	ENGINE_PLATFORM_API void* GetFunction(const std::string& name);
	// ����Ƿ��Ѽ���
	ENGINE_PLATFORM_API bool IsLoaded() const { return m_Handle != nullptr; }
	// ��ȡ������Ϣ
	ENGINE_PLATFORM_API std::string GetLastError() const { return m_LastError; }

	// ģ�巽��������ʹ��
	template<typename T>
	T GetFunction(const std::string& name) {
		return reinterpret_cast<T>(GetFunction(name));
	}

private:
	void* m_Handle = nullptr;
	std::string m_LastError;
};