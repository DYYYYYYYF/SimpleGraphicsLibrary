#include "GLMesh.h"
#include "GLMaterial.h"
#include "Platform/File/JsonObject.h"
#include "Resource/Manager/ResourceManager.h"
#include <Logger.hpp>
#include "Renderer/Renderer.h"

GLMesh::GLMesh(const MeshDesc& AssetDesc) {
	if (!Load(AssetDesc)) {
		return;
	}
}

GLMesh::~GLMesh() {
	Unload();
}

void GLMesh::Bind() const {
	if (IsLoaded_) {
		glBindVertexArray(VAO_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	}
}

void GLMesh::Unbind() const {
	if (IsLoaded_) {
		glBindVertexArray(0);
	}
}

bool GLMesh::Load(const struct MeshDesc& AssetDesc) {
	// 基础信息
	Name_ = AssetDesc.Name;

	// 材质
	for (size_t i = 0; i < AssetDesc.Materials.size(); ++i) {
		MaterialDesc MatDesc = AssetDesc.Materials[i];

		std::shared_ptr<IMaterial> Mat = DynamicCast<IMaterial>(ResourceManager::Instance().Acquire(ResourceType::eMaterial, MatDesc.Name));
		if (!Mat) {
			Mat = DynamicCast<IMaterial>(
				ResourceManager::Instance().LoadResourceFromDescriptor(ResourceType::eMaterial, &MatDesc)
			);
		}

		if (!Mat) {
			LOG_WARN << "Load material '" << MatDesc.Name << "' failed! Use built-in material!";
			Mat = DynamicCast<IMaterial>(ResourceManager::Instance().Acquire(ResourceType::eMaterial, BUILTIN_PBR_MATERIAL));
		}
		Materials_.push_back(Mat);
	}

	// 顶点数据
	Vertices_ = AssetDesc.Vertices;
	Indices_ = AssetDesc.Indices;
	SubMeshes_ = AssetDesc.SubMeshes;
	if (Vertices_.size() == 0 || Indices_.size() == 0 || SubMeshes_.size() == 0) {
		LOG_ERROR << "Invalid vertex data.";
		return false;
	}

	Setup();

	LOG_DEBUG << "Mesh '" << Name_ << "' loaded.";
	IsLoaded_ = true;
	IsValid_ = true;
	return true;
}

void GLMesh::Setup(){
	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glGenBuffers(1, &EBO_);

	glBindVertexArray(VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, Vertices_.size() * sizeof(Vertex),
		Vertices_.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices_.size() * sizeof(unsigned int),
		Indices_.data(), GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, normal));

	// TexCoord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, texCoord));

	// Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, tangent));

	glBindVertexArray(0);
}

void GLMesh::Unload() {
	for (auto& Material : Materials_) {
		Material.reset();
	}

	if (IsLoaded_) {
		glDeleteVertexArrays(1, &VAO_);
		VAO_ = NULL;

		glDeleteBuffers(1, &VBO_);
		VBO_ = NULL;

		glDeleteBuffers(1, &EBO_);
		EBO_ = NULL;
	}

	LOG_DEBUG << "Mesh '" << Name_ << "' unloaded.";
}
