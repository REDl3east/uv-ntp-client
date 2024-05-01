cmake_minimum_required(VERSION 3.0.0)
project(uv-ntp-client VERSION 0.1.0 LANGUAGES C)

set(UV_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/libuv)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(MAIN_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/uv-ntp-client.c
)

add_executable(uv-ntp-client ${MAIN_SRC})
target_link_libraries(uv-ntp-client PRIVATE uv_a)