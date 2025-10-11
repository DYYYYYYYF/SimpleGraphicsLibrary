project(EngineEngine)

file(GLOB ENGINE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(EngineEngine SHARED ${ENGINE_SOURCES})

# 链接库
target_link_libraries(EngineEngine PRIVATE
    EngineCore
    EnginePlatform
)

target_compile_definitions(EngineEngine PRIVATE ENGINE_ENGINE_EXPORTS)