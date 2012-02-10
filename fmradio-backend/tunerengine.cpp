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

#include "tunerengine.h"
#include "v4ltuner.h"

#include <sys/ioctl.h>
#include <errno.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#define BT_DEV_ID 0

TunerEngine::TunerEngine(QObject *parent) :
    QObject(parent)
  , m_tuner(0)
  , m_adaptor(this)
  , m_signalTimer(this)
  , m_signalLevel(0)
  , m_started(false)
  , m_freq(0)
  , m_waitingForBt(false)
  , m_needToExit(false)
{
    if (QDBusConnection::sessionBus().registerService("com.akozhan.fmradio") &&
        QDBusConnection::sessionBus().registerObject("/com/akozhan/fmradio", this, QDBusConnection::ExportAdaptors))
    {
        qDebug() << "Service is registered";
    }

    m_tuner = new V4LTuner(this);
    connect(&m_signalTimer, SIGNAL(timeout()), this, SLOT(updateSignal()));

    connect(&m_playThread, SIGNAL(playbackStarted()), this, SLOT(playbackStarted()));
    qDebug() << "Backend started";
}

TunerEngine::~TunerEngine()
{
    qDebug() << "TunerEngine::~TunerEngine()";
}

void TunerEngine::disableBt()
{
    int dd;

    qDebug() << "try to turn off BT";

    dd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (dd == -1) {
        qDebug() << "Failed to open HCI socket: %s" << strerror(errno);
        return;
    }

    if (ioctl(dd, HCIDEVDOWN, BT_DEV_ID) < 0 && errno != EALREADY) {
        qDebug() << "Failed to turn Bluetooth device off: %s" << strerror(errno);
    }

    close(dd);

    m_waitingForBt = false;

    if(m_needToExit)
    {
        QCoreApplication::quit();
    }
}

bool TunerEngine::configure_bt_muxer(bool on)
{
    struct sockaddr_hci a;
    int dd;
    bool was_up;

    /* The magic numbers: */
    const short ocf = 0x00;
    const short ogf = 0x3f;
    char cmd[4] = { 0xf3, 0x88, 0x01, on ? 0x02 : 0x01 };

    /* Open HCI socket. */
    dd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (dd == -1) {
        qDebug() << "Failed to open HCI socket: %s" << strerror(errno);
        return false;
    }

    /* Try to turn the specificied BT device on. */
    if (ioctl(dd, HCIDEVUP, BT_DEV_ID) < 0) {
        if (errno == EALREADY) {
            /* The BT device was found to be already on; no need to turn it off later. */
            was_up = true;
        } else {
           qDebug() << "Failed to turn Bluetooth device on: %s" << strerror(errno);
            goto failed;
        }
    } else {
        /* Turned on succesfully. */
        qDebug() << "Turned on BT";
        was_up = false;
    }

    /* Bind HCI socket to this device. */
    memset(&a, 0, sizeof(a));
    a.hci_family = AF_BLUETOOTH;
    a.hci_dev = BT_DEV_ID;
    if (bind(dd, (struct sockaddr *) &a, sizeof(a)) < 0) {
        qDebug() << "Failed to bind HCI socket: %s" << strerror(errno);
        goto failed;
    }

    /* Now send the magic command. */
    if (hci_send_cmd(dd, ogf, ocf, sizeof(cmd), cmd) < 0) {
        qDebug() << "Failed to send magic HCI command: %s" << strerror(errno);
        goto failed;
    }

    qDebug("Muxer config changed to 0x%x", cmd[3]);

    close(dd);

    if (!was_up) {
        /* Try to shut down bluetooth if we enabled it. */
        /* Give ample time for everything to be setup though. */
        QTimer::singleShot(500, this, SLOT(disableBt()));
        m_waitingForBt = true;
    }

    return true;

failed:
    close(dd);
    return false;
}

void TunerEngine::start()
{
    configure_bt_muxer(true);
    m_playThread.start(QThread::TimeCriticalPriority);
    m_signalTimer.start(2, 7);

    qDebug() << "Playback started";
}

void TunerEngine::playbackStarted()
{
    m_started = true;

    if(m_freq == 0)
    {
        bool stereo;
        m_signalLevel = m_tuner->signalLevel(stereo);
        emit tuned(m_tuner->currentFreq(), m_signalLevel);
    }
    else
    {
        tuneToFreq(m_freq);
    }
}

void TunerEngine::stop()
{
    m_started = false;
    m_signalTimer.stop();
    m_playThread.quit();
    configure_bt_muxer(false);

    emit signalChanged(0, false);

    qDebug() << "Playback stopped";
}

void TunerEngine::exit()
{
    stop();
    m_playThread.wait(500);

    if(m_waitingForBt)
    {
        m_needToExit = true;
    }
    else
    {
        QCoreApplication::quit();
    }
}

void TunerEngine::tuneToFreq(double freq)
{
    if(m_started)
    {
        m_tuner->tuneToFreq(freq);
        bool stereo;
        m_signalLevel = m_tuner->signalLevel(stereo);
        emit tuned(freq, m_signalLevel);
    }
    else
    {
        m_freq = freq;
    }
}

void TunerEngine::scan(bool forward)
{
    m_tuner->scan(forward);
    bool stereo;
    m_signalLevel = m_tuner->signalLevel(stereo);
    emit tuned(m_tuner->currentFreq(), m_signalLevel);
}

void TunerEngine::updateSignal()
{
    bool stereo;
    uint signal = m_tuner->signalLevel(stereo);

    if(m_signalLevel != signal)
    {
        m_signalLevel = signal;
        emit signalChanged(signal, stereo);
    }
}
