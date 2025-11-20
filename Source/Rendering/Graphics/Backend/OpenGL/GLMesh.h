#pragma once

#include "glad/glad.h"
#include "Resource/IMesh.h"

class GLMesh : public IMesh {
public:
	GLMesh(const std::string& mesh);
	virtual ~GLMesh();

public:
	virtual void Load(const std::string& mesh) override;
	virtual void UnLoad() override;

	virtual void Bind() const override;
	virtual void Unbind() const override;

protected:
	void Setup();

private:
	GLuint VAO_, VBO_, EBO_;
	bool IsLoaded_;

};