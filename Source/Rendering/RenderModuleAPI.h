#pragma once

#ifdef ENGINE_RENDERING_EXPORTS
#ifdef _WIN32
#define ENGINE_RENDERING_API __declspec(dllexport)
#else
#define ENGINE_RENDERING_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_RENDERING_API __declspec(dllimport)
#else
#define ENGINE_RENDERING_API
#endif
#endif

// 资源路径
#ifndef MESH_CONFIG_PATH
#define MESH_CONFIG_PATH "../Assets/Meshes/Configs"
#endif
#ifndef MESH_ASSET_PATH
#define MESH_ASSET_PATH "../Assets/Meshes/Sources"
#endif
#ifndef MATERIAL_CONFIG_PATH
#define MATERIAL_CONFIG_PATH "../Assets/Materials"
#endif
#ifndef SHADER_CONFIG_PATH
#define SHADER_CONFIG_PATH "../Assets/Shaders/Configs"
#endif
#ifndef SHADER_ASSET_PATH
#define SHADER_ASSET_PATH "../Assets/Shaders/Sources"
#endif
#ifndef TEXTURE_ASSET_PATH
#define TEXTURE_ASSET_PATH "../Assets/Textures"
#endif
