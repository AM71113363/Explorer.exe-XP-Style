#include <windows.h>
#include <commctrl.h>
#include "listview.h"
#include "operations.h"
#include "main.h"
#include "resource.h"
//#include "mydebug.h"

UCHAR P[32];

extern HINSTANCE ins;
HWND hToolBar;
RECT ToolRect ={ 0, 24, 784, 39};
HIMAGELIST ImageListToolBarOn; 
HIMAGELIST ImageListToolBarHot;
extern HMENU hMainMenu;

void CreateToolbar(HWND hwnd) 
{                // ----------  create toolbar window
    hToolBar = CreateWindow(TOOLBARCLASSNAME,0, 
					WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_NODIVIDER | 
					CCS_NOPARENTALIGN | TBSTYLE_LIST | CCS_NOMOVEY | CCS_NORESIZE, 
                    ToolRect.left, ToolRect.top, ToolRect.right, ToolRect.bottom, 
				    hwnd, (HMENU)ID_TOOLBAR, ins, NULL); 

    SNDMSG(hToolBar,TB_SETSTYLE,0,SNDMSG(hToolBar,TB_GETSTYLE,0,0)&~TBSTYLE_TRANSPARENT);
   	SNDMSG(hToolBar,TB_BUTTONSTRUCTSIZE,(WPARAM)(sizeof(TBBUTTON)),0);
	SNDMSG(hToolBar,TB_SETBITMAPSIZE,0,MAKELONG(24,24));
	SNDMSG(hToolBar,TB_SETEXTENDEDSTYLE,0,(LPARAM)1); //TBSTYLE_EX_DRAWDDARROWS = 1  TBSTYLE_EX_MIXEDBUTTONS = 8
   // hMainMenu = LoadMenu(ins,MAKEINTRESOURCE(MAINMENU));
    
// ------------------------  create imagelist   
  
    ImageListToolBarOn = ImageList_Create(24,24,ILC_COLOR32|ILC_MASK,0,50);
	ImageListToolBarHot    = ImageList_Create(24,24,ILC_COLOR32|ILC_MASK,0,50);
    HBITMAP hBmp;
// ----------------------- load ON images
    hBmp = LoadBitmap(ins,MAKEINTRESOURCE(BMP_TOOLBAR_ON));
    // ImageList_Add(ImageListToolBarOn, hBmp, 0);  
    ImageList_AddMasked(ImageListToolBarOn, hBmp, RGB(0,0,0)); 
	SNDMSG(hToolBar, TB_SETIMAGELIST, 0,( LPARAM)ImageListToolBarOn);
    DeleteObject(hBmp);
// ----------------------- load HOT images
    hBmp = LoadBitmap(ins,MAKEINTRESOURCE(BMP_TOOLBAR_HOT));
    // ImageList_Add(ImageListToolBarHot, hBmp, 0);  
    ImageList_AddMasked(ImageListToolBarHot, hBmp, RGB(0,0,0)); 
	SNDMSG(hToolBar, TB_SETHOTIMAGELIST, 0, (LPARAM)ImageListToolBarHot);
    DeleteObject(hBmp);
    
// ----------------------- ad buttons images    BTNS_AUTOSIZE |BTNS_SHOWTEXT|BTNS_NOPREFIX| BTNS_DROPDOWN;
    TBBUTTON tbb;

	tbb.iBitmap		= 0;
	tbb.idCommand	= IDTB_BACK;
	tbb.fsState		= 0;
	tbb.fsStyle		= BTNS_AUTOSIZE | BTNS_SHOWTEXT | BTNS_DROPDOWN;
	tbb.dwData		= 0;
	tbb.iString     = (int)"Back";
	SNDMSG(hToolBar, TB_INSERTBUTTON, 0, (LPARAM)(&tbb));

	tbb.iBitmap		= 1; // forward
	tbb.idCommand	= IDTB_FORW;
	tbb.fsState		= 0;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_DROPDOWN;
	tbb.dwData		= 0;
    tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 1, (LPARAM)&tbb);
	
	tbb.iBitmap		= 28; // folder up
	tbb.idCommand	= IDTB_UP;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 2, (LPARAM)&tbb);

	tbb.iBitmap		= -1;
	tbb.idCommand	= 0;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 3, (LPARAM)(&tbb));
	
	tbb.iBitmap		= 17;
	tbb.idCommand	= IDTB_SEARCH; //search
	tbb.fsState		= TBSTATE_ENABLED; //TBSTATE_PRESSED;
	tbb.fsStyle		= BTNS_AUTOSIZE | BTNS_SHOWTEXT;
	tbb.dwData		= 0;
	tbb.iString     = (int)"Search";
	SNDMSG( hToolBar, TB_INSERTBUTTON, 4, (LPARAM)&tbb);

	tbb.iBitmap		= -1;
	tbb.idCommand	= 0;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG( hToolBar,TB_INSERTBUTTON,5,(LPARAM)(&tbb));

	tbb.iBitmap		= 9;
	tbb.idCommand	= IDTB_HOME;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;//BTNS_AUTOSIZE | BTNS_SHOWTEXT;
	tbb.dwData		= 0;
	tbb.iString     = 0;//"HOME";
	SNDMSG(hToolBar, TB_INSERTBUTTON, 6, (LPARAM)&tbb);
    
	tbb.iBitmap		= 8;
	tbb.idCommand	= IDTB_REFRESH;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 7, (LPARAM)&tbb);

	tbb.iBitmap		= -1;
	tbb.idCommand	= 0;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 8, (LPARAM)&tbb);

	tbb.iBitmap		= 10;
	tbb.idCommand	= IDTB_DELETE;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG( hToolBar,TB_INSERTBUTTON,9,(LPARAM)(&tbb));

	tbb.iBitmap		= 5;
	tbb.idCommand	= IDTB_CUTMODE;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 10, (LPARAM)&tbb);

	tbb.iBitmap		= 6;
	tbb.idCommand	= IDTB_COPYMODE;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 11, (LPARAM)&tbb);

	tbb.iBitmap		= 7;
	tbb.idCommand	= IDTB_PASTE;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 12, (LPARAM)&tbb);

	tbb.iBitmap		= -1;
	tbb.idCommand	= 0;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 0xd, (LPARAM)(&tbb));
	
	
	tbb.iBitmap		= 31;
	tbb.idCommand	= IDTB_NEWFOLDER;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 14, (LPARAM)&tbb);

	tbb.iBitmap		= -1;
	tbb.idCommand	= 0;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
 	tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 15, (LPARAM)(&tbb));

	tbb.iBitmap		= 32;
	tbb.idCommand	= IDTB_VIEWMODE;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_WHOLEDROPDOWN;
	tbb.dwData		= 0;
    tbb.iString     = 0;
	SNDMSG(hToolBar, TB_INSERTBUTTON, 16, (LPARAM)&tbb);
	SNDMSG(hToolBar,TB_AUTOSIZE ,0,0);	
