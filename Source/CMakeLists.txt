# ============================================================================
# Source 目录 CMakeLists.txt
# 职责：管理引擎各个模块的添加顺序和依赖关系
# ============================================================================

# 这个文件被根目录的CMakeLists.txt通过add_subdirectory(Source)调用

message(STATUS "")
message(STATUS "Configuring Engine Modules...")
message(STATUS "----------------------------------------")

# ============================================================================
# 全局包含目录设置
# ============================================================================

# 让所有子模块都能通过 #include <Core/xxx.h> 的方式包含头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ============================================================================
# 模块依赖关系说明
# ============================================================================
# Core (无依赖) - 基础模块
#   ↓
# Platform (依赖 Core) - 平台抽象
#   ↓
# Graphics (依赖 Core, Platform) - 图形API
#   ↓
# Rendering (依赖 Core, Graphics) - 渲染系统
#   ↓
# Resource (依赖 Core, Graphics, Rendering) - 资源管理
#   ↓
# Scene (依赖 Core, Rendering) - 场景系统
#   ↓
# Components (依赖 Core, Rendering, Scene) - 组件系统
#   ↓
# Input (依赖 Core, Platform) - 输入系统
#   ↓
# Engine (依赖以上所有) - 引擎主模块

# ============================================================================
# 按依赖顺序添加模块
# ============================================================================

# 第一层：基础模块
message(STATUS "[1/9] Configuring Core...")
add_subdirectory(Core)

# 第二层：平台层
message(STATUS "[2/9] Configuring Platform...")
add_subdirectory(Platform)

## 第三层：图形层
#message(STATUS "[3/9] Configuring Graphics...")
#add_subdirectory(Graphics)
#
## 第四层：渲染层
#message(STATUS "[4/9] Configuring Rendering...")
#add_subdirectory(Rendering)
#
## 第五层：资源管理
#message(STATUS "[5/9] Configuring Resource...")
#add_subdirectory(Resource)
#
## 第六层：场景系统
#message(STATUS "[6/9] Configuring Scene...")
#add_subdirectory(Scene)
#
## 第七层：组件系统
#message(STATUS "[7/9] Configuring Components...")
#add_subdirectory(Components)
#
## 第八层：输入系统
#message(STATUS "[8/9] Configuring Input...")
#add_subdirectory(Input)
#
## 第九层：引擎主模块
#message(STATUS "[9/9] Configuring Engine...")
#add_subdirectory(Engine)

# ============================================================================
# 可选模块
# ============================================================================

if(ENGINE_BUILD_AUDIO)
    message(STATUS "[Optional] Configuring Audio...")
    add_subdirectory(Audio)
endif()

if(ENGINE_BUILD_PHYSICS)
    message(STATUS "[Optional] Configuring Physics...")
    add_subdirectory(Physics)
endif()

message(STATUS "----------------------------------------")

# ============================================================================
# 创建统一的Engine接口库（可选，简化外部使用）
# ============================================================================

option(ENGINE_CREATE_UNIFIED_INTERFACE "Create unified Engine interface library" ON)

if(ENGINE_CREATE_UNIFIED_INTERFACE)
    # 创建接口库，不包含源文件，只提供链接接口
    add_library(Engine INTERFACE)
    
    # 链接所有必需模块
    target_link_libraries(Engine INTERFACE
        EngineCore
        EnginePlatform
        #EngineGraphics
        #EngineRendering
        #EngineResource
        #EngineScene
        #EngineComponents
        #EngineInput
        #EngineEngine
    )
    
    # 链接可选模块
    if(ENGINE_BUILD_AUDIO)
        target_link_libraries(Engine INTERFACE EngineAudio)
    endif()
    
    if(ENGINE_BUILD_PHYSICS)
        target_link_libraries(Engine INTERFACE EnginePhysics)
    endif()
    
    # 设置包含目录
    target_include_directories(Engine INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/Engine>
    )
    
    # 创建别名
    add_library(Engine::Engine ALIAS Engine)
    
    message(STATUS "Created unified Engine interface library")
endif()

# ============================================================================
# 模块验证
# ============================================================================

# 检查所有必需模块是否成功创建
set(REQUIRED_TARGETS
    EngineCore
    EnginePlatform
    #EngineGraphics
    #EngineRendering
    #EngineResource
    #EngineScene
    #EngineComponents
    #EngineInput
    #EngineEngine
)

foreach(TARGET ${REQUIRED_TARGETS})
    if(NOT TARGET ${TARGET})
        message(FATAL_ERROR "Required target ${TARGET} was not created!")
    endif()
endforeach()

message(STATUS "All required modules configured successfully")

# ============================================================================
# 模块信息汇总
# ============================================================================

message(STATUS "")
message(STATUS "Engine Modules Summary:")
message(STATUS "  ✓ Core")
message(STATUS "  ✓ Platform")
message(STATUS "  ✓ Graphics")
message(STATUS "  ✓ Rendering")
message(STATUS "  ✓ Resource")
message(STATUS "  ✓ Scene")
message(STATUS "  ✓ Components")
message(STATUS "  ✓ Input")
message(STATUS "  ✓ Engine")

if(ENGINE_BUILD_AUDIO)
    message(STATUS "  ✓ Audio")
else()
    message(STATUS "  ✗ Audio (disabled)")
endif()

if(ENGINE_BUILD_PHYSICS)
    message(STATUS "  ✓ Physics")
else()
    message(STATUS "  ✗ Physics (disabled)")
endif()

message(STATUS "")