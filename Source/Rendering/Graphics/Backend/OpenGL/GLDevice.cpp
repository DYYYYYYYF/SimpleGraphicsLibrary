#include "GLDevice.h"

#ifdef _WIN32

#include "Logger.hpp"
#include "glad/wglext.h"
#include "File/File.h"
#include "Window/Window.h"
#include "GLMesh.h"
#include "GLMaterial.h"
#include "GLShader.h"
#include "GLTexture.h"
#include "Command/CommandList.h"

static const double aspect_ratio = 16.0 / 9.0;
static const int WIDTH = 1200;
static const int HEIGHT = static_cast<int>(WIDTH / aspect_ratio);

GLDevice::GLDevice() {
	BackendAPI_ = BackendAPI::eUnknown;
	Window_ = nullptr;
	BuiltinShader_ = nullptr;
}

bool GLDevice::Initialize(Window* Win){
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

	glViewport(0, 0, WIDTH, HEIGHT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableVertexAttribArray(0);

	LOG_INFO << "OpenGL device create successfully.";
	return true;
}

void GLDevice::ExecuteCommandList(const CommandList& CmdList) {
	for (const auto& Cmd : CmdList.GetCommands()) {
		switch (Cmd->Type_)
		{
		case CommandType::eClear: {
			ClearCommand* ClearCmd = static_cast<ClearCommand*>(Cmd.get());

			// 翻译成OpenGL调用
			glClearColor(ClearCmd->Color_[0],
				ClearCmd->Color_[1],
				ClearCmd->Color_[2],
				ClearCmd->Color_[3]);

			GLint ClearType = 0;
			if (ClearCmd->ClearColor_) ClearType |= GL_COLOR_BUFFER_BIT;
			if (ClearCmd->ClearDepth_) ClearType |= GL_DEPTH_BUFFER_BIT;
			glClear(ClearType);
			break;
		}
		case CommandType::eDrawIndexed: {
			DrawIndexedCommand* DrawCmd = static_cast<DrawIndexedCommand*>(Cmd.get());

			// 1. 从句柄获取OpenGL资源
			GLMesh* Mesh = (GLMesh*)DrawCmd->DrawCall_.resources.mesh;
			if (!Mesh) {
				return;
			}

			GLMaterial* Material = (GLMaterial*)DrawCmd->DrawCall_.resources.material;
			if (!Material) {
				return;
			}

			// 2. 绑定状态
			Material->Apply();
			Mesh->Bind();

			// 3. 设置Uniform
			GLShader* Shader = (GLShader*)Material->GetShader().get();
			if (Shader) {
				GLint loc = glGetUniformLocation(Shader->GetProgramID(), "uModel");
				glUniformMatrix4fv(loc, 1, GL_FALSE, DrawCmd->DrawCall_.modelMatrix.data());
			}
		
			// 4. 绘制
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			break;
		}
		}
	}
	// Object pass

	SwapBuffers();
}


void GLDevice::MakeCurrent() {
	wglMakeCurrent(m_hDC, m_hRC);
}

void GLDevice::SwapBuffers() {
	::SwapBuffers(m_hDC);
}

bool GLDevice::InitOpenGLContext() {
	wglCreateContextAttribsARB = nullptr;
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

void GLDevice::Destroy() {
	if (BuiltinShader_) {
		BuiltinShader_->Unload();
	}

	if (m_hRC) {
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(m_hRC);
		m_hRC = nullptr;
	}
	if (m_hDC) {
		ReleaseDC((HWND)Window_->GetNativeHandle(), m_hDC);
		m_hDC = nullptr;
	}

	LOG_INFO << "Destroying OpenGL device.";
}

std::shared_ptr<IMesh> GLDevice::CreateMesh(const std::string& AssetPath) {
	return std::make_shared<GLMesh>(AssetPath);
}

std::shared_ptr<IMaterial> GLDevice::CreateMaterial(const struct MaterialDesc& AssetDesc) {
	return std::make_shared<GLMaterial>(AssetDesc);
}

std::shared_ptr<IShader> GLDevice::CreateShader(const struct ShaderDesc& AssetDesc) {
	return std::make_shared<GLShader>(AssetDesc);
}

std::shared_ptr<ITexture> GLDevice::CreateTexture(const std::string& AssetPath) {
	return std::make_shared<GLTexture>(AssetPath);
}

#endif