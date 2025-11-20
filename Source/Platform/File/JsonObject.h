#pragma once

#include "File.h"
#include <string>
#include <memory>

class JsonObject {
public:
	enum class Type { eNull, eBool, eNumber, eString, eObject, eArray };

public:
	ENGINE_PLATFORM_API JsonObject();
	ENGINE_PLATFORM_API JsonObject(File file);
	ENGINE_PLATFORM_API JsonObject(JsonObject::Type type);
	ENGINE_PLATFORM_API JsonObject(const std::string& content);

public:
	// ---------------------- 引擎API（与任何库无关）--------------------------
	// 类型检查
	ENGINE_PLATFORM_API bool IsObject() const;
	ENGINE_PLATFORM_API bool IsArray() const;
	ENGINE_PLATFORM_API bool IsString() const;
	ENGINE_PLATFORM_API bool IsNull() const;
	ENGINE_PLATFORM_API bool IsBool() const;
	ENGINE_PLATFORM_API bool IsNumber() const;

	// 获取
	ENGINE_PLATFORM_API float GetFloat() const;
	ENGINE_PLATFORM_API std::string GetString() const;
	ENGINE_PLATFORM_API bool GetBool(bool defaultValue = false) const;
	ENGINE_PLATFORM_API int GetInt(int defaultValue = 0) const;
	ENGINE_PLATFORM_API double GetDouble(double defaultValue = 0.0) const;
	ENGINE_PLATFORM_API JsonObject Get(const std::string& key) const;

	// 设置
	ENGINE_PLATFORM_API void SetString(const std::string& key, const std::string& value);
	ENGINE_PLATFORM_API void SetInt(const std::string& key, int value);
	ENGINE_PLATFORM_API void SetFloat(const std::string& key, float value);
	ENGINE_PLATFORM_API void SetBool(const std::string& key, bool value);
	ENGINE_PLATFORM_API void Set(const std::string& key, const JsonObject& value);

	// 数组
	ENGINE_PLATFORM_API JsonObject ArrayItemAt(size_t index) const;
	ENGINE_PLATFORM_API void ArrayPush(const JsonObject& value);
	ENGINE_PLATFORM_API bool ArrayRemoveAt(size_t index);

	// 对象操作
	ENGINE_PLATFORM_API bool HasKey(const std::string& key) const;
	ENGINE_PLATFORM_API bool Remove(const std::string& key);
	ENGINE_PLATFORM_API std::vector<std::string> GetKeys() const;

	// 文件操作
	ENGINE_PLATFORM_API bool SaveToFile(const File& file) const;
	ENGINE_PLATFORM_API bool LoadFromFile(const File& file);

	// 实用方法
	ENGINE_PLATFORM_API bool IsEmpty() const;
	ENGINE_PLATFORM_API JsonObject Clone() const;
	ENGINE_PLATFORM_API void Merge(const JsonObject& other);

	ENGINE_PLATFORM_API size_t Size() const;
	ENGINE_PLATFORM_API std::string Dump(int indent = -1) const;
	ENGINE_PLATFORM_API void Clear();

private:
	struct JsonHandle;
	std::shared_ptr<JsonHandle> Handle;

};