cmake_minimum_required(VERSION 3.17)
project(excelfiller CXX C)

set(CMAKE_CXX_STANDARD 20)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(BUILD_EXAMPLES "Build examples" ON)

include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(project_options INTERFACE -march=native)
endif()

include(Dependencies.cmake)

include(cmake/lint-targets.cmake)

add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

excelfiller_setup_dependencies()

# https://github.com/kuba--/zip add_subdirectory(Source/zip)
add_subdirectory(Source/ExcelFiller)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
