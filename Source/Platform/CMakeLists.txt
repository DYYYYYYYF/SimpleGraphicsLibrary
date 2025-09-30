project(EnginePlatform)

# 头文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Platform库
set(Platform_SOURCES 
     ${CMAKE_CURRENT_SOURCE_DIR}/Window/Window.cpp
)

set(Platform_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Window/Window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Window/WindowImpl.h
)

# 平台相关源文件
if(WIN32)
    list(APPEND Platform_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Window/Windows/WindowsWindow.cpp
    )
elseif(APPLE)
    list(APPEND Platform_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Window/Apple/AppleWindow.mm
    )

    set_source_files_properties( ${CMAKE_CURRENT_SOURCE_DIR}/Window/Apple/AppleWindow.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

# 创建Core库
add_library(EnginePlatform STATIC ${Platform_SOURCES} ${Platform_HEADERS})

target_link_libraries(EnginePlatform PRIVATE 
    EngineCore
)

target_include_directories(EnginePlatform PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)