#pragma once

#include "RenderModuleAPI.h"
#include <string>
#include <memory>

class IMesh;

class MeshLoader {
public:
	static bool Load(const std::string& filename);
};