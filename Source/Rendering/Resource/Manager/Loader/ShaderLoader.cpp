#include "ShaderLoader.h"
#include "Resource/IShader.h"
#include "Platform/File/JsonObject.h"
#include <Logger.hpp>

#define SHADER_NAME_KEY "Name"
#define SHADER_STAGE_KEY "Stages"
#define SHADER_STAGE_NAME_KEY "Name"
#define SHADER_STAGE_SOURCE_KEY "Source"

bool ShaderLoader::Load(const std::string& FilePath, struct ShaderDesc& Desc) {
	File MaterialAsset(SHADER_CONFIG_PATH + FilePath);
	if (!MaterialAsset.IsExist()) {
		return false;
	}

	JsonObject Content = JsonObject(MaterialAsset.ReadBytes());
	Desc.Name = Content.Get(SHADER_NAME_KEY).GetString();

	JsonObject StageParams = Content.Get(SHADER_STAGE_KEY);
	for (size_t i = 0; i < StageParams.Size(); ++i) {
		JsonObject StageDesc = StageParams.ArrayItemAt(i);
		const std::string& Key = StageDesc.Get(SHADER_STAGE_NAME_KEY).GetString();
		const std::string& Value = StageDesc.Get(SHADER_STAGE_SOURCE_KEY).GetString();

		ShaderStage StageType = ConvertStageType(Key);
		if (StageType == ShaderStage::eUnknow) {
			continue;
		}

		Desc.Stages.insert(std::make_pair(StageType, Value));
	}

	return true;
}

ShaderStage ShaderLoader::ConvertStageType(const std::string& StageString) {
	if (StageString.compare("vert") == 0) {
		return ShaderStage::eVertex;
	}
	else if(StageString.compare("frag") == 0)
	{
		return ShaderStage::eFragment;
	}

	return ShaderStage::eUnknow;
}