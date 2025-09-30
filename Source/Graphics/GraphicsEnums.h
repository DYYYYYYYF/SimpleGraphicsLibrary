#pragma once

namespace Engine {
	namespace Graphics {

		enum class GraphicsBackend {
			None,
			OpenGL,
			DirectX12,
			Vulkan
		};

		enum class PrimitiveTopology {
			TriangleList,
			TriangleStrip,
			LineList,
			PointList
		};

		enum class TextureFormat {
			None,
			RGBA8,
			RGBA16F,
			RGBA32F,
			Depth24Stencil8
		};

		enum class BufferUsage {
			Static,
			Dynamic,
			Stream
		};

		enum class ShaderStage {
			Vertex,
			Fragment,
			Geometry,
			Compute
		};

		enum class BlendMode {
			Opaque,
			AlphaBlend,
			Additive,
			Multiply
		};

		enum class CullMode {
			None,
			Front,
			Back
		};

		enum class DepthTest {
			Never,
			Less,
			Equal,
			LessEqual,
			Greater,
			NotEqual,
			GreaterEqual,
			Always
		};

	} // namespace Graphics
} // namespace Engine