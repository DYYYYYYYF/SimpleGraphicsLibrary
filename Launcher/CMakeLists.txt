message(STATUS "Building GameEngine Launcher")

file(GLOB LAUNCHER_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(Launcher ${LAUNCHER_SOURCES})

# 链接库
target_link_libraries(Launcher PRIVATE
    Editor
)
 
# 设置工作目录（对于IDE）
set_target_properties(Launcher PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Launcher>"
    XCODE_GENERATE_SCHEME TRUE
    XCODE_SCHEME_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Launcher>"
)

