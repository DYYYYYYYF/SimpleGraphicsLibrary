#pragma once

#include "IResource.h"
#include "IMaterial.h"
#include "Core/BaseMath.h"

enum class ShaderStage {
	eVertex = 0,
	eFragment,
	eUnknow
};

struct UniformInfo {
	int offset;      // 在 UBO 中的偏移
	int size;        // 大小（按 std140 对齐）
	MaterialValue::Type type;       // GL_FLOAT_VEC4, GL_FLOAT etc.
};

struct ShaderUniformLayout {
	uint32_t blockIndex; // UBO block 编号
	uint32_t binding;    // layout(binding = X)
	int blockSize; // UBO 总字节大小
	std::unordered_map<std::string, UniformInfo> uniforms;
};

struct ShaderDesc : public IResourceDesc{
	std::string Name;
	std::unordered_map<ShaderStage, std::string> Stages;
};

class IShader : public IResource{
public:
	IShader() { Type_ = ResourceType::eShader; }

public:
	// 使用和管理
	virtual bool Load(const ShaderDesc& Desc) = 0;
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
	virtual void UploadMaterial(const IMaterial& Mat) = 0;
};