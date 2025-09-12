include(GoogleTest)
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 52eb810 # v1.17.0
)
FetchContent_MakeAvailable(googletest)
