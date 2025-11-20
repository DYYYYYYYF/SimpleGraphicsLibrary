#include "JsonFile.h"

JsonFile::JsonFile(const std::string& filename) : File(filename) {
	if (!IsExist()) {
		return;
	}

	JsonObj = JsonObject(ReadBytes());
	JsonObject Name = JsonObj.Get("Name");
	if (Name.IsString()) {
		std::string n = Name.GetString();
		n = "";
	}
}

JsonFile::~JsonFile() {
	JsonObj.Clear();
}