cmake_minimum_required(VERSION 3.17)
project(excelfiller CXX C)

option(USE_CONAN "Use conan to download package dependencies" ON)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(BUILD_EXAMPLES "Build examples" ON)

include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(project_options INTERFACE -march=native)
endif()

add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

if (USE_CONAN)
    if (BUILD_EXAMPLES)
        # Set up some extra Conan dependencies based on our needs before loading Conan
        set(CONAN_EXTRA_REQUIRES nlohmann_json/3.9.1 spdlog/1.8.5)
        set(CONAN_EXTRA_OPTIONS "")
    endif()
    include(cmake/Conan.cmake)
    run_conan()
endif()

find_package(fmt REQUIRED)
find_package(pugixml REQUIRED)

# https://github.com/kuba--/zip
add_subdirectory(Source/zip)
add_subdirectory(Source/ExcelFiller)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
