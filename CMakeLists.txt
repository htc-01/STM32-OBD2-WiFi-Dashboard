cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME car111plus)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/Src/flash_storage.c)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# ========== LVGL SOURCE COLLECTION ==========
file(GLOB_RECURSE LVGL_CORE_SOURCES      Middlewares/LVGL/src/core/*.c)
file(GLOB_RECURSE LVGL_MISC_SOURCES      Middlewares/LVGL/src/misc/*.c)
file(GLOB_RECURSE LVGL_WIDGET_SOURCES    Middlewares/LVGL/src/widgets/arc/*.c
                                         Middlewares/LVGL/src/widgets/label/*.c
                                         Middlewares/LVGL/src/widgets/button/*.c
                                         Middlewares/LVGL/src/widgets/image/*.c
                                         Middlewares/LVGL/src/widgets/chart/*.c
                                         Middlewares/LVGL/src/widgets/scale/*.c
                                         Middlewares/LVGL/src/widgets/line/*.c)
file(GLOB_RECURSE LVGL_FONT_SOURCES      Middlewares/LVGL/src/font/*.c)
file(GLOB_RECURSE LVGL_THEME_SOURCES     Middlewares/LVGL/src/themes/*.c)
file(GLOB_RECURSE LVGL_TICK_SOURCES      Middlewares/LVGL/src/tick/*.c)
file(GLOB_RECURSE LVGL_DISPLAY_SOURCES   Middlewares/LVGL/src/display/*.c)
file(GLOB_RECURSE LVGL_DRAW_SOURCES      Middlewares/LVGL/src/draw/*.c)
file(GLOB_RECURSE LVGL_STDLIB_SOURCES    Middlewares/LVGL/src/stdlib/builtin/*.c)
file(GLOB_RECURSE LVGL_LAYOUT_SOURCES    Middlewares/LVGL/src/layouts/*.c)
file(GLOB_RECURSE LVGL_INDEV_SOURCES     Middlewares/LVGL/src/indev/*.c)
file(GLOB_RECURSE LVGL_OSAL_SOURCES      Middlewares/LVGL/src/osal/*.c)
file(GLOB_RECURSE LVGL_LIBS_SOURCES      Middlewares/LVGL/src/libs/bin_decoder/*.c)

# Top-level src/*.c files not caught by subdir globs
set(LVGL_TOPLEVEL_SOURCES Middlewares/LVGL/src/lv_init.c
                          Middlewares/LVGL/src/stdlib/lv_mem.c
                          Middlewares/LVGL/src/others/sysmon/lv_sysmon.c
                          Middlewares/LVGL/src/others/observer/lv_observer.c)

# ========== FLASH-SIZE TRIMMING for STM32F401 ==========
# Keep only the fonts actually enabled in lv_conf.h
list(FILTER LVGL_FONT_SOURCES EXCLUDE REGEX "/lv_font_source_han_sans.*")
list(FILTER LVGL_FONT_SOURCES EXCLUDE REGEX "/lv_font_simsun.*")
list(FILTER LVGL_FONT_SOURCES EXCLUDE REGEX "/lv_font_dejavu.*")
list(FILTER LVGL_FONT_SOURCES EXCLUDE REGEX "/lv_font_unscii.*")
list(FILTER LVGL_FONT_SOURCES EXCLUDE REGEX "/lv_font_montserrat_28_compressed.*")

# Exclude all montserrat sizes, then re-add only the ones we need
list(FILTER LVGL_FONT_SOURCES EXCLUDE REGEX "/lv_font_montserrat_[0-9]+.*")
list(APPEND LVGL_FONT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/LVGL/src/font/lv_font_montserrat_12.c")
list(APPEND LVGL_FONT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/LVGL/src/font/lv_font_montserrat_14.c")
list(APPEND LVGL_FONT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/LVGL/src/font/lv_font_montserrat_16.c")
list(APPEND LVGL_FONT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/LVGL/src/font/lv_font_montserrat_20.c")

# Exclude unused layouts (GRID not typically used by Guider absolute positioning)
list(FILTER LVGL_LAYOUT_SOURCES EXCLUDE REGEX "/lv_grid\\.c$")

# Exclude widgets with corrupted source files in the imported LVGL tree
list(FILTER LVGL_WIDGET_SOURCES EXCLUDE REGEX "/lv_canvas\\.c$")
list(FILTER LVGL_WIDGET_SOURCES EXCLUDE REGEX "/lv_span\\.c$")

# Exclude 3D / vector draw units
list(FILTER LVGL_DRAW_SOURCES EXCLUDE REGEX "/lv_draw_3d\\.c$")
list(FILTER LVGL_DRAW_SOURCES EXCLUDE REGEX "/lv_draw_vector\\.c$")
list(FILTER LVGL_DRAW_SOURCES EXCLUDE REGEX "/lv_draw_sw_vector\\.c$")

# Exclude GPU/SIMD-specific draw units
list(FILTER LVGL_DRAW_SOURCES EXCLUDE REGEX "/draw/(nema_gfx|sdl|opengles|nxp|renesas|vg_lite|dma2d|dave2d|pxp|g2d)/.*")
list(FILTER LVGL_DRAW_SOURCES EXCLUDE REGEX "/draw/sw/blend/(arm2d|neon|helium)/.*")
list(FILTER LVGL_DRAW_SOURCES EXCLUDE REGEX "/draw/sw/arm2d/.*")

# Exclude unused OSAL implementations, keep only NONE for bare-metal
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_windows\\.c$")
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_sdl2\\.c$")
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_rtthread\\.c$")
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_pthread\\.c$")
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_mqx\\.c$")
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_linux\\.c$")
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_freertos\\.c$")
list(FILTER LVGL_OSAL_SOURCES EXCLUDE REGEX "/lv_cmsis_rtos2\\.c$")

set(LVGL_SOURCES ${LVGL_TOPLEVEL_SOURCES}
                 ${LVGL_CORE_SOURCES}
                 ${LVGL_MISC_SOURCES}
                 ${LVGL_WIDGET_SOURCES}
                 ${LVGL_FONT_SOURCES}
                 ${LVGL_THEME_SOURCES}
                 ${LVGL_TICK_SOURCES}
                 ${LVGL_DISPLAY_SOURCES}
                 ${LVGL_DRAW_SOURCES}
                 ${LVGL_STDLIB_SOURCES}
                 ${LVGL_LAYOUT_SOURCES}
                 ${LVGL_INDEV_SOURCES}
                 ${LVGL_OSAL_SOURCES}
                 ${LVGL_LIBS_SOURCES})

# Add include paths (GUI first to prefer GUI/custom.h over Core/Inc/custom.h)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    GUI
    GUI/guider_customer_fonts
    Middlewares/LVGL
    Core/Inc
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/gc9a01.c
    Core/Src/font.c
    Core/Src/lv_port_disp.c
    Core/Src/lv_port_tick.c
    Core/Src/gauge.c
    Core/Src/gauge_flash_data.c
    Core/Src/flash_storage.c
    Core/Src/at24c02.c
    GUI/gui_guider.c
    GUI/events_init.c
    GUI/widgets_init.c
    GUI/setup_scr_speed.c
    GUI/setup_scr_record.c
    GUI/custom.c
    GUI/images/_NXP_Logo_RGB565A8_30x25.c
    GUI/guider_fonts/lv_font_LiberationSans_10.c
    ${LVGL_SOURCES}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Cortex-M4: enforce Thumb-2 instruction set
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -mthumb -fshort-enums)
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -mthumb)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
