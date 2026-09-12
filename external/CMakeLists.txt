add_subdirectory(SDL2)

add_library(external INTERFACE)
target_link_libraries(external INTERFACE
    external::SDL2
    external::SDL2main
)

# SDL2.dll 的运行时位置（供各处 post-build 拷贝，避免手工往 bin/ 放 dll）

set(SRT_SDL2_DLL "${CMAKE_CURRENT_SOURCE_DIR}/SDL2/lib/SDL2.dll" CACHE FILEPATH "SDL2 运行时 DLL 路径")

if(NOT EXISTS "${SRT_SDL2_DLL}")
    message(FATAL_ERROR
        "Missing SDL2.dll: ${SRT_SDL2_DLL}\n"
        "Please check if external/SDL2/lib/SDL2.dll exists"
    )
endif()

function(srt_copy_runtime_dll target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SRT_SDL2_DLL}" "$<TARGET_FILE_DIR:${target}>"
        COMMENT "Copy SDL2.dll to $<TARGET_FILE_DIR:${target}>"
    )
endfunction()
