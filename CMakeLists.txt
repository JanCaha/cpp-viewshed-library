cmake_minimum_required(VERSION 3.20.0)

# Project Information
project(viewshed VERSION 4.1.0 DESCRIPTION "C++ Viewshed library based on GDAL and Qt" LANGUAGES CXX)

# Library Name
SET(LIBRARY_NAME "viewshed")

# Options
option(BUILD_TESTS "Build tests." ON)
option(BUILD_DOCUMENTATION "Build documentation." OFF)
option(PACK_DEB "Pack DEB file for installation in Debian/Ubuntu/Mint and derivatives." ON)
option(NEEDS_QT "Allows optionally building library without searching for Qt. Cannot build binaries with this set to OFF. For running test mainly." ON)
option(BUILD_GUI_APP "Allows building of executable program with GUI" ON)
option(CELL_EVENT_DATA_FLOAT "Create CellEvents with data stored `float` instead of `double`. Lowers significantly RAM memory usage." ON)
option(OUTPUT_RASTER_DATA_FLOAT "Create output rasters with data stored `Float32` instead of `Float64`. Lowers RAM memory usage." ON)

if(BUILD_GUI_APP)
    set(NEEDS_QT ON)
endif()

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Module Path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Dependencies
find_package(GDAL REQUIRED)
find_package(Threads REQUIRED)
find_package(SimpleRasters REQUIRED)

if(NEEDS_QT)
    set(CMAKE_AUTOMOC TRUE)

    find_package(Qt5 COMPONENTS Core Xml Test REQUIRED)

    set(QT_INCLUDES
        ${Qt5Core_INCLUDE_DIRS}
    )

    set(QT_LIBS
        Qt5::Core
    )

    if(BUILD_GUI_APP)
        find_package(Qt5 COMPONENTS Core Widgets Xml Gui Test REQUIRED)

        set(QT_INCLUDES
            ${Qt5Core_INCLUDE_DIRS}
        )

        set(QT_LIBS
            ${QT_LIBS}
            Qt5::Gui
            Qt5::Widgets
        )
    endif()

    set(HAS_QT TRUE)
else()
    message(STATUS "Not using Qt")
endif()

# Public Headers
set(LIBRARY_VIEWSHED_PUBLIC_HEADERS
    include/viewshed/abstractlos.h
    include/viewshed/abstractviewshed.h
    include/viewshed/abstractviewshedalgorithm.h
    include/viewshed/cellevent.h
    include/viewshed/celleventposition.h
    include/viewshed/constants.h
    include/viewshed/defaultdatatypes.h
    include/viewshed/enums.h
    include/viewshed/inverselos.h
    include/viewshed/inverseviewshed.h
    include/viewshed/los.h
    include/viewshed/losevaluator.h
    include/viewshed/losimportantvalues.h
    include/viewshed/losnode.h
    include/viewshed/point.h
    include/viewshed/rasterposition.h
    include/viewshed/threadtasks.h
    include/viewshed/viewshed.h
    include/viewshed/viewshedvalues.h
    include/viewshed/visibility.h
    include/viewshed/visibilityangledifferencetoglobalhorizon.h
    include/viewshed/visibilityangledifferencetolocalhorizon.h
    include/viewshed/visibilitydistanceglobalhorizon.h
    include/viewshed/visibilitydistancelocalhorizon.h
    include/viewshed/visibilityelevationdifference.h
    include/viewshed/visibilityelevationdifferencetolocalhorizon.h
    include/viewshed/visibilityelevationdifferencetoglobalhorizon.h
    include/viewshed/visibilityfuzzy.h
    include/viewshed/visibilityhorizons.h
    include/viewshed/visibilityhorizonscount.h
    include/viewshed/visibilityviewangle.h
    include/viewshed/visibilityboolean.h
    include/viewshed/visibilityalgorithms.h
    include/viewshed/visibilityslopetoviewangle.h
    include/viewshed/viewshedlibrary.h
    include/viewshed/viewshedutils.h
    include/viewshed/viewshedtypes.h
)

# Subdirectories
add_subdirectory(src)

# Create and install package configuration files
include(CMakePackageConfigHelpers)

# Configure version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Configure config file template
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

# Install configuration files
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

# Documentation
if(BUILD_DOCUMENTATION)
    add_subdirectory(docs)
endif()

# Tests
if(BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

# Packaging (DEB)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND PACK_DEB)
    include(Packing)
    file(GLOB old_versions "${CMAKE_SOURCE_DIR}/_packages/*.deb")

    if(old_versions)
        file(REMOVE ${old_versions})
    endif()

    add_custom_target(pack_viewshed_library_deb
        COMMAND ${CMAKE_CPACK_COMMAND} -G DEB -C Release
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_dependencies(pack_viewshed_library_deb library_viewshed)
endif()