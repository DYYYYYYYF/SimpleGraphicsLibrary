project(EngineGraphics)

# 源文件
set(GRAPHICS_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/API/RendererAPI.cpp
)

set(GRAPHICS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/API/RendererAPI.h
)

find_package(OpenGL QUIET)
find_package(Vulkan QUIET)

# OpenGL后端
if(ENGINE_ENABLE_OPENGL)
    list(APPEND GRAPHICS_SOURCES
       Backend/OpenGL/glad/glad.c
    )
    list(APPEND GRAPHICS_HEADERS
        Backend/OpenGL/glad/glad.h
        Backend/OpenGL/glad/KHR/khrplatform.h
    )
endif()

# Vulkan后端
if(ENGINE_ENABLE_VULKAN AND DVULKAN_ENABLED)
    target_include_directories(RayTracer PUBLIC ${Vulkan_INCLUDE_DIRS})
	target_link_libraries(RayTracer PUBLIC Vulkan::Vulkan)
endif()

# 创建库
add_library(EngineGraphics ${GRAPHICS_SOURCES} ${GRAPHICS_HEADERS})

# 包含目录
target_include_directories(EngineGraphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<$<BOOL:${ENGINE_ENABLE_OPENGL}>: Backend/OpenGL/glad>
)

# 链接依赖
target_link_libraries(EngineGraphics
    PUBLIC
        EngineCore
        EnginePlatform
    PRIVATE
        $<$<BOOL:${ENGINE_ENABLE_OPENGL}>:OpenGL::GL>
)

# 编译定义
if(ENGINE_ENABLE_OPENGL)
    target_compile_definitions(EngineGraphics PUBLIC ENGINE_OPENGL_ENABLED)
endif()
if(ENGINE_ENABLE_DX12 AND WIN32)
    target_compile_definitions(EngineGraphics PUBLIC ENGINE_DX12_ENABLED)
endif()
if(ENGINE_ENABLE_VULKAN)
    target_compile_definitions(EngineGraphics PUBLIC ENGINE_VULKAN_ENABLED)
endif()

# 设置文件夹
set_target_properties(EngineGraphics PROPERTIES FOLDER "Graphics")