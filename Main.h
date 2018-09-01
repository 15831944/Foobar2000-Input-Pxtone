#pragma once
#include "Pxtone.h"
#include "foobar2000\ATLHelpers\ATLHelpers.h"

enum {
	piston_bits_per_sample = 16,
	piston_channels = 2,
	piston_sample_rate = 44100,

	piston_step_buffer_size = 512,
	piston_bytes_per_sample = piston_bits_per_sample / 8,
	piston_total_sample_width = piston_bytes_per_sample * piston_channels,
};

extern pfc::string8 sz_exist_file, sz_target_file;
extern LPPxtoneDecoder m_p_main, m_p_tag;

class input_piston
{
private:
	// Main obj
	byte *m_p_data;
	t_filesize m_file_size;
	LoopInfo m_loop_info;
	service_ptr_t<file> m_file;
	t_input_open_reason m_reason;
public:
	input_piston();
	~input_piston();
	static bool g_is_our_path(const char *p_path, const char *p_extension);

	void read_custom_data(LPPxtoneDecoder,void *, t_filesize&, abort_callback &);
	void get_info(file_info &p_info, abort_callback &p_abort);
	void open(service_ptr_t<file> p_filehint, const char * p_path, t_input_open_reason p_reason, abort_callback &p_abort);
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
};