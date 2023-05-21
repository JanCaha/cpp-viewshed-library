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

add_library(library_viewshed SHARED ${LIBRARY_VIEWSHED_SRC})

set_target_properties(library_viewshed PROPERTIES OUTPUT_NAME "viewshed")

set_target_properties(library_viewshed PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(library_viewshed PROPERTIES PUBLIC_HEADER "${LIBRARY_VIEWSHED_PUBLIC_HEADERS}")
set_target_properties(library_viewshed PROPERTIES PRIVATE_HEADER "${LIBRARY_VIEWSHED_PRIVATE_HEADERS}")

target_include_directories(library_viewshed PUBLIC ${CMAKE_SOURCE_DIR}/include/viewshed)
target_include_directories(library_viewshed PUBLIC ${CMAKE_BINARY_DIR}/bs_thread_pool_src-src)
target_include_directories(library_viewshed PRIVATE ${GDAL_INCLUDE_DIR})

target_link_libraries(library_viewshed
    bs_thread_pool
    ${GDAL_LIBRARY}
)

if(SIMPLERASTERS_SYS_DEP)
    target_include_directories(library_viewshed PUBLIC ${simplerasters_INCLUDE_DIR})
    target_link_libraries(library_viewshed
        ${simplerasters_LIBRARY}
    )
else()
    target_include_directories(library_viewshed PUBLIC ${CMAKE_BINARY_DIR}/_deps/simplerasters-src/src/api)
    target_link_libraries(library_viewshed
        simplerasters
    )
endif()

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