#
# SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.15)

project(videoplayer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

find_package(MpvQt)

add_executable(videoplayer)

target_sources(videoplayer
    PRIVATE
        main.cpp
        mpvitem.h mpvitem.cpp
        mpvproperties.h
        qml.qrc
)

target_link_libraries(videoplayer
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Quick
        MpvQt::MpvQt
)

install(TARGETS videoplayer DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
