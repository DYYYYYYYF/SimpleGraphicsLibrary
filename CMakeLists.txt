cmake_minimum_required(VERSION 3.16)

project(SimpleGraphicsLibrary
		VERSION 1.0.0
		LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# 选项
option(SGL_BUILD_EXAMPLES "Build examples" ON)

# 第三方库
add_subdirectory(3rd)

# 核心模块
add_subdirectory(libs/Core)
add_subdirectory(libs/Graphics)

# 库
add_library(SimpleGraphicsLibrary INTERFACE)
target_link_libraries(SimpleGraphicsLibrary INTERFACE
	SimpleGraphicsLibrary::Core
	SimpleGraphicsLibrary::Graphics
)

# 设置别名
add_library(SimpleGraphicsLibrary::SimpleGraphicsLibrary ALIAS SimpleGraphicsLibrary)

# 示例
if(SGL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
