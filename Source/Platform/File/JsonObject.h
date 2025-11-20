#pragma once

#include <string>
#include <memory>

class JsonObject {
public:
	enum class Type { eNull, eBool, eNumber, eString, eObject, eArray };

public:
	JsonObject();
	JsonObject(JsonObject::Type type);
	JsonObject(const std::string& content);

public:

	// 引擎API（与任何库无关）
	bool IsObject() const;
	bool IsArray() const;
	bool IsString() const;

	float GetFloat() const;
	std::string GetString() const;

	std::string Dump(int indent = -1) const;

	JsonObject Get(const std::string& key) const;
	void Set(const std::string& key, const JsonObject& value);

	void Clear();

private:
	struct JsonHandle;
	std::shared_ptr<JsonHandle> Handle;

};