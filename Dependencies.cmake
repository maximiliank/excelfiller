# Done as a function so that updates to variables like CMAKE_CXX_FLAGS don't
# propagate out to other targets
function(excelfiller_setup_dependencies)

  # For each dependency, see if it's already been provided to us by a parent
  # project

  if(NOT TARGET fmtlib::fmtlib)
    find_package(fmt REQUIRED)
  endif()

  if(NOT TARGET pugixml::pugixml)
    find_package(pugixml REQUIRED)
  endif()

  if(BUILD_EXAMPLES)
    if(NOT TARGET spdlog::spdlog)
      find_package(spdlog REQUIRED)
    endif()

    if(NOT TARGET nlohmann_json::nlohmann_json)
      find_package(nlohmann_json REQUIRED)
    endif()
  endif()
endfunction()
