#
# SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#

find_program(Ytdlp_EXE yt-dlp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ytdlp
    FOUND_VAR
        Ytdlp_FOUND
    REQUIRED_VARS
        Ytdlp_EXE
)
