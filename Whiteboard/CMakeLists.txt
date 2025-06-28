cmake_minimum_required(VERSION 3.16)

project(Whiteboard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Network Widgets)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appWhiteboard
    main.cpp


    mygraphicsview.h mygraphicsview.cpp
    boardwidget.h boardwidget.cpp
    whiteboardserver.h whiteboardserver.cpp
    whiteboardclient.h whiteboardclient.cpp
    whiteboardcanvas.h whiteboardcanvas.cpp
    penciltool.h penciltool.cpp
    cursortracker.h cursortracker.cpp
    tool.h
)

set_target_properties(appWhiteboard PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appWhiteboard
    PRIVATE Qt6::Core Qt6::Network Qt6::Widgets
)

install(TARGETS appWhiteboard
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
