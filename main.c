#include <windows.h>
#include "resource.h"

extern HINSTANCE ins;
HWND AngleFoto;
RECT AngleRect ={ 745,0,39,23};

void CenterOnScreen(HWND hwnd)
{
     RECT rC, rD;
     SystemParametersInfo(SPI_GETWORKAREA, 0, &rD, 0);
     GetWindowRect(hwnd, &rC);
     int nX=((rD.right - rD.left) / 2) -((rC.right - rC.left) / 2);
     int nY=((rD.bottom - rD.top) / 2) -((rC.bottom - rC.top) / 2);
     SetWindowPos(hwnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
return;
}

void CreateAngleFoto(HWND hwnd)
{
	HBITMAP hFoto = LoadBitmap(ins,MAKEINTRESOURCE(BMP_WINDOW));
    AngleFoto = CreateWindow("Static","",WS_VISIBLE|WS_CHILD|SS_BITMAP,
            AngleRect.left, AngleRect.top, AngleRect.right,AngleRect.bottom,hwnd,0,ins,NULL); 
    SNDMSG(AngleFoto,STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)(HANDLE)hFoto);
return;	
}

