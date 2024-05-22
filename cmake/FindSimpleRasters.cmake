include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(simplerasters_PKGCONF simplerasters)

# Include dir
find_path(simplerasters_INCLUDE_DIR
    NAMES simplerasters.h
    PATHS
    /usr/include
    /usr/local/include
    PATH_SUFFIXES simplerasters
    "${CMAKE_PREFIX_PATH}/include"
    ${simplerasters_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(simplerasters_LIBRARY
    NAMES simplerasters
    PATHS
    /usr/lib
    /usr/lib64
    /usr/local/lib
    "${CMAKE_PREFIX_PATH}/lib"
    ${simplerasters_PKGCONF_LIBRARY_DIRS}
)

if(simplerasters_LIBRARY)
    message(STATUS "SimpleRasters installed. Found at: ${simplerasters_LIBRARY}. Include dir at: ${simplerasters_INCLUDE_DIR}")

    if(NOT TARGET SimpleRasters::SimpleRasters)
        add_library(SimpleRasters::SimpleRasters UNKNOWN IMPORTED)
        set_target_properties(SimpleRasters::SimpleRasters PROPERTIES
            IMPORTED_LOCATION "${simplerasters_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${simplerasters_INCLUDE_DIR}")
    endif()
else()
    message(FATAL_ERROR "Could not find SimpleRasters.")
endif()
