#include <windows.h>
#include <commctrl.h>
#include "resource.h"
//#include "mydebug.h"

extern HINSTANCE ins;
HWND hStatusBar;

void CreateStatusbar(HWND hwnd)
{
    RECT rc = { 0, 0, 0, 0 };
    hStatusBar = CreateWindow(STATUSCLASSNAME, 0, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 
        rc.left, rc.top, rc.right, rc.bottom, hwnd, (HMENU)ID_STATUSBAR, ins, 0);
        
    GetClientRect(hwnd, &rc);
    int nHalf = rc.right / 2;
    int nParts[4] = { nHalf, nHalf + nHalf / 3, nHalf + nHalf * 2 / 3, -1 };
    SNDMSG(hStatusBar, SB_SETPARTS, 4, (LPARAM)&nParts);

    // Put some texts into each part of the status bar and setup each part
    SNDMSG(hStatusBar, SB_SETTEXT, 0, (LPARAM)"Status Bar: Part 1");
    SNDMSG(hStatusBar, SB_SETTEXT, 1| SBT_POPOUT, (LPARAM)"Part 2");
    SNDMSG(hStatusBar, SB_SETTEXT, 2| SBT_POPOUT, (LPARAM)"Part 3");
    SNDMSG(hStatusBar, SB_SETTEXT, 3| SBT_POPOUT, (LPARAM)"Part 4");
  return;
}

void SMS(int x, UCHAR *sms)
{
    SNDMSG(hStatusBar, SB_SETTEXT, x, (LPARAM)sms);
return;
}


