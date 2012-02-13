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

#ifndef TUNERMODEL_H
#define TUNERMODEL_H

#include <QObject>
#include <QDBusInterface>
#include "dbus/fmradiointerface.h"

class ContextProperty;

class TunerModel : public QObject
{
    Q_OBJECT
public:
    explicit TunerModel(QObject *parent = 0);
    ~TunerModel();

signals:
    void scanCompleted(double freq);
    void ready();
    void signalChanged();
    void turnedOff();
    void speakerStateChanged();

public slots:
    void powerOn(bool on);
    void tuneToFreq(double freq);
    void scan(bool forward);
    void storeStation(int id, double value);
    double getStation(int id);

    uint signalLevel();
    bool isStereo();
    double currentFreq();

    void setLoudSpeaker(bool loud);
    bool isLoudSpeaker();

private slots:
    void slotOnTuned(double freq, uint signal);
    void slotOnSignalChanged(uint signal, bool stereo);
    void slotOnBackendClosed(int code, QProcess::ExitStatus status);
    void onSpeakerChanged();
private:
    enum TunerState {
        StateIdle,
        StateTuning,
        StateScanning
    };

    TunerState m_state;
    TunerState m_nextState;

    bool m_scanForward;

    bool m_isPowered;
    double m_currentFreq;
    uint m_signal;
    bool m_stereo;
    FMRadioInterface m_engine;

    QProcess m_backend;
    QSettings m_settings;
    QDBusInterface m_loudSpeakerIface;
    ContextProperty* m_loudSpeakerProperty;
    bool m_speakerEnabled;
};

#endif // TUNERMODEL_H
