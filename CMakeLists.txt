cmake_minimum_required(VERSION 4.1)
project(CorsairHarpoonRGBWirelessLinuxDriver)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

set(CMAKE_CXX_STANDARD 20)

add_executable(CorsairHarpoonRGBWirelessLinuxDriver SRC/main.cpp
        SRC/Driver/USB/USB.cpp
        SRC/Driver/USB/USB.hpp
        SRC/Driver/Mouse/MouseDriver.cpp
        SRC/Driver/Mouse/MouseDriver.hpp
)

target_include_directories(CorsairHarpoonRGBWirelessLinuxDriver PRIVATE ${LIBUSB_INCLUDE_DIRS})
target_link_libraries(CorsairHarpoonRGBWirelessLinuxDriver PRIVATE ${LIBUSB_LIBRARIES})
