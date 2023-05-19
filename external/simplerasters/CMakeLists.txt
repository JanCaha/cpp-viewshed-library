include(FetchContent)
FetchContent_Declare(simplerasters
    GIT_REPOSITORY https://github.com/JanCaha/cpp-simplerasters
    GIT_TAG 60b30a2bd3a0ed60ed581b258149fcfb2f55d8ac
)

FetchContent_GetProperties(simplerasters)

if(NOT simplerasters_POPULATED)
    FetchContent_Populate(simplerasters)

    add_subdirectory(
        "${simplerasters_SOURCE_DIR}"
        "${simplerasters_BINARY_DIR}"
        EXCLUDE_FROM_ALL
    )

    message(STATUS "SimpleRasters Installed. Dir: ${simplerasters_SOURCE_DIR}")
endif()