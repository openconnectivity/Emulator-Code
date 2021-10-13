cmake_minimum_required(VERSION 3.5)

project(vscode-dimlight)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtkmm-3.0)

add_executable(${PROJECT_NAME} main/main.c)

target_link_directories(${PROJECT_NAME} PRIVATE /home/esp32test2/iot-lite/iotivity-lite/port/linux ${GTKMM_LIBRARY_DIRS})

target_include_directories(${PROJECT_NAME} PRIVATE ${GTKMM_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE iotivity-lite-server ${GTKMM_LIBRARIES})

# Add other flags to the compiler
target_compile_definitions(${PROJECT_NAME} PRIVATE ${GTKMM_CFLAGS_OTHER})
