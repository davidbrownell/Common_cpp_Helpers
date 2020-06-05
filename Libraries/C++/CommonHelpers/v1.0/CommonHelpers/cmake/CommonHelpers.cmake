cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_MODULE_PATH "$ENV{DEVELOPMENT_ENVIRONMENT_CMAKE_MODULE_PATH}")

if(NOT WIN32)
    string(REPLACE ":" ";" CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
endif()

include(BuildHelpers)

function(Impl)
    get_filename_component(_this_path ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
    get_filename_component(_details_path "${_this_path}/../Details" REALPATH)

    file(
        GLOB_RECURSE
        _details_files
        ${_details_path}/*.*
    )

    build_library(
        NAME
            CommonHelpers

        FILES
            ${_details_files}

            ${_this_path}/../Compare.h
            ${_this_path}/../Constructor.h
            ${_this_path}/../Copy.h
            ${_this_path}/../Finally.h
            ${_this_path}/../Misc.h
            ${_this_path}/../Move.h
            ${_this_path}/../SharedObject.h
            ${_this_path}/../Stl.h
            ${_this_path}/../TestHelpers.h
            ${_this_path}/../ThreadPool.cpp
            ${_this_path}/../ThreadPool.h
            ${_this_path}/../ThreadSafeCounter.h
            ${_this_path}/../TypeTraits.h

        PUBLIC_INCLUDE_DIRECTORIES
            ${_this_path}/../..
    )
endfunction()

Impl()
