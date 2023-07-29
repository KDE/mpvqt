MpvQt is a [libmpv](https://github.com/mpv-player/mpv/) wrapper for Qt Quick 2/Qml.

## How to use

- Create a class extending `MpvAbstractItem` (check the [mpvitem.h](https://invent.kde.org/georgefb/mpvqt/-/blob/master/examples/video-player/mpvitem.h)/[mpvitem.cpp](https://invent.kde.org/georgefb/mpvqt/-/blob/master/examples/video-player/mpvitem.cpp) files in the example)
- Register the class `qmlRegisterType<ClassName>("com.example.mpv", 1, 0, "NameUsedInQml");`
- In your qml file import mpv `import com.example.mpv 1.0`
- Then create an instance `NameUsedInQml {}` (check the [Main.qml](https://invent.kde.org/georgefb/mpvqt/-/blob/master/examples/video-player/Main.qml) file in the example)
