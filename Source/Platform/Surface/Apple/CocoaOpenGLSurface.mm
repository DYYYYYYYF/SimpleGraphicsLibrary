#include "CocoaOpenGLSurface.h"

#ifdef __APPLE__

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <iostream>

namespace Engine {
    namespace Platform {
        
        CocoaOpenGLSurface::CocoaOpenGLSurface(NSView* view, uint32_t width, uint32_t height)
            : m_View(view)
            , m_GLContext(nil)
            , m_PixelFormat(nil)
            , m_Width(width)
            , m_Height(height)
        {
        }
        
        CocoaOpenGLSurface::~CocoaOpenGLSurface() {
            Shutdown();
        }
        
        bool CocoaOpenGLSurface::Initialize() {
            @autoreleasepool {
                // 配置OpenGL像素格式（macOS最高4.1）
                NSOpenGLPixelFormatAttribute attrs[] = {
                    NSOpenGLPFADoubleBuffer,
                    NSOpenGLPFADepthSize, 24,
                    NSOpenGLPFAStencilSize, 8,
                    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
                    NSOpenGLPFAColorSize, 32,
                    NSOpenGLPFAAlphaSize, 8,
                    NSOpenGLPFAAccelerated,
                    NSOpenGLPFANoRecovery,
                    0
                };
                
                m_PixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
                if (!m_PixelFormat) {
                    std::cerr << "Failed to create NSOpenGLPixelFormat" << std::endl;
                    return false;
                }
                
                m_GLContext = [[NSOpenGLContext alloc] initWithFormat:m_PixelFormat shareContext:nil];
                if (!m_GLContext) {
                    std::cerr << "Failed to create NSOpenGLContext" << std::endl;
                    [m_PixelFormat release];
                    m_PixelFormat = nil;
                    return false;
                }
                
                [m_GLContext setView:m_View];
                [m_GLContext makeCurrentContext];
                
                // 设置VSync
                GLint swapInterval = 1;
                [m_GLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
                
                [m_GLContext update];
                
                std::cout << "Cocoa OpenGL Surface initialized" << std::endl;
                return true;
            }
        }
        
        void CocoaOpenGLSurface::Shutdown() {
            @autoreleasepool {
                if (m_GLContext) {
                    [NSOpenGLContext clearCurrentContext];
                    [m_GLContext release];
                    m_GLContext = nil;
                }
                
                if (m_PixelFormat) {
                    [m_PixelFormat release];
                    m_PixelFormat = nil;
                }
            }
        }
        
        void CocoaOpenGLSurface::MakeCurrent() {
            @autoreleasepool {
                if (m_GLContext) {
                    [m_GLContext makeCurrentContext];
                }
            }
        }
        
        void CocoaOpenGLSurface::SwapBuffers() {
            @autoreleasepool {
                if (m_GLContext) {
                    [m_GLContext flushBuffer];
                }
            }
        }
        
        void* CocoaOpenGLSurface::GetNativeHandle() const {
            return (__bridge void*)m_GLContext;
        }
        
    } // namespace Platform
} // namespace Engine

#endif // __APPLE__