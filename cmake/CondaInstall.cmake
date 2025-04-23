# Conda-specific installation for WINDOWS
if(DEFINED ENV{CONDA_BUILD} AND WIN32)
    message(STATUS "Conda environment detected. Adjusting install paths for Conda.")

    # Ensure static libraries are not built
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)

    install(TARGETS library_viewshed
        COMPONENT lib
        RUNTIME DESTINATION $ENV{LIBRARY_BIN}
        LIBRARY DESTINATION $ENV{LIBRARY_LIB}
        PUBLIC_HEADER DESTINATION $ENV{LIBRARY_INC}/${LIBRARY_NAME}
        PRIVATE_HEADER DESTINATION $ENV{LIBRARY_INC}/${LIBRARY_NAME}
        INCLUDES DESTINATION $ENV{LIBRARY_INC}/${LIBRARY_NAME}
    )

    if(HAS_QT)
        install(TARGETS
            viewshed inverseviewshed viewshedcalculator losextractor viewshed_widgets
            COMPONENT bin
            RUNTIME DESTINATION $ENV{LIBRARY_BIN}
            LIBRARY DESTINATION $ENV{LIBRARY_LIB}
        )
    endif()
else()
    message(FATAL_ERROR "Conda environment not detected. Please activate a Conda environment.")
endif()