#pragma once

#include "glad/glad.h"
#include "Resource/IMesh.h"

class GLMesh : public IMesh {
public:
	GLMesh(const struct MeshDesc& AssetDesc);
	virtual ~GLMesh();

public:
	virtual bool Load(const struct MeshDesc& AssetDesc) override;
	virtual void Unload() override;

	virtual void Bind() const override;
	virtual void Unbind() const override;

protected:
	void Setup();

private:
	GLuint VAO_, VBO_, EBO_;
	bool IsLoaded_;

};