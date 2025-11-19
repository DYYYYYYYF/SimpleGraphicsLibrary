#version 460 core

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec3 vColor;

layout(set = 0, binding = 0, std140) uniform MaterialUBO{
    vec4 albedo;
    vec4 mra;
    vec4 emissive;
} material;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(material.mra.xyz, 1.0);
}
