if(BUILD_DOCUMENTATION)
    # check if Doxygen is installed
    find_package(Doxygen)

    if(DOXYGEN_FOUND AND CMAKE_BUILD_TYPE STREQUAL "Release")
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message(STATUS "VIEWSHED LIBRARY: Documentation will be build.")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target(run_doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

        add_custom_target(build_documentation
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/docs/html ${CMAKE_SOURCE_DIR}/documentation
            DEPENDS run_doxygen)

        if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
            if(BUILD_DOCUMENTATION)
                add_dependencies(viewshed_shared build_documentation)
            endif()
        endif()

    else(DOXYGEN_FOUND AND CMAKE_BUILD_TYPE STREQUAL "Release")
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif(DOXYGEN_FOUND AND CMAKE_BUILD_TYPE STREQUAL "Release")
endif(BUILD_DOCUMENTATION)