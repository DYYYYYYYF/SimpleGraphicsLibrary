#include "OpenGLDevice.h"

#include "Logger.hpp"
#include "glad/wglext.h"
#include "File/File.h"
#include "OpenGLShader.h"

static const double aspect_ratio = 16.0 / 9.0;
static const int WIDTH = 1200;
static const int HEIGHT = static_cast<int>(WIDTH / aspect_ratio);

// 顶点数据（位置 + 纹理坐标）
float vertices[] = {
	// 位置 (x, y, z)      // 颜色 (r, g, b)
	-0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,  // 顶点1：蓝色
	-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,  // 顶点2：红色
	 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,  // 顶点3：绿色
	 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 0.0f   // 顶点4：蓝色
};

unsigned int indices[] = {
	// 注意索引从0开始! 
	// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
	// 这样可以由下标代表顶点组合成矩形

	0, 1, 2, // 第一个三角形
	0, 2, 3
};

OpenGLDevice::OpenGLDevice() {
	BackendAPI_ = BackendAPI::eUnknown;
	Window_ = nullptr;
	BuiltinShader_ = nullptr;
}

bool OpenGLDevice::Initialize(Window* Win){
	BackendAPI_ = BackendAPI::eOpenGL;
	Window_ = Win;

	if (!InitOpenGLContext()) {
		LOG_ERROR << "Init opengl context failed!";
		return false;
	}

	if (!gladLoadGL()) {
		LOG_ERROR << "Init GLAD failed!";
		return false;
	}

	// 初始化Shader
	BuiltinShader_ = new OpenGLShader();
	if (!BuiltinShader_) {
		return false;
	}
	
	if (!BuiltinShader_->Load("")) {
		return false;
	}

	//初始化FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// 初始化Texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is incomplete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 初始化 VAO/VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 位置属性
	glVertexAttribPointer(
		0,                      // 属性位置
		3,                      // 3个分量(x, y, z)
		GL_FLOAT,               // 数据类型
		GL_FALSE,               // 不需要归一化
		6 * sizeof(float),      // 步长：6个float(位置3 + 颜色3)
		(void*)0                // 偏移量：从开头开始
	);
	glEnableVertexAttribArray(0);

	// 纹理坐标属性
	glVertexAttribPointer(
		1,                          // 属性位置
		3,                          // 3个分量(r, g, b)
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),          // 步长：6个float
		(void*)(3 * sizeof(float))  // 偏移量：跳过前3个float
	);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glViewport(0, 0, WIDTH, HEIGHT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	LOG_INFO << "OpenGL device create successfully.";
	return true;
}

void OpenGLDevice::Draw() {
	// Object pass
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	BuiltinShader_->Use();

	glEnableVertexAttribArray(0);
	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);

	SwapBuffers();
}