// Rendering/CommandBuffer.h
#pragma once
#include "Graphics/GraphicsDevice.h"
#include <vector>
#include <functional>

namespace Engine {
	namespace Rendering {

		// 渲染命令类型
		enum class CommandType {
			SetViewport,
			Clear,
			SetBlendMode,
			SetCullMode,
			SetDepthTest,
			BindShader,
			BindTexture,
			BindVertexBuffer,
			BindIndexBuffer,
			SetShaderParam,
			Draw,
			DrawIndexed
		};

		// 命令参数
		struct CommandParams {
			CommandType type;

			union {
				struct { Graphics::Viewport viewport; } setViewport;
				struct { bool color; bool depth; bool stencil; } clear;
				struct { Graphics::BlendMode mode; } setBlendMode;
				struct { Graphics::CullMode mode; } setCullMode;
				struct { Graphics::DepthTest test; bool writeEnabled; } setDepthTest;
				struct { Graphics::ShaderPtr shader; } bindShader;
				struct { uint32_t slot; Graphics::TexturePtr texture; } bindTexture;
				struct { Graphics::VertexBufferPtr buffer; } bindVertexBuffer;
				struct { Graphics::IndexBufferPtr buffer; } bindIndexBuffer;
				struct { Graphics::PrimitiveTopology topology; uint32_t count; uint32_t start; } draw;
				struct { Graphics::PrimitiveTopology topology; uint32_t count; uint32_t start; int32_t base; } drawIndexed;
			};

			// 用于存储shader参数的回调
			std::function<void(Graphics::GraphicsDevice*)> shaderParamCallback;
		};

		// 命令缓冲 - 类似Unity的CommandBuffer
		class CommandBuffer {
		public:
			CommandBuffer(const std::string& name = "CommandBuffer");
			~CommandBuffer();

			// 获取名称
			const std::string& GetName() const { return m_Name; }

			// 清空所有命令
			void Clear();

			// 获取命令数量
			size_t GetCommandCount() const { return m_Commands.size(); }

			// 视口控制
			void SetViewport(const Graphics::Viewport& viewport);

			// 清屏命令
			void ClearRenderTarget(bool clearColor, bool clearDepth, bool clearStencil);
			void ClearRenderTarget(const Graphics::Color& color, bool clearDepth = true);

			// 渲染状态命令
			void SetBlendMode(Graphics::BlendMode mode);
			void SetCullMode(Graphics::CullMode mode);
			void SetDepthTest(Graphics::DepthTest test, bool writeEnabled = true);

			// 资源绑定命令
			void SetShader(Graphics::ShaderPtr shader);
			void SetTexture(uint32_t slot, Graphics::TexturePtr texture);
			void SetVertexBuffer(Graphics::VertexBufferPtr buffer);
			void SetIndexBuffer(Graphics::IndexBufferPtr buffer);

			// Shader参数设置命令
			void SetInt(const char* name, int32_t value);
			void SetFloat(const char* name, float value);
			void SetVector2(const char* name, float x, float y);
			void SetVector3(const char* name, float x, float y, float z);
			void SetVector4(const char* name, float x, float y, float z, float w);
			void SetMatrix4(const char* name, const float* matrix);

			// 绘制命令
			void Draw(Graphics::PrimitiveTopology topology, uint32_t vertexCount,
				uint32_t startVertex = 0);

			void DrawIndexed(Graphics::PrimitiveTopology topology, uint32_t indexCount,
				uint32_t startIndex = 0, int32_t baseVertex = 0);

			// 执行所有命令
			void Execute(Graphics::GraphicsDevice* device);

			// 执行指定范围的命令
			void Execute(Graphics::GraphicsDevice* device, size_t startIndex, size_t count);

		private:
			std::string m_Name;
			std::vector<CommandParams> m_Commands;

			void AddCommand(const CommandParams& cmd);
		};

		using CommandBufferPtr = std::shared_ptr<CommandBuffer>;

	} // namespace Rendering
} // namespace Engine