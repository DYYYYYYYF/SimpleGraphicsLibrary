#include "GLMaterial.h"

#include "Resource/IShader.h"
#include "Resource/ITexture.h"
#include "GLShader.h"
#include "Platform/File/JsonObject.h"
#include "Resource/Manager/ResourceManager.h"
#include <Logger.hpp>

GLMaterial::GLMaterial() { Name_ = ""; }
GLMaterial::GLMaterial(const MaterialDesc& Desc) {
	if (!Load(Desc)) {
		return;
	}

	IsValid_ = true;
}

GLMaterial::~GLMaterial() {
	Unload();
}

bool GLMaterial::Load(const MaterialDesc& Desc) {
	// 加载Uniform数据
	Name_ = Desc.Name;
	Uniforms_ = Desc.Uniforms;

	// 加载Shader资产
	std::string ShaderAsset = Desc.ShaderPath;;
	Shader_ = DynamicCast<IShader>(ResourceManager::Instance().Acquire(ResourceType::eShader, ShaderAsset));
	if (!Shader_) {
		Shader_ = DynamicCast<IShader>(
			ResourceManager::Instance().LoadResource(ResourceType::eShader, ShaderAsset)
		);
	}

	if (!Shader_) {
		LOG_WARN << "Load shader '" << Desc.Name << "' failed! Use built-in shader!";
		Shader_ = DynamicCast<IShader>(ResourceManager::Instance().Acquire(ResourceType::eShader, BUILTIN_PBR_SHADER));
	}

	// 加载Texture资产
	Desc.TexturePaths;

	LOG_DEBUG << "Material '" << Name_ << "' loaded.";
	IsValid_ = true;
	return true;
}

void GLMaterial::Unload() {
	if (Shader_) {
		Shader_.reset();
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
	Shader_->UploadMaterial(*this);
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
