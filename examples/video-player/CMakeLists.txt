#
# SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.15)

project(videoplayer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 COMPONENTS Core Quick REQUIRED)

find_package(MpvQt)

qt_standard_project_setup(REQUIRES 6.5)
qt_add_executable(videoplayer)

qt_policy(SET QTP0001 NEW)
qt_add_qml_module(videoplayer
    URI com.example.mpvqt
    QML_FILES
        Main.qml
)

target_sources(videoplayer
    PRIVATE
        main.cpp
        mpvitem.h mpvitem.cpp
        mpvproperties.h
)

target_link_libraries(videoplayer
    PRIVATE
        Qt6::Core
        Qt6::Quick
        MpvQt::MpvQt
)

install(TARGETS videoplayer DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
