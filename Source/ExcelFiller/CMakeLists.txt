set(TARGET_NAME ExcelFiller)

file(GLOB_RECURSE headers include/${TARGET_NAME}/*.h
     include/${TARGET_NAME}/*.hpp)
file(GLOB_RECURSE sources src/*.cpp src/*.h src/*.hpp)

add_library(${TARGET_NAME} ${sources} ${headers})

set_target_properties(${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${headers}")

target_include_directories(${TARGET_NAME} PRIVATE include/)
target_include_directories(
  ${TARGET_NAME} SYSTEM
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
            $<INSTALL_INTERFACE:include/>)
target_link_libraries(
  ${TARGET_NAME}
  PRIVATE project_options project_warnings zip::zip fmt::fmt spdlog::spdlog
  PUBLIC pugixml::pugixml)

install(TARGETS project_options EXPORT ${TARGET_NAME}Targets)
install(TARGETS project_warnings EXPORT ${TARGET_NAME}Targets)

install(
  TARGETS ${TARGET_NAME}
  EXPORT ${TARGET_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  PUBLIC_HEADER DESTINATION include/${TARGET_NAME})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${TARGET_NAME}ConfigVersion.cmake
  VERSION 0.0.1
  COMPATIBILITY AnyNewerVersion)

install(
  EXPORT ${TARGET_NAME}Targets
  FILE ${TARGET_NAME}Targets.cmake
  NAMESPACE ${TARGET_NAME}::
  DESTINATION lib/cmake/${TARGET_NAME})

configure_file(${TARGET_NAME}Config.cmake.in ${TARGET_NAME}Config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${TARGET_NAME})

if(USE_CONAN)
  install(FILES "${CMAKE_BINARY_DIR}/Findfmt.cmake"
                "${CMAKE_BINARY_DIR}/Findpugixml.cmake"
          DESTINATION lib/cmake/${TARGET_NAME})
endif()

add_library(${TARGET_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})
