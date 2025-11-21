#pragma once

#include "SystemModuleAPI.h"
#include "EngineSystem.h"
#include "Rendering/Resource/IResource.h"

#include <string>
#include <memory>
#include <unordered_map>

class ResourceSystem : public EngineSystem {
public:
	ENGINE_SYSTEM_API static ResourceSystem& Instance();

public:
	ENGINE_SYSTEM_API virtual bool Initialize() override;
	ENGINE_SYSTEM_API virtual void Shutdown() override;

	ENGINE_SYSTEM_API std::shared_ptr<IResource> Acquire(ResourceType Type, const std::string& Name);
	ENGINE_SYSTEM_API std::shared_ptr<IResource> Acquire(uint64_t ID);
	ENGINE_SYSTEM_API void Release(uint64_t ID);

public:
	std::unordered_map<uint64_t, std::shared_ptr<IResource>> Resources_;

	std::unordered_map<std::string, uint32_t> MeshNameMap_;
	std::unordered_map<std::string, uint32_t> MaterialNameMap_;
	std::unordered_map<std::string, uint32_t> ShaderNameMap_;
	std::unordered_map<std::string, uint32_t> TextureNameMap_;

};