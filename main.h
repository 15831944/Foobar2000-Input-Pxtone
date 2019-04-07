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

typedef bool		(__cdecl *PXPROC1)(HWND, int, int, int, float, bool, void *);
typedef void *		(__cdecl *PXPROC2)(void);
typedef const char *(__cdecl *PXPROC3)(void);
typedef void		(__cdecl *PXPROC4)(int *, int *, int *, int *);
typedef bool		(__cdecl *PXPROC5)(void);
typedef bool		(__cdecl *PXPROC6)(HMODULE, const char *, const char *);
typedef bool		(__cdecl *PXPROC7)(void *, int);
typedef bool		(__cdecl *PXPROC8)(int, int);
typedef int			(__cdecl *PXPROC9)(int);
typedef void		(__cdecl *PXPROC10)(float);
typedef int			(__cdecl *PXPROC11)(void);
typedef void		(__cdecl *PXPROC12)(bool);
typedef void		(__cdecl *PXPROC13)(int *, float *, int *, int *);
typedef const char *(__cdecl *PXPROC14)(void);

enum {
	pxtone_bits_per_sample = 16,
	pxtone_channels = 2,
	pxtone_sample_rate = 44100,

	pxtone_step_buffer_size = 512,
	pxtone_bytes_per_sample = pxtone_bits_per_sample / 8,
	pxtone_total_sample_width = pxtone_bytes_per_sample * pxtone_channels,
};

typedef struct _loop_info {
	float bpm;
	int beats, ticks, measures;
	int meas_intro, meas_loop, loops;
} loop_info, *p_loop_info;

typedef struct _pxtone_decoder {
	HMODULE instance;
	PXPROC1 ready;
	PXPROC1 reset;
	PXPROC2 get_direct_sound;
	PXPROC3 get_last_error;
	PXPROC4 get_quality;
	PXPROC5 release;
	PXPROC6 tune_load;
	PXPROC7 tune_read;
	PXPROC5 tune_release;
	PXPROC8 tune_start;
	PXPROC9 tune_fade_out;
	PXPROC10 tune_set_vol;
	PXPROC11 tune_stop;
	PXPROC5 tune_is_streaming;
	PXPROC12 tune_set_loop;
	PXPROC13 tune_get_inf;
	PXPROC11 tune_get_repeat;
	PXPROC11 tune_get_play;
	PXPROC14 tune_get_name;
	PXPROC14 tune_get_com;
	PXPROC7 tune_vomit;
} pxtone_decoder, *p_pxtone_decoder;

class initquit_pxtone : public initquit {
public:
	void on_init();
	void on_quit();
protected:
	p_pxtone_decoder create_decoder(const char *p_filename);
	void destroy_decoder(p_pxtone_decoder p_pxtone);
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