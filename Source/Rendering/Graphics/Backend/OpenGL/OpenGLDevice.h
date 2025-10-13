#include "Graphics/IGraphicsDevice.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "glad/glad.h"

class OpenGLDevice : public IGraphicsDevice {

public:
	bool Initialize(Window* Win, BackendAPI Type) override;
	void Draw() override;
	void Destroy() override;

private:
	bool InitOpenGLContext();

private:
	Window* Window_;

	// OpenGL properties
	GLuint shaderProgram;
	GLuint textureID;
	GLuint VAO, VBO, EBO;
	GLuint FBO;

	// OpenGL handle
	HGLRC m_hRC;
	HDC m_hDC;

};