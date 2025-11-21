#include "JsonObject.h"
#include <nlohmann/json.hpp>

struct JsonObject::JsonHandle {
	nlohmann::json value;
};

JsonObject::JsonObject() : Handle(std::make_shared<JsonHandle>()) {}

JsonObject::JsonObject(File file) : Handle(std::make_shared<JsonHandle>()) {
	if (file.IsExist()) {
		Handle->value = JsonObject(file.ReadBytes()).Handle->value;
	}
	else {
		Handle->value = nlohmann::json();
	}
}

JsonObject::JsonObject(JsonObject::Type type) : Handle(std::make_shared<JsonHandle>()) {
	switch (type)
	{
	case JsonObject::Type::eObject: Handle->value = nlohmann::json::object(); break;
	case JsonObject::Type::eArray:  Handle->value = nlohmann::json::array();  break;
	}
}

JsonObject::JsonObject(const std::string& content) : Handle(std::make_shared<JsonHandle>()) {
	try {
		Handle->value = nlohmann::json::parse(content);
	}
	catch (...) {
		Handle->value = nlohmann::json(); 
	}
}

bool JsonObject::IsObject() const {
	return Handle->value.is_object();
}

bool JsonObject::IsArray() const {
	return Handle->value.is_array();
}

bool JsonObject::IsString() const {
	return Handle->value.is_string();
}

bool JsonObject::IsNull() const {
	return Handle->value.is_null();
}

bool JsonObject::IsBool() const {
	return Handle->value.is_boolean();
}

bool JsonObject::IsNumber() const {
	return Handle->value.is_number();
}

std::string JsonObject::GetString() const {
	if (Handle->value.is_string())
		return Handle->value.get<std::string>();
	return "";
}

JsonObject JsonObject::ArrayItemAt(size_t index) const {
	JsonObject Obj;
	if (IsArray()) {
		Obj.Handle->value = Handle->value.at(index);
	}
	else
	{
		Obj.Handle->value = nlohmann::json();
	}
	return Obj;
}

void JsonObject::ArrayPush(const JsonObject& value) {
	if (IsArray()) {
		Handle->value.push_back(value.Handle->value);
	}
}

bool JsonObject::ArrayRemoveAt(size_t index) {
	if (IsArray() && index < Handle->value.size()) {
		Handle->value.erase(Handle->value.begin() + index);
		return true;
	}
	return false;
}

bool JsonObject::HasKey(const std::string& key) const {
	if (!IsObject()) return false;
	return Handle->value.find(key) != Handle->value.end();
}

bool JsonObject::Remove(const std::string& key) {
	if (!IsObject()) return false;
	return Handle->value.erase(key) > 0;
}

std::vector<std::string> JsonObject::GetKeys() const {
	std::vector<std::string> keys;
	if (IsObject()) {
		for (auto it = Handle->value.begin(); it != Handle->value.end(); ++it) {
			keys.push_back(it.key());
		}
	}
	return keys;
}

bool JsonObject::SaveToFile(File file) const {
	try {
		std::string content = Dump(2);
		file.WriteBytes(content.c_str(), content.size());
		return true;
	}
	catch (...) {
		return false;
	}
}

bool JsonObject::LoadFromFile(const File& file) {
	try {
		if (!file.IsExist()) return false;
		std::string content = file.ReadBytes();
		Handle->value = nlohmann::json::parse(content);
		return true;
	}
	catch (...) {
		return false;
	}
}

bool JsonObject::IsEmpty() const {
	return Handle->value.empty();
}

JsonObject JsonObject::Clone() const {
	JsonObject copy;
	copy.Handle->value = Handle->value;
	return copy;
}

void JsonObject::Merge(const JsonObject& other) {
	if (IsObject() && other.IsObject()) {
		Handle->value.update(other.Handle->value);
	}
}

size_t JsonObject::Size() const {
	return Handle->value.size();
}

std::string JsonObject::Dump(int indent) const {
	return Handle->value.dump(indent);
}

float JsonObject::GetFloat() const {
	if (Handle->value.is_number())
		return Handle->value.get<float>();
	return 0.0f;
}

bool JsonObject::GetBool(bool defaultValue) const {
	if (Handle->value.is_boolean())
		return Handle->value.get<bool>();
	return defaultValue;
}

int JsonObject::GetInt(int defaultValue) const {
	if (Handle->value.is_number())
		return Handle->value.get<int>();
	return defaultValue;
}

double JsonObject::GetDouble(double defaultValue) const {
	if (Handle->value.is_number())
		return Handle->value.get<double>();
	return defaultValue;
}

JsonObject JsonObject::Get(const std::string& key) const {
	JsonObject Obj;
	auto it = Handle->value.find(key);
	if (it != Handle->value.end()) {
		Obj.Handle->value = *it;
	}
	else {
		Obj.Handle->value = nlohmann::json();
	}
	return Obj;
}

void JsonObject::SetString(const std::string& key, const std::string& value) {
	Handle->value[key] = value;
}

void JsonObject::SetInt(const std::string& key, int value) {
	Handle->value[key] = value;
}

void JsonObject::SetFloat(const std::string& key, float value) {
	Handle->value[key] = value;
}

void JsonObject::SetBool(const std::string& key, bool value) {
	Handle->value[key] = value;
}

void JsonObject::Set(const std::string& key, const JsonObject& value) {
	Handle->value[key] = value.Handle->value;
}

void JsonObject::Clear() {
	Handle->value = nlohmann::json();
}