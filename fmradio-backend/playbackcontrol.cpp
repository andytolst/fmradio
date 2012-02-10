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

#include "playbackcontrol.h"

#include <alsa/asoundlib.h>

#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

#include <QDebug>
#include <QTimer>

#define ALSA_MIXER_NAME "hw:2"
#define ALSA_PCM_CAPTURE_NAME "hw:2,0"

static snd_pcm_t *a_pcm = NULL;

/** Sets mixer switch <name> to <value> */
static bool mixer_set_enum_value(snd_hctl_t *mixer,
    const char * name, const char * value)
{
    int err;

    snd_ctl_elem_id_t *id;
    snd_ctl_elem_id_alloca(&id);
    snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
    snd_ctl_elem_id_set_name(id, name);

    snd_hctl_elem_t *elem = snd_hctl_find_elem(mixer, id);
    if (!elem) {
        qDebug("Couldn't find mixer element '%s'", name);
        return false;
    }

    snd_ctl_elem_info_t *info;
    snd_ctl_elem_info_alloca(&info);
    snd_ctl_elem_info_set_id(info, id);
    err = snd_hctl_elem_info(elem, info);
    if (err) {
        qDebug("Couldn't get mixer element '%s' information (%d)", name,
            err);
        return false;
    }

    long value_idx = -1;
    int i, items = snd_ctl_elem_info_get_items(info);
    for (i = 0; i < items; i++) {
        snd_ctl_elem_info_set_item(info, i);
        err = snd_hctl_elem_info(elem, info);
        if (err) {
            qDebug("Couldn't get mixer element '%s' information (%d)", name,
                err);
            return false;
        }
        if (strcmp(snd_ctl_elem_info_get_item_name(info), value) == 0) {
            value_idx = i;
            break;
        }
    }
    if (value_idx < 0) {
        qDebug("Couldn't find mixer '%s' value '%s'", name, value);
        return false;
    }

    snd_ctl_elem_value_t *control;
    snd_ctl_elem_value_alloca(&control);
    snd_ctl_elem_value_set_id(control, id);

    items = snd_ctl_elem_info_get_count(info);
    for (i = 0; i < items; i++) {
        snd_ctl_elem_value_set_enumerated(control, i, value_idx);
    }

    err = snd_hctl_elem_write(elem, control);
    if (err) {
        qDebug("Couldn't set mixer element '%s' (%d)", name, err);
        return false;
    }

    return true;
}

PlaybackControl::PlaybackControl(QObject *parent) :
    QObject(parent), m_sourceBuffer(0)
{
}

bool PlaybackControl::configureAlsaForRadio()
{
    snd_hctl_t *mixer;
    int err = snd_hctl_open(&mixer, ALSA_MIXER_NAME, 0);
    if (err < 0) {
        qDebug("Failed to open ALSA mixer '%s' (%d)", ALSA_MIXER_NAME, err);
        return false;
    }

    err = snd_hctl_load(mixer);
    if (err < 0) {
        qDebug("Failed to load ALSA hmixer elements (%d)", err);
        snd_hctl_close(mixer);
        return false;
    }

    bool res = mixer_set_enum_value(mixer, "Mode Switch", "Rx");
    // The above uploads firmware and a few other things,
    // so let's give it some time.
    usleep(500000);
    res &= mixer_set_enum_value(mixer, "Codec Mode", "FmRx");
    res &= mixer_set_enum_value(mixer, "Audio Switch", "Digital");

    snd_hctl_close(mixer);
    qDebug() << "PlaybackControl::configureAlsaForRadio()";
    return res;
}

bool PlaybackControl::configureAlsaForBt()
{
    snd_hctl_t *mixer;
    int err = snd_hctl_open(&mixer, ALSA_MIXER_NAME, 0);
    if (err < 0) {
        qDebug("Failed to open ALSA mixer '%s' (%d)", ALSA_MIXER_NAME, err);
        return false;
    }

    err = snd_hctl_load(mixer);
    if (err < 0) {
        qDebug("Failed to load ALSA hmixer elements (%d)", err);
        snd_hctl_close(mixer);
        return false;
    }

    bool res = mixer_set_enum_value(mixer, "Audio Switch", "Analog");
    res &= mixer_set_enum_value(mixer, "Codec Mode", "Bt");
    res &= mixer_set_enum_value(mixer, "Mode Switch", "Off");

    snd_hctl_close(mixer);

    qDebug() << "PlaybackControl::configureAlsaForBt()";

    return res;
}

