<!--
SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>

SPDX-License-Identifier: CC0-1.0
-->

MpvQt is a [libmpv](https://github.com/mpv-player/mpv/) wrapper for Qt Quick 2/Qml.

## How to use

- Create a class extending `MpvAbstractItem`

```c++
class MpvItem : public MpvAbstractItem
{
    Q_OBJECT
    QML_ELEMENT
    // ...
}
```
- Add the class to a qml module

```cmake
qt_add_qml_module(videoplayer
    URI com.example.mpvqt
    QML_FILES
        Main.qml
)

target_sources(videoplayer
    PRIVATE
        mpvitem.h mpvitem.cpp
)
```
or
```cmake
qt_add_qml_module(videoplayer
    URI com.example.mpvqt
    QML_FILES
        Main.qml
    SOURCES
        mpvitem.h mpvitem.cpp
)
```

- In your qml file import mpv `import com.example.mpvqt`
- Then create an instance `MpvItem {}`, `MpvItem` is the class name extending MpvAbstractItem,
 if you want to use another name replace `QML_ELEMENT` with `QML_NAMED_ELEMENT(VideoPlayer)`

## Config file
MpvQt loads a config file located at `<config_folder>/mpvqt/mpvqt.conf`,
the file has to be manually created and is a regular mpv config file.
The settings apply to all applications using MpvQt,
applications can override the properties set in MpvQt's config file.

### Applications using MpvQt
 - [Haruna](https://invent.kde.org/multimedia/haruna)
 - [Jellyfin Desktop](https://github.com/jellyfin/jellyfin-desktop)
 - [PlasmaTube](https://invent.kde.org/multimedia/plasmatube)
