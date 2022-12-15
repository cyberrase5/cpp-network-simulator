#TODOS
The waiting until calling receive in send() and in packet.h needs to either be done in a separate thread or with a signal through QTimer or something so that the gui and animations don't get blocked.

To create a handler that calls send() when and where appropriate.


# Requirements with Qt Creator
There shouldn't be any other requirements if you use Qt Creator.

On windows go to https://www.qt.io/download and find an installer if you just want a binary. You probably want the open source version. Alternatively, you can compile from source. Instructions here: https://doc.qt.io/qt-6/build-sources.html

On macOS you could use homebrew to install Qt Creator. Run `brew install --cask qt-creator`

On linux just use your package manager of choice. 

# Requirements without Qt Creator
## A compiler
On windows you should probably just install MinGW. Using other compilers through WSL 2 is probably possible as well.

On macOS, you could use clang. Install Xcode and its command line tools.

## Qt
This application was built with Qt version 6.4.0

On windows go to https://www.qt.io/download and find an installer if you just want a binary. You probably want the open source version. Alternatively, you can compile from source. Instructions here: https://doc.qt.io/qt-6/build-sources.html

On macOS, you could use homebrew to install qt. Run `brew install qt`

On linux just use your package manager of choice. 

## cmake
On windows, follow instructions from https://cmake.org/install/

On macOS, you could use homebrew to install cmake. Run `brew install cmake`

On linux just use your package manager of choice. 

## make


# Building the gui without Qt Creator
Run cmake on this directory and make. If you don't mind where build files and the application binary are placed run \
`cmake .` \
`make`

If you want build files and the application binary somewhere else, navigate to your desired directory and run \
`cmake $PATH_TO_GUI`, where `$PATH_TO_GUI` is the path to this directory. \
`make`

Now you should have a runnable binary

# Building with Qt Creator
Open the directory in Qt Creator, build and run.

# Source content

## MainWindow
MainWindow contains the source code for the main window of the program. mainwindow.ui holds the layout of the window.

## NodeItem
Is a visual representation of nodes and their connections.

## PacketItem
Describes the animation item that moves when packets are sent between nodes.

## Resources.qrc
Is a container that holds and links resources external to the source code.

## main.cpp
Entry point of the program.
