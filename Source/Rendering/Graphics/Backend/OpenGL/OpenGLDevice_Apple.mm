#include "OpenGLDevice.h"

#ifdef __APPLE__
#include "Logger.hpp"
#include "Window/Window.h"
#include <GL/gl.h>

void OpenGLDevice::MakeCurrent() {
    if (glContext_){
	    [glContext_ makeCurrentContext];
    }
}

void OpenGLDevice::SwapBuffers() {
    if (glContext_){
	    [glContext_ flushBuffer];
    }
}


bool OpenGLDevice::InitOpenGLContext() {
    AppleWindowView* windowView = (AppleWindowView*)window->GetNSView();
    // 配置像素格式
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAAccelerated,           // 硬件加速
        NSOpenGLPFADoubleBuffer,          // 双缓冲
        NSOpenGLPFAColorSize, 24,         // RGB 24位
        NSOpenGLPFAAlphaSize, 8,          // Alpha 8位（总共32位）
        NSOpenGLPFADepthSize, 32,         // 32位深度缓冲
        NSOpenGLPFAStencilSize, 0,        // 不使用模板缓冲
        
        // 注意：macOS 最高只支持 OpenGL 4.1 Core Profile
        // 不支持 OpenGL 4.6！
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        
        // 可选：多重采样抗锯齿
        // NSOpenGLPFAMultisample,
        // NSOpenGLPFASampleBuffers, 1,
        // NSOpenGLPFASamples, 4,
        
        0  // 必须以 0 结束
    };
    
    // 创建像素格式
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!pixelFormat) {
        LOG_ERROR << "Failed to create NSOpenGLPixelFormat";
        return false;
    }
    
    // macOS 不需要临时上下文，直接创建现代版本
    glContext_ = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    [pixelFormat release];
    
    if (!glContext_) {
        LOG_ERROR << "Failed to create NSOpenGLContext";
        return false;
    }
    
    // 绑定到视图
    [glContext_ setView:windowView];
    // 激活上下文
    [glContext_ makeCurrentContext];
    
    // 设置垂直同步（可选）
    GLint swapInterval = 1;  // 1 = 开启 VSync, 0 = 关闭
    [glContext_ setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
    
    // 保存引用
    windowView_ = windowView;
    
    // 验证 OpenGL 版本
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    LOG_INFO << "OpenGL Version: " << version;
    LOG_INFO << "OpenGL Renderer: " << renderer;
    
    return true;
}

void OpenGLDevice::Destroy(){
	if (glContext_) {
		[glContext_ clearDrawable] ;
		[glContext_ release] ;
		glContext_ = nil;
	}

	LOG_INFO << "Destroying OpenGL device.";
}

#endif