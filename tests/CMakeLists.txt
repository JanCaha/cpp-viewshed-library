if(BUILD_TESTS)
    message(STATUS "VIEWSHED LIBRARY: Tests will be build.")

    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.13.0
    )

    FetchContent_GetProperties(googletest)

    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif()

    include(CTest)
    enable_testing(true)

    set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

    configure_file(testsettings.in.h testsettings.h @ONLY)

    include_directories(SYSTEM
        ${GTEST_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include/viewshed
        ${CMAKE_CURRENT_BINARY_DIR}
        ${GDAL_INCLUDE_DIR}
    )

    if(SIMPLERASTERS_SYS_DEP)
        include_directories(
            ${simplerasters_LIBRARY}
        )
    else()
        include_directories(
            simplerasters
        )
    endif()

    # ############################################################
    # Prepare targets for tests:
    add_custom_target(build_tests)

    # 'run_tests' triggers 'ctest'.
    add_custom_target(run_tests COMMAND ${CMAKE_CTEST_COMMAND} "-VV")

    # 'run_tests' implies 'build_tests'
    add_dependencies(run_tests build_tests)

    # ############################################################

    # ############################################################
    # Macro to add a test:
    macro(ADD_LIBRARY_TEST testname)
        add_executable(${testname} "${testname}.cpp")

        target_link_libraries(${testname}
            library_viewshed
            ${GDAL_LIBRARY}
            gtest
            gmock
            gtest_main
        )

        if(SIMPLERASTERS_SYS_DEP)
            target_link_libraries(${testname}
                SimpleRasters::simplerasters
            )
        else()
            target_link_libraries(${testname}
                simplerasters
            )
        endif()

        add_test(${testname} ${testname} -maxwarnings 10000)
        add_dependencies(build_tests ${testname})
    endmacro(ADD_LIBRARY_TEST)

    # ############################################################

    # ############################################################
    # Tests:
    ADD_LIBRARY_TEST(testcellevent)
    ADD_LIBRARY_TEST(testviewpoint)
    ADD_LIBRARY_TEST(testlosalgorithms)
    ADD_LIBRARY_TEST(testviewshed)
    ADD_LIBRARY_TEST(testinverselosalgorithms)
    ADD_LIBRARY_TEST(testinverseviewshed)
endif()