#pragma once

#include "IResource.h"
#include "Core/BaseMath.h"
#include "ITexture.h"

#include <memory>
#include <unordered_map>

class IShader;

struct MaterialValue {
	enum class Type {
		Float,
		Vector2,
		Vector3,
		Vector4,
		Matrix4
	};

	Type type;
	std::vector<float> data; // 通用存储
};

struct MaterialDesc {
	std::string Name;
	std::string ShaderPath;
	std::unordered_map<std::string, MaterialValue> Uniforms;
	std::unordered_map<TextureSlot, std::string> TexturePaths;
};

class IMaterial : public IResource {
public:
	IMaterial() { Type_ = ResourceType::eMaterial; }

public:
	struct TextureEntry {
		TextureSlot slot;
		std::shared_ptr<ITexture> texture;
		bool enabled = true;
	};

public:
	virtual bool Load(const MaterialDesc& Desc) = 0;
	virtual void Apply() const = 0;
	virtual void Unbind() const = 0;

public:
	// 纹理贴图管理
	void SetTexture(TextureSlot slot, std::shared_ptr<ITexture> texture) {
		Textures_[slot] = texture;
	}

	std::shared_ptr<ITexture> GetTexture(TextureSlot slot) const {
		auto it = Textures_.find(slot);
		return (it != Textures_.end()) ? it->second : nullptr;
	}

	void RemoveTexture(TextureSlot slot) {
		Textures_.erase(slot);
	}

	std::shared_ptr<IShader> GetShader() { return Shader_; }
	std::unordered_map<std::string, MaterialValue> GetUniforms()const { return Uniforms_; }

protected:
	// 材质参数
	std::unordered_map<std::string, MaterialValue> Uniforms_;
	// Shader引用（共享）
	std::shared_ptr<IShader> Shader_;
	// 纹理引用（共享所有权）
	std::unordered_map<TextureSlot, std::shared_ptr<ITexture>> Textures_;
};