if(BUILD_TESTS)
    message(STATUS "VIEWSHED LIBRARY: Tests will be build.")

    include(CTest)
    enable_testing(true)

    find_package(Qt5Test REQUIRED)

    set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

    configure_file(testsettings.in.h testsettings.h @ONLY)

    include_directories(SYSTEM
        ${QT_INCLUDE_DIR}
        ${QGIS_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
    )

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
        set_target_properties(${testname} PROPERTIES AUTOMOC TRUE)

        target_link_libraries(${testname}
            ${QGIS_CORE_LIBRARY}
            ${QGIS_GUI_LIBRARY}
            Qt5::Core
            Qt5::Gui
            Qt5::Xml
            Qt5::Widgets
            Qt5::Test
            ${LIB_NAME}
        )
        add_test(${testname} ${testname} -maxwarnings 10000)
        add_dependencies(build_tests ${testname})
    endmacro(ADD_LIBRARY_TEST)

    # ############################################################

    # ############################################################
    # Tests:
    ADD_LIBRARY_TEST(testviewpoint)
    ADD_LIBRARY_TEST(testmemoryraster)
    ADD_LIBRARY_TEST(testlosalgorithms)
    ADD_LIBRARY_TEST(testviewshed)
    ADD_LIBRARY_TEST(testinverselosalgorithms)
    ADD_LIBRARY_TEST(testinverseviewshed)
endif()