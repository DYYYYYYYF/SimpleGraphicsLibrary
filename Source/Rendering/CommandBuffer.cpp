// Rendering/CommandBuffer.cpp
#include "CommandBuffer.h"

namespace Engine {
	namespace Rendering {

		CommandBuffer::CommandBuffer(const std::string& name) : m_Name(name) {
		}

		CommandBuffer::~CommandBuffer() {
			Clear();
		}

		void CommandBuffer::Clear() {
			m_Commands.clear();
		}

		void CommandBuffer::SetViewport(const Graphics::Viewport& viewport) {
			CommandParams cmd;
			cmd.type = CommandType::SetViewport;
			cmd.setViewport.viewport = viewport;
			AddCommand(cmd);
		}

		void CommandBuffer::ClearRenderTarget(bool clearColor, bool clearDepth, bool clearStencil) {
			CommandParams cmd;
			cmd.type = CommandType::Clear;
			cmd.clear.color = clearColor;
			cmd.clear.depth = clearDepth;
			cmd.clear.stencil = clearStencil;
			AddCommand(cmd);
		}

		void CommandBuffer::ClearRenderTarget(const Graphics::Color& color, bool clearDepth) {
			// 先设置清屏颜色（通过回调）
			CommandParams colorCmd;
			colorCmd.type = CommandType::SetShaderParam;
			colorCmd.shaderParamCallback = [color](Graphics::GraphicsDevice* device) {
				device->SetClearColor(color);
				};
			AddCommand(colorCmd);

			// 再执行清屏
			ClearRenderTarget(true, clearDepth, false);
		}

		void CommandBuffer::SetBlendMode(Graphics::BlendMode mode) {
			CommandParams cmd;
			cmd.type = CommandType::SetBlendMode;
			cmd.setBlendMode.mode = mode;
			AddCommand(cmd);
		}

		void CommandBuffer::SetCullMode(Graphics::CullMode mode) {
			CommandParams cmd;
			cmd.type = CommandType::SetCullMode;
			cmd.setCullMode.mode = mode;
			AddCommand(cmd);
		}

		void CommandBuffer::SetDepthTest(Graphics::DepthTest test, bool writeEnabled) {
			CommandParams cmd;
			cmd.type = CommandType::SetDepthTest;
			cmd.setDepthTest.test = test;
			cmd.setDepthTest.writeEnabled = writeEnabled;
			AddCommand(cmd);
		}

		void CommandBuffer::SetShader(Graphics::ShaderPtr shader) {
			CommandParams cmd;
			cmd.type = CommandType::BindShader;
			cmd.bindShader.shader = shader;
			AddCommand(cmd);
		}

		void CommandBuffer::SetTexture(uint32_t slot, Graphics::TexturePtr texture) {
			CommandParams cmd;
			cmd.type = CommandType::BindTexture;
			cmd.bindTexture.slot = slot;
			cmd.bindTexture.texture = texture;
			AddCommand(cmd);
		}

		void CommandBuffer::SetVertexBuffer(Graphics::VertexBufferPtr buffer) {
			CommandParams cmd;
			cmd.type = CommandType::BindVertexBuffer;
			cmd.bindVertexBuffer.buffer = buffer;
			AddCommand(cmd);
		}

		void CommandBuffer::SetIndexBuffer(Graphics::IndexBufferPtr buffer) {
			CommandParams cmd;
			cmd.type = CommandType::BindIndexBuffer;
			cmd.bindIndexBuffer.buffer = buffer;
			AddCommand(cmd);
		}

		void CommandBuffer::SetInt(const char* name, int32_t value) {
			CommandParams cmd;
			cmd.type = CommandType::SetShaderParam;
			std::string paramName = name;
			cmd.shaderParamCallback = [paramName, value](Graphics::GraphicsDevice* device) {
				device->SetShaderInt(paramName.c_str(), value);
				};
			AddCommand(cmd);
		}

