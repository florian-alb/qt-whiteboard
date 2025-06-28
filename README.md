# QT Whiteboard

A proof-of-concept interactive online whiteboard application built with Qt 6.9, implementing drawing and text features with collaborative session support.

## Features

- **Text Writing**: Add, edit, move, and style text elements on the whiteboard
- **Session Management**: Host or join collaborative whiteboard sessions
- **Real-time Collaboration**: Changes synchronize across all connected clients
- **User Management**: View connected users with admin privileges

## Architecture

The application follows the Presentation-Business-Data (PBD) architecture model:

- **Data Layer**: Contains data models like WhiteboardElement, TextItem, User, Session, WhiteboardModel
- **Business Layer**: Contains controllers like WhiteboardController, SessionController, SyncManager
- **Presentation Layer**: Contains UI components like MainWindow, CanvasView, Toolbar, StylePanel, etc.
- **Networking**: Uses QTcpServer and QTcpSocket for TCP communication

## Building the Application

### Prerequisites

- Qt 6.9 or later
- C++17 compatible compiler
- CMake 3.16 or later

### Build Steps

1. Clone the repository
2. Navigate to the project directory
3. Create a build directory:
   ```
   mkdir build
   cd build
   ```
4. Run CMake:
   ```
   cmake ..
   ```
5. Build the project:
   ```
   cmake --build .
   ```

## Usage

1. Launch the application
2. Choose to create a new session or join an existing one
3. If creating a session:
   - Enter a session name and your username
   - Share the generated session code with others
4. If joining a session:
   - Enter the session code and your username
5. Use the toolbar to select tools:
   - Select tool: Select and move existing elements
   - Text tool: Add text elements to the whiteboard
6. Double-click on text elements to edit them
7. Use the style panel to change font properties and colors

## Project Structure

```
Whiteboard/
├── main.cpp
├── whiteboardapp.cpp
├── whiteboardapp.h
├── data/
│   ├── whiteboardelement.cpp
│   ├── whiteboardelement.h
│   ├── textitem.cpp
│   ├── textitem.h
│   ├── user.cpp
│   ├── user.h
│   ├── session.cpp
│   ├── session.h
│   ├── whiteboardmodel.cpp
│   └── whiteboardmodel.h
├── business/
│   ├── whiteboardcontroller.cpp
│   ├── whiteboardcontroller.h
│   ├── sessioncontroller.cpp
│   ├── sessioncontroller.h
│   ├── syncmanager.cpp
│   └── syncmanager.h
├── networking/
│   ├── networkservice.cpp
│   └── networkservice.h
└── presentation/
    ├── mainwindow.cpp
    ├── mainwindow.h
    ├── canvasview.cpp
    ├── canvasview.h
    ├── toolbar.cpp
    ├── toolbar.h
    ├── stylepanel.cpp
    ├── stylepanel.h
    ├── sessiondialog.cpp
    ├── sessiondialog.h
    ├── userlistpanel.cpp
    └── userlistpanel.h
```

## Future Enhancements

Future versions could include:
- Additional drawing tools (lines, shapes, freehand drawing)
- Undo/redo functionality
- Session persistence
- User authentication
- Enhanced styling options
