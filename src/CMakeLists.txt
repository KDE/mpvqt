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
)

target_link_libraries(MpvQt
    PUBLIC
        Qt6::Quick
        Libmpv::Libmpv
)

ecm_generate_headers(MpvQt_HEADERS
    HEADER_NAMES MpvAbstractItem
                 MpvController
    REQUIRED_HEADERS MpvQt_HEADERS
)

install(TARGETS MpvQt EXPORT MpvQtTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mpvqt_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/mpvqt_version.h
    ${MpvQt_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MpvQt COMPONENT Devel
)

install(FILES
    "${PROJECT_BINARY_DIR}/MpvQtConfig.cmake"
    "${PROJECT_BINARY_DIR}/MpvQtConfigVersion.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/FindLibmpv.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT MpvQtTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE MpvQtTargets.cmake NAMESPACE MpvQt:: )
