#pragma once

#include "IResource.h"
#include "Core/BaseMath.h"
#include <vector>

class IMaterial;
struct MaterialDesc;
class CommandList;

struct Vertex {
	FVector3 position;
	FVector3 normal;
	FVector2 texCoord;
	FVector3 tangent;
	FVector3 bitangent;
};

struct SubMeshDesc {
	uint32_t BaseVertex;      // 顶点在全局 Vertices 数组中的起始索引
	uint32_t BaseIndex;       // 索引在全局 Indices 数组中的起始索引
	uint32_t IndexCount;      // 该 SubMesh 的索引数量
	uint32_t MaterialIndex;   // 对应的材质索引
	std::string Name;         // SubMesh 名称
};

struct MeshDesc : public IResourceDesc {
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<SubMeshDesc> SubMeshes;
	std::vector<MaterialDesc> Materials;

	// AABB 包围盒（可选）
	FVector3 BoundsMin;
	FVector3 BoundsMax;
};

class IMesh : public IResource {
public:
	IMesh() { Type_ = ResourceType::eMesh; }

public:
	virtual bool Load(const struct MeshDesc& AssetDesc) = 0;
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void Draw(CommandList& CmdList) = 0;

	uint32_t GetIndexCount() const { return (uint32_t)Indices_.size(); }

	// 获取原始数据（用于物理碰撞等）
	const std::vector<Vertex>& GetVertices() const { return Vertices_; }
	const std::vector<unsigned int>& GetIndices() const { return Indices_; }
	std::shared_ptr<IMaterial> GetMaterial(uint64_t i) { return i < Materials_.size() ? Materials_[i] : nullptr; }
	const std::vector<std::shared_ptr<IMaterial>>& GetMaterials() const { return Materials_; }
	uint64_t GetMaterialCount() const { return Materials_.size(); }

	// 包围盒
	FVector3 GetCenter() const { return (BoundsMin + BoundsMax) * 0.5f; }
	FVector3 GetExtent() const { return BoundsMax - BoundsMin; }
	float GetRadius() const { return (GetExtent() * 0.5f).norm(); }
	bool IsValid() const { return BoundsMin[0] != FLT_MAX && BoundsMax[0] != -FLT_MAX; }

protected:
	std::vector<Vertex> Vertices_;
	std::vector<uint32_t> Indices_;
	std::vector<std::shared_ptr<IMaterial>> Materials_;
	std::vector<SubMeshDesc> SubMeshes_;

	FVector3 BoundsMin;
	FVector3 BoundsMax;
};