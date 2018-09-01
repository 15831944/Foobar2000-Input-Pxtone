/*
  foo_input_pxtone, a Pxtone chiptune / lo-tech music decoder for foobar2000
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

#include "Main.h"
#include "Pxtone.h"

pfc::string8 sz_exist_file, sz_target_file;
LPPxtoneDecoder m_p_main, m_p_tag;

class myinitquit : public initquit {
public:
	void on_init() {
		pfc::string8 sz_temp;
		sz_exist_file = core_api::get_my_full_path();
		sz_exist_file.truncate(sz_exist_file.scan_filename());
		sz_exist_file += DLL_NAME ".dll";
		uGetTempPath(sz_temp);
		uGetTempFileName(sz_temp, pfc::string_filename(sz_exist_file), 0, sz_target_file);

		if (!CopyFile(pfc::stringcvt::string_os_from_utf8(sz_exist_file), pfc::stringcvt::string_os_from_utf8(sz_target_file), FALSE))
			throw exception_io();

		m_p_tag = CreateDecoder(sz_target_file);
		m_p_main = CreateDecoder(sz_exist_file);

		m_p_tag->Ready(NULL, piston_channels,
			piston_sample_rate, piston_bits_per_sample, 0, FALSE, NULL);
	}

	void on_quit() {

		m_p_tag->Release();
		DestroyDecoder(m_p_tag);
		DestroyDecoder(m_p_main);
		DeleteFileA(sz_target_file);
	}
};

static initquit_factory_t<myinitquit> g_myinitquit_factory;

DECLARE_COMPONENT_VERSION("Pxtone", "0.9.1.8", \
"Program : Pxtone chiptune / lo-tech music decoder\n"\
"Platform : Intel 80386\n"\
"Programmer : Pxtone Collage (Studio Pixel), Wilbert Lee\n"\
"\n"\
"Pxtone Collage is a chiptune / lo-tech music decoder for Windows\n" \
"\n"\
"The great programmer of Pxtone Collage is Pixel, who is celebrated as the programmer of 'Cave Story'\n" \
"\n"\
"Copyright (C) 2005 Studio Pixel\n"\
"Copyright (C) 2012 Wilbert Lee\n"\
);
DECLARE_FILE_TYPE("Pxtone Tune Sound Format", "*.PTTUNE;*.PTCOP");
// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_input_pxtone.dll");