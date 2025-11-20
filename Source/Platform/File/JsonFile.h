#pragma once

#include "File.h"
#include "JsonObject.h"

class JsonFile : public File {
public:
	ENGINE_PLATFORM_API JsonFile(const std::string& filename);
	ENGINE_PLATFORM_API ~JsonFile();



private:
	JsonObject JsonObj;

};