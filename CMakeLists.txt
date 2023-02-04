cmake_minimum_required(VERSION 3.20.0)

project(viewshed VERSION 1.9.1 DESCRIPTION "Viewshed Library based on QGIS/Qt and C++ 17" LANGUAGES CXX)

option(BUILD_TESTS "Build tests." ON)
option(BUILD_DOCUMENTATION "Build documentation." ON)
option(PACK_DEB "Pack DEB file for installation in Debian/Ubuntu/Mint and derivatives." ON)

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
    src/visibility_algorithms/boolean.cpp
    src/visibility_algorithms/viewangle.cpp
    src/visibility_algorithms/elevationdifference.cpp
    src/visibility_algorithms/horizons.cpp
    src/visibility_algorithms/elevationdifferencetolocalhorizon.cpp
    src/visibility_algorithms/elevationdifferencetoglobalhorizon.cpp
    src/visibility_algorithms/angledifferencetolocalhorizon.cpp
    src/visibility_algorithms/angledifferencetoglobalhorizon.cpp
    src/visibility_algorithms/horizondistance.cpp
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
    include/viewshed/abstractlos.h
    include/viewshed/abstractviewshed.h
    include/viewshed/abstractviewshedalgorithm.h
    include/viewshed/cellevent.h
    include/viewshed/celleventposition.h
    include/viewshed/enums.h
    include/viewshed/inverselos.h
    include/viewshed/inverseviewshed.h
    include/viewshed/los.h
    include/viewshed/losevaluator.h
    include/viewshed/losimportantvalues.h
    include/viewshed/losnode.h
    include/viewshed/memoryraster.h
    include/viewshed/point.h
    include/viewshed/rasterposition.h
    include/viewshed/threadtasks.h
    include/viewshed/utils.h
    include/viewshed/viewshed.h
    include/viewshed/viewshedvalues.h
    include/viewshed/visibility.h
    include/viewshed/visibilityangledifferencetoglobalhorizon.h
    include/viewshed/visibilityangledifferencetolocalhorizon.h
    include/viewshed/visibilityelevationdifference.h
    include/viewshed/visibilityhorizondistance.h
    include/viewshed/visibilityelevationdifferencetolocalhorizon.h
    include/viewshed/visibilityelevationdifferencetoglobalhorizon.h
    include/viewshed/visibilityhorizons.h
    include/viewshed/visibilityviewangle.h
    include/viewshed/visibilityboolean.h
    include/viewshed/visibilityalgorithms.h
    ${CMAKE_BINARY_DIR}/_deps/bs_thread_pool_src-src/BS_thread_pool.hpp
)
add_library(library_viewshed SHARED
    ${MEMORY_RASTER}
    ${STRUCTURES}
    ${VISIBILITY_ALGS}
    ${VISIBILITY}
    ${UTILS}
    ${THREAD_TASKS}
    ${LOS}
)

set_target_properties(library_viewshed PROPERTIES OUTPUT_NAME "viewshed")

add_subdirectory(external)

set_target_properties(library_viewshed PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(library_viewshed PROPERTIES SOVERSION 1)

set_target_properties(library_viewshed PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(library_viewshed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/viewshed)
target_include_directories(library_viewshed PUBLIC ${CMAKE_BINARY_DIR}/bs_thread_pool_src-src)

target_link_libraries(library_viewshed
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

install(TARGETS library_viewshed
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/viewshed
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# ############################################################

# ############################################################
# Binaries :
macro(create_binary bin_name filename)
    add_executable(${bin_name} ${filename})

    set_target_properties(${bin_name} PROPERTIES AUTOMOC TRUE)

    target_include_directories(${bin_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/viewshed)
    target_include_directories(${bin_name} PUBLIC ${CMAKE_BINARY_DIR}/bs_thread_pool_src-src)

    target_link_libraries(${bin_name}
        ${MINIMAL_QGIS_LIBS}
        ${QGIS_GUI_LIBRARY}
        bs_thread_pool
        library_viewshed
    )
endmacro(create_binary)

create_binary(viewshed src/bin/viewshed.cpp)

create_binary(inverseviewshed src/bin/inverseviewshed.cpp)
create_binary(viewshedcalculator src/bin/viewshedcalculator.cpp)
add_custom_target(create_binaries)

add_dependencies(create_binaries viewshed)

add_dependencies(create_binaries viewshed inverseviewshed viewshedcalculator)

install(TARGETS
    viewshed inverseviewshed viewshedcalculator
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PERMISSIONS WORLD_READ WORLD_WRITE WORLD_EXECUTE)

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

        add_dependencies(pack_deb library_viewshed)
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