project(EngineCore)

# Core库
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/EventManager.cpp
)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/EventManager.h
)

# 创建Core库
add_library(EngineCore SHARED ${CORE_SOURCES} ${CORE_HEADERS})

# 链接Cocoa库
if(APPLE)
    target_link_libraries(EngineCore 
        "-framework Cocoa"
        "-framework Foundation"
    )
endif()

# 通用第三方库
target_link_libraries(EngineCore 
    UL::Logger
)

# 确保源文件也能找到头文件
target_include_directories(EngineCore PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    UL::Logger
)

# 宏
target_compile_definitions(EngineCore PRIVATE ENGINE_CORE_EXPORTS)
