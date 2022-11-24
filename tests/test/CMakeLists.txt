cmake_minimum_required(VERSION 2.8)
project(test)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "-std=c++20 -D__const__=  -fPIC -fno-strict-aliasing -Wall ${CMAKE_CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR}/../../)
link_directories(${PROJECT_SOURCE_DIR}/../../lib)

include_directories("${PROJECT_SOURCE_DIR}../../third_party/libgo_")
include_directories("${PROJECT_SOURCE_DIR}../../third_party/libgo_/libgo")
include_directories("${PROJECT_SOURCE_DIR}../../third_party/libgo_/libgo/linux")
link_directories(${PROJECT_SOURCE_DIR}../../third_party/libgo_/build)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)

foreach(var ${SRC_LIST})
    string(REGEX REPLACE ".*/" "" var ${var})
    string(REGEX REPLACE ".cpp" "" tgt ${var})

    add_executable(${tgt} ${var})
    set(LINK_ARGS libgo pthread dl yaml-cpp)

    target_link_libraries(${tgt}  -Wl,--start-group ${LINK_ARGS} -Wl,--end-group)
endforeach(var)