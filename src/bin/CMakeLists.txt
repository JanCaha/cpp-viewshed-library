# Binaries :
macro(create_binary bin_name filename add_widgets)
    if(${add_widgets})
        add_executable(${bin_name} ${filename}
            pointwidget.h pointwidget.cpp
            doublevalidator.h doublevalidator.cpp
            fileselectorwidget.h fileselectorwidget.cpp
        )
    else()
        add_executable(${bin_name} ${filename} commandlinehelper.h)
    endif()

    set_target_properties(${bin_name} PROPERTIES AUTOMOC TRUE)

    target_include_directories(${bin_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/viewshed)
    target_include_directories(${bin_name} PUBLIC ${CMAKE_BINARY_DIR}/bs_thread_pool_src-src)
    target_include_directories(${bin_name} PRIVATE ${QT_INCLUDES})
    target_include_directories(${bin_name} PRIVATE ${GDAL_INCLUDE_DIR})
    target_include_directories(${bin_name} PUBLIC ${CMAKE_BINARY_DIR}/_deps/simplerasters-src/src/api)

    target_link_libraries(${bin_name}
        ${QT_LIBS}
        bs_thread_pool
        library_viewshed
        ${GDAL_LIBRARY}
        simplerasters
    )
endmacro(create_binary)

create_binary(viewshed viewshed.cpp OFF)

create_binary(inverseviewshed inverseviewshed.cpp OFF)

create_binary(viewshedcalculator viewshedcalculator.cpp ON)

create_binary(losextractor losextractor.cpp ON)

add_custom_target(create_binaries)

add_dependencies(create_binaries viewshed)

add_dependencies(create_binaries
    viewshed
    inverseviewshed
    viewshedcalculator

    # losextractor
)

install(TARGETS
    viewshed inverseviewshed # viewshedcalculator
    RUNTIME DESTINATION bin
)
