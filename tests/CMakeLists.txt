# 自动化单元测试：约定"全部通过返回 0，任一失败返回非 0"，且不得阻塞在窗口事件循环

set(SRT_TESTS
    loader/JsonLoaderTest)

foreach(t IN LISTS SRT_TESTS)
    get_filename_component(tName "${t}" NAME)
    add_executable(${tName} "${CMAKE_CURRENT_SOURCE_DIR}/${t}.cpp")
    target_link_libraries(${tName} PRIVATE 
        simple_cg
        external
    )
    target_compile_definitions(${tName} PRIVATE
        SRT_LOG_LEVEL=${SRT_LOG_LEVEL}
        SRT_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
        SRT_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    )
    srt_copy_runtime_dll(${tName})

    add_test(NAME ${tName} COMMAND ${tName})
    set_tests_properties(${tName} PROPERTIES TIMEOUT 30)   # 防止死循环挂死
endforeach()

# —— SDL 初始化冒烟测试（无头运行，不弹窗）——
add_executable(SDL2Test "${CMAKE_CURRENT_SOURCE_DIR}/SDLwrapper/SDL2Test.cpp")
target_link_libraries(SDL2Test PRIVATE simple_cg external::SDL2)
srt_copy_runtime_dll(SDL2Test)
add_test(NAME SDL2_smoke COMMAND SDL2Test)
set_tests_properties(SDL2_smoke PROPERTIES
    TIMEOUT 15
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")   # 关键：无人值守时不开真窗口

# 注意：不要注册 tests/loader/JsonLoader.cpp —— 它永远 return 0，
# 读不到 assets/test.json 也只是打印错误，没有任何断言价值（当手动工具用即可）