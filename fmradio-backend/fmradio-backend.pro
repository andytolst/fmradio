#
#  fmradio-backend - backend for FM radio receiver
#  Copyright (C) 2012 Andrey Kozhanov <andy.tolst@gmail.com>
#
#  This application is based on fmrxd - a daemon to enable and multiplex access to the N950/N9 radio tuner
#  Copyright (C) 2011 Javier S. Pedro <maemo@javispedro.com>
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

TARGET = fmradio-backend
QT-=gui
QT+=dbus

CONFIG += link_pkgconfig mobility
PKGCONFIG +=  alsa bluez

MOBILITY += multimedia systeminfo

HEADERS += \
    tunermodel.h \
    tunerengine.h \
    v4ltuner.h \
    playbackthread.h \
    playbackcontrol.h \
    dbus/fmradioadaptor.h

SOURCES += main.cpp \
    tunerengine.cpp \
    v4ltuner.cpp \
    playbackthread.cpp \
    playbackcontrol.cpp \
    dbus/fmradioadaptor.cpp

OTHER_FILES += com.akozhan.fmradio.service \
               ../debian/fmradio.aegis

target.path += /opt/fmradio/
INSTALLS += target

dbus_service.files   = com.akozhan.fmradio.service
dbus_service.path    = $$APP_INSTALL_ROOT/share/dbus-1/services
INSTALLS += dbus_service
