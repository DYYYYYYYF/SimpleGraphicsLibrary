# 示例项目
message(STATUS "Building GameEngine Examples")

# 通用示例配置函数
function(add_example target_name source_dir)
    # 获取源文件
    file(GLOB EXAMPLE_SOURCES 
        "${source_dir}/*.cpp" 
        "${source_dir}/*.h"
    )
    
    # 创建可执行文件
    add_executable(${target_name} ${EXAMPLE_SOURCES})
    
    # 链接库
    target_link_libraries(${target_name} PRIVATE
        Engine::Engine
    )
 
    # 设置输出目录
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
    
    # 如果有assets文件夹，复制到输出目录
    if(EXISTS "${source_dir}/assets")
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${source_dir}/assets"
            "$<TARGET_FILE_DIR:${target_name}>/assets"
            COMMENT "Copying assets for ${target_name}"
        )
    endif()
       
    # 修改runtime目标前拷贝DLL
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:EngineCore>
        $<TARGET_FILE_DIR:${target_name}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:EnginePlatform>
        $<TARGET_FILE_DIR:${target_name}>
        COMMENT "Copying dlls for ${target_name}"
    )
    
    # 设置工作目录（对于IDE）
    set_target_properties(${target_name} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${target_name}>"
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${target_name}>"
    )
    
    # 添加到examples文件夹（在IDE中组织）
    set_target_properties(${target_name} PROPERTIES FOLDER "Examples")
endfunction()

# 添加各个示例
add_example(BasicWindow "${CMAKE_CURRENT_SOURCE_DIR}/BasicWindow")
add_example(SimpleEvent "${CMAKE_CURRENT_SOURCE_DIR}/SimpleEvent")
