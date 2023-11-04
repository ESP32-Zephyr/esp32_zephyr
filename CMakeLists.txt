# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
set(app esp32_devkitc_wroom)
project(${app})

if(SSID)
    message(STATUS "Found SSID")
else()
    message(SEND_ERROR "SSID not found")
endif()

if(PSK)
    message(STATUS "Found PSK")
else()
    message(SEND_ERROR "PSK not found")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/cmake-git-version-tracking.cmake)

FILE(GLOB ${app}_sources src/*.c)

target_sources(app PRIVATE ${${app}_sources})
target_link_libraries(app PRIVATE cmake_git_version_tracking)
target_compile_definitions(app PRIVATE -D__APP_NAME__=\"${app}\" -DSSID=\"${SSID}\" -DPSK=\"${PSK}\")
