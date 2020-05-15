cmake_minimum_required(VERSION 3.5.0)

project(CommonHelpers LANGUAGES CXX)

set(CMAKE_MODULE_PATH "$ENV{DEVELOPMENT_ENVIRONMENT_CMAKE_MODULE_PATH}")

if(NOT WIN32)
    string(REPLACE ":" ";" CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
endif()

include(BuildHelpers)

function(Impl)
    get_filename_component(_this_path ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)

    build_library(
        NAME
            CommonHelpers

        IS_INTERFACE
            ON

        FILES
            ${_this_path}/../Compare.h
            ${_this_path}/../Constructor.h
            ${_this_path}/../Copy.h
            ${_this_path}/../Misc.h
            ${_this_path}/../Move.h
            ${_this_path}/../SharedObject.h
            ${_this_path}/../TestHelpers.h
            ${_this_path}/../TypeTraits.h

        PUBLIC_INCLUDE_DIRECTORIES
            ${_this_path}/../..
            ${_this_path}/../Details
    )
endfunction()

Impl()
