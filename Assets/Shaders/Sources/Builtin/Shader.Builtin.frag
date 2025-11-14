#version 460 core
layout(location = 0) in vec3 TexCoord; 
layout(location = 0) out vec4 FragColor; 
uniform sampler2D screenTexture;

void main() { 
	FragColor = vec4(TexCoord, 1.0);
	//texture(screenTexture, TexCoord); 
}