#pragma once

#include "Resource/IMaterial.h"
#include "glad/glad.h"

class GLMaterial : public IMaterial {
public:
	GLMaterial();
	GLMaterial(const std::string& filename);
	GLMaterial(const struct MaterialDesc& Desc);
	virtual ~GLMaterial();

public:
	virtual bool Load(const MaterialDesc& Desc) override;
	virtual bool Load(const std::string& filename) override;
	virtual void Unload() override;

	virtual void Apply() const override;
	virtual void Unbind() const override;

private:
	void ApplyUniformBuffer() const;
	void ApplyTextures() const;
	void SetTextureFlag(TextureSlot slot, bool enabled) const;

};
