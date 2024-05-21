cmake_minimum_required(VERSION 3.18)

include($ENV{PICO_SDK_EXTERNAL_PATH}/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_SDK_EXTERNAL_PATH}/pico_extras_import.cmake)

project(ktane_debugger_module C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(ktane_debugger_module
    src/main.cpp
    src/lcd1602_RGB_Module.c
)

target_link_libraries(ktane_debugger_module PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_add_extra_outputs(ktane_debugger_module)

pico_enable_stdio_usb(ktane_debugger_module 0)
pico_enable_stdio_uart(ktane_debugger_module 1)
