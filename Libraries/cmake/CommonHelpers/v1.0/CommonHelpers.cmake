set(_common_helpers_version "1.0")
get_filename_component(_common_helpers_root "${CMAKE_CURRENT_LIST_DIR}/../../../C++/CommonHelpers/v${_common_helpers_version}" ABSOLUTE)

add_library(CommonHelpers INTERFACE)

target_include_directories(CommonHelpers 
    INTERFACE
        "${_common_helpers_root}"
        "${_common_helpers_root}/CommonHelpers/Details/boost_extract"
)

file(GLOB _headers "${_common_helpers_root}/CommonHelpers/*.h")

target_sources(CommonHelpers
    INTERFACE
        ${_headers}
)
