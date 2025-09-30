project(EngineGraphics)

# 源文件
set(GRAPHICS_SOURCES
    GraphicsDevice.cpp
    GraphicsContext.cpp
    GraphicsResource.cpp
    VertexLayout.cpp
)

set(GRAPHICS_HEADERS
    GraphicsDevice.h
    GraphicsContext.h
    GraphicsEnums.h
    GraphicsResource.h
    Shader.h
    Texture.h
    Buffer.h
    RenderTarget.h
    VertexLayout.h
)

# OpenGL后端
if(ENGINE_ENABLE_OPENGL)
    list(APPEND GRAPHICS_SOURCES
        Backend/OpenGL/OpenGLDevice.cpp
        Backend/OpenGL/OpenGLShader.cpp
        Backend/OpenGL/OpenGLTexture.cpp
        Backend/OpenGL/OpenGLBuffer.cpp
        Backend/OpenGL/OpenGLUtils.cpp
    )
    list(APPEND GRAPHICS_HEADERS
        Backend/OpenGL/OpenGLDevice.h
        Backend/OpenGL/OpenGLShader.h
        Backend/OpenGL/OpenGLTexture.h
        Backend/OpenGL/OpenGLBuffer.h
        Backend/OpenGL/OpenGLUtils.h
    )
endif()

# DirectX12后端
if(ENGINE_ENABLE_DX12)
    list(APPEND GRAPHICS_SOURCES
        Backend/D3D12/D3D12Device.cpp
        # ...
    )
endif()

# 创建库
add_library(EngineGraphics ${GRAPHICS_SOURCES} ${GRAPHICS_HEADERS})

# 包含目录
target_include_directories(EngineGraphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖
target_link_libraries(EngineGraphics
    PUBLIC
        EngineCore
        EnginePlatform
    PRIVATE
        $<$<BOOL:${ENGINE_ENABLE_OPENGL}>:glad>
        $<$<BOOL:${ENGINE_ENABLE_OPENGL}>:OpenGL::GL>
)

# 编译定义
if(ENGINE_ENABLE_OPENGL)
    target_compile_definitions(EngineGraphics PUBLIC ENGINE_OPENGL_ENABLED)
endif()
if(ENGINE_ENABLE_DX12)
    target_compile_definitions(EngineGraphics PUBLIC ENGINE_DX12_ENABLED)
endif()
if(ENGINE_ENABLE_VULKAN)
    target_compile_definitions(EngineGraphics PUBLIC ENGINE_VULKAN_ENABLED)
endif()

# 设置文件夹
set_target_properties(EngineGraphics PROPERTIES FOLDER "Engine")