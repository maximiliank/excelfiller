macro(run_conan)
    # Download automatically, you can also just copy the conan.cmake file
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (isMultiConfig)
        SET(CONAN_CMAKE_GENERATOR cmake_find_package_multi)
        SET(CONAN_CMAKE_FILE_URL "https://raw.githubusercontent.com/conan-io/cmake-conan/develop/conan.cmake")
        # We cannot expect the hash of the development version
        #SET(CONAN_CMAKE_FILE_HASH 855c95e546dcaf0538369e8a6dfa37e219cb9be2f9523eb2119755ebee8eef4f)
    else ()
        SET(CONAN_CMAKE_GENERATOR cmake_find_package)
        SET(CONAN_CMAKE_FILE_URL "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake")
        SET(CONAN_CMAKE_FILE_HASH 396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484)
    endif ()

    if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "${CONAN_CMAKE_FILE_URL}"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=${CONAN_CMAKE_FILE_HASH}
                TLS_VERIFY ON)
    endif ()

    include(${CMAKE_BINARY_DIR}/conan.cmake)

    conan_cmake_configure(
            REQUIRES
            ${CONAN_EXTRA_REQUIRES}
            fmt/7.1.3
            pugixml/1.11
            OPTIONS
            ${CONAN_EXTRA_OPTIONS}
            GENERATORS ${CONAN_CMAKE_GENERATOR})

    if (NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
    endif()
    foreach (TYPE ${CMAKE_CONFIGURATION_TYPES})
        conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})
        conan_cmake_install(PATH_OR_REFERENCE .
                BUILD missing
                REMOTE conancenter
                SETTINGS ${settings})
    endforeach ()
endmacro()
