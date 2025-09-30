// Graphics/OpenGL/Cocoa/CocoaOpenGLSurface.h
#pragma once
#include "../OpenGLSurface.h"

#ifdef __APPLE__

#ifdef __OBJC__
@class NSOpenGLContext;
@class NSOpenGLPixelFormat;
@class NSView;
#else
typedef struct objc_object NSOpenGLContext;
typedef struct objc_object NSOpenGLPixelFormat;
typedef struct objc_object NSView;
#endif

namespace Engine {
	namespace Platform {

		class CocoaOpenGLSurface : public OpenGLSurface {
		public:
			CocoaOpenGLSurface(NSView* view, uint32_t width, uint32_t height);
			~CocoaOpenGLSurface() override;

			bool Initialize() override;
			void Shutdown() override;
			void MakeCurrent() override;
			void SwapBuffers() override;

			void* GetNativeHandle() const override;
			uint32_t GetWidth() const override { return m_Width; }
			uint32_t GetHeight() const override { return m_Height; }

		private:
			NSView* m_View;
			NSOpenGLContext* m_GLContext;
			NSOpenGLPixelFormat* m_PixelFormat;
			uint32_t m_Width;
			uint32_t m_Height;
		};

	} // namespace Platform
} // namespace Engine

#endif // __APPLE__