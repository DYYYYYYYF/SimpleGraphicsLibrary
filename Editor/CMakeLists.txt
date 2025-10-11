# 示例项目
message(STATUS "Building GameEngine Editor")

file(GLOB EDITOR_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(Editor SHARED ${EDITOR_SOURCES})

# 链接库
target_link_libraries(Editor PRIVATE
    Engine::Engine
)

target_include_directories(Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
 
# 设置工作目录（对于IDE）
set_target_properties(Editor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Editor>"
    XCODE_GENERATE_SCHEME TRUE
    XCODE_SCHEME_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Editor>"
)

# 宏
target_compile_definitions(Editor PRIVATE EDITOR_EXPORTS)
