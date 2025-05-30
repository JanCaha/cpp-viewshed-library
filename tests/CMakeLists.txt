
message(STATUS "VIEWSHED LIBRARY: Tests will be build.")

find_package(GTest REQUIRED CONFIG)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(testsettings.in.h testsettings.h @ONLY)

include_directories(SYSTEM
    ${GTEST_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include/viewshed
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
macro(add_library_test testname)
    add_executable(${testname} "${testname}.cpp")

    target_link_libraries(${testname}
        viewshed_shared
        GDAL::GDAL
        SimpleRasters::simplerasters_shared
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
    )

    add_test(${testname} ${testname} -maxwarnings 10000)
    add_dependencies(build_tests ${testname})
endmacro(add_library_test)

macro(add_gui_test filename)
    string(REPLACE "test" "" testname ${filename})
    set(testname test_gui_${testname})
    add_executable(${testname} "gui/${filename}.cpp")
    
    target_link_libraries(${testname}
        viewshed_shared
        GDAL::GDAL
        SimpleRasters::simplerasters_shared
        ${QT_LIBS}
        Qt5::Test
        viewshed_widgets
    )

    include_directories(${testname} ${CMAKE_SOURCE_DIR}/src/bin)

    add_test(${testname} ${testname} -maxwarnings 10000)
    add_dependencies(build_tests ${testname})
endmacro(add_gui_test)

# ############################################################

# ############################################################
# Tests:
add_library_test(testcellevent)
add_library_test(testviewpoint)
add_library_test(testlosalgorithms)
add_library_test(testviewshed)
add_library_test(testinverselosalgorithms)
add_library_test(testinverseviewshed)
add_library_test(testviewshedtiming)

if(HAS_QT)
    add_gui_test(testviewshedcalculator)
    add_gui_test(testpointwidget)
    add_gui_test(testfileselectorwidget)
endif()