include(LibFindMacros)

if(WIN32)
    if(DEFINED ENV{CONDA_BUILD})
        message(STATUS "CONDA_BUILD detected adjusting search paths for SimpleRasters, looking into $ENV{LIBRARY_PREFIX}")

        # Include dir
        find_path(simplerasters_INCLUDE_DIR
            NAMES simplerasters.h
            PATHS
            $ENV{BUILD_PREFIX}/Library/include
            PATH_SUFFIXES simplerasters
        )

        find_path(simplerasters_LIBRARY
            NAMES simplerasters.dll
            PATHS
            $ENV{BUILD_PREFIX}/Library/bin
            $ENV{BUILD_PREFIX}/Library/lib
        )
    else()
        message(FATAL_ERROR "Windows build detected but not a Conda build. Please activate a Conda environment.")
    endif()

# UNIX search
else()
    message(STATUS "Unix detected. Using standard paths for SimpleRasters.")

    # Include dir
    find_path(simplerasters_INCLUDE_DIR
        NAMES simplerasters.h
        PATHS
        /usr/include
        /usr/local/include
        "${CMAKE_PREFIX_PATH}/include"
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
    )
endif()

if(simplerasters_LIBRARY AND simplerasters_INCLUDE_DIR)
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
