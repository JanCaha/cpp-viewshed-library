include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(simplerasters_PKGCONF simplerasters)

# Include dir
find_path(simplerasters_INCLUDE_DIR
    NAMES simplerasters.h
    PATHS
    /usr/include
    /usr/local/include
    "${CMAKE_PREFIX_PATH}/include"
    $ENV{LIBRARY_INC}
    PATH_SUFFIXES simplerasters
)

# Finally the library itself
find_library(simplerasters_LIBRARY
    NAMES simplerasters
    PATHS
    /usr/lib
    /usr/lib64
    /usr/local/lib
    "${CMAKE_PREFIX_PATH}/lib"
    "${CMAKE_PREFIX_PATH}/Library/bin"
    $ENV{LIBRARY_LIB}
    $ENV{LIBRARY_BIN}
)
SET(AAA $ENV{LIBRARY_LIB})
message(STATUS "*** SimpleRasters library: ${AAA}")

message(STATUS "SimpleRasters dirs: $ENV{LIBRARY_LIB} $ENV{LIBRARY_BIN} $ENV{LIBRARY_INC} /// ${CMAKE_PREFIX_PATH}/include")
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
