cmake_minimum_required(VERSION 3.13)
project(lcd_c C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_subdirectory(lib/st7735s_driver)

add_executable(lcd_c main.c)
target_link_libraries(lcd_c PRIVATE st7735s_driver)