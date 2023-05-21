include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(simplerasters_PKGCONF simplerasters)

# Include dir
find_path(simplerasters_INCLUDE_DIR
    NAMES simplerasters.h
    PATHS ${simplerasters_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(simplerasters_LIBRARY
    NAMES simplerasters
    PATHS ${simplerasters_PKGCONF_LIBRARY_DIRS}
)

if(simplerasters_PKGCONF_FOUND)
    message(STATUS "Library simplerasters found at: ${simplerasters_LIBRARY}.")
endif()
