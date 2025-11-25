#version 460 core

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTexcoord;
layout(location = 3) in vec3 iTangent;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec2 vTexcoord;
layout(location = 2) out vec3 vTangent;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjMat;

void main() {
	gl_Position = ProjMat * ViewMat * ModelMat * vec4(iPosition, 1.0);
	vNormal = iNormal;
	vTexcoord = iTexcoord;
	vTangent = iTangent;
}