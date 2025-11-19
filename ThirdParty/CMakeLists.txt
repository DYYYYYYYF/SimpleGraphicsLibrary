# UncleDon-Logger 日志库
if(NOT EXISTS UncleDon-Logger)
    include(FetchContent)
    FetchContent_Declare(
        UncleDon-Logger
        GIT_REPOSITORY https://github.com/DYYYYYYYF/UncleDon-Logger.git
        GIT_TAG 1.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(UL_BUILD_EXAMPLE_EXE OFF)           # 关闭Example编译
    FetchContent_MakeAvailable(UncleDon-Logger)
else()
    set(UL_BUILD_EXAMPLE_EXE OFF)			# 关闭Example编译
    add_subdirectory(UncleDon-Logger)
endif()


# Eigen数学库
if(NOT EXISTS Eigen)
    include(FetchContent)
    FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 5.0.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(Eigen)

    message(STATUS "Eigen源码路径: ${eigen_SOURCE_DIR}")
    message(STATUS "Eigen构建路径: ${eigen_BINARY_DIR}")    
endif()

set(Eigen_SOURCE_DIR GLOBAL Eigen)
