include(FetchContent)
FetchContent_Declare(simplerasters
    GIT_REPOSITORY https://github.com/JanCaha/cpp-simplerasters
    GIT_TAG dce9f6584985c0db8ea628fee96cfb07c8be7e64
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