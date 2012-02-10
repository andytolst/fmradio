/*
 *  fmradio-qml - UI for FM radio
 *  Copyright (C) 2012 Andrey Kozhanov <andy.tolst@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

import QtQuick 1.0

Item {
    id: meter
    property int value: 0
    property bool powered: false
    width: 160
    height: 90
    smooth: true
    clip: true
    state: "PoweredOff"

    onPoweredChanged: {
        state = powered ? "PoweredOn" : "PoweredOff"
    }

    states: [
        State {
        name: "PoweredOff"

        PropertyChanges { target: light; opacity: 0 }
        },

        State {
        name: "PoweredOn"

        PropertyChanges { target: light; opacity: 1 }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.Linear }
    }

    Image {
        id: background
        anchors.rightMargin: 9
        anchors.leftMargin: 8
        anchors.bottomMargin: 5
        anchors.topMargin: 5
        z: -1

        smooth: true
        fillMode: Image.PreserveAspectFit
        source: "meter_background.png"

        anchors.fill: parent
        opacity: 1
    }

    Image {
        id: light

        smooth: true
        fillMode: Image.PreserveAspectFit
        source: "meter_light.png"

        anchors.fill: parent
        opacity: 0
    }

    Image {
        id: foreground
        z: 1

        smooth: true
        fillMode: Image.PreserveAspectFit
        source: "meter_foreground.png"

        anchors.fill: parent
        opacity: 1
    }

    Item {
        id: clippingArea
        clip: true
        anchors.rightMargin: 8
        anchors.leftMargin: 8
        anchors.bottomMargin: 6
        anchors.topMargin: 6

        anchors.fill: parent

        Image {
            id: arrowImage;
            x: 70
            y: 18
            height: meter.height - 2
            z: 1
            clip: true
            rotation:(value * 80/100) - 40
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -31

            transformOrigin: "Bottom"

            smooth: true
            fillMode: Image.PreserveAspectFit
            source: "meter_arrow.png"

            Behavior on rotation {
                SpringAnimation {
                    spring: 1.4
                    damping: 0.15
                }
            }
        }
    }
}
