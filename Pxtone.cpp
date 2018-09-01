#include "Pxtone.h"

LPPxtoneDecoder CreateDecoder(const char *lpFileName)
{
	LPPxtoneDecoder lpPxtoneDecoder = new PxtoneDecoder();
	ZeroMemory(lpPxtoneDecoder, sizeof(PxtoneDecoder));
	lpPxtoneDecoder->hInst = LoadLibraryA(lpFileName);
	if (lpPxtoneDecoder->hInst)
	{
		// fill function pointer
		// global interface
		lpPxtoneDecoder->Ready = reinterpret_cast<LPPX_READY>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Ready"));
		lpPxtoneDecoder->Reset = reinterpret_cast<LPPX_RESET>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Reset"));
		lpPxtoneDecoder->Release = reinterpret_cast<LPPX_RELEASE>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Release"));

		lpPxtoneDecoder->GetDirectSound = reinterpret_cast<LPPX_GETDIRECTSOUND>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_GetDirectSound"));
		lpPxtoneDecoder->GetLastError = reinterpret_cast<LPPX_GETLASTERROR>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_GetLastError"));
		lpPxtoneDecoder->GetQuality = reinterpret_cast<LPPX_GETQUALITY>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_GetQuality"));
		// tune
		lpPxtoneDecoder->TuneLoad = reinterpret_cast<LPPXT_LOAD>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_Load"));
		lpPxtoneDecoder->TuneRead = reinterpret_cast<LPPXT_READ>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_Read"));
		lpPxtoneDecoder->TuneRelease = reinterpret_cast<LPPXT_RELEASE>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_Release"));
		lpPxtoneDecoder->TuneStart = reinterpret_cast<LPPXT_START>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_Start"));
		lpPxtoneDecoder->TuneFadeOut = reinterpret_cast<LPPXT_FADEOUT>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_Fadeout"));
		lpPxtoneDecoder->TuneSetVolume = reinterpret_cast<LPPXT_SETVOLUME>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_SetVolume"));
		lpPxtoneDecoder->TuneStop = reinterpret_cast<LPPXT_STOP>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_Stop"));
		lpPxtoneDecoder->TuneIsStreaming = reinterpret_cast<LPPXT_ISSTREAMING>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_IsStreaming"));
		lpPxtoneDecoder->TuneSetLoop = reinterpret_cast<LPPXT_SETLOOP>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_SetLoop"));
		lpPxtoneDecoder->TuneGetInformation = reinterpret_cast<LPPXT_GETINFORMATION>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_GetInformation"));
		lpPxtoneDecoder->TuneGetRepeatMeas = reinterpret_cast<LPPXT_GETREPEATMEAS>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_GetRepeatMeas"));
		lpPxtoneDecoder->TuneGetPlayMeas = reinterpret_cast<LPPXT_GETPLAYMEAS>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_GetPlayMeas"));
		lpPxtoneDecoder->TuneGetName = reinterpret_cast<LPPXT_GETNAME>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_GetName"));
		lpPxtoneDecoder->TuneGetComment = reinterpret_cast<LPPXT_GETCOMMENT>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_GetComment"));
		lpPxtoneDecoder->TuneVomit = reinterpret_cast<LPPXT_VOMIT>(GetProcAddress(lpPxtoneDecoder->hInst, "pxtone_Tune_Vomit"));
	}
	else
	{
		SAFE_DELETE(lpPxtoneDecoder);
		// failed, release mem!
	}
	return lpPxtoneDecoder;
}

void DestroyDecoder(LPPxtoneDecoder lpPxtoneDecoder)
{
	if (lpPxtoneDecoder != NULL)
	{
		if (lpPxtoneDecoder->hInst != NULL)
		{
			FreeLibrary(lpPxtoneDecoder->hInst);
			lpPxtoneDecoder->hInst = NULL;
		}
		SAFE_DELETE(lpPxtoneDecoder);
	}
}

int MeasToMSecs(LPLoopInfo lpLoop, int measures)
{
	return (float)measures * (float)lpLoop->beats / lpLoop->bpm * 60000.0f;
}

