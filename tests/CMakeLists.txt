find_package(Qt5Test REQUIRED)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(testsettings.in.h testsettings.h @ONLY)

include_directories(SYSTEM
    ${QT_INCLUDE_DIR}
    ${QGIS_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

macro(ADD_MY_TEST testname testsrc)
    add_executable(${testname} ${testsrc})
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
endmacro(ADD_MY_TEST)

# ############################################################
# Tests:
ADD_MY_TEST(testviewpoint testviewpoint.cpp)
ADD_MY_TEST(testmemoryraster testmemoryraster.cpp)
ADD_MY_TEST(testlos testlos.cpp)