bool PlaybackControl::start()
{
    if(configureAlsaForRadio())
    {
        if(configurePlaybackSource())
        {
            return configurePlaybackSink();
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool PlaybackControl::configurePlaybackSource()
{
    int err = snd_pcm_open(&a_pcm, ALSA_PCM_CAPTURE_NAME, SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK /*| SND_PCM_ASYNC*/);
    if (err < 0)
    {
        qDebug("Failed to open ALSA PCM device '%s' (%d)", ALSA_PCM_CAPTURE_NAME, err);
        return false;
    }

    /* -f S16_LE -r 48000 -c 2; latency up to 0.3 seconds */
    err = snd_pcm_set_params(a_pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_MMAP_INTERLEAVED /*SND_PCM_ACCESS_RW_INTERLEAVED*/, 2, 48000, 0, 300000);
    if (err < 0)
    {
        qDebug("Failed to open ALSA PCM device '%s' (%d)", ALSA_PCM_CAPTURE_NAME, err);
        snd_pcm_close(a_pcm);
        return false;
    }

    snd_async_handler_t *ahandler;
    err = snd_async_add_pcm_handler(&ahandler, a_pcm, PlaybackControl::capture_callback, this);
    if(err < 0)
    {
        qDebug() << "Not able to register PCM callback";
    }

    err = snd_pcm_start(a_pcm);
    if (err < 0)
    {
        qDebug("Couldn't start PCM device (%d)", err);
        snd_pcm_close(a_pcm);
        return false;
    }

//    qDebug() << "Configured PCM playback" << snd_pcm_state(a_pcm);

//    snd_pcm_uframes_t buffer_size, period_size;
//    snd_pcm_get_params 	( a_pcm,
//            &buffer_size,
//            &period_size
//        );
//    qDebug() << "Params:" << buffer_size << period_size;

    return true;
}

bool PlaybackControl::configurePlaybackSink()
{
    QAudioFormat format;
    // Set up the format
    format.setFrequency(48000);
    format.setChannels(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "raw audio format not supported by backend, cannot play audio.";
        return false;
    }

    m_audioOutput = new QAudioOutput(info, format, this);
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(slotOnStateChanged(QAudio::State)));
    m_sourceBuffer = m_audioOutput->start();

    qDebug() << "Starting playback..." << m_audioOutput->periodSize();

    return true;
}

void PlaybackControl::slotOnStateChanged(QAudio::State state)
{
    qDebug() << "Audio state changed" << state;

    if(state == QAudio::IdleState && m_audioOutput->error() == QAudio::UnderrunError)
    {
        delete m_audioOutput;
        configurePlaybackSink();
    }
}

void PlaybackControl::capture_callback(snd_async_handler_t *ahandler)
{
    PlaybackControl* self = static_cast<PlaybackControl*>(snd_async_handler_get_callback_private(ahandler));

    if(self->m_sourceBuffer == 0)
        return;

    snd_pcm_sframes_t avail;
    snd_pcm_uframes_t toread = 0;

    size_t bytes;
    int err;

    avail = snd_pcm_avail_update(a_pcm);
    if (avail < 0) {
        qDebug("ALSA stream avail failed (%d)", (int) avail);
        err = snd_pcm_recover(a_pcm, avail, 0);
        if (err < 0) {
            qDebug("ALSA stream failed to recover (%d)", err);
            self->playbackError();
            return;
        }
        else
        {
            err = snd_pcm_start(a_pcm);
        }
        return;
    }

    //want to read all available frames
    toread = avail;

    const snd_pcm_channel_area_t* areas;
    snd_pcm_uframes_t offset;

    err = snd_pcm_mmap_begin (a_pcm, &areas, &offset, &toread);
    if(err < 0)
    {
        qDebug() << "get mmap error";
    }

    bytes = snd_pcm_frames_to_bytes(a_pcm, toread);
    size_t offsetbytes = snd_pcm_frames_to_bytes(a_pcm, offset);

    const char* wrAddr = (char*)(areas->addr) + offsetbytes;

    self->m_sourceBuffer->write(wrAddr, bytes);

    snd_pcm_mmap_commit(a_pcm, offset, toread);
}


PlaybackControl::~PlaybackControl()
{
    qDebug() << "PlaybackControl::~PlaybackControl()";

    if (a_pcm) {
        snd_pcm_close(a_pcm);
        a_pcm = NULL;
    }

    m_audioOutput->stop();

    configureAlsaForBt();
}


