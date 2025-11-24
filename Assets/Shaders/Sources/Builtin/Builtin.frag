#version 460 core

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vTangent;

layout(set = 0, binding = 0, std140) uniform MaterialUBO{
    vec4 albedo;
    vec4 emissive;
    vec4 metallic_roughness_ao;
} material;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(material.metallic_roughness_ao.xyz, 1.0);
}
