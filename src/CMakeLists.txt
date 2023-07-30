#
# SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#

add_library(MpvQt)
add_library(MpvQt::MpvQt ALIAS MpvQt)

set_target_properties(MpvQt PROPERTIES
    VERSION     ${MPVQT_VERSION}
    SOVERSION   ${MPVQT_SOVERSION}
    EXPORT_NAME MpvQt
)

target_sources(MpvQt
    PRIVATE
        mpvabstractitem.h mpvabstractitem.cpp
        mpvcontroller.h mpvcontroller.cpp
        mpvrenderer.h mpvrenderer.cpp
)

ecm_generate_export_header(MpvQt
    BASE_NAME MpvQt
    VERSION 1.0.0
    USE_VERSION_HEADER
)

target_include_directories(MpvQt
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/MpvQt>"
    PRIVATE ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

target_link_libraries(MpvQt
    PRIVATE
        Qt${QT_MAJOR_VERSION}::Quick
        Libmpv::Libmpv
)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    if (QT_MAJOR_VERSION STREQUAL "5")
        target_link_libraries(MpvQt PRIVATE
            Qt5::X11Extras
        )
    endif ()
endif ()

ecm_generate_headers(MpvQt_HEADERS
    HEADER_NAMES MpvAbstractItem
    REQUIRED_HEADERS MpvQt_HEADERS
)

install(TARGETS MpvQt EXPORT MpvQtTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/mpvqt_export.h
  ${MpvQt_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MpvQt COMPONENT Devel
)

install(FILES
    "${CMAKE_SOURCE_DIR}/MpvQtConfig.cmake"
    "${CMAKE_SOURCE_DIR}/MpvQtConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT MpvQtTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE MpvQtTargets.cmake NAMESPACE MpvQt:: )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/mpvqt_version.h"
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MpvQt COMPONENT Devel
)
