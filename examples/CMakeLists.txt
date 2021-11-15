find_package(spdlog REQUIRED)

set(TEST_DATA_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/get_file_path.hpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/get_file_path.hpp" @ONLY)

add_executable(example example.cpp)
target_include_directories(example PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(example project_options project_warnings ExcelFiller spdlog::spdlog)

add_subdirectory(EBAbenchmark)
