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
    id: container
    height: 854; width: 480

    MouseArea {
        anchors.fill: parent
    }

    Image {
        id: backButton
        width: 91
        height: 93
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        smooth: true
        source: "back_button.png"

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                container.opacity = 0
                container.height = 0
            }
        }
    }

    Text {
        id: text3
        x: 96
        y: 62
        width: 374
        height: 30
        color: "#d1ce04"
        text: " version 0.0.1"
        font.pixelSize: 27
    }

    Text {
            id: text2
            x: 5
            y: 103
            width: 450
            height: 31
            color: "#57cc2c"
            text: "&copy; Andrey Kozhanov <a href=\"mailto:andy.tolst@gmail.com\">andy.tolst@gmail.com</a>"
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 23
            onLinkActivated: {
                console.log("Link clicked:"+ link);
                Qt.openUrlExternally(link);
            }
        }

    Text {
            id: text1
            x: 96
            y: 5
            width: 344
            height: 52
            color: "#ffd500"
            text: "FM Radio"
            font.bold: true
            font.pixelSize: 49
        }

    Rectangle {
        id: background
        x: 0
        y: 0
        opacity: 0.8
        color: "black"
        z: -1
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
    }

    Flickable {
        x: 10
        y: 155
        width: 460
        height: 689
        clip: true
        anchors.topMargin: 155
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        contentHeight: helpText.height
        contentWidth: helpText.width
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent

        Text {
            id: helpText
            width: 450
            color: "#ffff5f"
            text: "<html><style>a { color: green; } a:visited { color: red; } a:active {color: yellow;} </style><body>
                   <b><u>Usage guide: </u></b><br>
                   Please insert headset for better signal quality, it acts as FM antenna.<br>
                   Nixie tube digits indicate current frequency<br>
                   Scrolling tuner scale allows to set frequency manually<br>
                   Single tap on (&lt;&lt;) and (&gt;&gt;) butons allows to adjust frequency. <br>
                   Long tap on (&lt;&lt;) and (&gt;&gt;) butons performs scan in corresponding direction.<br>
                   Single tap on favorite station buton sets frequency. <br>
                   Long tap on favorite station resets button's value to current frequency. <br>
                   <br>
                   This software is open source <br>
                   Source code and license details of this application is available on
                   GitHub <a href=\"https:\/\/github.com\/andytolst\/fmradio\">https://github.com/andytolst/fmradio</a>
                   </body> </html> "
            font.pointSize: 24
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            textFormat: Text.RichText

            onLinkActivated: {
                console.log("Link clicked:"+ link);
                Qt.openUrlExternally(link);
            }
        }
    }




}
