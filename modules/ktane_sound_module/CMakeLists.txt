cmake_minimum_required(VERSION 3.18)

include($ENV{PICO_SDK_EXTERNAL_PATH}/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_SDK_EXTERNAL_PATH}/pico_extras_import.cmake)

project(ktane_sound_module C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(ktane_sound_module
    src/main.c
    src/audio/strike.c
    src/audio/snip.c
    src/audio/singlebeep.c
    src/audio/needy_warning.c
    src/audio/needy_activated.c
    src/audio/doublebeep.c
    src/audio/doublebeep_1_25.c
    src/audio/alarm_clock_beep.c
)

target_link_libraries(ktane_sound_module PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    pico_stdlib
    pico_audio_i2s
    hardware_gpio

)

target_compile_definitions(ktane_sound_module PRIVATE
# compile time configuration of I2S
    PICO_AUDIO_I2S_MONO_INPUT=1
)

pico_add_extra_outputs(ktane_sound_module)

# enable usb output, disable uart output
pico_enable_stdio_usb(ktane_sound_module 0)
pico_enable_stdio_uart(ktane_sound_module 1)
