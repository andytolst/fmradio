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
    property double value
    property bool powered: false

    width: image100.width * 5;
    height: image100.height;

    NixieTubeDigit {
        id: image100
        x: 0
        anchors.top : parent.top
        value: (parent.value/100)%10 - 0.5
        isEmpty: value === 0
        powered: parent.powered
    }

    NixieTubeDigit {
        id: image10
        anchors.top : parent.top
        anchors.left: image100.right
        value: (parent.value/10)%10 - 0.5
        powered: parent.powered
    }

    NixieTubeDigit {
        id: image1
        anchors.top : parent.top
        anchors.left: image10.right
        value: parent.value%10 - 0.5
        powered: parent.powered
    }

    NixieTubeDigit {
        id: imageDot
        anchors.top : parent.top
        anchors.left: image1.right
        isDot: true
        powered: parent.powered
    }

    NixieTubeDigit {
        id: imageDec
        anchors.top : parent.top
        anchors.left: imageDot.right
        value: (parent.value.toFixed(1)*10)%10
        powered: parent.powered
    }
}
