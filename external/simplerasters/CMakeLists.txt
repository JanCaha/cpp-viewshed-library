include(FetchContent)
FetchContent_Declare(simplerasters
    GIT_REPOSITORY https://github.com/JanCaha/cpp-simplerasters
    GIT_TAG 734b8c7179394173427c89e6ab3da7dd0e645f11
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