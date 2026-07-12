cmake_minimum_required(VERSION 3.10)
project(simple_cg)
set(SOURCES 
    src/cg_SDLwraper.cpp
    src/cg_sampler.cpp
    src/cg_sampler_raytracing.cpp 
    src/cg_structure.cpp 
    src/cg_interaction.cpp)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(simple_cg PRIVATE cg_math)
target_link_libraries(simple_cg PRIVATE SDL2 SDL2main)