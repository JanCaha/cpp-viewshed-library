include(FetchContent)
FetchContent_Declare(simplerasters
    GIT_REPOSITORY https://github.com/JanCaha/cpp-simplerasters
    GIT_TAG 0204cf8f7f90c4e0ae6e872d397d9fa181c86e3b
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