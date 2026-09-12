
# ====== General settings ====== 
cmake_minimum_required(VERSION 3.23)
project(SimpleCG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# ====== Build mode switch ====== 
set(SRT_MODE "release" CACHE STRING "构建模式：release（正式）| debug（调试）| test（单元测试）")
set_property(CACHE SRT_MODE PROPERTY STRINGS release debug test)
if(NOT SRT_MODE MATCHES "^(release|debug|test)$")
    message(FATAL_ERROR "SRT_MODE 只能是 release / debug / test，当前='${SRT_MODE}'")
endif()

# 模式 → 产物目录 / 是否建测试 / 日志级别
if(SRT_MODE STREQUAL "release")
    set(SRT_OUTPUT_DIR   "${CMAKE_SOURCE_DIR}/bin")#输出到bin
    set(SRT_BUILD_TESTS  OFF)
    set(SRT_BUILD_EXAMPLES OFF)
    set(SRT_LOG_LEVEL    1)
elseif(SRT_MODE STREQUAL "debug")
    set(SRT_OUTPUT_DIR   "${CMAKE_SOURCE_DIR}/debug")#输出到debug
    set(SRT_BUILD_TESTS  OFF)
    set(SRT_BUILD_EXAMPLES ON)
    set(SRT_LOG_LEVEL    3)
else()  # test
    set(SRT_OUTPUT_DIR   "${CMAKE_SOURCE_DIR}/debug")#输出到debug
    set(SRT_BUILD_TESTS  ON)
    set(SRT_BUILD_EXAMPLES OFF)
    set(SRT_LOG_LEVEL    2)
endif()

# 打印一行 STATUS 信息，方便配置时确认当前模式参数。
message(STATUS "[SRT] 模式=${SRT_MODE}  产物目录=${SRT_OUTPUT_DIR}  测试=${SRT_BUILD_TESTS}  日志级别=${SRT_LOG_LEVEL}")

# ====== Multi cfg output ====== 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SRT_OUTPUT_DIR}")
foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} "${SRT_OUTPUT_DIR}")
endforeach()

# ====== Assemble ====== 
# dependency
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs")
# release
if(SRT_MODE STREQUAL "release" OR SRT_MODE STREQUAL "debug")
	add_subdirectory(app)           #
endif()
# example for debug
if(SRT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
# unit tests
if(SRT_BUILD_TESTS)
    enable_testing()                # 必须在顶层调用
    add_subdirectory(tests)
endif()


#[[
target_link_libraries(${main} simple_cg)
target_link_libraries(${main} SDL2 SDL2main)


set(Test JsonLoader)
set(Test0_1 SDL2Test)
set(Test0_2 JsonLoaderTest)
set(Test1_1 BasicRayTracing)
set(Test1_2 DirectLight)
set(Test1_3 HardShadows)
set(Test1_4 RecursiveReflection)
set(Test1 GeneralRaytracer)
set(Test2_1 Line)
set(Test2_2 Triangle)

set(main ${Test1})

add_executable(${main} "${CMAKE_CURRENT_SOURCE_DIR}/example/${main}.cpp")

#set(LIBSUBDIRS 
#    simple_cg 
#    SDL2)
#
#foreach(subdir IN LISTS LIBSUBDIRS)
#    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/${libsubdir}")
#endforeach(subdir IN LISTS LIBSUBDIRS)


add_custom_command(TARGET ${main} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/lib/SDL2.dll"
    "${CMAKE_CURRENT_BINARY_DIR}/../debug"

    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE_DIR:${main}>/${main}.exe"
    "${CMAKE_CURRENT_BINARY_DIR}/../debug"

    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../debug/${main}" #运行程序

    COMMENT "Running ${main}..."
)

#add_custom_command(TARGET ${main} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy
#        "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/lib/SDL2.dll"
#        $<TARGET_FILE_DIR:${main}>
#        COMMAND $<TARGET_FILE:${main}>#运行程序
#        COMMENT "Running ${main}..."
#)

]]