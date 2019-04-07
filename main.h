/*
  foo_input_pxtone, a pxtone chiptune / lo-tech music decoder for foobar2000
  Copyright (C) 2005 Studio Pixel
  Copyright (C) 2015 Wilbert Lee

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

#pragma once
#include "foobar2000\ATLHelpers\ATLHelpers.h"
#define DLL_NAME "pxtone"
#define SAFE_DELETE(X) { if (X != nullptr) { delete X; X = nullptr; } }
#define GET_PXTONE_ADDR(FUNC, TYPE, NAME) p_pxtone->FUNC = \
reinterpret_cast<TYPE>(GetProcAddress(p_pxtone->instance, NAME))

typedef bool(__cdecl *LPPX_READY)(HWND, int, int, int, float, bool, void *);
typedef bool(__cdecl *LPPX_RESET)(HWND, int, int, int, float, bool, void *);
typedef void *(__cdecl *LPPX_GETDIRECTSOUND)(void);
typedef const char *(__cdecl *LPPX_GETLASTERROR)(void);
typedef void(__cdecl *LPPX_GETQUALITY)(int *, int *, int *, int *);
typedef bool(__cdecl *LPPX_RELEASE)(void);
typedef bool(__cdecl *LPPXT_LOAD)(HMODULE, const char *, const char *);
typedef bool(__cdecl *LPPXT_READ)(void *, int);
typedef bool(__cdecl *LPPXT_RELEASE)(void);
typedef bool(__cdecl *LPPXT_START)(int, int);
typedef int(__cdecl *LPPXT_FADEOUT)(int);
typedef void(__cdecl *LPPXT_SETVOLUME)(float);
typedef int(__cdecl *LPPXT_STOP)(void);
typedef bool(__cdecl *LPPXT_ISSTREAMING)(void);
typedef void(__cdecl *LPPXT_SETLOOP)(bool);
typedef void(__cdecl *LPPXT_GETINFORMATION)(int *, float *, int *, int *);
typedef int(__cdecl *LPPXT_GETREPEATMEAS)(void);
typedef int(__cdecl *LPPXT_GETPLAYMEAS)(void);
typedef const char* (__cdecl *LPPXT_GETNAME)(void);
typedef const char* (__cdecl *LPPXT_GETCOMMENT)(void);
typedef bool(__cdecl *LPPXT_VOMIT)(void *, int);

typedef struct _loop_info {
	float bpm;
	int beats, ticks, measures;
	int meas_intro, meas_loop, loops;
} loop_info, *p_loop_info;

typedef struct _pxtone_decoder {
	HMODULE instance;
	LPPX_READY ready;
	LPPX_RESET reset;
	LPPX_GETDIRECTSOUND get_direct_sound;
	LPPX_GETLASTERROR get_last_error;
	LPPX_GETQUALITY get_quality;
	LPPX_RELEASE release;
	LPPXT_LOAD tune_load;
	LPPXT_READ tune_read;
	LPPXT_RELEASE tune_release;
	LPPXT_START tune_start;
	LPPXT_FADEOUT tune_fade_out;
	LPPXT_SETVOLUME tune_set_vol;
	LPPXT_STOP tune_stop;
	LPPXT_ISSTREAMING tune_is_streaming;
	LPPXT_SETLOOP tune_set_loop;
	LPPXT_GETINFORMATION tune_get_inf;
	LPPXT_GETREPEATMEAS tune_get_repeat;
	LPPXT_GETPLAYMEAS tune_get_play;
	LPPXT_GETNAME tune_get_name;
	LPPXT_GETCOMMENT tune_get_com;
	LPPXT_VOMIT tune_vomit;
} pxtone_decoder, *p_pxtone_decoder;

enum {
	pxtone_bits_per_sample = 16,
	pxtone_channels = 2,
	pxtone_sample_rate = 44100,

	pxtone_step_buffer_size = 512,
	pxtone_bytes_per_sample = pxtone_bits_per_sample / 8,
	pxtone_total_sample_width = pxtone_bytes_per_sample *pxtone_channels,
};

class initquit_pxtone : public initquit {
public:
	void on_init();
	void on_quit();
protected:
	p_pxtone_decoder create_pxtone_decoder(const char *p_filename);
	void destroy_pxtone_decoder(p_pxtone_decoder p_pxtone);
};

class input_pxtone {
private:
	byte *m_p_data;
	t_filesize m_file_size;
	loop_info m_loop_info;
	service_ptr_t<file> m_file;
	t_input_open_reason m_reason;
public:
	input_pxtone();
	~input_pxtone();
	static bool g_is_our_path(const char *p_path, const char *p_extension);

	void read_custom_data(p_pxtone_decoder p_pxtone, t_filesize size, abort_callback &p_abort);
	void get_info(file_info &p_info, abort_callback &p_abort);
	void open(service_ptr_t<file> p_filehint, const char *p_path, t_input_open_reason p_reason, abort_callback &p_abort);
	void decode_initialize(unsigned int p_flags, abort_callback &p_abort);
	bool decode_run(audio_chunk &p_chunk, abort_callback &p_abort);
	void decode_seek(double p_seconds, abort_callback &p_abort);
	bool decode_can_seek();

	t_filestats get_file_stats(abort_callback & p_abort) { return m_file->get_stats(p_abort); }
	void decode_on_idle(abort_callback & p_abort) { m_file->on_idle(p_abort); }
	bool decode_get_dynamic_info(file_info &p_out, double &p_timestamp_delta);
	bool decode_get_dynamic_info_track(file_info &p_out, double &p_timestamp_delta) { return false; }
	static bool g_is_our_content_type(const char *p_content_type) { return false; }
	void retag(const file_info &p_info, abort_callback &p_abort);
protected:
	int meas2msecs(p_loop_info p_loop, int measures);
};
