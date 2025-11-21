#pragma once

#include "IObject.h"
#include "Core/UniqueID.h"

class BaseObject : public IObject {
public:
	BaseObject() : UniqueID_(UUID::Generate()) {}
	~BaseObject() = default;

public:
	uint64_t GetID() const { return UniqueID_; }

private:
	uint64_t UniqueID_;
};