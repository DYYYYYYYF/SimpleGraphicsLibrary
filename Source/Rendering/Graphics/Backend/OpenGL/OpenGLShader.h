#pragma once
#include "Graphics/RHI/IShader.h"
#include "glad/glad.h"

#include <unordered_map>

class OpenGLShader : public IShader {
public:
	OpenGLShader();
	virtual ~OpenGLShader();

public:
	// 使用和管理
	virtual bool Load(const std::string& path) override;
	virtual void Unload() override;
	virtual void Bind() override;
	virtual void Unbind() override;

	// Uniform设置
	virtual void SetInt(const std::string& name, int value) override;
	virtual void SetFloat(const std::string& name, float value) override;
	virtual void SetVec2(const std::string& name, const FVector2& value) override;
	virtual void SetVec3(const std::string& name, const FVector3& value) override;
	virtual void SetVec4(const std::string& name, const FVector4& value) override;
	virtual void SetMat3(const std::string& name, const FMatrix3& value) override;
	virtual void SetMat4(const std::string& name, const FMatrix4& value) override;

public:
	uint32_t GetProgramID() const { return ProgramID_; }

private:
	GLint GetUniformLocation(const std::string& name) const;
	bool AddStage(const std::string& source, ShaderStage stage);
	bool CompileShader(const std::string& source, GLuint& Obj);

private:
	GLuint ProgramID_;
	std::unordered_map<ShaderStage, GLuint> ShaderStages_;

};