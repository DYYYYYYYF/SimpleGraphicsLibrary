#pragma once
#include "Graphics/IGraphicsDevice.h"

#ifdef _WIN32

#include "glad/glad.h"
#include <windows.h>
#include "glad/wglext.h"

class IShader;
class IMesh;
class IMaterial;

class GLDevice : public IGraphicsDevice {
public:
	GLDevice();
	virtual bool Initialize(Window* Win) override;
	virtual void ExecuteCommandList(const CommandList& cmdList) override;
	virtual void MakeCurrent() override;
	virtual void SwapBuffers() override;
	virtual void Destroy() override;

	virtual std::shared_ptr<IMesh> CreateMesh(const std::string& AssetPath) override;
	virtual std::shared_ptr<IMaterial> CreateMaterial(const std::string& AssetPath) override;

private:
	bool InitOpenGLContext();

private:
	Window* Window_;
	IShader* BuiltinShader_;

	// OpenGL properties
	GLuint textureID;
	GLuint VAO, VBO, EBO;
	GLuint FBO;

	// OpenGL handle
	HGLRC m_hRC;
	HDC m_hDC;
	// WGL 扩展函数指针
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

};

#endif
