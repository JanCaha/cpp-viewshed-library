# Binaries :
macro(create_binary bin_name filename add_widgets)
    add_executable(${bin_name} ${filename} commandlinehelper.h)

    set_target_properties(${bin_name} PROPERTIES AUTOMOC TRUE)

    target_include_directories(${bin_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/viewshed)
    target_include_directories(${bin_name} PUBLIC bs_thread_pool)
    target_include_directories(${bin_name} PRIVATE ${QT_INCLUDES})
    target_include_directories(${bin_name} PRIVATE ${GDAL_INCLUDE_DIR})
    target_include_directories(${bin_name} PRIVATE SimpleRasters::SimpleRasters)
    target_include_directories(${bin_name} PRIVATE viewshed_widgets)

    target_link_libraries(${bin_name}
        ${QT_LIBS}
        bs_thread_pool
        library_viewshed
        ${GDAL_LIBRARY}
        SimpleRasters::SimpleRasters
        viewshed_widgets
    )
endmacro(create_binary)

if(HAS_QT)
    message(STATUS "Has Qt so binaries can be build.")

    add_library(viewshed_widgets SHARED
        pointwidget.h pointwidget.cpp
        doublevalidator.h doublevalidator.cpp
        fileselectorwidget.h fileselectorwidget.cpp
        viewshedcalculatorwindow.h viewshedcalculatorwindow.cpp)

    set_target_properties(viewshed_widgets PROPERTIES OUTPUT_NAME "viewshedwidgets")

    set_target_properties(viewshed_widgets PROPERTIES VERSION ${PROJECT_VERSION})

    target_include_directories(viewshed_widgets PUBLIC ${QT_INCLUDES})
    target_include_directories(viewshed_widgets PUBLIC SimpleRasters::SimpleRasters)
    target_include_directories(viewshed_widgets PRIVATE ${GDAL_INCLUDE_DIR})

    target_link_libraries(viewshed_widgets
        ${QT_LIBS}
        library_viewshed
        ${GDAL_LIBRARY}
        SimpleRasters::SimpleRasters
    )

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
        losextractor
    )

    install(TARGETS
        viewshed inverseviewshed viewshedcalculator losextractor viewshed_widgets
        COMPONENT bin
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()