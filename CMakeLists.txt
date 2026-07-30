cmake_minimum_required(VERSION 3.10)
project(simpleCG)

set(Test0 SDL2_test)
set(Test1 BasicRayTracing)
set(Test2 DirectLight)
set(Test3 HardShadows)
set(Test4 RecursiveReflection)

set(main ${Test4})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_executable(${main} "${CMAKE_CURRENT_SOURCE_DIR}/example/${main}.cpp")

set(SUBDIRS 
    simple_cg 
    SDL2)

foreach(subdir IN LISTS SUBDIRS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/${subdir}")
endforeach(subdir IN LISTS SUBDIRS)

target_link_libraries(${main} simple_cg)
target_link_libraries(${main} SDL2 SDL2main)

add_custom_command(TARGET ${main} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/lib/SDL2.dll"
        $<TARGET_FILE_DIR:${main}>
        COMMAND $<TARGET_FILE:${main}>#运行程序
        COMMENT "Running ${main}..."
)