#pragma once

#include "RenderModuleAPI.h"
#include <string>
#include <memory>

class IMaterial;

class MaterialLoader{
public:
	static bool Load(const std::string& FilePath, struct MaterialDesc& Desc);
};