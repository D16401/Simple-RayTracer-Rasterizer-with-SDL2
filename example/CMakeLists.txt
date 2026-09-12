# 交互式示例：只构建，不注册 CTest（它们会阻塞在窗口事件循环）
set(SRT_BUILD_EXAMPLES
    raytracer/BasicRayTracing
    raytracer/DirectLight
    raytracer/HardShadows
    raytracer/RecursiveReflection
    rasterizer/Line
    rasterizer/Triangle
)

foreach(exm IN LISTS SRT_BUILD_EXAMPLES)
    #get_filename_component(输出变量 输入路径 NAME) 从路径中取出最后一段。
    #例如 raytracer/BasicRayTracing → exName = BasicRayTracing。
    get_filename_component(exmName "${exm}" NAME)
    add_executable(${exmName} "${CMAKE_CURRENT_SOURCE_DIR}/${exm}.cpp")
    target_link_libraries(${exmName} PRIVATE 
        simple_cg 
        external
    )
    target_compile_definitions(${exmName} PRIVATE
        SRT_LOG_LEVEL=${SRT_LOG_LEVEL}
        SRT_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
    )
    srt_copy_runtime_dll(${exmName})
endforeach()