#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "toolbar.h"
#include "listview.h"
#include "main.h"
#include "resource.h"
//#include "mydebug.h"

extern HINSTANCE ins;
HWND hAddressBar;
HWND hComboBox;
HWND ComboEdit;
WNDPROC OldEditProc;
RECT AddressRect ={ 0, 64, 784,26};
RECT ComboRect ={ 55,2,655,220};

UCHAR P[32];

int DefaultImage=-1;

void DefaultComboView(UCHAR *Path)
{
    if(DefaultImage>=0)
    {
        SNDMSG(hComboBox, CB_SETCURSEL, 0, 0);               
        return;                   
    }
    HIMAGELIST himl=ImageList_Create(16,16,ILC_COLOR32	 | ILC_MASK,0,1);
	
	HICON hIcon=LoadIcon(ins,MAKEINTRESOURCE(ID_MAIN_ICON));
	DefaultImage=ImageList_ReplaceIcon(himl,-1,hIcon);
	DestroyIcon(hIcon);
	SNDMSG(hComboBox,CBEM_SETIMAGELIST , 0,(LPARAM)himl);
    
  	 
   	COMBOBOXEXITEM cbItem;
    memset(&cbItem,0,sizeof(COMBOBOXEXITEM));	
	cbItem.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
	cbItem.iItem = 0;
	cbItem.iImage = DefaultImage;
	cbItem.iSelectedImage = DefaultImage;
	cbItem.iIndent = 0;
	cbItem.iOverlay = 0;
	cbItem.pszText = Path;
    
    SNDMSG(hComboBox, CBEM_INSERTITEM, 0, (LPARAM)&cbItem);
    SNDMSG(hComboBox, CB_SETCURSEL, 0, 0);
    
 return;
}



int GoComboEdit(UCHAR isBack)
{
    UCHAR ThisDir[MAX_PATH];
	memset(ThisDir,0,MAX_PATH); UCHAR b=0,f=1;
    DWORD len = GetWindowTextLength( ComboEdit );
    if(!len) return 0;      
	len = GetWindowText(ComboEdit,ThisDir,MAX_PATH);
    int maxlen = (int)SNDMSG(hComboBox, CB_GETCOUNT,0,0);
    int selection = (int)SNDMSG(hComboBox, CB_GETCURSEL,0,0);
    if(isBack)
    {
       if(selection>0) b=1;
       selection++;
       if(selection==maxlen) f=0;      
    }
    Back_Forw_status(b,f);
	
    if(strcmp(ThisDir,"Computer\\") == 0)
	{
		ExploreDrive();
		return 1;
	}
	if(ThisDir[1] != ':'){ return 0; }
	if(len == 2)
	{
    	ThisDir[len] = '\\';
		ThisDir[len+1] = 0;
    }
    else
    {
	    ThisDir[len] = 0;	
	}
    sprintf(HLP.ThisDir,"%s\0",ThisDir);
    HLP.IsBack=isBack;
    CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);					
return 1;
} 
						
LRESULT CALLBACK EditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if((message == WM_KEYUP) && (wParam == VK_RETURN))
    { 
          UCHAR ThisDir[MAX_PATH];
          memset(ThisDir,0,MAX_PATH);
          DWORD len = GetWindowText(ComboEdit,ThisDir,MAX_PATH);
          if(len>2)
          {     
              if(strcmp(ThisDir,"Computer\\") == 0)
	          {
		          SetWindowText(ComboEdit,"");
                  ExploreDrive();
	          }
	          if(ThisDir[1] == ':')
	          {
	               if(len == 2)
	               {
    	               ThisDir[len] = '\\';
		               ThisDir[len+1] = 0;
                   }
                   else
                   {
	                  ThisDir[len] = 0;	
	               }
	               SetWindowText(ComboEdit,"");
	               if(ThisDir[len-1] != '\\') ThisDir[len] ='\\';
                   sprintf(HLP.ThisDir,"%s\0",ThisDir);
                   HLP.IsBack=0;
                   CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
              } 
           }
           else // case C:   verify
           {
              if(isalpha(ThisDir[0]))
              {
                 ThisDir[0] = toupper(ThisDir[0]);
                 ThisDir[1] = ':';
                 ThisDir[2] = '\\';
                 ThisDir[3] = 0;
                 SetWindowText(ComboEdit,"");
                 sprintf(HLP.ThisDir,"%s\0",ThisDir);
                 HLP.IsBack=0;
                 CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
              }
           } 
    }             
	return CallWindowProc(OldEditProc, hwnd, message, wParam, lParam);
}


