include(FetchContent)

set(ABSL_ENABLE_INSTALL ON)
FetchContent_Declare(
  absl
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG 20250814.0
)
FetchContent_MakeAvailable(absl)

FetchContent_Declare(
  re2
  GIT_REPOSITORY https://github.com/google/re2.git
  GIT_TAG 2025-08-12
)
FetchContent_MakeAvailable(re2)
