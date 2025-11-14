#include "OpenGLDevice.h"

#include "Logger.hpp"
#include "glad/glad.h"
#include "glad/wglext.h"
#include "Window/Window.h"
#include "File/File.h"

#include <GL/gl.h>

static const double aspect_ratio = 16.0 / 9.0;
static const int WIDTH = 1200;
static const int HEIGHT = static_cast<int>(WIDTH / aspect_ratio);

const char* vertexShaderSource =
"#version 460 core\n"
"layout (location = 0) in vec2 aPos; \n\
		layout(location = 1) in vec2 aTexCoord; \n\
		layout(location = 0) out vec2 TexCoord;"
	"void main() { gl_Position = vec4(aPos, 0.0, 1.0); TexCoord = aTexCoord; }";
const char* fragmentShaderSource = "\
		#version 460 core\n\
		layout(location = 0) in vec2 TexCoord; \
		layout(location = 0) out vec4 FragColor; \
		uniform sampler2D screenTexture; \
		void main() { FragColor = texture(screenTexture, TexCoord); }";

// 顶点数据（位置 + 纹理坐标）
float vertices[] = {
	// 位置 (x, y, z)      // 颜色 (r, g, b)
	-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,  // 顶点1：红色
	 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,  // 顶点2：绿色
	 0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f   // 顶点3：蓝色
};

unsigned int indices[] = {
	// 注意索引从0开始! 
	// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
	// 这样可以由下标代表顶点组合成矩形

	0, 1, 2, // 第一个三角形
	0, 2, 3
};

bool OpenGLDevice::Initialize(Window* Win){
	BackendAPI_ = BackendAPI::OpenGL;
	Window_ = Win;

	File ShaderFile();

	if (!InitOpenGLContext()) {
		LOG_ERROR << "Init opengl context failed!";
		return false;
	}

	if (!gladLoadGL()) {
		LOG_ERROR << "Init GLAD failed!";
		return false;
	}

	GLuint vertShader;
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertShader);
	GLuint fragShader;
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragShader);

	// 初始化Shader
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

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
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3); 

	SwapBuffers(m_hDC);
}


void OpenGLDevice::Destroy(){
	wglDeleteContext(m_hRC);
	ReleaseDC((HWND)Window_->GetNativeHandle(), m_hDC);

	LOG_INFO << "Destroying OpenGL device.";
}

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
bool OpenGLDevice::InitOpenGLContext() {
	m_hDC = GetDC((HWND)Window_->GetNativeHandle());

	//初始化像素格式
	static    PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),    //上述格式描述符的大小
		1,                                //版本号
		PFD_DRAW_TO_WINDOW |              //格式支持窗口
		PFD_SUPPORT_OPENGL |              //格式必须支持OpenGL
		PFD_DOUBLEBUFFER,                 //必须支持双缓冲
		PFD_TYPE_RGBA,                    //申请RGBA 格式
		24,                               //选定色彩深度
		0, 0, 0, 0, 0, 0, 0, 0,           //忽略RGBA
		0,                                //无累加缓存
		0, 0, 0, 0,                       //忽略聚集位
		32,                               //32位Z-缓存(深度缓存)
		0,                                //无蒙板缓存
		0,                                //无辅助缓存
		PFD_MAIN_PLANE,                   //主绘图层
		0,                                //Reserved
		0, 0, 0                           //忽略层遮罩
	};

	//选择一个最适合pfd描述的像素格式索引值
	int nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);

	//为DC设置像素格式
	SetPixelFormat(m_hDC, nPixelFormat, &pfd);

	//创建RC
	HGLRC tempContext = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, tempContext);

	// 2. 加载 wglCreateContextAttribsARB
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	// 3. 创建真正的现代 OpenGL 上下文
	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4, // OpenGL 4.x
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, // Core Profile
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB, // 可选：启用调试
		0
	};

	m_hRC = wglCreateContextAttribsARB(m_hDC, 0, attribs);
	wglMakeCurrent(m_hDC, m_hRC);
	wglDeleteContext(tempContext); // 删除临时上下文

	return true;
}