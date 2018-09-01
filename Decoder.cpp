#include <cstdio>
#include <windows.h>
#include <Shlwapi.h>
#include "Main.h"
#include "Pxtone.h"
// decoder

input_piston::input_piston()
{
	m_file_size = 0;
	m_p_data = NULL;
}

input_piston::~input_piston()
{
	if (m_p_data != NULL)
	{
		m_p_main->TuneStop();
		m_p_main->TuneRelease();
		m_p_main->Release();
		delete[]m_p_data;
		m_p_data = NULL;
	}
}

void input_piston::read_custom_data(LPPxtoneDecoder m, void *p, t_filesize &size, abort_callback &p_abort)
{
	size = m_file->get_size(p_abort);
	p = new byte[size];
	m_file->read(p, size, p_abort);
	if (!m->TuneRead(p, size))
	{
		m->TuneRelease();
		delete[]p;
		throw exception_io_data();
	}
}

void input_piston::get_info(file_info & p_info, abort_callback & p_abort)
{
	LPPxtoneDecoder p_module;

	if (m_reason == input_open_info_read)
		p_module = m_p_tag;
	else
		p_module = m_p_main;

	p_module->TuneSetLoop(false);
	p_module->TuneGetInformation(&m_loop_info.beats,
		&m_loop_info.bpm, &m_loop_info.ticks,
		&m_loop_info.measures);

	m_loop_info.loops = 0;
	m_loop_info.meas_loop = p_module->TuneGetPlayMeas();
	m_loop_info.meas_intro = p_module->TuneGetRepeatMeas();

	// note that the values below should be based on contents of the file itself, NOT on user-configurable variables for an example. To report info that changes independently from file contents, use get_dynamic_info/get_dynamic_info_track instead.
	p_info.meta_set("title", p_module->TuneGetName());
	p_info.info_set("encoding", "lossless");
	p_info.info_set_int("samplerate", piston_sample_rate);
	p_info.info_set_int("channels", piston_channels);
	p_info.info_set_int("bitspersample", piston_bits_per_sample);
	p_info.set_length(MeasToMSecs(&m_loop_info, m_loop_info.measures) / 1000);
	p_info.info_set_bitrate((piston_bits_per_sample * piston_channels * piston_sample_rate + 500 /* rounding for bps to kbps*/) / 1000 /* bps to kbps */);
	
	if (m_reason == input_open_info_read)
	{
		m_p_tag->TuneRelease();
		delete[]m_p_data;
	}
}

void input_piston::open(service_ptr_t<file> p_filehint, const char * p_path, t_input_open_reason p_reason, abort_callback & p_abort)
{
	m_file = p_filehint;
	m_reason = p_reason;
	input_open_file_helper(m_file, p_path, p_reason, p_abort);
	switch (p_reason)
	{
	case input_open_info_read:
		read_custom_data(m_p_tag, m_p_data, m_file_size, p_abort);
		break;
	case input_open_decode:
		m_p_main->Ready(NULL, piston_channels,
			piston_sample_rate, piston_bits_per_sample, 0, FALSE, NULL);
		read_custom_data(m_p_main, m_p_data, m_file_size, p_abort);
		break;
	case input_open_info_write:
		throw exception_io_data();
		break;
	default:
		break;
	}
}

void input_piston::decode_initialize(unsigned int p_flags, abort_callback & p_abort)
{
	m_file->reopen(p_abort);
	//m_p_main->TuneSetVolume(1.0f);
	m_p_main->TuneStart(0, 0);
}

bool input_piston::decode_run(audio_chunk & p_chunk, abort_callback & p_abort)
{
	static bool b_ret;
	static byte a[512];
	b_ret = m_p_main->TuneVomit(a, 128);
	p_chunk.set_data_fixedpoint(a, 512, piston_sample_rate, piston_channels, piston_bits_per_sample,
		audio_chunk::g_guess_channel_config(piston_channels));
	return b_ret;
}

void input_piston::decode_seek(double p_seconds, abort_callback & p_abort)
{
	int samples_to_do;

	p_seconds *= 1000;
	if (m_p_main->TuneIsStreaming())
		m_p_main->TuneStop();

	if (m_loop_info.loops == 0 || p_seconds < MeasToMSecs(&m_loop_info, m_loop_info.meas_loop))
	{
		samples_to_do = p_seconds * 44100 / 1000;
	}
	else
	{
		int correct_position = p_seconds;
		correct_position -= MeasToMSecs(&m_loop_info, m_loop_info.meas_intro);
		correct_position %= MeasToMSecs(&m_loop_info, m_loop_info.meas_loop - m_loop_info.meas_intro);
		correct_position += MeasToMSecs(&m_loop_info, m_loop_info.meas_intro);
		samples_to_do = correct_position * 44100 / 1000;
	}

	//m_p_main->TuneSetVolume(1.0f);
	m_p_main->TuneStart(samples_to_do, 0);
}

bool input_piston::decode_can_seek()
{
	return true;
}

bool input_piston::decode_get_dynamic_info(file_info & p_out, double & p_timestamp_delta)
{
	return false;
}

void input_piston::retag(const file_info & p_info, abort_callback & p_abort)
{
}

bool input_piston::g_is_our_path(const char *p_path, const char *p_extension)
{
	if (stricmp_utf8(p_extension, "pttune") == 0)
		return true;
	else if (stricmp_utf8(p_extension, "ptcop") == 0)
		return true;
	else
		return false;
}

static input_singletrack_factory_t<input_piston> g_input_piston_factory;