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

#include "tunermodel.h"
#include <QDebug>
#include <QProcess>

TunerModel::TunerModel(QObject *parent) :
    QObject(parent)
  , m_state(StateIdle)
  , m_nextState(StateIdle)
  , m_scanForward(true)
  , m_signal(0)
  , m_stereo(false)
  , m_engine("com.akozhan.fmradio", "/com/akozhan/fmradio", QDBusConnection::sessionBus())
  , m_settings(QDir::homePath() + "/.config/fmradio/fmradio.conf", QSettings::IniFormat)
{
    connect(&m_engine, SIGNAL(tuned(double,uint)), this, SLOT(slotOnTuned(double,uint)));
    connect(&m_engine, SIGNAL(signalChanged(uint,bool)), this, SLOT(slotOnSignalChanged(uint,bool)));

    m_currentFreq = m_settings.value("lastFreq", 87.5).toDouble();
}

TunerModel::~TunerModel()
{
    qDebug() << "Terminating backend";

    m_settings.setValue("lastFreq", m_currentFreq);
    m_engine.exit().waitForFinished();
}

void TunerModel::storeStation(int id, double value)
{
    m_settings.setValue("station" + QString::number(id), value);
}

double TunerModel::getStation(int id)
{
    return m_settings.value("station" + QString::number(id)).toDouble();
}

void TunerModel::slotOnBackendClosed(int code, QProcess::ExitStatus status)
{
    qDebug() << "backend process finished" << code << status;

    emit turnedOff();
}

void TunerModel::powerOn(bool on)
{
    if(on)
    {
        m_state = StateScanning;
        m_nextState = StateIdle;
        m_engine.start();
    }
    else
    {
        m_engine.stop();
    }
}

void TunerModel::slotOnTuned(double freq, uint signal)
{
    qDebug() << "Engine tuned to " << freq << signal;
    m_signal = signal;
    emit signalChanged();
    if(m_state == StateScanning && m_nextState == StateIdle)
    {
        qDebug() << "Scan completed";
        m_currentFreq = freq;
        emit scanCompleted(freq);
    }

    m_state = StateIdle;
    if(m_nextState == StateTuning && m_currentFreq!=freq)
    {
        tuneToFreq(m_currentFreq);
    }
    else if (m_nextState == StateScanning)
    {
        scan(m_scanForward);
    }

    m_nextState = StateIdle;
}

void TunerModel::tuneToFreq(double freq)
{
    m_currentFreq = freq;

    if(m_state == StateIdle)
    {
        m_state = StateTuning;
        QMetaObject::invokeMethod(&m_engine, "tuneToFreq", Qt::QueuedConnection, Q_ARG(double, m_currentFreq));
    }
    else
    {
        m_nextState = StateTuning;
    }
}

void TunerModel::scan(bool forward)
{
    m_scanForward = forward;

    if(m_state == StateIdle)
    {
        m_state = StateScanning;
        m_engine.scan(m_scanForward);
    }
    else if(m_state != StateScanning)
    {
        m_nextState = StateScanning;
    }
}

double TunerModel::currentFreq()
{
    return m_currentFreq;
}

uint TunerModel::signalLevel()
{
    return m_signal;
}

bool TunerModel::isStereo()
{
    return m_stereo;
}

void TunerModel::slotOnSignalChanged(uint signal, bool stereo)
{
    qDebug() << "Signal changed " << signal << "Stereo: " << stereo;
    m_signal = signal;
    m_stereo = stereo;
    emit signalChanged();
}
