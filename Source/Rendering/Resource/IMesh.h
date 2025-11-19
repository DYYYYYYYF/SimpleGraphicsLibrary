#pragma once

#include "Core/BaseMath.h"
#include <vector>
#include <string>

class IMesh {
public:
	struct Vertex {
		FVector3 position;
		FVector3 normal;
		FVector2 texCoord;
		FVector3 tangent;
	};

public:
	virtual void Load(const std::string& mesh) = 0;
	virtual void UnLoad() = 0;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	uint32_t GetIndexCount() const { return (uint32_t)Indices_.size(); }

	// 获取原始数据（用于物理碰撞等）
	const std::vector<Vertex>& GetVertices() const { return Vertices_; }
	const std::vector<unsigned int>& GetIndices() const { return Indices_; }

protected:
	std::vector<Vertex> Vertices_;
	std::vector<uint32_t> Indices_;
};