return; 
}


void ToolBarButtonDropDown(HWND hwnd, LPNMHDR lpnm, LPNMTOOLBAR lpnmTB, int nIndex) 
{ 
    RECT rc; 
	TPMPARAMS tpm; 
	HMENU hPopupMenu = NULL; 

	SNDMSG(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc); 
	MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2); 
	tpm.cbSize = sizeof(TPMPARAMS); 
	tpm.rcExclude = rc; 
	hPopupMenu = GetSubMenu(hMainMenu, nIndex); 
	TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rc.left, rc.bottom, hwnd, &tpm); 
	
return;
}

void Back_Forw_status(UCHAR b,UCHAR f)
{
	if(b==0){
	  SNDMSG(hToolBar, TB_ENABLEBUTTON,(WPARAM) IDTB_BACK,(LPARAM) MAKELONG(FALSE, 0)); 
    }else{
	  SNDMSG(hToolBar, TB_ENABLEBUTTON,(WPARAM) IDTB_BACK,(LPARAM) MAKELONG(TRUE, 0)); 
    }
    if(f==0){
	  SNDMSG(hToolBar, TB_ENABLEBUTTON,(WPARAM) IDTB_FORW,(LPARAM) MAKELONG(FALSE, 0)); 
    }else{
	  SNDMSG(hToolBar, TB_ENABLEBUTTON,(WPARAM) IDTB_FORW,(LPARAM) MAKELONG(TRUE, 0)); 
    }
}

void Func_Up()
{
    //if(strcmp(CurrentDirListView,"Computer\\") return; // no Up for Computer Path
	DWORD n = strlen(CurrentDirListView);
	UCHAR *p;
	if(n == 0) return;
	if(n <4 )// its a computer drive
	{
		ExploreDrive();
		return;
	}
	if(CurrentDirListView[n-1]=='\\') CurrentDirListView[n-1]=0;
	p=strrchr(CurrentDirListView,'\\');
	if(p)
	{
	   p[1]=0;
       sprintf(HLP.ThisDir,"%s\0",CurrentDirListView);
       HLP.IsBack=0;
       CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
    }
return;
}


void ToolBarFuncDELETE()
{
   if(IsValidOperation()==YES)
   {
       if(SetOperation(1,CurrentDirListView)==YES)
       {
            SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_CUTMODE,(LPARAM) MAKELONG(FALSE, 0)); 
            SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_COPYMODE,(LPARAM) MAKELONG(FALSE, 0));
            SMS(0,"Done");
            ExecuteOperation();
       }
   }
}

void ToolBarFuncCUT()
{ 
    if(IsValidOperation()==YES)
    {
        if(SetOperation(2,CurrentDirListView)==YES)
        {
            SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_CUTMODE,(LPARAM) MAKELONG(TRUE, 0)); 
            SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_COPYMODE,(LPARAM) MAKELONG(FALSE, 0)); 
        }
    }
    else
    {
        SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_CUTMODE,(LPARAM) MAKELONG(FALSE, 0)); 
        SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_COPYMODE,(LPARAM) MAKELONG(FALSE, 0)); 
        SetOperation(0,NULL);
    }
}

void ToolBarFuncCOPY()
{
    if(IsValidOperation()==YES)
    {
         if(SetOperation(3,CurrentDirListView)==YES)
         {
              SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_COPYMODE,(LPARAM) MAKELONG(TRUE, 0)); 
              SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_CUTMODE,(LPARAM) MAKELONG(FALSE, 0)); 
         }
    }
    else
    {
         SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_CUTMODE,(LPARAM) MAKELONG(FALSE, 0)); 
         SNDMSG(hToolBar, TB_CHECKBUTTON,(WPARAM) IDTB_COPYMODE,(LPARAM) MAKELONG(FALSE, 0)); 
         SetOperation(0,NULL);
    }  
}

void ToolBarFuncPASTE()
{ 
    if(IsValidCutCopyOperation(CurrentDirListView)==YES)
    {
          ExecuteOperation();                 
    }   
}




