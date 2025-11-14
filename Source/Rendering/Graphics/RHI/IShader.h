#pragma once
#include <string>

enum class ShaderStage {
	eVertex = 0,
	eFragment
};

class IShader {
public:
	virtual bool Load(const std::string& path) = 0;
	virtual void Unload() = 0;
	virtual void Use() = 0;
};