project(EngineFramework)

file(GLOB FRAMEWORK_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Actors/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Actors/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Components/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Components/*.h"
)

message(${FRAMEWORK_SOURCES})
add_library(EngineFramework SHARED ${FRAMEWORK_SOURCES})

# Á´½Ó¿â
target_link_libraries(EngineFramework PRIVATE
    EngineCore
    EnginePlatform
    EngineRendering
)

target_compile_definitions(EngineFramework PRIVATE ENGINE_FRAMEWORK_EXPORTS)