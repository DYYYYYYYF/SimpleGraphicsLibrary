#include "GLMaterial.h"

#include "Resource/IShader.h"
#include "Resource/ITexture.h"
#include "GLShader.h"

GLMaterial::GLMaterial(const std::string& mesh) {
	Load();
}

GLMaterial::~GLMaterial() {
	Unload();
}

void GLMaterial::Load() {
	glGenBuffers(1, &UBO_);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO_);

	Shader_ = std::make_shared<GLShader>("");
}

void GLMaterial::Unload() {
	if (UBO_ != NULL) {
		glDeleteBuffers(1, &UBO_);
	}
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
