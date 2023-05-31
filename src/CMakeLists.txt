cmake_minimum_required(VERSION 3.5)

project(qtLoadView LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

#寻找进程间通信用的库
find_package(ZeroMQ REQUIRED)
include_directories(${ZeroMQ_INCLUDE_DIR})

if(ANDROID)
  add_library(qtLoadView SHARED
    main.cpp
    mainwindow.cpp
    editorIdMapper.cpp
    topButtonBar.cpp
    centerWidget.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(qtLoadView
    main.cpp
    mainwindow.cpp
    editorIdMapper.cpp
    topButtonBar.cpp
    centerWidget.cpp
    mainwindow.h
    mainwindow.ui
  )
endif()

target_link_libraries(qtLoadView PRIVATE ${ZeroMQ_LIBRARY} Qt5::Widgets)
