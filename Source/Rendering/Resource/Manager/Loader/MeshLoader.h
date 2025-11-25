#pragma once

#include "RenderModuleAPI.h"
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifndef BUILTIN_SHADER_CONFIG_PATH
#define BUILTIN_SHADER_CONFIG_PATH "/Builtin/Builtin.json"
#endif 

class IMesh;
struct MaterialDesc;
enum class TextureSlot : uint32_t;

class MeshLoader {
public:
	static bool Load(const std::string& FilePath, struct MeshDesc& Desc, uint32_t Flags = 0);

private:
	static void ProcessNode(aiNode* node, const aiScene* scene,
		MeshDesc& meshDesc, const std::string& directory);

	static void ProcessMesh(aiMesh* mesh, const aiScene* scene,
		MeshDesc& meshDesc, const std::string& directory);

	static MaterialDesc ProcessMaterial(aiMaterial* material,
		const std::string& directory);

	static std::vector<std::string> LoadMaterialTextures(aiMaterial* mat,
		aiTextureType type,
		const std::string& directory);

	static TextureSlot ConvertTextureType(aiTextureType type);

	static uint32_t GetDefaultFlags() {
		return aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeMeshes;
	}

};