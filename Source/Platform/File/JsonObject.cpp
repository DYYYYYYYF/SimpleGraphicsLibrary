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

void JsonObject::Set(const std::string& key, const JsonObject& value) {
	Handle->value[key] = value.Handle->value;
}

void JsonObject::Clear() {
	Handle->value = nlohmann::json();
}