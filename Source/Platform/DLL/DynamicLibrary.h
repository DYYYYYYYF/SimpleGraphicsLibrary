#pragma once

#include "../PlatformMoudleAPI.h"

#include <string>

class DynamicLibrary {
public:
	ENGINE_PLATFORM_API DynamicLibrary() = default;
	ENGINE_PLATFORM_API virtual ~DynamicLibrary();

	// 禁止拷贝
	DynamicLibrary(const DynamicLibrary&) = delete;
	DynamicLibrary& operator=(const DynamicLibrary&) = delete;

	// 加载动态库
	ENGINE_PLATFORM_API bool Load(const std::string& path);
	// 卸载动态库
	ENGINE_PLATFORM_API void Unload();
	// 获取函数指针
	ENGINE_PLATFORM_API void* GetFunction(const std::string& name);
	// 检查是否已加载
	ENGINE_PLATFORM_API bool IsLoaded() const { return m_Handle != nullptr; }
	// 获取错误信息
	ENGINE_PLATFORM_API std::string GetLastError() const { return m_LastError; }

	// 模板方法，方便使用
	template<typename T>
	T GetFunction(const std::string& name) {
		return reinterpret_cast<T>(GetFunction(name));
	}

private:
	void* m_Handle = nullptr;
	std::string m_LastError;
};