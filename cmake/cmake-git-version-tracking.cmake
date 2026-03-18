include(FetchContent)

set(VERSION_TRACKING_MODULE_PATH "" CACHE STRING "The location of the cmake-git-version-tracking repository")
FetchContent_Declare(cmake_git_version_tracking
    GIT_REPOSITORY https://github.com/andrew-hardin/cmake-git-version-tracking.git
    GIT_TAG        master)
FetchContent_MakeAvailable(cmake_git_version_tracking)
