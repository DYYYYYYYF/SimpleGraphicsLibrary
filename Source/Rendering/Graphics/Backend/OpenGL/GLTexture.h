#pragma once
#include "Resource/ITexture.h"
#include "glad/glad.h"

class GLTexture : public ITexture {
public:
	GLTexture();
	GLTexture(const std::string& AssetPath);
	virtual ~GLTexture();

public:
	// 使用和管理
	virtual bool Load(const std::string& AssetPath) override;
	virtual void Unload() override;
	virtual void Bind(uint32_t binding) const override;
	virtual void Unbind() const override;
	virtual void* GetNativeHandle() const { return nullptr; };

private:
	GLuint TextureID;

};