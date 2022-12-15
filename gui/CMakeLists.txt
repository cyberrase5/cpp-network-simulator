cmake_minimum_required(VERSION 3.5)

project(network_simulator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    packetitem.h
    nodeitem.h
    nodes.h
    packet.h
    client.cpp
    router.cpp
    server.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(network_simulator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Resources.qrc
    )
else()
    add_executable(network_simulator
        ${PROJECT_SOURCES}
        Resources.qrc
    )
endif()

target_link_libraries(network_simulator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)



if(QT_VERSION_MAJOR EQUAL 6)
    set_target_properties(network_simulator PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )

    install(TARGETS network_simulator
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    qt_finalize_executable(network_simulator)
endif()
