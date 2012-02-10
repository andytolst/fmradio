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

#include "v4ltuner.h"

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <QDebug>

#define TUNER_DEVICE "/dev/radio0"

static int tuner_fd = -1;
static bool tuner_precise;

static inline unsigned long v4l_to_hz(unsigned f)
{
    if (tuner_precise) {
        /* f * 62.5 */
        return (f * 125UL) / 2;
    } else {
        return f * 62500UL;
    }
}

static inline unsigned hz_to_v4l(unsigned long f)
{
    if (tuner_precise) {
        /* f / 62.5 */
        return (f * 2UL) / 125;
    } else {
        return f / 62500;
    }
}

static inline unsigned long mhz_to_hz(double f)
{
    return f * 1000000.0;
}

V4LTuner::V4LTuner(QObject *parent) : QObject(parent)
{
    struct v4l2_tuner tuner;
    int res;

    if (tuner_fd == -1) {
        tuner_fd = open(TUNER_DEVICE, O_RDONLY);
    }

    if (tuner_fd == -1) {
        qCritical() << "Couldn't open V4L2 tuner";
    }

    tuner.index = 0;
    res = ioctl(tuner_fd, VIDIOC_G_TUNER, &tuner);
    if (res < 0) {
        qCritical() << "Couldn't get V4L2 tuner information";
    }

    if (tuner.type != V4L2_TUNER_RADIO) {
        qCritical() << "Not a radio tuner\n";
    }

    tuner_precise = (tuner.capability & V4L2_TUNER_CAP_LOW) ?
                            TRUE : FALSE;
    unsigned long tuner_min = v4l_to_hz(tuner.rangelow);
    unsigned long tuner_max = v4l_to_hz(tuner.rangehigh);
    bool stereo = tuner.capability & V4L2_TUNER_CAP_STEREO;

    qDebug() << "Min range:" << tuner_min << "Max range:" << tuner_max << "Prescise:" << tuner_precise << "Stereo: " << stereo;
}

V4LTuner::~V4LTuner()
{
    if (tuner_fd != -1) {
        close(tuner_fd);
        tuner_fd = -1;
    }
}

bool V4LTuner::tuneToFreq(double freq)
{
    int res = 0;
    if (freq != 0.0)
    {
        struct v4l2_frequency t_freq;
        t_freq.tuner = 0;
        t_freq.type = V4L2_TUNER_RADIO;
        t_freq.frequency = hz_to_v4l(mhz_to_hz(freq));
        res = ioctl(tuner_fd, VIDIOC_S_FREQUENCY, &t_freq);
        if (res < 0)
        {
            qDebug() << "Failed to tune" << freq;
            return false;
        }
        qDebug() << "Tuned to " << freq;
    }
    return true;
}

uint V4LTuner::signalLevel(bool &isStereo)
{
    struct v4l2_tuner t_tuner;
    t_tuner.index = 0;

    if (ioctl(tuner_fd, VIDIOC_G_TUNER, &t_tuner) < 0) {
        qDebug() << "Failed to get signal level:";
        return 0;
    }

    isStereo = t_tuner.audmode & V4L2_TUNER_MODE_STEREO;
//    isStereo = t_tuner.rxsubchans &  V4L2_TUNER_SUB_STEREO;

    return t_tuner.signal < 0 ? 0 : t_tuner.signal;
}

double V4LTuner::currentFreq()
{
    struct v4l2_frequency t_freq;
    t_freq.tuner = 0;

    if (ioctl(tuner_fd, VIDIOC_G_FREQUENCY, &t_freq) < 0) {
        qDebug() << "Failed to get freq: ";
        return 0;
    }

    return v4l_to_hz(t_freq.frequency) / 1000000.0;
}

void V4LTuner::scan(bool forward)
{
    struct v4l2_hw_freq_seek t_freq_seek;
    t_freq_seek.tuner = 0;
    t_freq_seek.type = V4L2_TUNER_RADIO;
    t_freq_seek.seek_upward = forward;

    if (ioctl(tuner_fd, VIDIOC_S_HW_FREQ_SEEK, &t_freq_seek) < 0) {
        qDebug() << "Failed to start seek:";
    }
}
