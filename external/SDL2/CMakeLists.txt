# SDL2：imported 目标 + 命名空间别名

add_library(external::SDL2 SHARED IMPORTED GLOBAL)#SHARED动态编译，IMPORTED不是由当前CMake编译
set_target_properties(external::SDL2 PROPERTIES
    IMPORTED_LOCATION               "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2.dll" #运行时的dll
    IMPORTED_IMPLIB                 "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2.lib" #链接用的导入库lib
    INTERFACE_INCLUDE_DIRECTORIES   "${CMAKE_CURRENT_SOURCE_DIR}/include"      #搜索路径
)

add_library(external::SDL2main STATIC IMPORTED GLOBAL)
set_target_properties(external::SDL2main PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2main.lib"
)



