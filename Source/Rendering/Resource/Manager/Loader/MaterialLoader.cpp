#include "MaterialLoader.h"
#include "Resource/IMaterial.h"
#include "Platform/File/JsonObject.h"
#include <Logger.hpp>

bool MaterialLoader::Load(const std::string& FilePath, struct MaterialDesc& Desc) {
	File MaterialAsset(MATERIAL_CONFIG_PATH + FilePath);
	if (!MaterialAsset.IsExist()) {
		return false;
	}

	JsonObject Content = JsonObject(MaterialAsset.ReadBytes());
	Desc.Name = Content.Get("Name").GetString();
	Desc.ShaderPath = Content.Get("UsedShader").GetString();

	JsonObject MaterialParams = Content.Get("MaterialParams");
	std::vector<std::string> Keys = MaterialParams.GetKeys();
	for (size_t i=0 ;i< MaterialParams.Size(); ++i){
		const std::string& Key = Keys[i];
		const JsonObject& Value = MaterialParams.Get(Key);

		std::string Dump = Value.Dump();

		MaterialValue MatVal;
		if (Value.IsNumber()) {
			MatVal.type = MaterialValue::Type::Float;
			MatVal.data = { Value.GetFloat() };
		}
		else if (Value.IsArray()) {
			size_t size = Value.Size();
			MatVal.data.resize(size);
			for (size_t ni = 0; ni < size; ++ni)
				MatVal.data[ni] = Value.ArrayItemAt(ni).GetFloat();

			if (size == 2) MatVal.type = MaterialValue::Type::Vector2;
			else if (size == 3) MatVal.type = MaterialValue::Type::Vector3;
			else if (size == 4) MatVal.type = MaterialValue::Type::Vector4;
			else if (size == 16) MatVal.type = MaterialValue::Type::Matrix4;
		}

		Desc.Uniforms[Key] = MatVal;
	}

	return true;
}