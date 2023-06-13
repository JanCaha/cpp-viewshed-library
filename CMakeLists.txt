cmake_minimum_required(VERSION 3.20.0)

project(viewshed VERSION 2.0.1 DESCRIPTION "Viewshed library based on QGIS/Qt and C++ 17" LANGUAGES CXX)

option(BUILD_TESTS "Build tests." ON)
option(BUILD_DOCUMENTATION "Build documentation." OFF)
option(PACK_DEB "Pack DEB file for installation in Debian/Ubuntu/Mint and derivatives." ON)
option(SIMPLERASTERS_SYS_DEP "USE simpleRasters library from system" ON)

option(CELL_EVENT_DATA_FLOAT "Create CellEvents with data stored `float` instead of `double`. Lowers significantly RAM memory usage." ON)
option(OUTPUT_RASTER_DATA_FLOAT "Create output rasters with data stored `Float32` instead of `Float64`. Lowers RAM memory usage." ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC TRUE)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets Xml Gui REQUIRED)

find_package(Threads REQUIRED)

if(SIMPLERASTERS_SYS_DEP)
    message(STATUS "Searching for library simplerasters in system!")
    find_package(SimpleRasters)
endif()

set(QT_INCLUDES
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
)

set(QT_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

find_package(GDAL REQUIRED)

set(LIBRARY_VIEWSHED_PRIVATE_HEADERS
    ${CMAKE_BINARY_DIR}/_deps/bs_thread_pool_src-src/BS_thread_pool.hpp
)

set(LIBRARY_VIEWSHED_PUBLIC_HEADERS
    include/viewshed/abstractlos.h
    include/viewshed/abstractviewshed.h
    include/viewshed/abstractviewshedalgorithm.h
    include/viewshed/cellevent.h
    include/viewshed/celleventposition.h
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
)

# ############################################################
# add sources both external and specific
add_subdirectory(external)

add_subdirectory(src)

# ############################################################
# install :
# https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html
set(INCLUDE_INSTALL_DIR include/viewshed)
set(SYSCONFIG_INSTALL_DIR lib/libviewshed.so)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_MODULE_PATH}/ViewshedConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/Viewshed/cmake
    PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

include(GNUInstallDirs)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/viewshed
)

install(TARGETS library_viewshed
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
    PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
)

install(TARGETS
    viewshed inverseviewshed viewshedcalculator losextractor
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ############################################################
# Pack :
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if(PACK_DEB)
        message(STATUS "VIEWSHED LIBRARY: DEB will be packed file.")

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
endif()

# ############################################################
# Add docs :
add_subdirectory(docs)

# ############################################################
# add tests :
add_subdirectory(tests)