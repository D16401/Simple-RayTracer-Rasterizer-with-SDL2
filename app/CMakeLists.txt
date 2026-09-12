# release Apps
set(SRT_APPS
    GeneralRaytracer
    GeneralRasterizer
)

foreach(app IN LISTS SRT_APPS)
    add_executable(${app} "${CMAKE_CURRENT_SOURCE_DIR}/${app}.cpp")
    target_link_libraries(${app} PRIVATE
        simple_cg
        external
    )    
    target_compile_definitions(${app} PRIVATE
        SRT_LOG_LEVEL=${SRT_LOG_LEVEL}
        SRT_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
    )
    #函数定义在 external/CMakeLists.txt 里
    #根目录的CMakeLists必须先执行add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external")
    srt_copy_runtime_dll(${app})
endforeach()
