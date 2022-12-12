cmake_minimum_required(VERSION 3.15)
project(tests)
# third party
# libgo
include_directories("${PROJECT_SOURCE_DIR}/../third_party/libgo")
include_directories("${PROJECT_SOURCE_DIR}/../third_party/libgo/libgo")
include_directories("${PROJECT_SOURCE_DIR}/../third_party/libgo/libgo/linux")
link_directories(${PROJECT_SOURCE_DIR}/../third_party/libgo/build)
# spdlog
include_directories("${PROJECT_SOURCE_DIR}/../third_party/spdlog/include")
link_directories(${PROJECT_SOURCE_DIR}/../third_party/spdlog/build)


add_subdirectory(test)
add_subdirectory(rpc)
add_subdirectory(tcp_server)
add_subdirectory(unit_test)