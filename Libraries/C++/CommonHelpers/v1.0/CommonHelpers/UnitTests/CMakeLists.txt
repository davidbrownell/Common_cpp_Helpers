cmake_minimum_required(VERSION 3.5.0)

project(CommonHelpers_UnitTests LANGUAGES CXX)

set(CMAKE_MODULE_PATH "$ENV{DEVELOPMENT_ENVIRONMENT_CMAKE_MODULE_PATH}")

if(NOT WIN32)
    string(REPLACE ":" ";" CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
endif()

include(BuildHelpers)

enable_testing()

function(Impl)
    get_filename_component(_this_path ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)

    include(${_this_path}/../cmake/CommonHelpers.cmake)

    build_tests(
        FILES
            ${_this_path}/Compare_UnitTest.cpp
            ${_this_path}/Constructor_UnitTest.cpp
            ${_this_path}/Copy_UnitTest.cpp
            ${_this_path}/Finally_UnitTest.cpp
            ${_this_path}/Misc_UnitTest.cpp
            ${_this_path}/Move_UnitTest.cpp
            ${_this_path}/SharedObject_UnitTest.cpp
            ${_this_path}/Stl_UnitTest.cpp
            ${_this_path}/TestHelpers_UnitTest.cpp
            ${_this_path}/ThreadPool_UnitTest.cpp
            ${_this_path}/ThreadSafeCounter_UnitTest.cpp
            ${_this_path}/TypeTraits_UnitTest.cpp

        LINK_LIBRARIES
            CommonHelpers

        # CXX_STANDARD
        #     20
    )
endfunction()

Impl()
