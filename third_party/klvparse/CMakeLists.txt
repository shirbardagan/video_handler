cmake_minimum_required(VERSION 3.10)

set(PROJECT "klvparse")
set(VERSION "2.0.1")
project("${PROJECT}" VERSION "${VERSION}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
find_package(nlohmann_json 3.2.0 REQUIRED)

if(CMAKE_INSTALL_PREFIX STREQUAL "C:/Program Files/${PROJECT}")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install")
endif()
message(STATUS CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

if (POLICY CMP0077)
    # Allow CMake 3.13+ to override options when using FetchContent / add_subdirectory.
    cmake_policy(SET CMP0077 NEW)
endif ()

if(${MAIN_PROJECT} AND (${CMAKE_VERSION} VERSION_EQUAL 3.13 OR ${CMAKE_VERSION} VERSION_GREATER 3.13))
    set(BuildTests_INIT ON)
    set(BuildPlugin_INIT ON)
else()
    set(BuildTests_INIT OFF)
    set(BuildPlugin_INIT OFF)
endif()

option(BuildTests "Build the unit tests when BUILD_TESTING is enabled." ${BuildTests_INIT})
option(BuildPlugin "Build the gstreamer plugin." ${BuildPlugin_INIT})

add_subdirectory("src")

if(BuildPlugin)
    add_subdirectory("gst-plugins")
endif()

if(BuildTests)
    add_subdirectory("tests")
endif()