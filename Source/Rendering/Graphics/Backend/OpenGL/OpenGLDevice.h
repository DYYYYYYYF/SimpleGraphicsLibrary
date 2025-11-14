#pragma once
#include "Graphics/IGraphicsDevice.h"

#include "glad/glad.h"

#ifdef _WIN32
#include <windows.h>
#include "glad/wglext.h"
#elif __APPLE__
#import <Cocoa/Cocoa.h>
#endif

class IShader;

class OpenGLDevice : public IGraphicsDevice {
public:
	OpenGLDevice();
	virtual bool Initialize(Window* Win) override;
	virtual void Draw() override;
	virtual void MakeCurrent() override;
	virtual void SwapBuffers() override;
	virtual void Destroy() override;

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
#ifdef _WIN32
	HGLRC m_hRC;
	HDC m_hDC;
	// WGL 扩展函数指针
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
#elif __APPLE__
	NSOpenGLContext* glContext_;
#endif

};