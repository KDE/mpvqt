<!--
SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>

SPDX-License-Identifier: CC0-1.0
-->

MpvQt is a [libmpv](https://github.com/mpv-player/mpv/) wrapper for Qt Quick 2/Qml.

## How to use

- Create a class extending `MpvAbstractItem` (check the [mpvitem.h](examples/video-player/mpvitem.h)/[mpvitem.cpp](examples/video-player/mpvitem.cpp) files in the example)
- Register the class `qmlRegisterType<ClassName>("com.example.mpv", 1, 0, "NameUsedInQml");`
- In your qml file import mpv `import com.example.mpv 1.0`
- Then create an instance `NameUsedInQml {}` (check the [Main.qml](examples/video-player/Main.qml) file in the example)

## Config file
MpvQt loads a config file located at `<config_folder>/mpvqt/mpvqt.conf`,
the file has to be manually created and is a regular mpv config file.
The settings apply to all applications using MpvQt,
applications can override the properties set in MpvQt's config file.

### Applications using MpvQt
 - [Haruna](https://invent.kde.org/multimedia/haruna)
 - [PlasmaTube](https://invent.kde.org/multimedia/plasmatube)
 - [Tokodon](https://invent.kde.org/network/tokodon)
