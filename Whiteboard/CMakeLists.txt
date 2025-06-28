cmake_minimum_required(VERSION 3.16)

project(Whiteboard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Network Widgets)

qt_standard_project_setup(REQUIRES 6.8)

# Source files
set(SOURCES
    main.cpp
    whiteboardapp.cpp
    
    # Data layer
    data/whiteboardelement.cpp
    data/textitem.cpp
    data/user.cpp
    data/session.cpp
    data/whiteboardmodel.cpp
    
    # Business layer
    business/whiteboardcontroller.cpp
    business/sessioncontroller.cpp
    business/syncmanager.cpp
    
    # Networking
    networking/networkservice.cpp
    
    # Presentation layer
    presentation/mainwindow.cpp
    presentation/canvasview.cpp
    presentation/toolbar.cpp
    presentation/stylepanel.cpp
    presentation/sessiondialog.cpp
    presentation/userlistpanel.cpp
)

# Header files
set(HEADERS
    whiteboardapp.h
    
    # Data layer
    data/whiteboardelement.h
    data/textitem.h
    data/user.h
    data/session.h
    data/whiteboardmodel.h
    
    # Business layer
    business/whiteboardcontroller.h
    business/sessioncontroller.h
    business/syncmanager.h
    
    # Networking
    networking/networkservice.h
    
    # Presentation layer
    presentation/mainwindow.h
    presentation/canvasview.h
    presentation/toolbar.h
    presentation/stylepanel.h
    presentation/sessiondialog.h
    presentation/userlistpanel.h
)

qt_add_executable(appWhiteboard
    ${SOURCES}
    ${HEADERS}
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
