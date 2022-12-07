cmake_minimum_required(VERSION 3.20)
project(rpc_test)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "-std=c++20 -D__const__=  -fPIC -fno-strict-aliasing -Wall ${CMAKE_CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR}/../../)
link_directories(${PROJECT_SOURCE_DIR}/../../lib)


aux_source_directory(${PROJECT_SOURCE_DIR}/server SERVER_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/client CLIENT_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/registry REGISTRY_SRC_LIST)

add_executable(rpc_server ${SERVER_SRC_LIST})
set(LINK_ARGS xzmjx libgo pthread dl yaml-cpp)
target_link_libraries(rpc_server -Wl,--start-group ${LINK_ARGS} -Wl,--end-group)

add_executable(rpc_client ${CLIENT_SRC_LIST})
set(LINK_ARGS xzmjx libgo pthread dl yaml-cpp)
target_link_libraries(rpc_client  -Wl,--start-group ${LINK_ARGS} -Wl,--end-group)

add_executable(rpc_registry ${REGISTRY_SRC_LIST})
set(LINK_ARGS xzmjx libgo pthread dl yaml-cpp)
target_link_libraries(rpc_registry  -Wl,--start-group ${LINK_ARGS} -Wl,--end-group)