# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if (WIN32)
    set(USERHOME $ENV{USERPROFILE})
else ()
    set(USERHOME $ENV{HOME})
endif ()

set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if (EXISTS ${picoVscode})
    include(${picoVscode})
endif ()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(midiusb C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_subdirectory(
        ${CMAKE_SOURCE_DIR}/lib/usb_midi_host
        ${CMAKE_SOURCE_DIR}/lib/pico-ssd1306
)

add_executable(
        midiusb
        midiusb.c
        lib/usb_midi_host/usb_midi_host.c
        lib/pico-ssd1306/ssd1306.c
        src/foot_controller.c
        src/led_controller.c
        src/tap_controller.c
        src/mp100_controller.c
)

pico_set_program_name(midiusb "midiusb")
pico_set_program_version(midiusb "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(midiusb 1)
pico_enable_stdio_usb(midiusb 0)

# Add the standard include files to the build
target_include_directories(midiusb PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_options(midiusb PRIVATE -Xlinker --print-memory-usage)
target_compile_options(midiusb PRIVATE -Wall -Wextra)
target_link_libraries(midiusb tinyusb_host tinyusb_board usb_midi_host_app_driver pico_stdlib hardware_i2c)

if (DEFINED PICO_BOARD)
    if (${PICO_BOARD} MATCHES "pico_w")
        message("board is pico_w")
        # Add additional compile and link options
        target_link_libraries(midiusb pico_cyw43_arch_none)
    else ()
        message("board is pico")
    endif ()
else ()
    message("board is not defined")
endif ()

pico_add_extra_outputs(midiusb)

add_custom_command(TARGET midiusb POST_BUILD
        COMMAND ${USERHOME}/.pico-sdk/picotool/2.0.0/picotool/picotool.exe load ${CMAKE_BINARY_DIR}/midiusb.uf2
        COMMAND ${USERHOME}/.pico-sdk/picotool/2.0.0/picotool/picotool.exe reboot
        COMMENT "Enviando arquivo e reiniciando a Raspberry Pi Pico...")