void CreateAddressbar(HWND hwnd) 
{ 
    hAddressBar = CreateWindow(TOOLBARCLASSNAME,0, 
							WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | CCS_NODIVIDER | 
							CCS_NOPARENTALIGN | TBSTYLE_LIST | CCS_NOMOVEY | CCS_NORESIZE| WS_TABSTOP, 
                 			 AddressRect.left, AddressRect.top, AddressRect.right, AddressRect.bottom, 
							  hwnd, (HMENU)ID_ADDRESSBAR, ins, NULL); 

    SNDMSG(hAddressBar,TB_SETSTYLE,0,SNDMSG(hAddressBar,TB_GETSTYLE,0,0)&~TBSTYLE_TRANSPARENT);
    SNDMSG(hAddressBar,TB_BUTTONSTRUCTSIZE,(WPARAM)(sizeof(TBBUTTON)),0);
	SNDMSG(hAddressBar,TB_SETBITMAPSIZE,0,MAKELONG(20,20));
	//SNDMSG(hAddressBar,TB_SETBUTTONSIZE,0,MAKELONG(20,20));
    
    TBBUTTON tbb;

	tbb.iBitmap		= I_IMAGENONE;
	tbb.idCommand	= ID_ADB_STATIC;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		=  BTNS_AUTOSIZE |BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
	tbb.iString     = (int)"  A&ddress";
	SNDMSG(hAddressBar, TB_INSERTBUTTON, 0, (LPARAM)(&tbb));

	tbb.iBitmap		= 0;
	tbb.idCommand	= 0;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
	tbb.iString     = 0;
	SNDMSG(hAddressBar, TB_INSERTBUTTON,1, (LPARAM)(&tbb));

    hComboBox = CreateWindow( WC_COMBOBOXEX, 0,  WS_CHILD|WS_VISIBLE|CBS_DROPDOWN ,
        ComboRect.left, ComboRect.top, ComboRect.right, ComboRect.bottom,
        hAddressBar,(HMENU)ID_ADDRESSBAR, ins, NULL);
       
   HIMAGELIST _a_ = ImageList_Create(20,20,ILC_COLOR32|ILC_MASK,0,1);
   HIMAGELIST _b_ = ImageList_Create(20,20,ILC_COLOR32|ILC_MASK,0,1);

    HBITMAP hBmp;
// ----------------------- load ON images
    hBmp = LoadBitmap(ins,MAKEINTRESOURCE(BMP_GO_ON));
    ImageList_Add(_a_, hBmp, 0);  // ImageList_AddMasked(ImageListToolBarOn, hBmp, RGB(255,0,255)); 
	SNDMSG(hAddressBar, TB_SETIMAGELIST, 0,( LPARAM)_a_);
    DeleteObject(hBmp);
// ----------------------- load HOT images
    hBmp = LoadBitmap(ins,MAKEINTRESOURCE(BMP_GO_HOT));
    ImageList_Add(_b_, hBmp, 0);  // ImageList_AddMasked(ImageListToolBarHot, hBmp, RGB(255,0,255)); 
	SNDMSG(hAddressBar, TB_SETHOTIMAGELIST, 0, (LPARAM)_b_);
   
    DeleteObject(hBmp);
    
   	tbb.iBitmap		= 655;
	tbb.idCommand	= 0;
	tbb.fsState		= 0;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
	tbb.iString     = 0;
	SNDMSG(hAddressBar, TB_INSERTBUTTON,2, (LPARAM)(&tbb));

  	tbb.iBitmap		= 0;
	tbb.idCommand	= ID_ADB_GO;
	tbb.fsState		= TBSTATE_ENABLED;
	tbb.fsStyle		= BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT ;
	tbb.dwData		= 0;
 	tbb.iString     = (int)" Go";
	SNDMSG(hAddressBar, TB_INSERTBUTTON, 3, (LPARAM)&tbb);

    UpdateWindow(hAddressBar); 
    SNDMSG(hAddressBar,TB_AUTOSIZE ,0,0);
 	ComboEdit = (HWND)SNDMSG(hComboBox, CBEM_GETEDITCONTROL, 0, 0);
 	OldEditProc= (WNDPROC) SetWindowLong(ComboEdit, GWL_WNDPROC, (LPARAM)EditProc);
return; 
}


void AddComboView(UCHAR *Path)
{
    SHFILEINFO sfi;
    HIMAGELIST himl;
 	ZeroMemory(&sfi,sizeof(SHFILEINFO));
    himl=(HIMAGELIST)SHGetFileInfo(Path,0,&sfi,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	if(himl == 0)
	{
         return;
    }
   	SNDMSG(hComboBox,CBEM_SETIMAGELIST , 0,(LPARAM)himl);
    int maxlen = (int)SNDMSG(hComboBox, CB_GETCOUNT,0,0);
    int selection = (int)SNDMSG(hComboBox, CB_GETCURSEL,0,0);

 	if(selection!=CB_ERR)
	{
       selection++;
       if(selection<maxlen)
       {
          maxlen--;
          for(;maxlen>=selection;maxlen--)
          {
            SNDMSG(hComboBox, CB_DELETESTRING,(WPARAM)maxlen,0);
          }
          maxlen++;        
       }              
    }   
    Back_Forw_status(1,0);
    
   	COMBOBOXEXITEM cbItem;
    memset(&cbItem,0,sizeof(COMBOBOXEXITEM));	
	cbItem.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
	cbItem.iItem = maxlen;
	cbItem.iImage = sfi.iIcon;
	cbItem.iSelectedImage = sfi.iIcon;
	cbItem.iIndent = 1;
	cbItem.iOverlay = 1;
	cbItem.pszText = Path;
    
    SNDMSG(hComboBox, CBEM_INSERTITEM, 0, (LPARAM)&cbItem);
    SNDMSG(hComboBox, CB_SETCURSEL, maxlen, 0);
    
 return;
}


void Func_Back()
{ 
  int maxlen = (int)SNDMSG(hComboBox, CB_GETCOUNT,0,0);
  int selection = (int)SNDMSG(hComboBox, CB_GETCURSEL,0,0);
  
  if(selection>0)
  {
      selection--;
  }
   SNDMSG(hComboBox, CB_SETCURSEL, selection, 0);
   GoComboEdit(1);
}



void Func_Forw()
{
  int maxlen = (int)SNDMSG(hComboBox, CB_GETCOUNT,0,0);
  int selection = (int)SNDMSG(hComboBox, CB_GETCURSEL,0,0);
  if(selection!=CB_ERR)
  {
     selection++;
     if(selection<maxlen)
     {
       SNDMSG(hComboBox, CB_SETCURSEL, selection, 0);
       GoComboEdit(1);
     }
  }
}


