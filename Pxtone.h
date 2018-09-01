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

#pragma once
#include <Windows.h>
#define DLL_NAME "pxtone"
#define SAFE_DELETE(x) { if (x != NULL){delete x; x= NULL;} }

typedef bool(__cdecl *LPPX_READY)(HWND, int, int, int, float, bool, void *);// PXTONEPLAY_WINAPI
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

typedef struct _LoopInfo
{
	float bpm;
	int beats, ticks, measures;
	int meas_intro, meas_loop, loops;
}LoopInfo, *LPLoopInfo;

typedef struct _PxtoneDecoder
{
	HMODULE hInst;
	LPPX_READY Ready;
	LPPX_RESET Reset;
	LPPX_GETDIRECTSOUND GetDirectSound;
	LPPX_GETLASTERROR GetLastError;
	LPPX_GETQUALITY GetQuality;
	LPPX_RELEASE Release;
	LPPXT_LOAD TuneLoad;
	LPPXT_READ TuneRead;
	LPPXT_RELEASE TuneRelease;
	LPPXT_START TuneStart;
	LPPXT_FADEOUT TuneFadeOut;
	LPPXT_SETVOLUME TuneSetVolume;
	LPPXT_STOP TuneStop;
	LPPXT_ISSTREAMING TuneIsStreaming;
	LPPXT_SETLOOP TuneSetLoop;
	LPPXT_GETINFORMATION TuneGetInformation;
	LPPXT_GETREPEATMEAS TuneGetRepeatMeas;
	LPPXT_GETPLAYMEAS TuneGetPlayMeas;
	LPPXT_GETNAME TuneGetName;
	LPPXT_GETCOMMENT TuneGetComment;
	LPPXT_VOMIT TuneVomit;
}PxtoneDecoder, *LPPxtoneDecoder;
LPPxtoneDecoder CreateDecoder(const char *);
void DestroyDecoder(LPPxtoneDecoder);
int MeasToMSecs(LPLoopInfo, int);