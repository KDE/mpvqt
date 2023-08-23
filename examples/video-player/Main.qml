/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Dialogs 1.0

import com.example.mpv 1.0

Window {
    width: 720
    height: 410
    visible: true
    title: mpv.mediaTitle || "press f key to select a file"

    MpvItem {
        id: mpv

        anchors.fill: parent

        // do not load file on Component.onCompleted
        // onReady: loadFile(["/path/to/video_file.mkv"])

        // the results of all the async methods calls are received by MpvItem::onAsyncReply
        onFileLoaded: commandAsync(["expand-text", "volume is ${volume}"], MpvItem.ExpandText);

        Rectangle {
            width: 200
            height: 40

            Text {
                text: `${mpv.formattedPosition} / ${mpv.formattedDuration}`
                anchors.centerIn: parent
            }
        }

        Shortcut {
            sequence: "space"
            onActivated: mpv.pause = !mpv.pause
        }

        Shortcut {
            sequence: "m"
            onActivated: mpv.setPropertyAsync(MpvProperties.Mute, !mpv.getProperty(MpvProperties.Mute))
        }

        Shortcut {
            sequence: "f"
            onActivated: fileDialog.open()
        }

        Shortcut {
            sequence: "escape"
            onActivated: Qt.quit()
        }
    }

    FileDialog {
        id: fileDialog

        title: "Select file"
        folder: shortcuts.movies
        onAccepted: {
            mpv.loadFile(fileDialog.fileUrls[0])
        }
    }
}
