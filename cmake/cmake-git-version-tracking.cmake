include(FetchContent)

set(VERSION_TRACKING_MODULE_PATH "" CACHE STRING "The location of the cmake-git-version-tracking repository")
FetchContent_Declare(cmake_git_version_tracking
    GIT_REPOSITORY https://github.com/andrew-hardin/cmake-git-version-tracking.git
    GIT_TAG        7d9442bfade5a47b77291e4117f81cc65887c7b9)
FetchContent_MakeAvailable(cmake_git_version_tracking)
