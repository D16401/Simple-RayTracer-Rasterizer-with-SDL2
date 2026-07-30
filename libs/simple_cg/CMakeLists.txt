cmake_minimum_required(VERSION 3.10)
project(simple_cg_lib)

set(mathSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_math/cg_math.cpp")
add_library(cg_math STATIC ${mathSrc})
target_include_directories(cg_math PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_math")

set(splrSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_sampler/cg_sampler.cpp")
add_library(cg_sampler STATIC ${splrSrc})
target_include_directories(cg_sampler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_sampler")

set(sdlwpSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_wrapper/sdl_wrapper.cpp")
add_library(sdl_wrapper STATIC ${sdlwpSrc})
target_link_libraries(sdl_wrapper PRIVATE cg_math)
target_link_libraries(sdl_wrapper PRIVATE cg_sampler)
target_link_libraries(sdl_wrapper PRIVATE SDL2 SDL2main)
target_include_directories(sdl_wrapper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/sdl_wrapper")

set(rtSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/raytracing/cg_rt_interaction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/raytracing/cg_rt_structure.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/raytracing/cg_sampler_rt.cpp")
add_library(raytracing STATIC ${rtSrc})
target_link_libraries(raytracing PRIVATE cg_math)
target_link_libraries(raytracing PRIVATE cg_sampler)
target_link_libraries(raytracing PRIVATE sdl_wrapper)
target_include_directories(raytracing PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/raytracing")

add_library(simple_cg INTERFACE)#接口库，集合
target_link_libraries(simple_cg INTERFACE cg_math cg_sampler raytracing sdl_wrapper)
target_include_directories(simple_cg INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
