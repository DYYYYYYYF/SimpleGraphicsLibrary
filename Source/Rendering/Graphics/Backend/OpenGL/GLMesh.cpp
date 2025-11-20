#include "GLMesh.h"

GLMesh::GLMesh(const std::string& mesh) : VAO_(NULL), VBO_(NULL), EBO_(NULL) {
	Load(mesh);
}

GLMesh::~GLMesh() {
	UnLoad();
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

void GLMesh::Load(const std::string& mesh) {
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
	IsLoaded_ = true;
}

void GLMesh::UnLoad() {
	if (IsLoaded_) {
		glDeleteVertexArrays(1, &VAO_);
		VAO_ = NULL;

		glDeleteBuffers(1, &VBO_);
		VBO_ = NULL;

		glDeleteBuffers(1, &EBO_);
		EBO_ = NULL;
	}
}