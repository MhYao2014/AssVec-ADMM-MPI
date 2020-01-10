cmake_minimum_required(VERSION 3.5.1) # CMake 最低版本要求，低于2.6 构建过程会被终止

project(AssVec VERSION 1.0) # 定义工程名称

set(CMAKE_BUILD_TYPE DEBUG) # 指定编译类型，debug 或者为 release
                            # debug 版会生成相关调试信息，可以使用 GDB 进行
                            # release不会生成调试信息。当无法进行调试时查看此处是否设置为 debug

set(CMAKE_CXX_FLAGS_DEBUG  "-pthread -std=c++11 -funroll-loops -O3 -march=native")   # 指定编译器
                                                                            # CMAKE_C_FLAGS_DEBUG  ----C 编译器
                                                                            # CMAKE_CXX_FLAGS_DEBUG----C++ 编译器

include_directories(/usr/local/openmpi/include) # 相关头文件的目录
set(HEADER_FILES
        src/dictionary.h
        src/args.h)

set(SOURCE_FILES
        src/main.cpp
        src/dictionary.cpp
        src/args.cpp)

add_library(AssVec-shared SHARED ${HEADER_FILES} ${SOURCE_FILES})
add_library(AssVec-static STATIC ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(AssVec-shared PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(AssVec-shared PROPERTIES OUTPUT_NAME AssVec POSITION_INDEPENDENT_CODE ON)
set_target_properties(AssVec-static PROPERTIES OUTPUT_NAME AssVec POSITION_INDEPENDENT_CODE ON)
add_executable(AssVec src/main.cpp)
target_link_libraries(AssVec AssVec-static pthread)
set_target_properties(AssVec PROPERTIES PUBLIC_HEADER "${HEADER_FILES}" OUTPUT_NAME AssVec)
