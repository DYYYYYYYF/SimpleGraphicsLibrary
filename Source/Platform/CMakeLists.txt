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
    ${CMAKE_CURRENT_SOURCE_DIR}/DLL/DynamicLibrary.h
)

# 平台相关源文件
if(WIN32)
    list(APPEND Platform_HEADERS 
        ${CMAKE_CURRENT_SOURCE_DIR}/Window/Windows/WindowsWindow.h
    )
    list(APPEND Platform_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Window/Windows/WindowsWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DLL/Windows/Win32DynamicLibrary.cpp
    )
elseif(APPLE)
    list(APPEND Platform_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Window/Apple/AppleWindow.mm
        ${CMAKE_CURRENT_SOURCE_DIR}/DLL/Apple/AppleDynamicLibrary.cpp
    )

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Window/Apple/AppleWindow.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

# 创建Platform库
add_library(EnginePlatform SHARED ${Platform_SOURCES} ${Platform_HEADERS})

target_link_libraries(EnginePlatform PRIVATE 
    EngineCore
)

target_include_directories(EnginePlatform PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 宏
target_compile_definitions(EnginePlatform PRIVATE ENGINE_PLATFORM_EXPORTS)
