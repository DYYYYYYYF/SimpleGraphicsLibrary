project(EngineRendering)

# 源文件
set(RENDERING_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/Renderer.cpp
)
set(RENDERING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/Renderer.h
)

# 平台相关源文件
if(WIN32)
    list(APPEND RENDERING_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Backend/OpenGL/OpenGLDevice_Win32.cpp
    )
elseif(APPLE)
    list(APPEND RENDERING_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Backend/OpenGL/OpenGLDevice_Apple.mm
    )

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Window/Apple/AppleWindow.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenGL QUIET)
find_package(Vulkan QUIET)

# OpenGL后端
if(ENGINE_ENABLE_OPENGL)
    list(APPEND RENDERING_SOURCES
       Graphics/Backend/OpenGL/OpenGLDevice.cpp
       Graphics/Backend/OpenGL/glad/glad.c
    )
    list(APPEND RENDERING_HEADERS
        Graphics/Backend/OpenGL/OpenGLDevice.h
        Graphics/Backend/OpenGL/glad/glad.h
        Graphics/Backend/OpenGL/glad/KHR/khrplatform.h
    )
endif()

# 创建库
add_library(EngineRendering SHARED ${RENDERING_SOURCES} ${RENDERING_HEADERS})

# Vulkan后端
if(ENGINE_ENABLE_VULKAN)
    target_include_directories(EngineRendering PUBLIC ${Vulkan_INCLUDE_DIRS})
	target_link_libraries(EngineRendering PUBLIC Vulkan::Vulkan)
endif()

# 包含目录
target_include_directories(EngineRendering
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}

    PRIVATE
        $<$<BOOL:${ENGINE_ENABLE_OPENGL}>: Graphics/Backend/OpenGL/glad>
)

# 链接依赖
target_link_libraries(EngineRendering
    PRIVATE
        EngineCore
        EnginePlatform
        $<$<BOOL:${ENGINE_ENABLE_OPENGL}>:OpenGL::GL>
)

# 编译定义
if(ENGINE_ENABLE_OPENGL)
    target_compile_definitions(EngineRendering PUBLIC ENGINE_OPENGL_ENABLED)
endif()
if(ENGINE_ENABLE_DX12 AND WIN32)
    target_compile_definitions(EngineRendering PUBLIC ENGINE_DX12_ENABLED)
endif()
if(ENGINE_ENABLE_VULKAN)
    target_compile_definitions(EngineRendering PUBLIC ENGINE_VULKAN_ENABLED)
endif()

# 设置文件夹
set_target_properties(EngineRendering PROPERTIES FOLDER "Rendering")

# 宏
target_compile_definitions(EngineRendering PRIVATE ENGINE_RENDERING_EXPORTS)