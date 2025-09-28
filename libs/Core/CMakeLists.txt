# Core库
set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Core/Engine.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/Window.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/EventManager.cpp
)

set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Core/Engine.h
    ${CMAKE_SOURCE_DIR}/include/Core/Window.h
    ${CMAKE_SOURCE_DIR}/include/Core/EventManager.h
    ${CMAKE_SOURCE_DIR}/src/Core/WindowImpl.h
)

# 平台相关源文件
if(WIN32)
    list(APPEND CORE_SOURCES 
        ${CMAKE_SOURCE_DIR}/src/Platform/Windows/WindowsWindow.cpp
    )
elseif(APPLE)
    list(APPEND CORE_SOURCES 
        ${CMAKE_SOURCE_DIR}/src/Platform/Apple/AppleWindow.mm
    )

    set_source_files_properties(${CMAKE_SOURCE_DIR}/src/Platform/Apple/AppleWindow.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

# 创建Core库
add_library(SGL_Core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# 设置别名
add_library(SimpleGraphicsLibrary::Core ALIAS SGL_Core)

# 链接Cocoa库
if(APPLE)
    target_link_libraries(SGL_Core 
        "-framework Cocoa"
        "-framework Foundation"
    )
endif()

# 设置属性
target_include_directories(SGL_Core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 确保源文件也能找到头文件 (用于编译时)
target_include_directories(SGL_Core PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 设置输出目录
set_target_properties(SGL_Core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)