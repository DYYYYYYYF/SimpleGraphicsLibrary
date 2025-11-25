#pragma once

#include "RenderModuleAPI.h"
#include <string>

class IMaterial;

class MaterialLoader{
public:
	static bool Load(const std::string& FilePath, struct MaterialDesc& Desc);
};