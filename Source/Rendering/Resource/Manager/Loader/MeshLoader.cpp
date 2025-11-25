#include "MeshLoader.h"
#include "Resource/IMesh.h"
#include "Resource/IMaterial.h"
#include <Logger.hpp>

bool MeshLoader::Load(const std::string& FilePath, struct MeshDesc& Desc, uint32_t Flags) {
	if (Flags == 0) {
		Flags = GetDefaultFlags();
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(MESH_ASSET_PATH + FilePath, Flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR << "ERROR::ASSIMP::" << importer.GetErrorString();
		return false;
	}

	std::string Directory = FilePath.substr(0, FilePath.find_last_of('/'));

	Desc.Name = scene->mRootNode->mName.C_Str();
	Desc.FilePath = FilePath;

	// 初始化包围盒
	Desc.BoundsMin = FVector3::Constant(std::numeric_limits<float>::max());
	Desc.BoundsMax = FVector3::Constant(std::numeric_limits<float>::lowest());

	// 预先加载所有材质
	Desc.Materials.reserve(scene->mNumMaterials);
	for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
		Desc.Materials.push_back(
			ProcessMaterial(scene->mMaterials[i], Directory)
		);
	}

	// 递归处理节点
	ProcessNode(scene->mRootNode, scene, Desc, Directory);

	return true;
}

void MeshLoader::ProcessNode(aiNode* node, const aiScene* scene,
	MeshDesc& meshDesc, const std::string& directory) {
	// 处理当前节点的所有网格
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, meshDesc, directory);
	}

	// 递归处理子节点
	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene, meshDesc, directory);
	}
}

void MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene,
	MeshDesc& meshDesc, const std::string& directory) {
	// 这两个参数
	(void)scene;
	/*	directory：在下面场景可以使用
		访问动画数据
		访问场景的全局变换
		访问嵌入的纹理数据*/
	(void)directory;

	SubMeshDesc subMesh;
	subMesh.Name = mesh->mName.C_Str();
	subMesh.BaseVertex = static_cast<uint32_t>(meshDesc.Vertices.size());
	subMesh.BaseIndex = static_cast<uint32_t>(meshDesc.Indices.size());
	subMesh.MaterialIndex = mesh->mMaterialIndex;

	// 处理顶点
	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// 位置
		vertex.position = FVector3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		// 更新包围盒
		meshDesc.BoundsMin = meshDesc.BoundsMin.cwiseMin(vertex.position);
		meshDesc.BoundsMax = meshDesc.BoundsMin.cwiseMax(vertex.position);

		// 法线
		if (mesh->HasNormals()) {
			vertex.normal = FVector3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			);
		}

		// 纹理坐标
		if (mesh->mTextureCoords[0]) {
			vertex.texCoord = FVector2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else {
			vertex.texCoord = FVector2(0.0f, 0.0f);
		}

		// 切线
		if (mesh->HasTangentsAndBitangents()) {
			vertex.tangent = FVector3(
				mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z
			);

			vertex.bitangent = FVector3(
				mesh->mBitangents[i].x,
				mesh->mBitangents[i].y,
				mesh->mBitangents[i].z
			);
		}

		meshDesc.Vertices.push_back(vertex);
	}

	// 处理索引
	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			meshDesc.Indices.push_back(face.mIndices[j] + subMesh.BaseVertex);
		}
	}

	subMesh.IndexCount = static_cast<uint32_t>(meshDesc.Indices.size()) - subMesh.BaseIndex;
	meshDesc.SubMeshes.push_back(subMesh);
}

