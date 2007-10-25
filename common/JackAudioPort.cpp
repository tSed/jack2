/*
Copyright (C) 2001-2003 Paul Davis
Copyright (C) 2004-2006 Grame

This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "JackPortType.h"
#include <string.h>

namespace Jack
{

static void AudioBufferInit(void* buffer, size_t buffer_size, jack_nframes_t)
{
    memset(buffer, 0, buffer_size);
}

static inline void MixAudioBuffer(float* mixbuffer, float* buffer, jack_nframes_t frames)
{
    jack_nframes_t frames_group = frames / 4;
    frames = frames % 4;

    while (frames_group > 0) {
        register float mixFloat1 = *mixbuffer;
        register float sourceFloat1 = *buffer;
        register float mixFloat2 = *(mixbuffer + 1);
        register float sourceFloat2 = *(buffer + 1);
        register float mixFloat3 = *(mixbuffer + 2);
        register float sourceFloat3 = *(buffer + 2);
        register float mixFloat4 = *(mixbuffer + 3);
        register float sourceFloat4 = *(buffer + 3);

        buffer += 4;
        frames_group--;

        mixFloat1 += sourceFloat1;
        mixFloat2 += sourceFloat2;
        mixFloat3 += sourceFloat3;
        mixFloat4 += sourceFloat4;

        *mixbuffer = mixFloat1;
        *(mixbuffer + 1) = mixFloat2;
        *(mixbuffer + 2) = mixFloat3;
        *(mixbuffer + 3) = mixFloat4;

        mixbuffer += 4;
    }

    while (frames > 0) {
        register float mixFloat1 = *mixbuffer;
        register float sourceFloat1 = *buffer;
        buffer++;
        frames--;
        mixFloat1 += sourceFloat1;
        *mixbuffer = mixFloat1;
        mixbuffer++;
    }
}

static void AudioBufferMixdown(void* mixbuffer, void** src_buffers, int src_count, jack_nframes_t nframes)
{
    void* buffer;

    // Copy first buffer
    memcpy(mixbuffer, src_buffers[0], nframes * sizeof(float));

	// Mix remaining buffers
    for (int i = 1; i < src_count; ++i) {
        buffer = src_buffers[i];
        MixAudioBuffer((float*)mixbuffer, (float*)buffer, nframes);
    }
}

const JackPortType gAudioPortType = {
    JACK_DEFAULT_AUDIO_TYPE,
    AudioBufferInit,
    AudioBufferMixdown
};

} // namespace Jack

