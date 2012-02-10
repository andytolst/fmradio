/*
 *  fmradio-backend - backend for FM radio receiver
 *  Copyright (C) 2012 Andrey Kozhanov <andy.tolst@gmail.com>
 *
 *  This application is based on fmrxd - a daemon to enable and multiplex access to the N950/N9 radio tuner
 *  Copyright (C) 2011 Javier S. Pedro <maemo@javispedro.com>
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

#ifndef TUNERENGINE_H
#define TUNERENGINE_H

#include <QObject>
#include "playbackthread.h"
#include <dbus/fmradioadaptor.h>
#include <QSystemAlignedTimer>

QTM_USE_NAMESPACE

namespace ResourcePolicy
{
    class ResourceSet;
}

class V4LTuner;

class TunerEngine : public QObject
{
    Q_OBJECT
public:
    explicit TunerEngine(QObject *parent = 0);
    ~TunerEngine();

private slots:
    void start();
    void stop();
    void exit();
    void tuneToFreq(double freq);
    void scan(bool forward);

signals:
    void tuned(double freq, uint signal);
    void signalChanged(uint signal, bool stereo);

private slots:
    void updateSignal();
    void playbackStarted();
    void disableBt();

private:
    bool configure_bt_muxer(bool on);

private:
    V4LTuner* m_tuner;
    PlaybackThread m_playThread;
    FMRadioAdaptor m_adaptor;

    QSystemAlignedTimer m_signalTimer;
    uint m_signalLevel;
    bool m_started;
    double m_freq;
    bool m_waitingForBt;
    bool m_needToExit;
};

#endif // TUNERENGINE_H