MaterialDesc MeshLoader::ProcessMaterial(aiMaterial* material,
	const std::string& directory) {
	MaterialDesc matDesc;

	// 材质名称
	aiString name;
	material->Get(AI_MATKEY_NAME, name);
	matDesc.Name = name.C_Str();

	// 默认 Shader（可以根据材质属性选择不同的 Shader）
	matDesc.ShaderPath = BUILTIN_SHADER_CONFIG_PATH;

	// 加载各种纹理
	auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, directory);
	if (!diffuseMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eDiffuse] = diffuseMaps[0];
	}

	// 也尝试查找 BASE_COLOR 纹理（PBR 标准）
	auto baseColorMaps = LoadMaterialTextures(material, aiTextureType_BASE_COLOR, directory);
	if (!baseColorMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eDiffuse] = baseColorMaps[0];
	}

	// Specular
	auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, directory);
	if (!specularMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eSpecular] = specularMaps[0];
	}

	// Normal
	auto normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, directory);
	if (!normalMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eNormal] = normalMaps[0];
	}

	// Metallic
	auto metallicMaps = LoadMaterialTextures(material, aiTextureType_METALNESS, directory);
	if (!metallicMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eMetallic] = metallicMaps[0];
	}

	// Roughness
	auto roughnessMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, directory);
	if (!roughnessMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eRoughness] = roughnessMaps[0];
	}

	// AO 纹理
	auto aoMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, directory);
	if (!aoMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eAO] = aoMaps[0];
	}
	// 也尝试 LIGHTMAP
	auto lightmapMaps = LoadMaterialTextures(material, aiTextureType_LIGHTMAP, directory);
	if (!lightmapMaps.empty() && aoMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eAO] = lightmapMaps[0];
	}

	// Emissive 纹理
	auto emissiveMaps = LoadMaterialTextures(material, aiTextureType_EMISSIVE, directory);
	if (!emissiveMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eEmissive] = emissiveMaps[0];
	}
	// 也尝试 EMISSION_COLOR
	auto emissionMaps = LoadMaterialTextures(material, aiTextureType_EMISSION_COLOR, directory);
	if (!emissionMaps.empty() && emissiveMaps.empty()) {
		matDesc.TexturePaths[TextureSlot::eEmissive] = emissionMaps[0];
	}

	// ===== PBR 参数提取 =====
	// 1. Albedo (基础颜色)
	aiColor3D color;
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		matDesc.Uniforms["MaterialUBO.albedo"] = MaterialValue{ MaterialValue::Type::Vector4, {color.r, color.g, color.b, 1.0f} };
	}
	else {
		// 默认白色
		matDesc.Uniforms["MaterialUBO.albedo"] = MaterialValue{ MaterialValue::Type::Vector4, {1.0f, 1.0f, 1.0f, 1.0f } };
	}

	// 2. Emissive (自发光)
	if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
		matDesc.Uniforms["MaterialUBO.emissive"] = MaterialValue{ MaterialValue::Type::Vector4, {color.r, color.g, color.b, 1.0f} };
	}
	else {
		// 默认无自发光
		matDesc.Uniforms["MaterialUBO.emissive"] = MaterialValue{ MaterialValue::Type::Vector4, {0.0f, 0.0f, 0.0f, 1.0f } };
	}

	// 3. Metallic (金属度)
	float metallic = 0.0f;
	if (material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) != AI_SUCCESS) {
		// 如果没有 metallic 参数，尝试从 specular 推断
		aiColor3D specular;
		if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
			// 高镜面反射可能意味着金属材质
			float specularIntensity = (specular.r + specular.g + specular.b) / 3.0f;
			metallic = specularIntensity > 0.5f ? 1.0f : 0.0f;
		}
	}

	// 4. Roughness (粗糙度)
	float roughness = 1.0f;
	if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) != AI_SUCCESS) {
		// 尝试从 shininess 转换
		float shininess = 0.0f;
		if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
			// shininess 范围通常是 0-1000，转换为 roughness (0-1)
			// 高 shininess = 低 roughness
			roughness = 1.0f - std::clamp(shininess / 1000.0f, 0.0f, 1.0f);
		}
	}

	// 5. AO (环境光遮蔽) - 默认为 1.0（无遮蔽）
	float ao = 1.0f;

	// 组合 metallic_roughness_ao
	matDesc.Uniforms["MaterialUBO.metallic_roughness_ao"] = MaterialValue{ MaterialValue::Type::Vector4, {metallic, roughness, ao, 1.0f} };

	// 漫反射颜色
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		matDesc.Uniforms["MaterialUBO.diffuse_color"] = MaterialValue{ MaterialValue::Type::Vector4, {color.r, color.g, color.b, 1.0f} };
	}

	// 镜面反射颜色
	if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
		matDesc.Uniforms["MaterialUBO.specular_olor"] = MaterialValue{ MaterialValue::Type::Vector4, {color.r, color.g, color.b, 1.0f} };
	}

	// 光泽度
	float value;
	if (material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
		matDesc.Uniforms["MaterialUBO.shininess"] = MaterialValue{ MaterialValue::Type::Vector4, {value, 0.0f, 0.0f, 0.0f } };
	}

	// 透明度
	float opacity = 1.0f;
	if (material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
		matDesc.Uniforms["MaterialUBO.opacity"] = MaterialValue{ MaterialValue::Type::Float, {opacity, 0.0f, 0.0f, 0.0f} };

		// 如果有透明度，更新 albedo 的 alpha 通道
		auto& albedo = matDesc.Uniforms["MaterialUBO.albedo"];
		if (albedo.data.size() == 4) {
			albedo.data[3] = opacity;
		}
	}

	// 折射率（IOR）
	float ior = 1.5f;
	if (material->Get(AI_MATKEY_REFRACTI, ior) == AI_SUCCESS) {
		matDesc.Uniforms["MaterialUBO.ior"] = MaterialValue{ MaterialValue::Type::Float, {ior, 0.0f, 0.0f, 0.0f} };
	}

	return matDesc;
}

std::vector<std::string> MeshLoader::LoadMaterialTextures(aiMaterial* mat,
	aiTextureType type,
	const std::string& directory) {
	std::vector<std::string> textures;

	for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string filename = std::string(str.C_Str());
		std::string filepath = directory + "/" + filename;

		textures.push_back(filepath);
	}

	return textures;
}

TextureSlot MeshLoader::ConvertTextureType(aiTextureType type) {
	switch (type) {
	case aiTextureType_DIFFUSE:   return TextureSlot::eDiffuse;
	case aiTextureType_SPECULAR:  return TextureSlot::eSpecular;
	case aiTextureType_NORMALS:   return TextureSlot::eNormal;
	case aiTextureType_METALNESS: return TextureSlot::eMetallic;
	case aiTextureType_DIFFUSE_ROUGHNESS: return TextureSlot::eRoughness;
	case aiTextureType_AMBIENT_OCCLUSION: return TextureSlot::eAO;
	case aiTextureType_EMISSIVE:  return TextureSlot::eEmissive;
	default: return TextureSlot::eDiffuse;
	}
}
