# Conda-specific installation logic
if(DEFINED ENV{CONDA_BUILD})
    message(STATUS "Conda environment detected. Adjusting install paths for Conda.")

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