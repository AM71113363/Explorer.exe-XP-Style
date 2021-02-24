#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "WinMain.h"
#include "main.h"
#include "menubar.h"
#include "toolbar.h"
#include "addressbar.h"
#include "statusbar.h"
#include "treeview.h"
#include "listview.h"
#include "operations.h"
#include "resource.h"
//#include "mydebug.h"

UCHAR P[32];
UCHAR RenameBufFile[MAX_PATH];
int   RenameBufFileStatus=0;

HANDLE WINDOWBUSY=NULL;

   
void ExploreDirThread(VOID *H)
{
     HELP *p=(HELP*)H;
     ResetEvent(WINDOWBUSY);
     EnableWindow(MainHWND,0);
     ExploreDir(p->ThisDir,p->IsBack);
     SetEvent(WINDOWBUSY); 
     EnableWindow(MainHWND,1);
     
}

UINT tempIndex;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void MoveObjects(int difx, int dify, int z);
char szClassName[ ] = "WindowsAppXpExplorer";



int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument,  int nFunsterStil)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
    ins = hThisInstance;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (ins,MAKEINTRESOURCE(ID_MAIN_ICON));
    wincl.hIconSm = LoadIcon (ins,MAKEINTRESOURCE(ID_MAIN_ICON));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindow(szClassName,"XP-Explorer",WS_OVERLAPPEDWINDOW,
    MainRect.left, MainRect.top, MainRect.right, MainRect.bottom,
    HWND_DESKTOP, NULL, ins, NULL);

    ShowWindow (hwnd, nFunsterStil);

    while (GetMessage (&messages, NULL, 0, 0))
    {
  //      if(!IsDialogMessage(hWnd, &messages))
    //    {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
  //      }
    }
    return messages.wParam;
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {    
        case WM_NOTIFY:
        {//WM_NOTIFY
        if(WaitForSingleObject(WINDOWBUSY,0))
        break;
        switch(LOWORD(wParam))
        {
            case ID_TOOLBAR:
                if(_lpnm->code == TBN_DROPDOWN)
                {
                     if(_lpnmTB->iItem == IDTB_VIEWMODE)
                               ToolBarButtonDropDown(hwnd, _lpnm, _lpnmTB, 2) ;
                } 
             break;
            case ID_TREE_VIEW:{   //-----------------------------------------------------------
                switch(_lpnm->code)
                {
                    case TVN_ITEMEXPANDED:{
                        RefreshTree(((NM_TREEVIEW FAR *) lParam));
                    }break;
                    case TVN_SELCHANGED:{
                        HTREEITEM hCurrent =TreeView_GetSelection(hTreeView);
                        UCHAR *ThisDir=GetFullPath(hCurrent);
                        if(ThisDir != 0)
                        {
                            sprintf(HLP.ThisDir,"%s\0",ThisDir);
                            HLP.IsBack=0;
                            CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
                        }
                    }break;
                }
            }break;
            case ID_LIST_VIEW:{    //--------------------------------------------------
                switch(_lpnm->code)
                {
                    case LVN_BEGINDRAG:{
                        if(IsValidOperation() == YES)
                        {
                            if(SetOperation(2,CurrentDirListView)==YES)
                            {
                                SNDMSG(hListView,LISTVIEW_BEGINDRAG,wParam,lParam);
                            }
                        }
                    }break;
                    
                    case LVN_KEYDOWN:{
                        LV_KEYDOWN *pnkd = (LV_KEYDOWN FAR *)lParam;  
                        if(pnkd->wVKey==0x0d)
                        {
				            UCHAR ThisDir[MAX_PATH];  memset(ThisDir,0,MAX_PATH);
                            int iSelected =GetSelectedInfoText(CurrentDirListView,ThisDir);
				            if(iSelected==ThisIsDir)
				            {
                                strcat(ThisDir,"\\");
                                sprintf(HLP.ThisDir,"%s\0",ThisDir);
                                HLP.IsBack=0;
                                CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
                                SetFocus(hListView);
                            }
                            else if(iSelected==ThisIsFile)
                            {
                                ShellExecute(hwnd, "open", ThisDir, NULL, NULL, SW_NORMAL);
                            }  
                        } 
                    }break;
                    case LVN_BEGINLABELEDITA:{
                        memset(RenameBufFile,0,MAX_PATH);
                        RenameBufFileStatus=GetSelectedInfoText(CurrentDirListView,RenameBufFile);
                    }break;
                    case LVN_ENDLABELEDITA:{
                        LV_DISPINFO *ptvdi = (LV_DISPINFO FAR *) lParam;
                        UCHAR *p = ptvdi[0].item.pszText;
                        if(RenameBufFileStatus>0 && p!=NULL)
                        {
                            UCHAR New[MAX_PATH];
                            sprintf(New,"%s%s\0",CurrentDirListView,p);
                            MoveFile(RenameBufFile,New);
                           	sprintf(HLP.ThisDir,"%s\0",CurrentDirListView);
                            HLP.IsBack=1;
                            CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
                            SetFocus(hListView);
                        }
                        RenameBufFileStatus=0;
                    }break;
				    case NM_DBLCLK:{	
				        UCHAR ThisDir[MAX_PATH]; memset(ThisDir,0,MAX_PATH);
                        int iSelected =GetSelectedInfoText(CurrentDirListView,ThisDir);
				        if(iSelected==ThisIsDir)
				        {
                            strcat(ThisDir,"\\");
                            sprintf(HLP.ThisDir,"%s\0",ThisDir);
                            HLP.IsBack=0;
                            CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
                            SetFocus(hListView); 
                            //ResetListValue();
                        }
                        else if(iSelected==ThisIsFile)
                        {
                            ShellExecute(hwnd, "open", ThisDir, NULL, NULL, SW_NORMAL);
                        } 			
                    }break;
				    case LVN_ITEMCHANGED:{
                        NM_LISTVIEW *pnmv = (NM_LISTVIEW FAR *) lParam;
                        if(pnmv->uChanged==LVIF_STATE)
                        {
                            UpdateListValue(pnmv->iItem,ListView_GetItemState(hListView,pnmv->iItem,LVIS_SELECTED));
                        }
                    }break;
                }
            }
            default:
              break;
             
        }//WM_NOTIFY //switch
        }break; 
        case WM_CREATE:
        {   
        	InitCommonControls(); // load common libraries
        	WINDOWBUSY = CreateEvent(NULL, TRUE, FALSE, "WWFF");
        	SetEvent(WINDOWBUSY); 
        	MainHWND=hwnd;
        	CenterOnScreen(hwnd);
      	    // save the current size overwrite it
        	GetClientRect(hwnd, &MainRect); 
        	// start creation of menubar
        	CreateMenubar(hwnd);
          //put foto after menubar
        	CreateAngleFoto(hwnd);
        	//put toolbar
        	CreateToolbar(hwnd) ;
            //put addressbar
            CreateAddressbar(hwnd);
          //put statusbar
            CreateStatusbar(hwnd);
              //put treeview
            CreateTreeView(hwnd);
           //put listview
            CreateListView(hwnd);
            ExploreDrive();
            //------------------------ end creating objects -------------------------
        }
        break; // end WM_CREATE
       case WM_CONTEXTMENU:
       {
           if((HWND) wParam == hListView)
           {
              int selected;HMENU hmenu,f;
              POINT PosMenu; 
              UCHAR ThisDir[MAX_PATH];
              GetCursorPos(&PosMenu);

              selected =GetSelectedInfoText(CurrentDirListView,ThisDir);
              hmenu = LoadMenu(ins, MAKEINTRESOURCE(MENU_RIGHT)); 
							
		      if(selected==ThisIsDir)
	          {
                    f=GetSubMenu(hmenu,1);
              }
              else if(selected==ThisIsFile)
              {
                    f=GetSubMenu(hmenu,0);
              }
              else if(selected==0)
              {
                  f=GetSubMenu(hmenu,2);
              }  
                
              TrackPopupMenuEx(f, TPM_LEFTALIGN|TPM_LEFTBUTTON, PosMenu.x, PosMenu.y,hwnd, NULL);
              DestroyMenu(hmenu);               
           }                                                                                                                                                                                            
       }        
       break;
        case WM_SIZE:
        {
            if(wParam == SIZE_MINIMIZED) return;
            if((LOWORD (lParam)) < 400 ) return;
            if((HIWORD (lParam)) < 300 ) return;
            int difx = LOWORD (lParam) - MainRect.right;
            int dify = HIWORD (lParam) - MainRect.bottom;
            MoveObjects(difx, dify,(int)LOWORD (lParam));
        }
        break;
        case WM_COMMAND:
         switch(LOWORD(wParam))
         {
//adressbar  functions
         	case ID_ADDRESSBAR:
                 if(HIWORD(wParam) == CBN_SELCHANGE)
                 {
                        GoComboEdit(1);
                 }
			break;
			case ID_ADB_GO:
                 GoComboEdit(0);
            break;
//menubar functions
            case IDMB_FILE:
                 MenuBarDropDown(hwnd, IDMB_FILE,0);
            break;		
			case IDMB_EDIT:
                 MenuBarDropDown(hwnd, IDMB_EDIT,1);
            break;    
			case IDMB_VIEW:
                 MenuBarDropDown(hwnd, IDMB_VIEW,2);
            break;    
			case IDMB_FAVORITES:
                 MenuBarDropDown(hwnd, IDMB_FAVORITES,3);
            break;    
			case IDMB_TOOLS:
                 MenuBarDropDown(hwnd, IDMB_TOOLS,4);
            break;    
			case IDMB_DEBUG:
                 MenuBarDropDown(hwnd, IDMB_DEBUG,5);
            break;    
         //   case IDM_DEBUG_ERROR:
         //   {
          //       SetDebugLevel(1);
          //       SetItemCheck(1,IDM_DEBUG_ERROR);
          //       SetItemCheck(0,IDM_DEBUG_ALL);
          //  }  break;
            case ID_VIEW_LARGE:
            {
                 ChangeListViewStyle(LVS_ICON);
                 SetMenuCheck(ID_VIEW_LARGE);
            }
            break;
            case ID_VIEW_SMALL:
            {
                 ChangeListViewStyle(LVS_SMALLICON);
                 SetMenuCheck(ID_VIEW_SMALL);
            }
            break;
            case ID_VIEW_LIST:
            {
                 ChangeListViewStyle(LVS_LIST);
                 SetMenuCheck(ID_VIEW_LIST);
            }
            break;
            case ID_VIEW_DETAILS:
            {
                 ChangeListViewStyle(LVS_REPORT);
                 SetMenuCheck(ID_VIEW_DETAILS);
            }
            break;
                     
//toolbar functions
            case IDTB_BACK:
                 Func_Back();	
            break;
            case IDTB_FORW:
                 Func_Forw();
            break;
            case IDTB_UP:
                 Func_Up();
            break;
            case IDTB_HOME:{
                  ResetTreeView(); ExploreDrive();
            }break;
            case IDTB_REFRESH:
        	case MENU_RIGHT_UPDATE:
            {
                 if(CurrentDirListView[0]>0)
                 { 
                     sprintf(HLP.ThisDir,"%s\0",CurrentDirListView);
                     HLP.IsBack=1;
                     CreateThread(0,0,(LPTHREAD_START_ROUTINE)ExploreDirThread,(LPVOID)&HLP,0,0);
                 }else{ ExploreDrive(); }
            }
            break;                 
            case IDTB_DELETE:
            case MENU_RIGHT_DELETE:
                 ToolBarFuncDELETE();  
            break;
            case IDTB_CUTMODE:
                 ToolBarFuncCUT();
            break;
            case IDTB_COPYMODE:
                 ToolBarFuncCOPY();
            break;
            case IDTB_PASTE:
                 ToolBarFuncPASTE();
            break;  
            case IDTB_NEWFOLDER:
	        case MENU_RIGHT_NEWFOLDER:
            {
                 if(CurrentDirListView[1]==':')
                 {
                    UCHAR ThisDir[MAX_PATH]; int index;
			        sprintf(ThisDir,"%sNewFolder\0",CurrentDirListView);
                    if(CreateDirectory(ThisDir,0)!=0)
                    {
                     index=AddData(0,ThisDir,"NewFolder","0","","DIR",0); 
                     ListView_EnsureVisible(hListView,index,FALSE);  
                     ListView_SetItemState(hListView,index,LVIS_SELECTED,LVIS_SELECTED);
                     ListView_EditLabel(hListView,index);                         
                    }                              
                 }
            }
            break;       

	        case MENU_RIGHT_OPEN:
            {
                 UCHAR ThisDir[MAX_PATH];
			     memset(ThisDir,0,MAX_PATH);
			     int iSelected =GetSelectedInfoText(CurrentDirListView,ThisDir);
				 if(iSelected==ThisIsFile)
                 {
                        ShellExecute(hwnd, "open", ThisDir, NULL, NULL, SW_NORMAL);
                 }	
	        }
	        break;
	        case MENU_RIGHT_EDIT:
            {
                 UCHAR ThisDir[MAX_PATH];
			     memset(ThisDir,0,MAX_PATH);
			     int iSelected =GetSelectedInfoText(CurrentDirListView,ThisDir);
				 if(iSelected==ThisIsFile)
                 {
                        ShellExecute(hwnd, "edit", ThisDir, NULL, NULL, SW_NORMAL);
                 }	
	        }
	        break;
            
            case 9098:
               MessageBox (NULL, "File Editor for Windows !\n Using the Win32 API" , "About...", 0);
         }
      break;
        case WM_DESTROY:
        {
             CloseHandle(WINDOWBUSY);
            PostQuitMessage (0);
        }
            break;
        default: 
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void MovePos(HWND target,int x,int y,int w,int h)
{
   if( !MoveWindow(target, x, y, w, h,1))
            SetWindowPos(target, NULL, x, y, w, h, SWP_NOZORDER );
   RECT	rc;
   GetClientRect(target,&rc);
   InvalidateRect(target,&rc,1);
return;
}

void MoveObjects(int difx, int dify, int z)
{
    MovePos(AngleFoto, AngleRect.left +difx, AngleRect.top, AngleRect.right,AngleRect.bottom);
    MovePos(hMenuBar, MenuRect.left, MenuRect.top, MenuRect.right +difx,MenuRect.bottom);
    MovePos(hToolBar, ToolRect.left, ToolRect.top, ToolRect.right +difx,ToolRect.bottom);
    MovePos(hAddressBar, AddressRect.left, AddressRect.top, AddressRect.right +difx,AddressRect.bottom);
    MovePos(hTreeView, TreeRect.left, TreeRect.top, TreeRect.right, TreeRect.bottom + dify);
    MovePos(hListView, ListRect.left, ListRect.top, ListRect.right + difx, ListRect.bottom + dify);
    MovePos(hComboBox, ComboRect.left, ComboRect.top, ComboRect.right +difx, ComboRect.bottom);
  
    SNDMSG(hAddressBar,  TB_DELETEBUTTON,(WPARAM)2,0);
    TBBUTTON tbb;
   	tbb.iBitmap		= (ComboRect.right +difx);
	tbb.idCommand	= 5003;
	tbb.fsState		= 0;
	tbb.fsStyle		= BTNS_SEP;
	tbb.dwData		= 0;
	tbb.iString     = 0;
	SNDMSG(hAddressBar, TB_INSERTBUTTON,2, (LPARAM)(&tbb));

    int nHalf = z / 2;
    int nParts[] = { nHalf, nHalf + nHalf / 3, nHalf + nHalf * 2 / 3, -1 };
    SNDMSG(hStatusBar, SB_SETPARTS, 4, (LPARAM)&nParts);
    SNDMSG(hAddressBar,TB_AUTOSIZE ,0,0); 
    SNDMSG(hStatusBar, WM_SIZE, 0, 0);                    
return;
}    

