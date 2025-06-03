set(LIBRARY_VIEWSHED_SRC
    visibility_algorithms/boolean.cpp
    visibility_algorithms/viewangle.cpp
    visibility_algorithms/fuzzy.cpp
    visibility_algorithms/distancelocalhorizon.cpp
    visibility_algorithms/distanceglobalhorizon.cpp
    visibility_algorithms/elevationdifference.cpp
    visibility_algorithms/horizons.cpp
    visibility_algorithms/horizonscount.cpp
    visibility_algorithms/elevationdifferencetolocalhorizon.cpp
    visibility_algorithms/elevationdifferencetoglobalhorizon.cpp
    visibility_algorithms/angledifferencetolocalhorizon.cpp
    visibility_algorithms/angledifferencetoglobalhorizon.cpp
    visibility_algorithms/slopetoviewangle.cpp

    utils/viewshedutils.cpp

    structures/cellevent.cpp
    structures/point.cpp
    structures/celleventposition.cpp
    structures/losnode.cpp
    structures/losimportantvalues.cpp

    visibility/visibility.cpp
    visibility/viewshed.cpp
    visibility/inverseviewshed.cpp
    visibility/abstractviewshed.cpp

    thread_tasks/threadtasks.cpp

    los/abstractlos.cpp
    los/los.cpp
    los/inverselos.cpp
    los/losevaluator.cpp
)

macro(library_settings target_name)
    string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LOWER)

    get_target_property(target_type ${target_name} TYPE)

    if (WIN32)
        if(target_type STREQUAL "STATIC_LIBRARY")
            set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "${LIBRARY_NAME_LOWER}_static")
        else()
            set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME_LOWER})
            target_compile_definitions(${target_name} PRIVATE VIEWSHED_EXPORTS)
        endif()
    else()
        set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME_LOWER})
    endif()
    
    set_target_properties(${target_name} PROPERTIES VERSION ${PROJECT_VERSION})

    include(GNUInstallDirs)

    target_include_directories(${target_name} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/viewshed>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}> 
    )
    
    target_link_libraries(${target_name}
        GDAL::GDAL
        SimpleRasters::simplerasters_shared
    )

    install(TARGETS ${target_name}
        COMPONENT lib
        EXPORT ViewshedTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
    )
    
endmacro(library_settings)

add_library(viewshed_shared SHARED ${LIBRARY_VIEWSHED_SRC})
add_library(viewshed_static STATIC ${LIBRARY_VIEWSHED_SRC})

library_settings(viewshed_shared)
library_settings(viewshed_static)

configure_file(
    ${CMAKE_SOURCE_DIR}/external/bsthreadpool/include/BS_thread_pool.hpp
    ${CMAKE_SOURCE_DIR}/include/viewshed/BS_thread_pool.hpp
    COPYONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/external/bsthreadpool/include/BS_thread_pool_utils.hpp
    ${CMAKE_SOURCE_DIR}/include/viewshed/BS_thread_pool_utils.hpp
    COPYONLY
)

install(
    EXPORT ViewshedTargets
    FILE ViewshedTargets.cmake
    NAMESPACE Viewshed::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/viewshed/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
)

if(${CELL_EVENT_DATA_FLOAT})
    message(STATUS "CellEvents will have data type `float`.")
    set(CELL_EVENT_DATA_TYPE float)
    target_compile_definitions(viewshed_shared PUBLIC CELL_EVENT_USE_FLOAT=1)
    target_compile_definitions(viewshed_static PUBLIC CELL_EVENT_USE_FLOAT=1)
else()
    set(CELL_EVENT_DATA_TYPE double)
    message(STATUS "CellEvents will have data type `double`.")
endif()

if(${OUTPUT_RASTER_DATA_FLOAT})
    set(OUTPUT_RASTER_DATA_TYPE GDALDataType::GDT_Float32)
    message(STATUS "Output rasters will have data type `Float32`.")
else()
    set(OUTPUT_RASTER_DATA_TYPE GDALDataType::GDT_Float64)
    message(STATUS "Output rasters will have data type `Float64`.")
endif()

configure_file(
    ../../include/viewshed/templates/defaultdatatypes.txt
    ${CMAKE_SOURCE_DIR}/include/viewshed/defaultdatatypes.h
    @ONLY
)

configure_file(
    ../../include/viewshed/templates/viewshedlibrary.in
    ${CMAKE_SOURCE_DIR}/include/viewshed/viewshedlibrary.h
)
