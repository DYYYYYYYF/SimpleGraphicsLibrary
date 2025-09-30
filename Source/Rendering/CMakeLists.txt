project(EngineRendering)

# 头文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Rendering库
set(Platform_SOURCES 
     ${CMAKE_CURRENT_SOURCE_DIR}/CommandBuffer.cpp
)

set(Platform_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/CommandBuffer.h
)

# 创建Rendering库
add_library(EngineRendering STATIC ${Platform_SOURCES} ${Platform_HEADERS})

target_link_libraries(EngineRendering PRIVATE 
    EngineCore
    EngineGraphics
)

target_include_directories(EngineRendering PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)