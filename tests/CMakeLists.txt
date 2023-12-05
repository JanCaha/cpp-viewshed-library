
message(STATUS "VIEWSHED LIBRARY: Tests will be build.")

find_package(GTest REQUIRED CONFIG)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(testsettings.in.h testsettings.h @ONLY)

include_directories(SYSTEM
    ${GTEST_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include/viewshed
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GDAL_INCLUDE_DIR}
    SimpleRasters::SimpleRasters
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

    target_link_libraries(${testname}
        library_viewshed
        ${GDAL_LIBRARY}
        SimpleRasters::SimpleRasters
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
    )

    add_test(${testname} ${testname} -maxwarnings 10000)
    add_dependencies(build_tests ${testname})
endmacro(ADD_LIBRARY_TEST)

macro(ADD_GUI_TEST filename)
    string(REPLACE "test" "" testname ${filename})
    set(testname test_gui_${testname})
    add_executable(${testname} "gui/${filename}.cpp")

    target_link_libraries(${testname}
        library_viewshed
        ${GDAL_LIBRARY}
        SimpleRasters::SimpleRasters
        ${QT_LIBS}
        Qt5::Test
        viewshed_widgets
    )
    include_directories(${testname} ${CMAKE_SOURCE_DIR}/src/bin)

    add_test(${testname} ${testname} -maxwarnings 10000)
    add_dependencies(build_tests ${testname})
endmacro(ADD_GUI_TEST)

# ############################################################

# ############################################################
# Tests:
ADD_LIBRARY_TEST(testcellevent)
ADD_LIBRARY_TEST(testviewpoint)
ADD_LIBRARY_TEST(testlosalgorithms)
ADD_LIBRARY_TEST(testviewshed)
ADD_LIBRARY_TEST(testinverselosalgorithms)
ADD_LIBRARY_TEST(testinverseviewshed)
ADD_LIBRARY_TEST(testviewshedtiming)

if(HAS_QT)
    ADD_GUI_TEST(testviewshedcalculator)
    ADD_GUI_TEST(testpointwidget)
    ADD_GUI_TEST(testfileselectorwidget)
endif()