#include <windows.h>
#include <commctrl.h>
#include "resource.h"

extern HINSTANCE ins;
RECT MenuRect ={ 0, 0, 744,23};
HWND hMenuBar;
HMENU hMainMenu;

UCHAR ViewReport[]={0,0,0,1};

void CreateMenubar(HWND hwnd) 
{
    hMenuBar = CreateWindow(TOOLBARCLASSNAME,0, 
							WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | CCS_NODIVIDER | 
							CCS_NOPARENTALIGN | TBSTYLE_LIST | CCS_NOMOVEY | CCS_NORESIZE, 
                  			MenuRect.left, MenuRect.top, MenuRect.right, MenuRect.bottom, 
				  			hwnd, (HMENU)ID_MENUBAR, ins, NULL); 
     
    SNDMSG(hMenuBar,TB_SETSTYLE,0,SNDMSG(hMenuBar,TB_GETSTYLE,0,0)&~TBSTYLE_TRANSPARENT);
    SNDMSG(hMenuBar,TB_BUTTONSTRUCTSIZE,(WPARAM)(sizeof(TBBUTTON)),0);
	SNDMSG(hMenuBar,TB_SETBITMAPSIZE,0,MAKELONG(16,8));
   // SNDMSG(hMenuBar,(WM_USER + 96),(WPARAM)50,0);
    hMainMenu = LoadMenu(ins,MAKEINTRESOURCE(MAINMENU));
    TBBUTTON tbb;

	tbb.iBitmap		= I_IMAGENONE;
	tbb.idCommand	= IDMB_FILE;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		=  BTNS_AUTOSIZE |BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
	tbb.iString     = (int)"  &File";
	SNDMSG(hMenuBar, TB_INSERTBUTTON, 0, (LPARAM)(&tbb));

	tbb.iBitmap		= I_IMAGENONE;
	tbb.idCommand	= IDMB_EDIT;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_AUTOSIZE |BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
	tbb.iString     = (int)" &Edit";
	SNDMSG(hMenuBar, TB_INSERTBUTTON, 1, (LPARAM)(&tbb));


	tbb.iBitmap		= I_IMAGENONE;
	tbb.idCommand	= IDMB_VIEW;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_AUTOSIZE |BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
	tbb.iString     = (int)" &View";
	SNDMSG(hMenuBar, TB_INSERTBUTTON, 2, (LPARAM)(&tbb));

 
	tbb.iBitmap		= I_IMAGENONE;
	tbb.idCommand	= IDMB_FAVORITES;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_AUTOSIZE |BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
	tbb.iString     = (int)" F&avorites";
	SNDMSG(hMenuBar, TB_INSERTBUTTON, 3, (LPARAM)(&tbb));


	tbb.iBitmap		= I_IMAGENONE;
	tbb.idCommand	= IDMB_TOOLS;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_AUTOSIZE |BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
	tbb.iString     = (int)" &Tools";
	SNDMSG(hMenuBar, TB_INSERTBUTTON, 4, (LPARAM)(&tbb));


	tbb.iBitmap		= I_IMAGENONE;
	tbb.idCommand	= IDMB_DEBUG;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_AUTOSIZE |BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
	tbb.iString     = (int)" &Help";
	SNDMSG(hMenuBar, TB_INSERTBUTTON, 5, (LPARAM)(&tbb));
 	SNDMSG(hMenuBar,TB_AUTOSIZE ,0,0);	
return; 
}

void MenuBarDropDown(HWND hwnd, UINT ID, int nIndex) // for each button on menu bar return 0,1,2,3,4 ..
{ 
    RECT rc; 
	TPMPARAMS tpm; 
	HMENU hPopupMenu = NULL; 
	SNDMSG(hMenuBar, TB_GETRECT, (WPARAM)ID, (LPARAM)&rc); 
	MapWindowPoints(hwnd, HWND_DESKTOP, (LPPOINT)&rc, 2); 
	tpm.cbSize = sizeof(TPMPARAMS); 
	tpm.rcExclude = rc; 
	hPopupMenu = GetSubMenu(hMainMenu, nIndex); 
	TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rc.left, rc.bottom, hwnd, &tpm); 
//	DestroyMenu(hPopupMenu);
return;
}

void SetItemCheck(UCHAR w,UINT id)
{ 
     if(w){
        CheckMenuItem(hMainMenu, id,MF_CHECKED);
     }else{
        CheckMenuItem(hMainMenu, id,MF_UNCHECKED);
     }           
}

void SetMenuCheck(UINT n)
{
    SetItemCheck(0,ID_VIEW_LARGE);
    SetItemCheck(0,ID_VIEW_SMALL);
    SetItemCheck(0,ID_VIEW_LIST);
    SetItemCheck(0,ID_VIEW_DETAILS);
    SetItemCheck(1,n); 
  //  memset(ViewReport,0,4);
   // ViewReport[n-ID_VIEW_LARGE]=1;
   // ViewReport
}


