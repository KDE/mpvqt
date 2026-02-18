/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import com.example.mpvqt

Window {
    id: window

    width: 1000
    height: 600
    visible: true
    title: "multiple videos"

    function addTab(videoFile: string) {
        const index = tabBar.count
        const tabPage = mpvComponent.createObject(tabLayout, {index: index, videoFile: videoFile})
        var tab = tabComponent.createObject(tabBar, {index: index, mpvItem: tabPage})

        tabBar.currentIndex = index
    }

    Component.onCompleted: {
        // window.addTab("/path/to/video/file1.mkv")
        // window.addTab("/path/to/video/file2.mkv")
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: tabBar

            background: Item {}

            Layout.fillWidth: true
            Layout.preferredHeight: 50

        }

        StackLayout {
            id: tabLayout

            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: tabBar.currentIndex
        }
    }

    Component {
        id: tabComponent

        TabButton {
            id: tabDelegate

            required property int index
            required property MpvItem mpvItem

            implicitHeight: parent.height
            text: `Tab ${index}`
            width: 190

            TapHandler {
                acceptedButtons: Qt.MiddleButton
                onSingleTapped: {
                    tabDelegate.destroy()
                    tabDelegate.mpvItem.destroy()
                }
            }
        }
    }

    Component {
        id: mpvComponent

        MpvItem {
            id: mpv

            required property int index
            required property string videoFile

            Layout.fillWidth: true
            Layout.fillHeight: true

            // do not load file on Component.onCompleted
            onReady: loadFile([videoFile])
            volume: index === 0 ? 80 : 0

            Rectangle {
                width: mpv.width
                height: 40
                color: Qt.alpha("#333", 0.5)
                anchors.bottom: mpv.bottom

                RowLayout {
                    anchors.fill: parent

                    Text {
                        text: `${mpv.formattedPosition} / ${mpv.formattedDuration}`
                        color: "#fff"
                        Layout.leftMargin: 10
                    }

                    Slider {
                        id: slider
                        from: 0
                        to: mpv.duration
                        value: mpv.position
                        onValueChanged: mpv.position = value

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.rightMargin: 10
                    }
                }
            }
        }
    }

}
