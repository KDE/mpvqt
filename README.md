<!--
SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>

SPDX-License-Identifier: CC-BY-SA-4.0
-->

MpvQt is a [libmpv](https://github.com/mpv-player/mpv/) wrapper for Qt Quick 2/Qml.

## How to use

- Create a class extending `MpvAbstractItem` (check the [mpvitem.h](examples/video-player/mpvitem.h)/[mpvitem.cpp](examples/video-player/mpvitem.cpp) files in the example)
- Register the class `qmlRegisterType<ClassName>("com.example.mpv", 1, 0, "NameUsedInQml");`
- In your qml file import mpv `import com.example.mpv 1.0`
- Then create an instance `NameUsedInQml {}` (check the [Main.qml](examples/video-player/Main.qml) file in the example)
