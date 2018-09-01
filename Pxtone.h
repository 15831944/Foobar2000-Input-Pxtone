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