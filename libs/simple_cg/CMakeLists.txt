set(mathSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_math/cg_math.cpp")
add_library(cg_math STATIC ${mathSrc})
target_include_directories(cg_math PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_math")

set(wdSrc
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_world/cg_wd_camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_world/cg_wd_scene.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_world/cg_wd_light.cpp")
add_library(cg_world STATIC ${wdSrc})
#cg_world的公开头文件（如cg_wd_camera.h）直接#include "cg_math.h"，因此必须PUBLIC传递
target_link_libraries(cg_world PUBLIC cg_math)
target_include_directories(cg_world PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_world")

set(splrSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_sampler/cg_sampler.cpp")
add_library(cg_sampler STATIC ${splrSrc})
#cg_sampler.h公开头文件#include "cg_world.h"，因此必须PUBLIC传递
target_link_libraries(cg_sampler PUBLIC cg_world)
target_include_directories(cg_sampler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_sampler")

set(sdlwpSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_wrapper/sdl_wrapper.cpp")
add_library(sdl_wrapper STATIC ${sdlwpSrc})
#sdl_wrapper.h公开头文件#include "cg_math.h"、"cg_sampler.h"和SDL2/SDL.h，因此必须PUBLIC传递
target_link_libraries(sdl_wrapper PUBLIC cg_math)
target_link_libraries(sdl_wrapper PUBLIC cg_sampler)
target_link_libraries(sdl_wrapper PUBLIC external::SDL2 external::SDL2main)
target_include_directories(sdl_wrapper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/sdl_wrapper")

set(rtSrc 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_raytracer/cg_rt_interaction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_raytracer/cg_sampler_rt.cpp")
add_library(cg_raytracer STATIC ${rtSrc})
#cg_raytracer的公开头文件（cg_raytracer.h/cg_sampler_rt.h）include了cg_sampler.h/cg_world.h等，因此必须PUBLIC传递
target_link_libraries(cg_raytracer PUBLIC cg_math)
target_link_libraries(cg_raytracer PUBLIC cg_world)
target_link_libraries(cg_raytracer PUBLIC cg_sampler)
target_include_directories(cg_raytracer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_raytracer")

set(rzSrc
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_rasterizer/cg_sampler_rz.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_rasterizer/cg_rz_drawer.cpp")
add_library(cg_rasterizer STATIC ${rzSrc})
#cg_rasterizer的公开头文件include了cg_sampler.h/cg_world.h等，因此必须PUBLIC传递
target_link_libraries(cg_rasterizer PUBLIC cg_math)
target_link_libraries(cg_rasterizer PUBLIC cg_world)
target_link_libraries(cg_rasterizer PUBLIC cg_sampler)
target_include_directories(cg_rasterizer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_rasterizer")

set(ldSrc
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_loader/cg_ld_jsonvalue.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_loader/cg_ld_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cg_loader/cg_ld_loader.cpp")
add_library(cg_loader STATIC ${ldSrc})
target_link_libraries(cg_loader PUBLIC cg_math)
target_link_libraries(cg_loader PUBLIC cg_sampler)
target_link_libraries(cg_loader PUBLIC cg_world)
target_link_libraries(cg_loader PUBLIC cg_raytracer)
target_link_libraries(cg_loader PUBLIC cg_rasterizer)
target_link_libraries(cg_loader PUBLIC sdl_wrapper)
target_include_directories(cg_loader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/cg_loader")

add_library(simple_cg INTERFACE)#接口库，集合
target_link_libraries(simple_cg INTERFACE cg_math cg_world cg_loader cg_sampler cg_raytracer cg_rasterizer sdl_wrapper)
target_include_directories(simple_cg INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
