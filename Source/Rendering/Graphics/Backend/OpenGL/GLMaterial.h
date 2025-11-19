#pragma once

#include "Resource/IMaterial.h"
#include "glad/glad.h"
#include <string>

class GLMaterial : public IMaterial {
public:
	GLMaterial(const std::string& mesh);
	virtual ~GLMaterial();

public:
	virtual void Load() override;
	virtual void Unload() override;

	virtual void Apply() const override;
	virtual void Unbind() const override;

private:
	void ApplyUniformBuffer() const;
	void ApplyTextures() const;
	void SetTextureFlag(TextureSlot slot, bool enabled) const;


private:
	GLuint UBO_;
};
