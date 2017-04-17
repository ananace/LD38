cmake_minimum_required(VERSION 3.1)

cmake_policy(SET CMP0063 NEW) # Honor visibility properties for all target types

project(LD38
    LANGUAGES CXX
    VERSION 0.0.0
)

set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_PATH "${PROJECT_PATH}/src")
set(LIB_PATH "${PROJECT_BINARY_DIR}/lib")
set(BIN_PATH "${PROJECT_BINARY_DIR}/bin")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_PATH}/cmake")

include(cmake/CompileFlags.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIB_PATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIB_PATH}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(IDE_FOLDER "")

add_subdirectory(external)
add_subdirectory(src)
