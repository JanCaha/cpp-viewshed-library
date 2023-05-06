include(FetchContent)
FetchContent_Declare(simplerasters_src
    GIT_REPOSITORY https://github.com/JanCaha/cpp-simplerasters
    GIT_TAG 734b8c7179394173427c89e6ab3da7dd0e645f11
)

FetchContent_GetProperties(simplerasters_src)

if(NOT simplerasters_src_POPULATED)
    FetchContent_Populate(simplerasters_src)

    add_library(simplerasters INTERFACE)
    target_include_directories(simplerasters INTERFACE ${simplerasters_src_SOURCE_DIR})
    target_link_libraries(simplerasters INTERFACE ${GDAL_LIBRARY})

    message(STATUS "SimpleRasters Installed. Dir: ${simplerasters_src_SOURCE_DIR}")
endif()