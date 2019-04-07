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
typedef bool(__cdecl *LPPXT_LOAD)(HMODUL
