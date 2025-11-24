#include "GLMesh.h"
#include "GLMaterial.h"
#include "Platform/File/JsonObject.h"
#include "Resource/Manager/ResourceManager.h"
#include <Logger.hpp>
#include "Command/CommandList.h"
#include "Renderer/Renderer.h"

GLMesh::GLMesh(const MeshDesc& AssetDesc) {
	if (!Load(AssetDesc)) {
		return;
	}
}

GLMesh::GLMesh(const std::string& FilePath) : VAO_(NULL), VBO_(NULL), EBO_(NULL) {
	if (!Load(FilePath)) {
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

		std::shared_ptr<IMaterial> Mat = DynamicCast<IMaterial>(
			ResourceManager::Instance().LoadResourceFromDescriptor(ResourceType::eMaterial, &MatDesc)
		);

		if (!Mat) continue;
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

bool GLMesh::Load(const std::string& FilePath) {
	// 读取配置
	File MeshSrc(MESH_CONFIG_PATH + FilePath);
	if (!MeshSrc.IsExist()) {
		return false;
	}

	JsonObject Content = JsonObject(MeshSrc.ReadBytes());
	Name_ = Content.Get("Name").GetString();

	// 加载材质
	JsonObject MaterialContent = Content.Get("MaterialConfig");
	if (MaterialContent.IsArray()) {
		size_t MaterialConfigCount = MaterialContent.Size();
		for (size_t i = 0; i < MaterialConfigCount; ++i) {
			// 获取信息
			JsonObject MaterialConfig = MaterialContent.ArrayItemAt(i);
			// 加载材质配置
			const std::string& MaterialConfigPath = MaterialConfig.Get("MaterialConfig").GetString();
			std::shared_ptr<IMaterial> Mat = DynamicCast<IMaterial>(
				ResourceManager::Instance().LoadResource(ResourceType::eMaterial, MaterialConfigPath)
			);

			if (!Mat) continue;
			Materials_.push_back(Mat);
		}
	}

	Vertex V1 = { FVector3(-0.5f,  0.5f, 0.0f), FVector3(0.0f, 0.0f, 1.0f), FVector2(0, 0), FVector3(0.0f, 0.0f, 0.0f) };
	Vertex V2 = { FVector3(-0.5f, -0.5f, 0.0f), FVector3(0.0f, 0.0f, 1.0f), FVector2(0, 1), FVector3(0.0f, 0.0f, 0.0f) };
	Vertex V3 = { FVector3(0.5f, -0.5f, 0.0f), FVector3(0.0f, 0.0f, 1.0f), FVector2(1, 0), FVector3(0.0f, 0.0f, 0.0f) };
	Vertex V4 = { FVector3(0.5f,  0.5f, 0.0f), FVector3(0.0f, 0.0f, 1.0f), FVector2(1, 1), FVector3(0.0f, 0.0f, 0.0f) };
	// 顶点数据（位置 + 纹理坐标）
	Vertices_ = { V1, V2, V3, V4};

	Indices_ = {
		// 注意索引从0开始! 
		// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
		// 这样可以由下标代表顶点组合成矩形

		0, 1, 2, // 第一个三角形
		0, 2, 3
	};

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

void GLMesh::Draw(CommandList& CmdList) {

	for (const SubMeshDesc& SubMesh : SubMeshes_) {
		IMaterial* Material = GetMaterial(SubMesh.MaterialIndex).get();
		if (!Material) {
			continue;
		}

		CmdList.DrawIndexed(this, Material, FMatrix4::Identity(), SubMesh.IndexCount, SubMesh.BaseIndex);
	}
}