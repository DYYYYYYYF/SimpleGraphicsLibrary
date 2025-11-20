#pragma once

#include "Core/BaseMath.h"
#include "ITexture.h"

#include <memory>
#include <string>
#include <unordered_map>

class IShader;

class IMaterial {
public:
	struct TextureEntry {
		TextureSlot slot;
		std::shared_ptr<ITexture> texture;
		bool enabled = true;
	};

	struct MaterialUBO {
		FVector4 Albedo_ = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		FVector4 MetallicRoughnessAO_ = FVector4(0.0f, 0.0f, 1.0f, 1.0f);
		FVector4 Emissive_ = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	};

public:
	virtual void Load(const std::string& filename) = 0;
	virtual void Unload() = 0;

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

	const std::string& GetName() const { return Name_; }
	bool IsValid() const { return IsValid_; }
	std::shared_ptr<IShader> GetShader() { return Shader_; }

protected:
	std::string Name_;
	bool IsValid_;

	// 材质参数
	MaterialUBO MaterialUBO_;

	// Shader引用（共享）
	std::shared_ptr<IShader> Shader_;
	// 纹理引用（共享所有权）
	std::unordered_map<TextureSlot, std::shared_ptr<ITexture>> Textures_;
};