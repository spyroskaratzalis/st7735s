cmake_minimum_required(VERSION 3.16)
project(st7735s_driver C)

add_library(st7735s STATIC
    src/display.c
    src/input.c
    src/hal/hal_lgpio.c
    src/graphics.c
    src/image.c
)

add_library(st7735s::st7735s ALIAS st7735s)

target_include_directories(st7735s
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(st7735s PUBLIC c_std_11)
target_compile_options(st7735s PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(st7735s
    PUBLIC
        lgpio
    PRIVATE
        m
)