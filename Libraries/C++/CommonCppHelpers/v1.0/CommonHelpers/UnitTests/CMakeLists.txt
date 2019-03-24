cmake_minimum_required(VERSION 3.5.0)

set(_cmake_paths "$ENV{DEVELOPMENT_ENVIRONMENT_CMAKE_MODULE_PATH}")

if(NOT WIN32)
    string(REPLACE ":" ";" ${_cmake_paths} ${_cmake_paths})
endif()

list(GET _cmake_paths 0 _last_cmake_path)

include("${_last_cmake_path}/cpp_common.cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(../../CommonHelpers.cmake)

foreach(_name IN ITEMS
    Copy_UnitTest
    Move_UnitTest
    TypeTraits_UnitTest
)
    add_executable(${_name} ${_name}.cpp)
    target_link_libraries(${_name} CommonHelpers)
endforeach()
