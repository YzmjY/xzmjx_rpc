cmake_minimum_required(VERSION 3.15)
project("xzmjx")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif ()
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

message("-------------- Env ---------------")
message("  CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("  CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("----------------------------------")

set(CMAKE_CXX_FLAGS "-std=c++20 -D__const__=  -fPIC -fno-strict-aliasing -Wall -Wno-volatile ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -DNDEBUG")
message("------------ Cxx flags -------------")
message("  CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
message("------------------------------------")


# third party
# libgo
include_directories("${PROJECT_SOURCE_DIR}/third_party/libgo_")
include_directories("${PROJECT_SOURCE_DIR}/third_party/libgo_/libgo")
include_directories("${PROJECT_SOURCE_DIR}/third_party/libgo_/libgo/linux")
link_directories(${PROJECT_SOURCE_DIR}/third_party/libgo_/build)
# spdlog
include_directories("${PROJECT_SOURCE_DIR}/third_party/spdlog/include")
link_directories(${PROJECT_SOURCE_DIR}/third_party/spdlog/build)
# yaml-cpp
include_directories("${PROJECT_SOURCE_DIR}/third_party/yaml-cpp/include")
link_directories(${PROJECT_SOURCE_DIR}/third_party/yaml-cpp/build)

include_directories("${PROJECT_SOURCE_DIR}")


aux_source_directory(${PROJECT_SOURCE_DIR}/utils XZMJX_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/net XZMJX_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/http XZMJX_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/rpc XZMJX_SRC_LIST)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
message("------------ LIBRARY_OUTPUT_PATH -------------")
message(${PROJECT_SOURCE_DIR}/lib)

set(TARGET "xzmjx")
set(STATIC_T "xzmjx_static")
set(LINK_ARGS xzmjx libgo static_hook pthread dl yaml-cpp)

add_library(${STATIC_T} STATIC ${XZMJX_SRC_LIST})
set_target_properties("${STATIC_T}" PROPERTIES OUTPUT_NAME "${TARGET}")
target_link_libraries("${STATIC_T}" -Wl,--start-group ${LINK_ARGS} -Wl,--end-group)

#add_subdirectory(tests)

