# ============================================================================
# GameEngine 根目录 CMakeLists.txt - 简化版
# ============================================================================

cmake_minimum_required(VERSION 3.15)

project(GameEngine 
    VERSION 1.0.0
    DESCRIPTION "Simple Game Engine"
    LANGUAGES CXX
)

# ============================================================================
# 全局配置
# ============================================================================

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib)

# 多配置生成器
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/Bin/${CONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/Lib/${CONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/Lib/${CONFIG})
endforeach()

# ============================================================================
# 构建选项
# ============================================================================

option(ENGINE_BUILD_SHARED "Build shared libraries" OFF)
option(ENGINE_BUILD_EXAMPLES "Build example programs" ON)
option(ENGINE_BUILD_EDITOR "Build editor" ON)
option(ENGINE_BUILD_AUDIO "Build audio module" OFF)
option(ENGINE_BUILD_PHYSICS "Build physics module" OFF)

option(ENGINE_ENABLE_OPENGL "Enable OpenGL backend" ON)
option(ENGINE_ENABLE_VULKAN "Enable Vulkan backend" ON)

if(WIN32)
option(ENGINE_ENABLE_DX12 "Enable DirectX12 backend" ON)
endif()

# ============================================================================
# 编译器设置
# ============================================================================

if(MSVC)
    add_compile_options(/W4 /permissive- /MP)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ============================================================================
# 平台检测
# ============================================================================

if(WIN32)
    add_compile_definitions(ENGINE_PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    add_compile_definitions(ENGINE_PLATFORM_LINUX)
elseif(APPLE)
    add_compile_definitions(ENGINE_PLATFORM_MACOS)
endif()

# ============================================================================
# 添加子目录
# ============================================================================

message(STATUS "========================================")
message(STATUS "Configuring GameEngine")
message(STATUS "========================================")

# 第三方库
message(STATUS "Configuring Third-Party Libraries...")
add_subdirectory(ThirdParty)

# 引擎源代码
message(STATUS "Configuring Engine Source...")
add_subdirectory(Source)

# 示例项目
if(ENGINE_BUILD_EXAMPLES)
    message(STATUS "Configuring Examples...")
    add_subdirectory(Examples)
endif()

# 编辑器
if(ENGINE_BUILD_EDITOR)
    message(STATUS "Configuring Editor...")
    add_subdirectory(Editor)
endif()

# 启动器
add_subdirectory(Launcher)

# ============================================================================
# 构建信息汇总
# ============================================================================

message(STATUS "========================================")
message(STATUS "Build Configuration:")
message(STATUS "  Version:       ${PROJECT_VERSION}")
message(STATUS "  Build Type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared Libs:   ${ENGINE_BUILD_SHARED}")
message(STATUS "  Examples:      ${ENGINE_BUILD_EXAMPLES}")
message(STATUS "  Editor:        ${ENGINE_BUILD_EDITOR}")
message(STATUS "")
message(STATUS "Graphics Backends:")
message(STATUS "  OpenGL:        ${ENGINE_ENABLE_OPENGL}")
message(STATUS "  DirectX12:     ${ENGINE_ENABLE_DX12}")
message(STATUS "  Vulkan:        ${ENGINE_ENABLE_VULKAN}")
message(STATUS "========================================")
