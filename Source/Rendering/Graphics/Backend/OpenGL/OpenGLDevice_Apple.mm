#include "OpenGLDevice.h"

#ifdef __APPLE__
#include "Logger.hpp"
#include "Window/Window.h"
#include <GL/gl.h>

void OpenGLDevice::MakeCurrent() {
	[glContext_ makeCurrentContext];
}

void OpenGLDevice::SwapBuffers() {
	[glContext_ flushBuffer];
}


bool OpenGLDevice::InitOpenGLContext() {
	nsView_ = (NSView*)window->GetNSView();

	NSOpenGLPixelFormatAttribute attrs[] = {
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		0
	};

	NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc]initWithAttributes:attrs];
	if (!pixelFormat) return false;

	glContext_ = [[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext : nil];
	[pixelFormat release] ;

	if (!glContext_) return false;

	[glContext_ setView : nsView_] ;

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