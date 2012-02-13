#
#  fmradio-qml - UI for FM radio
#  Copyright (C) 2012 Andrey Kozhanov <andy.tolst@gmail.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

TARGET = fmradio
QT += declarative dbus

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable link_pkgconfig
PKGCONFIG += contextsubscriber-1.0

HEADERS += \
    tunermodel.h \
    dbus/fmradiointerface.h

SOURCES += main.cpp \
    tunermodel.cpp \
    dbus/fmradiointerface.cpp

!isEmpty(MEEGO_VERSION_MAJOR) {
DEFINES+=MEEGO_OS
}

RESOURCES += resources.qrc

OTHER_FILES += \
    qml/MainView.qml \
    qml/MainWindowDevice.qml \
    qml/MainWindowDesktop.qml \
    qml/components/TunerScale.qml \
    qml/components/AnalogMeter.qml \
    qml/components/Button.qml \
    qml/components/DigitalTuner.qml \
    qml/components/NixieTubeDigit.qml \
    qml/components/PowerSwitch.qml \
    qml/components/SeekButton.qml \
    fmradio.conf \
    qml/components/HelpButton.qml \
    qml/components/HelpScreen.qml \
    qml/components/SpeakerButton.qml

target.path += /opt/fmradio/
INSTALLS += target

desktop_file.target  = desktop_file
desktop_file.files   = fmradio.desktop
desktop_file.path    = $$APP_INSTALL_ROOT/share/applications
INSTALLS += desktop_file

desktop_icon.files   = fmradio80.png
desktop_icon.path    = $$APP_INSTALL_ROOT/share/icons/hicolor/80x80/apps
INSTALLS += desktop_icon

resource_conf.files   = fmradio.conf
resource_conf.path    = $$APP_INSTALL_ROOT/share/policy/etc/syspart.conf.d
INSTALLS += resource_conf



