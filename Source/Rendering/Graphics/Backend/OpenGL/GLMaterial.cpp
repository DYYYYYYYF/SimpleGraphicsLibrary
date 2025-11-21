#include "GLMaterial.h"

#include "Resource/IShader.h"
#include "Resource/ITexture.h"
#include "GLShader.h"
#include "Platform/File/JsonObject.h"
#include <Logger.hpp>

GLMaterial::GLMaterial() { Name_ = ""; }

GLMaterial::GLMaterial(const std::string& filename) {
	if (!Load(filename)) {
		return;
	}

	IsValid_ = true;
}

GLMaterial::~GLMaterial() {
	Unload();
}

bool GLMaterial::Load(const std::string& filename) {
	File MaterialFile("../Assets/Materials" + filename);
	if (!MaterialFile.IsExist()) {
		LOG_ERROR << "Material file '" << filename << "' is not exist!";
		return false;
	}

	JsonObject MaterialObj(MaterialFile);
	if (!MaterialObj.IsObject()) {
		LOG_ERROR << "Material file '" << filename << "' is invalid json!";
		return false;
	}

	// 加载数据
	Name_ = MaterialObj.Get("Name").GetString();

	JsonObject UBOData = MaterialObj.Get("MaterialUBO");
	if (!UBOData.IsObject()) {
		LOG_ERROR << "Material file '" << filename << "' is invalid json!";
		return false;
	}

	JsonObject AlbedoData = UBOData.Get("Albedo");
	if (AlbedoData.IsArray() && AlbedoData.Size() == 4) {
		MaterialUBO_.Albedo_ = FVector4(
			AlbedoData.ArrayItemAt(0).GetFloat(),
			AlbedoData.ArrayItemAt(1).GetFloat(),
			AlbedoData.ArrayItemAt(2).GetFloat(),
			AlbedoData.ArrayItemAt(3).GetFloat()
		);
	}

	JsonObject EmissiveData = UBOData.Get("Emissive");
	if (EmissiveData.IsArray() && AlbedoData.Size() == 4) {
		MaterialUBO_.Albedo_ = FVector4(
			EmissiveData.ArrayItemAt(0).GetFloat(),
			EmissiveData.ArrayItemAt(1).GetFloat(),
			EmissiveData.ArrayItemAt(2).GetFloat(),
			EmissiveData.ArrayItemAt(3).GetFloat()
		);
	}

	JsonObject MetallicRoughnessAOData = UBOData.Get("MetallicRoughnessAO");
	if (MetallicRoughnessAOData.Get("MetallicRoughnessAO").IsArray() && AlbedoData.Size() == 4) {
		MaterialUBO_.Albedo_ = FVector4(
			MetallicRoughnessAOData.ArrayItemAt(0).GetFloat(),
			MetallicRoughnessAOData.ArrayItemAt(1).GetFloat(),
			MetallicRoughnessAOData.ArrayItemAt(2).GetFloat(),
			MetallicRoughnessAOData.ArrayItemAt(3).GetFloat()
		);
	}

	std::string ShaderAsset = MaterialObj.Get("UsedShader").GetString();
	Shader_ = std::make_shared<GLShader>(filename);
	if (!Shader_) {
		return false;
	}

	// 创建Buffer
	glGenBuffers(1, &UBO_);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO_);

	LOG_DEBUG << "Material '" << Name_ << "' loaded.";
	IsValid_ = true;
	return true;
}

void GLMaterial::Unload() {
	if (Shader_) {
		Shader_->Unload();
		Shader_.reset();
	}

	if (UBO_ != NULL) {
		glDeleteBuffers(1, &UBO_);
	}

	LOG_DEBUG << "Material '" << Name_ << "' unloaded.";
}

void GLMaterial::Apply() const {
	if (!Shader_ || !Shader_->IsValid()) return;

	Shader_->Bind();

	// 设置标量参数
	ApplyUniformBuffer();
	// 绑定纹理
	ApplyTextures();
}

void GLMaterial::ApplyUniformBuffer() const {
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUBO), &MaterialUBO_, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO_);	// param2对应的binding=0
}

void GLMaterial::ApplyTextures() const {
	for (const auto& [slot, texture] : Textures_) {
		if (texture && texture->IsValid()) {
			uint32_t binding = static_cast<uint32_t>(slot);

			// 核心：直接使用槽位枚举值作为binding
			texture->Bind(binding);

			// 设置纹理启用标志
			SetTextureFlag(slot, true);
		}
		else {
			SetTextureFlag(slot, false);
		}
	}
}

void GLMaterial::SetTextureFlag(TextureSlot slot, bool enabled) const {
	switch (slot) {
	case TextureSlot::eAlbedo:
		Shader_->SetInt("material.hasAlbedoMap", enabled ? 1 : 0);
		break;
	case TextureSlot::eNormal:
		Shader_->SetInt("material.hasNormalMap", enabled ? 1 : 0);
		break;
	case TextureSlot::eMetallic:
		Shader_->SetInt("material.hasMetallicMap", enabled ? 1 : 0);
		break;
	case TextureSlot::eRoughness:
		Shader_->SetInt("material.hasRoughnessMap", enabled ? 1 : 0);
		break;
	case TextureSlot::eAO:
		Shader_->SetInt("material.hasAOMap", enabled ? 1 : 0);
		break;
	case TextureSlot::eEmissive:
		Shader_->SetInt("material.hasEmissiveMap", enabled ? 1 : 0);
		break;
		// ... 其他纹理类型
	}
}

void GLMaterial::Unbind() const {
	if (Shader_) {
		Shader_->Unbind();
	}
}
