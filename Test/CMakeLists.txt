# `include_directories` 指定了头文件的搜索路径
# `${PROJECT_SOURCE_DIR}` 是 CMakeLists.txt 所在的目录
include_directories(${PROJECT_SOURCE_DIR}/MyTinySTL)
# `set` 命令定义了源文件的变量 `APP_SRC` 和可执行文件的输出路径`EXECUTABLE_OUTPUT_PATH` 
set(APP_SRC test_my.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
# `add_executable` 命令指定了编译生成可执行文件的名称和源文件列表
add_executable(stltest ${APP_SRC})