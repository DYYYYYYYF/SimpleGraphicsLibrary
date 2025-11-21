#pragma once

#include "RenderModuleAPI.h"
#include <string>
#include "Core/UniqueID.h"

#ifndef DynamicCast
#define DynamicCast std::dynamic_pointer_cast
#endif

enum class ResourceType {
	eMesh = 0,
	eMaterial,
	eShader,
	eTexture
};

class IResource {
public:
	IResource() : UniqueID_(UUID::Generate()), RefCount(0), IsValid_(false){}
	virtual bool Load(const std::string& path) = 0;
	virtual void Unload() = 0;

public:
	const std::string& GetName() const { return Name_; }
	bool IsValid() const { return IsValid_; }
	ResourceType GetResourceType() const { return Type_; }

	uint64_t GetID() const { return UniqueID_; }
	uint32_t GetReferCount() const { return RefCount; }
	uint32_t Refer() { return ++RefCount; }
	uint32_t Release() { return --RefCount; }

protected:
	uint64_t UniqueID_;
	uint32_t RefCount;

	std::string Name_;
	bool IsValid_;
	ResourceType Type_;
};