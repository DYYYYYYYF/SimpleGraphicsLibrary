#pragma once

#include "File.h"
#include "Core/BaseMath.h"
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
	template<typename T>
	ENGINE_PLATFORM_API void SetArray(const std::string& key, const std::vector<T>& values);
	// 设置向量
	ENGINE_PLATFORM_API void SetVector2(const std::string& key, const FVector2& value);
	ENGINE_PLATFORM_API void SetVector3(const std::string& key, const FVector3& value);
	ENGINE_PLATFORM_API void SetVector4(const std::string& key, const FVector4& value);

	// 获取向量（带默认值）
	ENGINE_PLATFORM_API FVector2 GetVector2(const std::string& key, const FVector2& defaultValue = FVector2()) const;
	ENGINE_PLATFORM_API FVector3 GetVector3(const std::string& key, const FVector3& defaultValue = FVector3()) const;
	ENGINE_PLATFORM_API FVector4 GetVector4(const std::string& key, const FVector4& defaultValue = FVector4()) const;

	// 设置矩阵
	ENGINE_PLATFORM_API void SetMatrix3(const std::string& key, const FMatrix3& value);
	ENGINE_PLATFORM_API void SetMatrix4(const std::string& key, const FMatrix4& value);

	// 获取矩阵（带默认值）
	ENGINE_PLATFORM_API FMatrix3 GetMatrix3(const std::string& key, const FMatrix3& defaultValue = FMatrix3()) const;
	ENGINE_PLATFORM_API FMatrix4 GetMatrix4(const std::string& key, const FMatrix4& defaultValue = FMatrix4()) const;

	ENGINE_PLATFORM_API JsonObject Read(const std::string& path) const;
	ENGINE_PLATFORM_API std::string ReadString(const std::string& path, const std::string& defaultValue = "") const;
	ENGINE_PLATFORM_API int ReadInt(const std::string& path, int defaultValue = 0) const;
	ENGINE_PLATFORM_API float ReadFloat(const std::string& path, float defaultValue = 0.0f) const;
	ENGINE_PLATFORM_API bool ReadBool(const std::string& path, bool defaultValue = false) const;
	ENGINE_PLATFORM_API double ReadDouble(const std::string& path, double defaultValue = 0.0) const;

	ENGINE_PLATFORM_API void Write(const std::string& path, const JsonObject& value);
	ENGINE_PLATFORM_API void WriteString(const std::string& path, const std::string& value);
	ENGINE_PLATFORM_API void WriteInt(const std::string& path, int value);
	ENGINE_PLATFORM_API void WriteFloat(const std::string& path, float value);
	ENGINE_PLATFORM_API void WriteBool(const std::string& path, bool value);

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
	ENGINE_PLATFORM_API bool SaveToFile(File file) const;
	ENGINE_PLATFORM_API bool LoadFromFile(const File& file);

	// 实用方法
	ENGINE_PLATFORM_API bool IsEmpty() const;
	ENGINE_PLATFORM_API JsonObject Clone() const;
	ENGINE_PLATFORM_API void Merge(const JsonObject& other);

	ENGINE_PLATFORM_API size_t Size() const;
	ENGINE_PLATFORM_API std::string Dump(int indent = -1) const;
	ENGINE_PLATFORM_API void Clear();

private:
	std::vector<std::string> SplitPath(const std::string& path) const;

private:
	struct JsonHandle;
	std::shared_ptr<JsonHandle> Handle;

};