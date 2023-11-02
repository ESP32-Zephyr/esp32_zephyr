# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esp32_zephyr)

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

FILE(GLOB esp32_zephyr_app_sources src/*.c)
target_sources(app PRIVATE ${esp32_zephyr_app_sources})
target_compile_definitions(app PRIVATE -DSSID=\"${SSID}\" -DPSK=\"${PSK}\")
