class GraphicsContext {
public:
	enum class API {
		None = 0,
		OpenGL,
		Vulkan,
		DirectX11,
		DirectX12
	};

	virtual ~GraphicsContext() = default;

	virtual void Init() = 0;
	virtual void SwapBuffers() = 0;
	virtual void MakeCurrent() = 0;

	virtual API GetAPI() const = 0;

	static std::unique_ptr<GraphicsContext> Create(void* nativeWindow);
};