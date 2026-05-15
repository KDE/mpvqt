#
# SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_library(MpvQt)
add_library(MpvQt::MpvQt ALIAS MpvQt)

set_target_properties(MpvQt PROPERTIES
    VERSION     ${MPVQT_VERSION}
    SOVERSION   ${MPVQT_SOVERSION}
    EXPORT_NAME MpvQt
)

if(${ECM_VERSION} VERSION_GREATER_EQUAL "6.27")
    qt_extract_metatypes(MpvQt OUTPUT_FILES METATYPES_FILE)
    get_filename_component(METATYPES_FILE_NAME ${METATYPES_FILE} NAME)
    target_sources(MpvQt
        INTERFACE
            $<INSTALL_INTERFACE:${KDE_INSTALL_QTMETATYPESDIR}/${METATYPES_FILE_NAME}>
    )
endif()

target_sources(MpvQt
    PRIVATE
        mpvabstractitem.h mpvabstractitem.cpp
        mpvcontroller.h mpvcontroller.cpp
        mpvrenderer.h mpvrenderer.cpp
)

ecm_generate_export_header(MpvQt
    BASE_NAME MpvQt
    VERSION ${PROJECT_VERSION}
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
if(${ECM_VERSION} VERSION_GREATER_EQUAL "6.27")
    install(FILES ${METATYPES_FILE} DESTINATION ${KDE_INSTALL_QTMETATYPESDIR})
endif()

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
