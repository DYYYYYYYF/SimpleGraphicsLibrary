project(EngineCore)

# Core库
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventManager.cpp
)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EventManager.h
)

# 创建Core库
add_library(EngineCore STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# 链接Cocoa库
if(APPLE)
    target_link_libraries(EngineCore 
        "-framework Cocoa"
        "-framework Foundation"
    )
endif()

# 确保源文件也能找到头文件
target_include_directories(EngineCore PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
