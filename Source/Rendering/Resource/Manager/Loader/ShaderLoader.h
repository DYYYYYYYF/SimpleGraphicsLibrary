#pragma once

#include "RenderModuleAPI.h"
#include <string>

enum class ShaderStage;

class ShaderLoader {
public:
	static bool Load(const std::string& FilePath, struct ShaderDesc& Desc);

private:
	static ShaderStage ConvertStageType(const std::string& StageString);
};