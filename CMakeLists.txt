#
# SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.15)

project(MpvQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 5.89 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMGenerateHeaders)
include(ECMGenerateExportHeader)

include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.hpp *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(KDEGitCommitHooks)
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

include(ECMSetupVersion)
ecm_setup_version(PROJECT
    VARIABLE_PREFIX MPVQT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/mpvqt_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/MpvQtConfigVersion.cmake"
    SOVERSION 1
)

set(X11_PLATFORMS Linux FreeBSD GNU/kFreeBSD NetBSD OpenBSD)

#
# FIND PACKAGES
#

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick)

find_package(Qt${QT_MAJOR_VERSION}Quick)
set_package_properties(Qt${QT_MAJOR_VERSION}Quick PROPERTIES TYPE REQUIRED)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS X11Extras)

    if (QT_MAJOR_VERSION STREQUAL "5")
        find_package(Qt5X11Extras)
        set_package_properties(Qt5X11Extras PROPERTIES TYPE REQUIRED)
    endif ()
endif ()

find_package(Libmpv)
set_package_properties(Libmpv PROPERTIES TYPE REQUIRED
    URL "https://mpv.io")

find_package(YouTubeDl)
set_package_properties(YouTubeDl PROPERTIES TYPE RUNTIME)

find_package(Ytdlp)
set_package_properties(Ytdlp PROPERTIES TYPE RUNTIME)

#
# END FIND PACKAGES
#

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/MpvQt")

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/MpvQtConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MpvQtConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(src)

option(BUILD_EXAMPLES "Whether to build the examples" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples/video-player)
endif()
