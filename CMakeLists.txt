cmake_minimum_required(VERSION 4.1)
project(CorsairHarpoonRGBWirelessLinuxDriver)

set(CMAKE_CXX_STANDARD 23)

find_package(hidapi REQUIRED)

add_executable(CorsairHarpoonRGBWirelessLinuxDriver SRC/main.cpp
        SRC/Driver/HID/Context/Context.cpp
        SRC/Driver/HID/Context/Context.hpp
        SRC/Driver/HID/Device/Device.cpp
        SRC/Driver/HID/Device/Device.hpp
        SRC/Driver/Mouse/Mouse.cpp
        SRC/Driver/Mouse/Mouse.hpp
)

target_link_libraries(CorsairHarpoonRGBWirelessLinuxDriver PRIVATE hidapi::hidraw)
