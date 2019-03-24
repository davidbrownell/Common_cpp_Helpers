add_library(CommonHelpers INTERFACE)

target_include_directories(CommonHelpers 
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/CommonHelpers/Details/boost_extract"
)

file(GLOB _headers "${CMAKE_CURRENT_LIST_DIR}/CommonHelpers/*.h")

target_sources(CommonHelpers
    INTERFACE
        ${_headers}
)
