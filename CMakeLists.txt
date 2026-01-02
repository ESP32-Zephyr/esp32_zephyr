# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
set(app esp32_devkitc_wroom)
project(${app})

list(APPEND CMAKE_MODULE_PATH ${ZEPHYR_BASE}/modules/nanopb)
include(nanopb)
include(${CMAKE_SOURCE_DIR}/cmake/cmake-git-version-tracking.cmake)

option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    add_subdirectory(doc)
endif()

zephyr_nanopb_sources(app proto/cmds.proto)
FILE(GLOB ${app}_sources src/*.c)

target_sources(app PRIVATE ${${app}_sources})
target_link_libraries(app PRIVATE cmake_git_version_tracking)
target_compile_definitions(app PRIVATE -D__APP_NAME__=\"${app}\")
