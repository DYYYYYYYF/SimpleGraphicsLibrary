class RendererAPI {
public:
	enum class API {
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
		DirectX11 = 3,
		DirectX12 = 4
	};

	enum class PrimitiveTopology {
		Points,
		Lines,
		Triangles,
		TriangleStrip
	};

	virtual ~RendererAPI() = default;

	virtual void Init() = 0;
	virtual void Destory() = 0;

	static API GetAPI() { return s_API; }

private:
	static API s_API;
};