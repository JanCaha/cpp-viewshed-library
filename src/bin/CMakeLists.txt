# Binaries :
add_custom_target(create_binaries)

macro(create_binary bin_name filename)
    add_executable(${bin_name} ${filename} commandlinehelper.h)

    set_target_properties(${bin_name} PROPERTIES AUTOMOC TRUE)

    target_include_directories(${bin_name} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/viewshed>
        $<INSTALL_INTERFACE:include/viewshed>
    )

    target_include_directories(${bin_name} PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/bsthreadpool/include>
        $<INSTALL_INTERFACE:include/viewshed>
    )

    target_link_libraries(${bin_name}
        ${QT_LIBS}
        GDAL::GDAL
        viewshed_shared
        SimpleRasters::simplerasters_shared
    )

    add_dependencies(create_binaries ${bin_name})

    install(TARGETS
        ${bin_name}
        COMPONENT bin
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endmacro(create_binary)

macro(create_gui_binary bin_name filename)
    create_binary(${bin_name} ${filename})

    target_include_directories(${bin_name} PRIVATE viewshed_widgets)
    target_link_libraries(${bin_name} viewshed_widgets)
endmacro(create_gui_binary)

if(HAS_QT)
    message(STATUS "Has Qt so binaries can be build.")

    create_binary(viewshed viewshed.cpp)

    create_binary(inverseviewshed inverseviewshed.cpp)

    add_dependencies(create_binaries
        viewshed
        inverseviewshed
    )

    if(BUILD_GUI_APP)
        add_library(viewshed_widgets SHARED
            pointwidget.h pointwidget.cpp
            doublevalidator.h doublevalidator.cpp
            fileselectorwidget.h fileselectorwidget.cpp
            viewshedcalculatorwindow.h viewshedcalculatorwindow.cpp)

        set_target_properties(viewshed_widgets PROPERTIES OUTPUT_NAME "viewshedwidgets")

        set_target_properties(viewshed_widgets PROPERTIES VERSION ${PROJECT_VERSION})

        install(TARGETS
            viewshed_widgets
            COMPONENT bin
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

        target_link_libraries(viewshed_widgets
            ${QT_LIBS}
            viewshed_shared
            GDAL::GDAL
            SimpleRasters::simplerasters_shared
        )

        create_gui_binary(viewshedcalculator viewshedcalculator.cpp)
        create_gui_binary(losextractor losextractor.cpp)
    endif()
endif()