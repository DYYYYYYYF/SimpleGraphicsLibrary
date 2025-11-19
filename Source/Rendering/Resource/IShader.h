#pragma once
#include <string>
#include "Core/BaseMath.h"

enum class ShaderStage {
	eVertex = 0,
	eFragment
};

class IShader {
public:

	// 使用和管理
	virtual bool Load(const std::string& path) = 0;
	virtual void Unload() = 0;
	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	// Uniform设置
	virtual void SetInt(const std::string& name, int value) = 0;
	virtual void SetFloat(const std::string& name, float value) = 0;
	virtual void SetVec2(const std::string& name, const FVector2& value) = 0;
	virtual void SetVec3(const std::string& name, const FVector3& value) = 0;
	virtual void SetVec4(const std::string& name, const FVector4& value) = 0;
	virtual void SetMat3(const std::string& name, const FMatrix3& value) = 0;
	virtual void SetMat4(const std::string& name, const FMatrix4& value) = 0;

	virtual bool IsValid() const { return IsValid_; }

protected:
	bool IsValid_;
};