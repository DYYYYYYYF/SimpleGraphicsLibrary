#include "GLTexture.h"

GLTexture::GLTexture() {}
GLTexture::GLTexture(const std::string& AssetPath) {
	Load(AssetPath);
}
GLTexture::~GLTexture() {
	Unload();
}

bool GLTexture::Load(const std::string& AssetPath) {

	return true;
}

void GLTexture::Unload() {

}

void GLTexture::Bind(uint32_t binding) const {

}

void GLTexture::Unbind() const {

}

