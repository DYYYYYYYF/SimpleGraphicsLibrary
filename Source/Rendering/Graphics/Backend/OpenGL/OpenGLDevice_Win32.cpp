#include "OpenGLDevice.h"

#ifdef _WIN32
#include "Logger.hpp"
#include "Window/Window.h"

void OpenGLDevice::MakeCurrent() {
	wglMakeCurrent(m_hDC, m_hRC);
}

void OpenGLDevice::SwapBuffers() {
	::SwapBuffers(m_hDC);
}

bool OpenGLDevice::InitOpenGLContext() {
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

void OpenGLDevice::Destroy() {
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

#endif