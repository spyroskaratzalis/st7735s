add_library(st7735s_driver STATIC)
target_compile_definitions(st7735s_driver PUBLIC USE_DEV_LIB)

file(GLOB_RECURSE DRIVER_SOURCES
    "Config/*.c" "Config/*.cpp"
    "Fonts/*.c"  "Fonts/*.cpp"
    "GUI/*.c"    "GUI/*.cpp"
    "LCD/*.c"    "LCD/*.cpp"
)

target_sources(st7735s_driver PRIVATE ${DRIVER_SOURCES})
target_include_directories(st7735s_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/Fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI
    ${CMAKE_CURRENT_SOURCE_DIR}/LCD
)
find_library(LGPIO_LIB lgpio REQUIRED)
target_link_libraries(st7735s_driver PUBLIC m ${LGPIO_LIB})