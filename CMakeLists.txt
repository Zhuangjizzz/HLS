cmake_minimum_required(VERSION 3.0.0)
project(hls VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 自动添加src目录下的所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 添加头文件目录
include_directories(src src/data src/parser src/run src/gengraph src/schedule)

# 添加可执行文件
add_executable(main ${SOURCES})

# 添加链接库
# target_link_libraries(hls ${LIBS})
