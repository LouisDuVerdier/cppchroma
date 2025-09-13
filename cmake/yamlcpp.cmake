include(FetchContent)
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG aa8d4e4750ec9fe9f8cc680eb90f1b15955c817e # Sep 4, 2025
)
FetchContent_MakeAvailable(yaml-cpp)
