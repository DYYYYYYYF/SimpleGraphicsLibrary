#pragma once

#include "IResource.h"

enum class TextureSlot : uint32_t {
	eAlbedo = 0,      // 基础颜色
	eNormal = 1,      // 法线
	eMetallic = 2,    // 金属度
	eRoughness = 3,   // 粗糙度
	eAO = 4,          // 环境光遮蔽
	eEmissive = 5,    // 自发光
	eHeight = 6,      // 高度图
	eOpacity = 7,     // 不透明度

	// 可扩展到15（大多数平台保证至少16个）
	Custom0 = 8,
	// ...
};

class ITexture : public IResource{
public:
	ITexture() { Type_ = ResourceType::eTexture; }

public:
	virtual bool Load(const std::string& path) = 0;
	virtual void Unload() = 0;

	virtual void Bind(uint32_t binding) const = 0;
	virtual void Unbind() const = 0;

	virtual void* GetNativeHandle() const = 0;
};