# Graphics库
set(GRAPHICS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Graphics/Renderer.cpp
    ${CMAKE_SOURCE_DIR}/src/Graphics/Texture.cpp
    ${CMAKE_SOURCE_DIR}/src/Graphics/Shader.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils/ResourceManager.cpp
)

set(GRAPHICS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Graphics/Renderer.h
    ${CMAKE_SOURCE_DIR}/include/Graphics/Texture.h
    ${CMAKE_SOURCE_DIR}/include/Graphics/Shader.h
    ${CMAKE_SOURCE_DIR}/include/Utils/ResourceManager.h
)

# 创建Graphics库
add_library(SGL_Graphics STATIC ${GRAPHICS_SOURCES} ${GRAPHICS_HEADERS})

# 设置别名
add_library(SimpleGraphicsLibrary::Graphics ALIAS SGL_Graphics)

# 设置属性
target_include_directories(SGL_Graphics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 设置输出目录
set_target_properties(SGL_Graphics PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)