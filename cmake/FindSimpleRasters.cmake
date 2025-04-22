include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(simplerasters_PKGCONF simplerasters)

if(DEFINED ENV{CONDA_PREFIX})
    set(CONDA_INCLUDE_DIR $ENV{CONDA_PREFIX}/Library/include)

    if(WIN32)
        set(CONDA_LIBRARY_DIR $ENV{CONDA_PREFIX}/Library/bin $ENV{CONDA_PREFIX}/Library/lib)
    else()
        set(CONDA_LIBRAWRY_DIR $ENV{CONDA_PREFIX}/lib)
    endif()
endif()

# Include dir
find_path(simplerasters_INCLUDE_DIR
    NAMES simplerasters.h
    PATHS
    /usr/include
    /usr/local/include
    "${CMAKE_PREFIX_PATH}/include"
    "${CONDA_INCLUDE_DIR}"
    PATH_SUFFIXES simplerasters
    ${simplerasters_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(simplerasters_LIBRARY
    NAMES simplerasters
    PATHS
    /usr/lib
    /usr/lib64
    /usr/local/lib
    "${CONDA_LIBRARY_DIR}"
    "${CMAKE_PREFIX_PATH}/lib"
    ${simplerasters_PKGCONF_LIBRARY_DIRS}
)

message(STATUS "SimpleRasters installed. Found at: ${simplerasters_LIBRARY}. Include dir at: ${simplerasters_INCLUDE_DIR}")

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
