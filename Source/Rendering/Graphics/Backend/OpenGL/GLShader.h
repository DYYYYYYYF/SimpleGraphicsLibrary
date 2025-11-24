#pragma once
#include "Resource/IShader.h"
#include "glad/glad.h"

#include <unordered_map>

class GLShader : public IShader {
public:
	GLShader();
	GLShader(const ShaderDesc& Desc);
	GLShader(const std::string& AssetPath);
	virtual ~GLShader();

public:
	// 使用和管理
	virtual bool Load(const ShaderDesc& Desc) override;
	virtual bool Load(const std::string& AssetPath) override;
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
	virtual void UploadMaterial(const IMaterial& Mat) override;

public:
	uint32_t GetProgramID() const { return ProgramID_; }

private:
	GLint GetUniformLocation(const std::string& name) const;
	bool AddStage(const std::string& source, ShaderStage stage);
	bool CompileShader(const std::string& source, GLuint& Obj);
	void ReflectUnifromBlock();
	MaterialValue::Type MapStd140Type(GLenum Type);
	int ComputeTypeSize(MaterialValue::Type Type);

private:
	GLuint ProgramID_;
	GLuint MaterialUBO_;
	ShaderUniformLayout MaterialLayout_;
	std::unordered_map<ShaderStage, GLuint> ShaderStages_;

};