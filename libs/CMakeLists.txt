include(FetchContent)

set(LIBUV_BUILD_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  uv
  GIT_REPOSITORY https://github.com/libuv/libuv
  GIT_TAG e9f29cb # v1.48.0: 2024.02.07, Version 1.48.0 (Stable)
  SOURCE_DIR ${UV_SRC_DIR}
)

FetchContent_MakeAvailable(uv)