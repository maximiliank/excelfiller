include(FetchContent)
FetchContent_Declare(
  zip_cpp
  GIT_REPOSITORY https://github.com/maximiliank/ZipCpp.git
  GIT_TAG origin/main
  UPDATE_DISCONNECTED ON
  SOURCE_SUBDIR somePathThatNeverExists)

FetchContent_GetProperties(zip_cpp)
if(NOT zip_cpp_POPULATED)
  FetchContent_MakeAvailable(zip_cpp)
endif()

if(NOT TARGET project_options)
  add_library(project_options INTERFACE)
endif()
if(NOT TARGET project_warnings)
  add_library(project_warnings INTERFACE)
endif()
