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
	// 引擎API（与任何库无关）
	ENGINE_PLATFORM_API bool IsObject() const;
	ENGINE_PLATFORM_API bool IsArray() const;
	ENGINE_PLATFORM_API bool IsString() const;

	ENGINE_PLATFORM_API float GetFloat() const;
	ENGINE_PLATFORM_API std::string GetString() const;

	ENGINE_PLATFORM_API JsonObject ArrayItemAt(size_t index) const;
	ENGINE_PLATFORM_API size_t Size() const;

	ENGINE_PLATFORM_API std::string Dump(int indent = -1) const;

	ENGINE_PLATFORM_API JsonObject Get(const std::string& key) const;
	ENGINE_PLATFORM_API void Set(const std::string& key, const JsonObject& value);

	ENGINE_PLATFORM_API void Clear();

private:
	struct JsonHandle;
	std::shared_ptr<JsonHandle> Handle;

};