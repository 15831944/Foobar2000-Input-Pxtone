/*
  Pxtone, a chiptune / lo-tech music decoder for foobar2000
  Copyright (C) 2005-2017 Studio Pixel
  Copyright (C) 2015-2020 Wilbert Lee

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <cstdio>
#include <windows.h>
#include <shlwapi.h>
#include "pxtnService.h"
#include "pxtnError.h"
#include "foobar2000\ATLHelpers\ATLHelpers.h"

#define _CHANNEL_NUM           2
#define _SAMPLE_PER_SECOND 44100

class input_pxtone {
private:
    byte* m_buffer;
    pxtnService pxtn;
    pxtnDescriptor desc;
    service_ptr_t<file> m_file;
    t_input_open_reason m_reason;
public:
    input_pxtone();
    ~input_pxtone();
    static bool g_is_our_path(const char* p_path, const char* p_extension);

    void read_custom_data(float pos, abort_callback& p_abort);
    void get_info(file_info& p_info, abort_callback& p_abort);
    void open(service_ptr_t<file> p_filehint, const char* p_path, t_input_open_reason p_reason, abort_callback& p_abort);
    void decode_initialize(unsigned int p_flags, abort_callback& p_abort);
    bool decode_run(audio_chunk& p_chunk, abort_callback& p_abort);
    void decode_seek(double p_seconds, abort_callback& p_abort);
    bool decode_can_seek() { return false; }

    t_filestats get_file_stats(abort_callback& p_abort) { return m_file->get_stats(p_abort); }
    void decode_on_idle(abort_callback& p_abort) { m_file->on_idle(p_abort); }
    bool decode_get_dynamic_info(file_info& p_out, double& p_timestamp_delta) { return false; }
    bool decode_get_dynamic_info_track(file_info& p_out, double& p_timestamp_delta) { return false; }
    static bool g_is_our_content_type(const char* p_content_type) { return false; }
    void retag(const file_info& p_info, abort_callback& p_abort) {}
};

const int buffersize = 512;
const std::uint32_t buffer_size = pxtnBITPERSAMPLE / 8 * _CHANNEL_NUM * buffersize;

input_pxtone::input_pxtone() : m_buffer(nullptr) {
}

input_pxtone::~input_pxtone() {
    if (m_buffer == nullptr)
        return;
    if (m_reason == input_open_info_read)
        goto end;
end:	delete[] m_buffer;
    pxtn.clear();
}

void input_pxtone::read_custom_data(float pos,
    abort_callback& p_abort) {
    pxtnERR        pxtn_err = pxtnERR_VOID;
    t_filesize size = m_file->get_size(p_abort);
    m_buffer = new byte[size];
    m_file->read(m_buffer, size, p_abort);

    if (!desc.set_memory_r(m_buffer, size))
        throw exception_io_file_corrupted("set_memory_r");

    pxtn_err = pxtn.init(); if (pxtn_err != pxtnOK)
        throw exception_io_device_full();

    if (!pxtn.set_destination_quality(_CHANNEL_NUM, _SAMPLE_PER_SECOND))
        throw exception_io_file_corrupted("set_destination_quality");

    pxtn_err = pxtn.read(&desc); if (pxtn_err != pxtnOK)
    {
        char string[25];
        itoa(pxtn_err, string, 10);
        throw exception_io_file_corrupted(string);
    }

    pxtn_err = pxtn.tones_ready(); if (pxtn_err != pxtnOK)
        throw exception_io_file_corrupted("pxtn.tones_ready");

    // PREPARATION PLAYING MUSIC.
    pxtnVOMITPREPARATION prep = { 0 };
    prep.flags = 0;
    prep.start_pos_float = pos;
    prep.master_volume = 1.0f;

    if (!pxtn.moo_preparation(&prep))
        throw exception_io_file_corrupted("pxtn.moo_preparation");
}

void input_pxtone::get_info(file_info& p_info, abort_callback& p_abort) {
    int32_t       buf_size = 0;

    pxtn.moo_set_loop(false);
    pxtn.moo_set_fade(0, 0);

    // ??? We don't want to recalculate the path
    //p_info.meta_set("title", pxtn.text->get_name_buf(&buf_size));
    p_info.set_length(pxtn.moo_get_total_sample() / _SAMPLE_PER_SECOND);
    p_info.info_set("encoding", "lossless");
    p_info.info_set_int("samplerate", _SAMPLE_PER_SECOND);
    p_info.info_set_int("channels", _CHANNEL_NUM);
    p_info.info_set_int("bitspersample", buffersize);
    p_info.info_set_bitrate((buffersize * _CHANNEL_NUM * _SAMPLE_PER_SECOND + 500) / 1000);
}

void input_pxtone::open(service_ptr_t<file> p_filehint, const char* p_path,
    t_input_open_reason p_reason, abort_callback& p_abort) {
    m_file = p_filehint;
    m_reason = p_reason;

    input_open_file_helper(m_file, p_path, p_reason, p_abort);

    // LOAD MUSIC FILE.
    switch (p_reason) {
    case input_open_info_read:
        read_custom_data(0, p_abort);
        break;
    case input_open_decode:
        read_custom_data(0, p_abort);
        break;
    case input_open_info_write:
        throw exception_io_data();
        break;
    }
}

void input_pxtone::decode_initialize(unsigned int p_flags, abort_callback& p_abort) {
    m_file->reopen(p_abort);
}

bool input_pxtone::decode_run(audio_chunk& p_chunk, abort_callback& p_abort) {
    static bool finished;
    static byte buffer[buffer_size];
    if (pxtn.moo_is_end_vomit() || !pxtn.moo_is_valid_data())
        return false;
    finished = pxtn.Moo(buffer, buffer_size);
    p_chunk.set_data_fixedpoint(buffer, buffer_size, _SAMPLE_PER_SECOND, _CHANNEL_NUM, pxtnBITPERSAMPLE,
        audio_chunk::g_guess_channel_config(_CHANNEL_NUM));
    return finished;
}

void input_pxtone::decode_seek(double p_seconds, abort_callback& p_abort) {
    int samples_to_do;

    read_custom_data(p_seconds, p_abort);
}

bool input_pxtone::g_is_our_path(const char* p_path, const char* p_extension) {
    if (stricmp_utf8(p_extension, "pttune") == 0)
        return true;
    else if (stricmp_utf8(p_extension, "ptcop") == 0)
        return true;
    else
        return false;
}

static input_singletrack_factory_t<input_pxtone> g_input_pxtone_factory;

DECLARE_COMPONENT_VERSION("Pxtone", "1.2.0", \
"Program: Pxtone chiptune / lo-tech music decoder\n"\
"Platform: Intel 80386\n"\
"Programmer: Pxtone Collage (Studio Pixel), Wilbert Lee\n"\
"\n"\
"Pxtone Collage is a chiptune / lo-tech music decoder for Windows.\n" \
"The great programmer of Pxtone Collage is Pixel, who is celebrated as the programmer of 'Cave Story'.\n" \
"\n"\
"Copyright 2005 Studio Pixel\n"\
"Copyright 2012 Wilbert Lee\n"\
);
DECLARE_FILE_TYPE("Pxtone Tune Sound Format", "*.PTTUNE;*.PTCOP");
// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_input_pxtone.dll");