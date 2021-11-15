set(TARGET_NAME zip)

set(sources include/zip/zip.h src/miniz.h src/zip.c)
add_library(${TARGET_NAME} STATIC ${sources})
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${TARGET_NAME} PRIVATE -std=c99 -Wall -Wextra -Werror -pedantic -Wno-implicit-function-declaration)
endif()

target_include_directories(${TARGET_NAME} PRIVATE include/zip)
target_include_directories(${TARGET_NAME} SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include/>)


add_library(zip::${TARGET_NAME} ALIAS ${TARGET_NAME})
