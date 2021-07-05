// Written by Adrian Musceac YO8RZZ , started October 2013.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "audiointerface.h"
#include <string>

AudioInterface::AudioInterface(unsigned sample_rate, unsigned channels, int normal) 
{
    _s_rec = NULL;
    _s_play = NULL;
    _s_short_play = NULL;
    _s_short_rec = NULL;
    _error=0;


    std::string q_play = "mmdvmsdr";
    std::string audio_rec = "mmdvmsdr_rec";


    pa_buffer_attr attr;
    attr.fragsize = 2048;
    attr.maxlength = 4096;
    attr.minreq = -1;
    attr.prebuf = -1;
    attr.tlength = 4096;

    if(!normal)
    {
        pa_sample_spec ss;
        ss.format = PA_SAMPLE_FLOAT32LE;
        ss.rate = sample_rate;
        ss.channels = channels;


        if (!(_s_rec = pa_simple_new(NULL, audio_rec.c_str(), PA_STREAM_RECORD, NULL, "record", &ss, NULL, &attr, &_error)))
        {
            fprintf(stderr, __FILE__": pa_simple_new() failed:\n");
        }
        if (!(_s_play = pa_simple_new(NULL, q_play.c_str(), PA_STREAM_PLAYBACK, NULL, "play", &ss, NULL, &attr, &_error)))
        {
            fprintf(stderr, __FILE__": pa_simple_new() failed:\n");
        }
    }
    else
    {
        pa_sample_spec ss_short;
        ss_short.format = PA_SAMPLE_S16LE;
        ss_short.rate = sample_rate;
        ss_short.channels = channels;


        if (!(_s_short_play = pa_simple_new(NULL, q_play.c_str(), PA_STREAM_PLAYBACK, NULL, "play", &ss_short, NULL, &attr, &_error)))
        {
            fprintf(stderr, __FILE__": pa_simple_new() failed:\n");
        }
        if (!(_s_short_rec = pa_simple_new(NULL, audio_rec.c_str(), PA_STREAM_RECORD, NULL, "record", &ss_short, NULL, &attr, &_error)))
        {
            fprintf(stderr, __FILE__": pa_simple_new() failed:\n");
        }
    }
}

AudioInterface::~AudioInterface()
{
    if (_s_rec)
      pa_simple_free(_s_rec);
    if (_s_play)
      pa_simple_free(_s_play);
    if (_s_short_play)
      pa_simple_free(_s_short_play);
    if (_s_short_rec)
      pa_simple_free(_s_short_rec);
}



int AudioInterface::read(float *buf, short bufsize)
{
    if(!_s_rec)
        return 1;
    if (pa_simple_read(_s_rec, buf, bufsize, &_error) < 0)
    {
        fprintf(stderr, __FILE__": pa_simple_read() failed:\n");
        return 1;
    }
    return 0;
}

int AudioInterface::write(float *buf, short bufsize)
{
    if(!_s_play)
        return 1;
    if(pa_simple_write(_s_play, buf, bufsize, &_error) < 0)
    {
        fprintf(stderr, __FILE__": pa_simple_write() failed:\n");
        return 1;
    }
    return 0;
}

int AudioInterface::write_short(short *buf, short bufsize)
{

    if(!_s_short_play)
        return 1;
    if(pa_simple_write(_s_short_play, buf, bufsize, &_error) < 0)
    {
        fprintf(stderr, __FILE__": pa_simple_write() failed:\n");
        return 1;
    }
    return 0;
}

int AudioInterface::read_short(short *buf, short bufsize)
{
    if(!_s_short_rec)
        return 0;
    if(pa_simple_read(_s_short_rec, buf, bufsize, &_error) < 0)
    {
        fprintf(stderr, __FILE__": pa_simple_read() failed:\n");
        return 0;
    }
    return 1;
}
