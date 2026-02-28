#pragma once

#include "IObject.h"
#include "Core/UniqueID.h"

class ABaseObject : public IObject {
public:
	ABaseObject() : UniqueID_(UUID::Generate()) {}
	~ABaseObject() = default;

public:
	uint64_t GetID() const { return UniqueID_; }

private:
	uint64_t UniqueID_;
};