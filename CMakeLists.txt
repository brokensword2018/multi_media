project(multi_meida)
cmake_minimum_required(VERSION 3.1.3)

SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  


# 使能c++11
set(CMAKE_CXX_STANDARD 11)

# 设置库文件路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
# 设置可执行文件路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# 设置子目录
set(SUB_DIR_LIST "util")

foreach(SUB_DIR ${SUB_DIR_LIST})
    aux_source_directory(${SUB_DIR} SRC_LIST)
endforeach()


# 编译静态库
add_library(${CMAKE_PROJECT_NAME}_static STATIC ${SRC_LIST})
target_link_libraries(${CMAKE_PROJECT_NAME}_static pthread)
set_target_properties(${CMAKE_PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")


# 构建tests
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(tests)

# 编译player
find_package(Qt5)
if(Qt5_FOUND)
    add_subdirectory(player) 
endif(Qt5_FOUND)



