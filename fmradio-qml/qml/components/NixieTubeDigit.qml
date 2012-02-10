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

import QtQuick 1.1

Item {
    id: item1
    property int value: 0
    property bool powered: false
    property bool isDot: false
    property bool isEmpty: false
    state: "PoweredOff"

    width: 92
    height: 165

    onPoweredChanged: {
        fadeIn.stop();
        if(powered)
        {
            digitImageNew.source = isEmpty ? "" : isDot ? "nixie_dot.png" : "nixie_digit_" + value + ".png";
        }

        state = powered ? "PoweredOn" : "PoweredOff"
    }

    onIsEmptyChanged: {
            digitImageNew.source = isEmpty ? "" : isDot ? "nixie_dot.png" : "nixie_digit_" + value + ".png";
    }

    onValueChanged: {
        if(powered)
        {
            digitImageOld.source = digitImageNew.source;
            fadeIn.start();
            fadeOut.start();

            digitImageNew.source = isEmpty ? "" : isDot ? "nixie_dot.png" : "nixie_digit_" + value + ".png";
        }
    }

    states: [
        State {
        name: "PoweredOff"

        PropertyChanges { target: tubeLight; opacity: 0 }
        PropertyChanges { target: digitImageNew; opacity: 0 }
        PropertyChanges { target: digitImageOld; opacity: 0 }
        },

        State {
        name: "PoweredOn"

        PropertyChanges { target: tubeLight; opacity: 1 }
        PropertyChanges { target: digitImageNew; opacity: 1 }
        PropertyChanges { target: digitImageOld; opacity: 0 }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.Linear }
    }

    NumberAnimation { id: fadeIn; target: digitImageNew; property: "opacity"; from: 0; to: 1; easing.type: Easing.Linear; duration: 500 }
    NumberAnimation { id: fadeOut; target: digitImageOld; property: "opacity"; from: 1; to: 0; easing.type: Easing.Linear; duration: 500 }

    Image {
        id: tubeImage
        z: 1
        anchors.fill: parent
        smooth: true
        fillMode: Image.PreserveAspectFit
        source: "nixie_tube_empty.png"
        opacity: 0.7
    }

    Image {
        id: tubeLight
        z: 2
        anchors.fill: parent
        smooth: true
        fillMode: Image.PreserveAspectFit
        source: "nixie_tube_light.png"
        opacity: 0
    }

    Image {
        id: digitImageNew
        height: 130
        sourceSize.width: 92
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        z: 3
        smooth: true
        fillMode: Image.PreserveAspectFit

        opacity: 0
    }

    Image {
        id: digitImageOld
        height: 130
        sourceSize.width: 92
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        z: 3
        smooth: true
        fillMode: Image.PreserveAspectFit

        opacity: 0
    }
}
