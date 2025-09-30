// Graphics/Shader.h
#pragma once
#include "GraphicsResource.h"

namespace Engine {
	namespace Graphics {

		// Shader - RHI着色器接口
		class Shader : public GraphicsResource {
		public:
			virtual ~Shader() = default;

			// 编译状态
			virtual bool IsCompiled() const = 0;
			virtual const char* GetCompileError() const = 0;

			// 绑定（立即执行）
			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			// 参数设置（立即执行，需要先Bind）
			virtual void SetInt(const char* name, int32_t value) = 0;
			virtual void SetFloat(const char* name, float value) = 0;
			virtual void SetVector2(const char* name, float x, float y) = 0;
			virtual void SetVector3(const char* name, float x, float y, float z) = 0;
			virtual void SetVector4(const char* name, float x, float y, float z, float w) = 0;
			virtual void SetMatrix4(const char* name, const float* matrix) = 0;

			// Uniform位置查询
			virtual int32_t GetUniformLocation(const char* name) = 0;
		};

		using ShaderPtr = std::shared_ptr<Shader>;

	} // namespace Graphics
} // namespace Engine