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
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME})
    set_target_properties(${target_name} PROPERTIES VERSION ${PROJECT_VERSION})

    target_include_directories(${target_name} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/viewshed>
        $<INSTALL_INTERFACE:include/viewshed>
    )

    target_include_directories(${target_name} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/bsthreadpool/include>
        $<INSTALL_INTERFACE:include/viewshed>
    )

    install(TARGETS ${target_name}
        COMPONENT lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
        PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
    )
endmacro(library_settings)

add_library(library_viewshed SHARED ${LIBRARY_VIEWSHED_SRC})
add_library(library_viewshed_static STATIC ${LIBRARY_VIEWSHED_SRC})

library_settings(library_viewshed)
library_settings(library_viewshed_static)

if(${CELL_EVENT_DATA_FLOAT})
    message(STATUS "CellEvents will have data type `float`.")
    set(CELL_EVENT_DATA_TYPE float)
    target_compile_definitions(library_viewshed PUBLIC CELL_EVENT_USE_FLOAT=1)
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
