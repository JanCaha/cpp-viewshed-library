include(FetchContent)
FetchContent_Declare(simplerasters
    GIT_REPOSITORY https://github.com/JanCaha/cpp-simplerasters
    GIT_TAG 0b6254585a6c3a44abb64b968808f4f246f2d759
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