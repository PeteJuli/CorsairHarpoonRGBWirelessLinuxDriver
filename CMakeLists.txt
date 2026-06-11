cmake_minimum_required(VERSION 4.1)
project(CorsairHarpoonRGBWirelessLinuxDriver)

set(CMAKE_CXX_STANDARD 23)

find_package(hidapi REQUIRED)

add_executable(CorsairHarpoonRGBWirelessLinuxDriver SRC/main.cpp
        SRC/Driver/HID/HID.cpp
        SRC/Driver/HID/HID.hpp
        SRC/Driver/Mouse/MouseDriver.cpp
        SRC/Driver/Mouse/MouseDriver.hpp
)

target_link_libraries(CorsairHarpoonRGBWirelessLinuxDriver PRIVATE hidapi::hidraw)
