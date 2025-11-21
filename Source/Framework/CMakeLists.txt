project(EngineFramework)

file(GLOB FRAMEWORK_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Actors/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Actors/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Components/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Components/*.h"
)

message(${FRAMEWORK_SOURCES})
add_library(EngineFramework SHARED ${FRAMEWORK_SOURCES})

target_link_libraries(EngineFramework PRIVATE
    EngineCore
    EnginePlatform
    EngineRendering
    EngineSystem
)

target_compile_definitions(EngineFramework PRIVATE ENGINE_FRAMEWORK_EXPORTS)