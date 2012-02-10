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

#ifndef PLAYBACKCONTROL_H
#define PLAYBACKCONTROL_H

#include <QObject>
#include <QBuffer>
#include <QAudio>

struct _snd_async_handler;
typedef _snd_async_handler snd_async_handler_t;

class QAudioOutput;

class PlaybackControl : public QObject
{
    Q_OBJECT
public:
    explicit PlaybackControl(QObject *parent = 0);
    ~PlaybackControl();
    bool start();
signals:
    void playbackError();
private slots:
    void slotOnStateChanged(QAudio::State state);

private:
    bool configureAlsaForRadio();
    bool configureAlsaForBt();

    bool configurePlaybackSource();
    bool configurePlaybackSink();

    static void capture_callback(snd_async_handler_t *ahandler);
private:
    QIODevice* m_sourceBuffer;
    QAudioOutput* m_audioOutput;
};

#endif // PLAYBACKCONTROL_H
