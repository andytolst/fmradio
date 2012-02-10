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
    id: tunerWidget
    width: 466
    height: 184

    property real value: 88
    property real tuningValue: 88
    property real minValue: 88
    property real maxValue: 108
    property bool moving: tuner.moving
    property bool powered: false
    function tuneToFreq(val) {
        if(val >= minValue && val <= maxValue) {
            tuneAnimation.stop()
            tuneAnimation.to = (tuner.contentWidth - tuner.width) * (val - minValue) / (maxValue - minValue)
            tuneAnimation.duration = Math.abs(value - val) > 0.3 ? 600 : 50
            tuneAnimation.easing.type = Math.abs(value - val) > 0.3 ? Easing.InOutQuint : Easing.Linear
            tuneAnimation.start()
            value = val;
        }
    }

    onPoweredChanged: {
        if(powered)
        {
            turnedOffCover.opacity = 0;
        }
        else
        {
            turnedOffCover.opacity = 0.6;
        }
    }

    Flickable {
        id: tuner
        property real value: 88
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.bottomMargin: 3
        anchors.topMargin: 3
        smooth: true

        anchors.fill: parent

        anchors.horizontalCenterOffset: 3
        anchors.horizontalCenter: parent.horizontalCenter
        flickableDirection: Flickable.HorizontalFlick
        contentWidth: scaleImage.width;
        contentHeight: scaleImage.height;
        clip: true

        Image {
            id: scaleImage
            x: 0
            y: 0
            width: 1700
            height: 180
            z:1
            smooth: true
            sourceSize.height: 0
            sourceSize.width: 1700
            clip: true
            fillMode: Image.TileHorizontally
            source: "radio_scale.png"
        }

        Rectangle {
            id: turnedOffCover
            x:0
            y:0
            z:2
            width: scaleImage.width
            height: scaleImage.height
            smooth: true
            color: "black"
            opacity: 0.600

            Behavior on opacity {
                NumberAnimation { duration: 500 }
            }
        }

        onContentXChanged: {
            var tmpVal = minValue + (contentX/(contentWidth - tuner.width)) * (maxValue - minValue)
            if(tmpVal >= minValue && tmpVal <= maxValue)
            {
                tunerWidget.tuningValue = tmpVal.toFixed(1)
            }

            if(moving) {
                tunerWidget.value = tuningValue
            }
        }

        onMovementStarted: {
            tuneAnimation.stop()
        }

        NumberAnimation on contentX { id: tuneAnimation; easing.type: Easing.InOutQuint; duration: 600 }
    }

    Image {
        id: blackCover

        z: 3
        fillMode: Image.TileVertically
        opacity: 1
        smooth: true
        source: "black_cover.png"
        anchors.fill: tuner
    }

    Image {
        id: scalePointer
        width: 49
        height: 180
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.bottomMargin: 2
        anchors.topMargin: 2
        smooth: true
        sourceSize.height: 0
        sourceSize.width: 0
        z: 2
        opacity: 1
        fillMode: Image.PreserveAspectFit
        source: "scale_pointer.png"
    }

    BorderImage {
        id: border_image1
        opacity: 1.000
        smooth: true
        border.bottom: 5
        border.top: 5
        border.right: 5
        border.left: 5
        z: 5
        anchors.fill: parent
        source: "border.png"
    }
}
