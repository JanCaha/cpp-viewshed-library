set(LIBRARY_VIEWSHED_SRC
    visibility_algorithms/boolean.cpp
    visibility_algorithms/viewangle.cpp
    visibility_algorithms/elevationdifference.cpp
    visibility_algorithms/horizons.cpp
    visibility_algorithms/elevationdifferencetolocalhorizon.cpp
    visibility_algorithms/elevationdifferencetoglobalhorizon.cpp
    visibility_algorithms/angledifferencetolocalhorizon.cpp
    visibility_algorithms/angledifferencetoglobalhorizon.cpp
    visibility_algorithms/horizondistance.cpp
    visibility_algorithms/slopetoviewangle.cpp

    memory_raster/memoryraster.cpp

    utils/utils.cpp

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

add_library(library_viewshed SHARED ${LIBRARY_VIEWSHED_SRC})

set_target_properties(library_viewshed PROPERTIES OUTPUT_NAME "viewshed")

set_target_properties(library_viewshed PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(library_viewshed PROPERTIES SOVERSION 1)

set_target_properties(library_viewshed PROPERTIES PUBLIC_HEADER "${LIBRARY_VIEWSHED_PUBLIC_HEADERS}")
set_target_properties(library_viewshed PROPERTIES PRIVATE_HEADER "${LIBRARY_VIEWSHED_PRIVATE_HEADERS}")

target_include_directories(library_viewshed PUBLIC ${CMAKE_SOURCE_DIR}/include/viewshed)
target_include_directories(library_viewshed PUBLIC ${CMAKE_BINARY_DIR}/bs_thread_pool_src-src)
target_include_directories(library_viewshed PRIVATE ${MINIMAL_QGIS_INCLUDES})

target_link_libraries(library_viewshed
    ${MINIMAL_QGIS_LIBS}
    bs_thread_pool
)

if(${CELL_EVENT_DATA_FLOAT})
    message(STATUS "CellEvents will have data type `float`.")
    target_compile_definitions(library_viewshed PUBLIC CELL_EVENT_TYPE=float)
else()
    message(STATUS "CellEvents will have data type `double`.")
    target_compile_definitions(library_viewshed PUBLIC CELL_EVENT_TYPE=double)
endif()

if(${OUTPUT_RASTER_DATA_FLOAT})
    message(STATUS "Output rasters will have data type `Float32`.")
    target_compile_definitions(library_viewshed PUBLIC OUTPUT_RASTER_DATA_TYPE=Qgis::DataType::Float32)
else()
    message(STATUS "Output rasters will have data type `Float64`.")
    target_compile_definitions(library_viewshed PUBLIC OUTPUT_RASTER_DATA_TYPE=Qgis::DataType::Float64)
endif()