cmake_minimum_required(VERSION 3.20.0)

project(viewshed VERSION 1.9.0 DESCRIPTION "Viewshed Library based on QGIS/Qt and C++ 17" LANGUAGES CXX)

option(BUILD_TESTS "Build tests." ON)
option(BUILD_DOCUMENTATION "Build documentation." ON)
option(PACK_DEB "Pack DEB file for installation in Debian/Ubuntu/Mint and derivatives." ON)

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
    src/visibility_algorithms/visibilityboolean.cpp
    src/visibility_algorithms/visibilityviewangle.cpp
    src/visibility_algorithms/visibilityelevationdifference.cpp
    src/visibility_algorithms/visibilityhorizons.cpp
    src/visibility_algorithms/visibilityelevationdifferencetolocalhorizon.cpp
    src/visibility_algorithms/visibilityelevationdifferencetoglobalhorizon.cpp
    src/visibility_algorithms/visibilityangledifferencetolocalhorizon.cpp
    src/visibility_algorithms/visibilityangledifferencetoglobalhorizon.cpp
    src/visibility_algorithms/visibilityhorizondistance.cpp
)

set(MEMORY_RASTER
    src/memory_raster/memoryraster.cpp
)
set(UTILS
    src/utils/utils.cpp
)

set(STRUCTURES
    src/structures/cellevent.cpp
    src/structures/point.cpp
    src/structures/celleventposition.cpp
    src/structures/losnode.cpp
    src/structures/losimportantvalues.cpp
)

set(VISIBILITY
    src/visibility/visibility.cpp
    src/visibility/viewshed.cpp
    src/visibility/inverseviewshed.cpp
    src/visibility/abstractviewshed.cpp
)

set(THREAD_TASKS
    src/thread_tasks/threadtasks.cpp
)

set(LOS
    src/los/abstractlos.cpp
    src/los/los.cpp
    src/los/inverselos.cpp
    src/los/losevaluator.cpp
)

set(MINIMAL_QGIS_LIBS
    ${QGIS_CORE_LIBRARY}
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    Qt5::Widgets
)

set(PUBLIC_HEADERS
    include/viewshead/abstractlos.h
    include/viewshead/abstractviewshed.h
    include/viewshead/abstractviewshedalgorithm.h
    include/viewshead/cellevent.h
    include/viewshead/celleventposition.h
    include/viewshead/enums.h
    include/viewshead/inverselos.h
    include/viewshead/inverseviewshed.h
    include/viewshead/los.h
    include/viewshead/losevaluator.h
    include/viewshead/losimportantvalues.h
    include/viewshead/losnode.h
    include/viewshead/memoryraster.h
    include/viewshead/point.h
    include/viewshead/rasterposition.h
    include/viewshead/threadtasks.h
    include/viewshead/utils.h
    include/viewshead/viewshed.h
    include/viewshead/visibilityangledifferencetoglobalhorizon.h
    include/viewshead/visibilityangledifferencetolocalhorizon.h
    include/viewshead/visibilityelevationdifference.h
    include/viewshead/visibilityhorizondistance.h
    include/viewshead/visibilityelevationdifferencetolocalhorizon.h
    include/viewshead/visibilityelevationdifferencetoglobalhorizon.h
    include/viewshead/visibilityhorizons.h
    include/viewshead/visibilityvalues.h
    include/viewshead/visibilityviewangle.h
    include/viewshead/visibilityboolean.h
    include/viewshead/visibility.h
    "${CMAKE_BINARY_DIR}/_deps/bs_thread_pool_src-src/BS_thread_pool.hpp"
)

add_library(${LIB_NAME} SHARED
    ${MEMORY_RASTER}
    ${STRUCTURES}
    ${VISIBILITY_ALGS}
    ${VISIBILITY}
    ${UTILS}
    ${THREAD_TASKS}
    ${LOS}
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

# ############################################################
# add tests :
add_subdirectory(tests)

# ############################################################

# ############################################################
# install :
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
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ViewshedConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/viewshed
)

include(GNUInstallDirs)

install(TARGETS ${LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/viewshed
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# ############################################################

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

        add_custom_target(pack_deb
            COMMAND ${CMAKE_CPACK_COMMAND} -G DEB -C Release
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        add_dependencies(pack_deb ${LIB_NAME})
    endif()
endif()

# ############################################################

# ############################################################
# Add uninstall :
add_custom_target(uninstall)

add_custom_target(delete_lib COMMAND rm ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/libviewshed*)
add_custom_target(delete_include COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/viewshed)
add_custom_target(delete_cmake COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/viewshed)

add_dependencies(uninstall delete_lib)
add_dependencies(uninstall delete_include)
add_dependencies(uninstall delete_cmake)

# ############################################################

# ############################################################
# Add docs :
add_subdirectory(docs)