		void CommandBuffer::SetFloat(const char* name, float value) {
			CommandParams cmd;
			cmd.type = CommandType::SetShaderParam;
			std::string paramName = name;
			cmd.shaderParamCallback = [paramName, value](Graphics::GraphicsDevice* device) {
				device->SetShaderFloat(paramName.c_str(), value);
				};
			AddCommand(cmd);
		}

		void CommandBuffer::SetMatrix4(const char* name, const float* matrix) {
			CommandParams cmd;
			cmd.type = CommandType::SetShaderParam;
			std::string paramName = name;
			// 复制矩阵数据
			std::vector<float> matrixData(matrix, matrix + 16);
			cmd.shaderParamCallback = [paramName, matrixData](Graphics::GraphicsDevice* device) {
				device->SetShaderMatrix4(paramName.c_str(), matrixData.data());
				};
			AddCommand(cmd);
		}

		void CommandBuffer::Draw(Graphics::PrimitiveTopology topology, uint32_t vertexCount, uint32_t startVertex) {
			CommandParams cmd;
			cmd.type = CommandType::Draw;
			cmd.draw.topology = topology;
			cmd.draw.count = vertexCount;
			cmd.draw.start = startVertex;
			AddCommand(cmd);
		}

		void CommandBuffer::DrawIndexed(Graphics::PrimitiveTopology topology, uint32_t indexCount,
			uint32_t startIndex, int32_t baseVertex) {
			CommandParams cmd;
			cmd.type = CommandType::DrawIndexed;
			cmd.drawIndexed.topology = topology;
			cmd.drawIndexed.count = indexCount;
			cmd.drawIndexed.start = startIndex;
			cmd.drawIndexed.base = baseVertex;
			AddCommand(cmd);
		}

		void CommandBuffer::Execute(Graphics::GraphicsDevice* device) {
			Execute(device, 0, m_Commands.size());
		}

		void CommandBuffer::Execute(Graphics::GraphicsDevice* device, size_t startIndex, size_t count) {
			if (!device) return;

			size_t endIndex = std::min(startIndex + count, m_Commands.size());

			for (size_t i = startIndex; i < endIndex; ++i) {
				const CommandParams& cmd = m_Commands[i];

				switch (cmd.type) {
				case CommandType::SetViewport:
					device->SetViewport(cmd.setViewport.viewport);
					break;

				case CommandType::Clear:
					device->Clear(cmd.clear.color, cmd.clear.depth, cmd.clear.stencil);
					break;

				case CommandType::SetBlendMode:
					device->SetBlendMode(cmd.setBlendMode.mode);
					break;

				case CommandType::SetCullMode:
					device->SetCullMode(cmd.setCullMode.mode);
					break;

				case CommandType::SetDepthTest:
					device->SetDepthTest(cmd.setDepthTest.test, cmd.setDepthTest.writeEnabled);
					break;

				case CommandType::BindShader:
					device->BindShader(cmd.bindShader.shader);
					break;

				case CommandType::BindTexture:
					device->BindTexture(cmd.bindTexture.slot, cmd.bindTexture.texture);
					break;

				case CommandType::BindVertexBuffer:
					device->BindVertexBuffer(cmd.bindVertexBuffer.buffer);
					break;

				case CommandType::BindIndexBuffer:
					device->BindIndexBuffer(cmd.bindIndexBuffer.buffer);
					break;

				case CommandType::SetShaderParam:
					if (cmd.shaderParamCallback) {
						cmd.shaderParamCallback(device);
					}
					break;

				case CommandType::Draw:
					device->Draw(cmd.draw.topology, cmd.draw.count, cmd.draw.start);
					break;

				case CommandType::DrawIndexed:
					device->DrawIndexed(cmd.drawIndexed.topology, cmd.drawIndexed.count,
						cmd.drawIndexed.start, cmd.drawIndexed.base);
					break;
				}
			}
		}

		void CommandBuffer::AddCommand(const CommandParams& cmd) {
			m_Commands.push_back(cmd);
		}

	} // namespace Rendering
} // namespace Engine