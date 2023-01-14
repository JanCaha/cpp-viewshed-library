cmake_minimum_required(VERSION 3.18.0)

project(viewshed VERSION 1.1.0 DESCRIPTION "Viewshed Library based on QGIS/Qt and C++ 17" LANGUAGES CXX)

set(LIB_NAME viewshed)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC TRUE)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(QGIS REQUIRED)

find_package(Qt5 COMPONENTS Core Widgets Xml Gui REQUIRED)

find_package(Threads REQUIRED)

include_directories(
    ${QGIS_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
)

set(VISIBILITY_ALGS
    src/visibility_algorithms/viewshedvisibility.cpp
    src/visibility_algorithms/viewshedviewangle.cpp
    src/visibility_algorithms/viewshedelevationdifference.cpp
    src/visibility_algorithms/viewshedhorizons.cpp
    src/visibility_algorithms/viewshedelevationdifferencetolocalhorizon.cpp
    src/visibility_algorithms/viewshedelevationdifferencetoglobalhorizon.cpp
    src/visibility_algorithms/viewshedangledifferencetolocalhorizon.cpp
    src/visibility_algorithms/viewshedangledifferencetoglobalhorizon.cpp
    src/visibility_algorithms/losevaluator.cpp
)

set(MEMORY_RASTER
    src/memory_raster/memoryraster.cpp
)
set(UTILS
    src/utils/utils.cpp
)

set(STRUCTURES
    src/structures/event.cpp
    src/structures/points.cpp
    src/structures/position.cpp
    src/structures/statusnode.cpp
)

set(VISIBILITY
    src/visibility/visibility.cpp
    src/visibility/viewshed.cpp
)

set(MINIMAL_QGIS_LIBS
    ${QGIS_CORE_LIBRARY}
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    Qt5::Widgets
)

set(PUBLIC_HEADERS
    include/viewshead/enums.h
    include/viewshead/event.h
    include/viewshead/iviewshedalgorithm.h
    include/viewshead/losevaluator.h
    include/viewshead/memoryraster.h
    include/viewshead/points.h
    include/viewshead/position.h
    include/viewshead/statusnode.h
    include/viewshead/viewshedangledifferencetoglobalhorizon.h
    include/viewshead/viewshedelevationdifferencetolocalhorizon.h
    include/viewshead/viewshedelevationdifferencetoglobalhorizon.h
    include/viewshead/viewshedangledifferencetolocalhorizon.h
    include/viewshead/viewshedelevationdifference.h
    include/viewshead/viewshed.h
    include/viewshead/viewshedhorizons.h
    include/viewshead/viewshedviewangle.h
    include/viewshead/viewshedvisibility.h
    include/viewshead/visibility.h
    include/viewshead/viewshedvalues.h
    include/viewshead/utils.h
    "${CMAKE_BINARY_DIR}/_deps/bs_thread_pool_src-src/BS_thread_pool.hpp"
)

add_library(${LIB_NAME} SHARED
    ${MEMORY_RASTER}
    ${STRUCTURES}
    ${VISIBILITY_ALGS}
    ${VISIBILITY}
    ${UTILS}

    # ${EXTERNAL_HEADERS}
)

add_subdirectory(external)

set_target_properties(${LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${LIB_NAME} PROPERTIES SOVERSION 1)

set_target_properties(${LIB_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(${LIB_NAME} PUBLIC include/viewshead)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_BINARY_DIR}/bs_thread_pool_src-src)

target_link_libraries(${LIB_NAME}
    ${MINIMAL_QGIS_LIBS}
    bs_thread_pool
)

include(GNUInstallDirs)

install(TARGETS ${LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/viewshed
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

include(Packing)

include(CTest)
enable_testing(true)
add_subdirectory(tests)

# https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html
set(INCLUDE_INSTALL_DIR include/viewshed)
set(SYSCONFIG_INSTALL_DIR lib/libviewshed.so)

# ...
include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_MODULE_PATH}/ViewshedConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/Viewshed/cmake
    PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfigConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfigConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/viewshed
)