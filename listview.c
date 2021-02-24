#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "statusbar.h"
#include "addressbar.h"
#include "operations.h"
#include "main.h"
#include "resource.h"
//#include "mydebug.h"
//#define LVS_EX_GRIDLINES 1
#define LVS_EX_FULLROWSELECT 32

UCHAR DropTarget[MAX_PATH];
void GetItemTextFromIndex(int index,UCHAR *FileName,UINT size);

extern HINSTANCE ins;
HWND hListView;
RECT ListRect ={ 225,90,559,396};
UCHAR CurrentDirListView[MAX_PATH];

WNDPROC OldListProc;
UCHAR isDraging=NO;
HCURSOR wait,ok;

SCROLLINFO si;

BOOL CanScroll(HWND hwnd)
{
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask =  SIF_RANGE | SIF_PAGE | SIF_POS;

    if(GetScrollInfo(hwnd, SB_VERT	, &si))
    {
		return si.nPos > si.nMin;
	}
return FALSE;
}


void ScrollDetect(HWND plv)
{
     RECT lpRect; LONG u; POINT p;
     GetWindowRect(plv,&lpRect);
     u=lpRect.bottom-lpRect.top;
     GetCursorPos(&p);
     ScreenToClient(hListView, &p);
     
     if(p.y>=u)
     {
       SendMessage(plv,WM_VSCROLL,(WPARAM)SB_LINEDOWN,(LPARAM)0); 
     } 
     else if(p.y<=0)
     {
            if(CanScroll(plv)!=FALSE)
            {
                SendMessage(plv,WM_VSCROLL,(WPARAM)SB_LINEUP,(LPARAM)0); 
            } 
      }
}


LRESULT CALLBACK ListProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case LISTVIEW_BEGINDRAG:
        {
            isDraging=YES;  
            SetCapture(hwnd);
            SetCursor(wait);

        }break;
        case WM_MOUSEMOVE:
        {
           if(wParam==MK_LBUTTON && isDraging==YES)
            {
                 ScrollDetect(hwnd);            
            }   
        }
        break; 
        case WM_LBUTTONUP:
        {
            if(isDraging==YES)
            {
                isDraging=NO;
                 SetCursor(ok); 
                ReleaseCapture();
                LV_HITTESTINFO Struct;
                  POINT NewPos; int hItemSelect;
                  NewPos.x=LOWORD(lParam);
                  NewPos.y=HIWORD(lParam);
                  Struct.pt = NewPos;
                  hItemSelect = ListView_HitTest(hListView, &Struct);
                  if(hItemSelect>=0)
                  {
                     
                     UINT len;
                     len=sprintf(DropTarget,"%s\0",CurrentDirListView);
                     GetItemTextFromIndex(hItemSelect,&DropTarget[len],MAX_PATH-len);
                     len=FileExist(DropTarget);
                     if(len==ThisIsDir)
                     {
                        if(SetDestPath(DropTarget,hItemSelect)==YES)
                        {
                            ExecuteOperation();
                        }
                        else{ SMS(0,"Can't Drop To Selected Folder"); }
                     }else{ SMS(0,"Can't Drop To File"); }
                  } 
            }
        }
        break;
    }            
	return CallWindowProc(OldListProc, hwnd, message, wParam, lParam);
}



void ChangeListViewStyle(DWORD NewStyle)
{ 
       DWORD LastStyle = GetWindowLong(hListView, GWL_STYLE);
       SetWindowLong(hListView, GWL_STYLE,(LastStyle & ~LVS_TYPEMASK) | NewStyle);
      // UpdateWindow(hListView);
}

void CreateListView(HWND hwnd)
{
   hListView = CreateWindowEx(0,"SysListView32",0,
                             WS_CHILD | WS_VISIBLE| WS_TABSTOP |LVS_REPORT |LVS_EDITLABELS |WS_CLIPSIBLINGS |LVS_SHOWSELALWAYS,
                             ListRect.left, ListRect.top, ListRect.right, ListRect.bottom,
                             hwnd,(HMENU)ID_LIST_VIEW , ins, NULL);
    LVCOLUMN LvCol;   
	memset(&LvCol,0,sizeof(LVCOLUMN));
	
	LvCol.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_FMT;
	LvCol.cx=0xc8;		
	LvCol.fmt = LVCFMT_RIGHT;
	
	LvCol.pszText = "Name";
	SNDMSG(hListView,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);

    LvCol.cx=0x62;
    LvCol.pszText = "Size";
	SNDMSG(hListView,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol);
	
    LvCol.cx=0x90;			
	LvCol.pszText = "Modified";
	SNDMSG(hListView,LVM_INSERTCOLUMN,2,(LPARAM)&LvCol);
    
    LvCol.cx=0x74;
    LvCol.fmt = LVCFMT_CENTER;
	LvCol.pszText = "Type";
	SNDMSG(hListView,LVM_INSERTCOLUMN,3,(LPARAM)&LvCol);
    
 //   SNDMSG(hListView,LVM_SETEXTENDEDLISTVIEWSTYLE,0,(WPARAM)LVS_EX_CHECKBOXES); 
 //  SNDMSG(hListView,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_GRIDLINES,LVS_EX_GRIDLINES);
//   SNDMSG(hListView,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
    SNDMSG(hListView, WM_SETREDRAW, TRUE, 0);
    wait=LoadCursor(ins, "DRAGDROP");
    ok=LoadCursor (NULL, IDC_ARROW);
    OldListProc= (WNDPROC) SetWindowLong(hListView, GWL_WNDPROC, (LPARAM)ListProc);
return;
}

int proc_ImlListView(unsigned char *Path)
{
    SHFILEINFO sfi;
    HIMAGELIST himl,hims;
 	ZeroMemory(&sfi,sizeof(SHFILEINFO));
    hims=(HIMAGELIST)SHGetFileInfo(Path,0,&sfi,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    himl=(HIMAGELIST)SHGetFileInfo(Path,0,&sfi,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

    SNDMSG(hListView,LVM_SETIMAGELIST,LVSIL_SMALL,(LPARAM)hims);
    SNDMSG(hListView,LVM_SETIMAGELIST,LVSIL_NORMAL,(LPARAM)himl);
	return sfi.iIcon;
}


UCHAR *format_time(FILETIME t)
{
   static UCHAR tTime[86];
   FILETIME    localtime;
   SYSTEMTIME  Ttime;
   FileTimeToLocalFileTime(&t, &localtime);
   FileTimeToSystemTime(&localtime, &Ttime);
   sprintf(tTime,"%02d/%02d/%04d %02d:%02d:%02d\0",Ttime.wDay,Ttime.wMonth,Ttime.wYear,Ttime.wHour,Ttime.wMinute,Ttime.wSecond);
 return tTime;    
}


int AddItem(UCHAR Replace,int iImage,UCHAR *hFile,UCHAR *size,UCHAR *modified,UCHAR *ext,int c)
{
    LVITEM LvItem;	static int index;
    memset(&LvItem,0,sizeof(LvItem)); 
	LvItem.mask=LVIF_TEXT | LVIF_IMAGE ;
	LvItem.cchTextMax = MAX_PATH;		 
	LvItem.iItem = c;					  
	LvItem.iImage = iImage;	//-------- Insert Image ------------------
    if(Replace)
    {  
        index = (int)SNDMSG(hListView,LVM_SETITEM,0,(LPARAM)&LvItem); 
    }
    else
    {  
        index = (int)SNDMSG(hListView,LVM_INSERTITEM,0,(LPARAM)&LvItem); 
    }    
    LvItem.iSubItem=0;	//-------- Insert Element 0 --------------
	LvItem.pszText = hFile; //"filename"
	SNDMSG(hListView,LVM_SETITEM,0,(LPARAM)&LvItem);
	LvItem.iSubItem=1;	//-------- Insert Element 1 --------------
	LvItem.pszText = size; // size
	SNDMSG(hListView,LVM_SETITEM,0,(LPARAM)&LvItem);
	LvItem.iSubItem=2;	//-------- Insert Element 2 --------------
	LvItem.pszText = modified; // date
	SNDMSG(hListView,LVM_SETITEM,0,(LPARAM)&LvItem);
	LvItem.iSubItem=3;	//-------- Insert Element 3 --------------
	LvItem.pszText = ext; // file extention
	SNDMSG(hListView,LVM_SETITEM,0,(LPARAM)&LvItem);
  //  SNDMSG(hListView, WM_SETREDRAW, TRUE, 0);// ----------------- refresh() ----------
 // RedrawWindow(hListView,NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
 return index;    
}

int AddData(UCHAR Replace,UCHAR *FullPath,UCHAR *hFile,UCHAR *size,UCHAR *modified,UCHAR *ext,int c)
{
   UCHAR fType[MAX_PATH];
   SHFILEINFO InfoFile;
   int iImage = proc_ImlListView(FullPath);	//-------- Insert Image ------------------

   SHGetFileInfo(FullPath, 0, &InfoFile,sizeof(SHFILEINFO),SHGFI_TYPENAME);
   sprintf(fType,"%s\0", InfoFile.szTypeName);

return	AddItem(Replace,iImage,hFile,size,modified,fType,c);
}


void ExploreDir(UCHAR *ThisDir,UCHAR isBack)
{ 
    UCHAR DirExp[MAX_PATH];
    sprintf(DirExp,"%s*.*\0",ThisDir);
    HANDLE h;
	WIN32_FIND_DATA info;
	h = FindFirstFile(DirExp, &info);
	if(h == INVALID_HANDLE_VALUE)
    { 
         return;
    } 
    //the path exist so put to combo
	if(!isBack) AddComboView(ThisDir);
	int nrFolders = 0;
	int nrFiles = 0;
    UCHAR size[86];
    ShowWindow(hListView,0);
    SNDMSG(hListView,LVM_DELETEALLITEMS,0,0);

    do
    {
      if(!(strcmp(info.cFileName, ".") == 0 || strcmp(info.cFileName, "..") == 0))
      {
        if(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	    {        
           sprintf(DirExp,"%s%s\0",ThisDir,info.cFileName);
           if(AddData(0,DirExp,info.cFileName,"","","DIR",0) <0) return;
           nrFolders++;
           sprintf(size,"Folder : %d\0",nrFolders);
           SMS(1,size);
        }
        else
        {
           ULARGE_INTEGER a;
           a.LowPart = info.nFileSizeLow;
           a.HighPart = info.nFileSizeHigh;
           sprintf(size,"%llu\0",a.QuadPart);
           UCHAR *ext =strrchr(info.cFileName,'.');
           if(!ext) ext = "";
           sprintf(DirExp,"%s%s\0",ThisDir,info.cFileName);
           if(AddData(0,DirExp,info.cFileName,size,format_time(info.ftLastWriteTime),ext,nrFolders) < 0) return;
           nrFiles++;
           sprintf(size,"File : %d\0",nrFiles);
           SMS(2,size);
        } 
         
      }
    } while(FindNextFile(h, &info) != 0);
    FindClose(h);   
    ShowWindow(hListView,1);
    SMS(3,"");
    sprintf(CurrentDirListView,"%s\0", ThisDir);
    GrowList(nrFolders+nrFiles);
  //  SetFocus(hListView);
  //  ListView_SetItemState(hListView,0,LVIS_SELECTED,LVIS_SELECTED);
return;
}

void ExploreDrive_()
{
     UCHAR drive[] = "A:\\*.*";
     WIN32_FIND_DATA wfd;
     HANDLE hfl;
     ULARGE_INTEGER a,b;
     
     UCHAR size[MAX_PATH];
     UCHAR cfile[]="A:";
     UCHAR cImage[]="A:\\";
     int i;
     EnableWindow(hListView,0);
     SendMessage(hListView,LVM_DELETEALLITEMS,0,0);   
     for(i = 26; i>0; i--)
     {
        drive[0]= i + 'A';
        cfile[0]= i + 'A';
        cImage[0]= i + 'A';
            
        hfl = FindFirstFile(drive, &wfd);
	    if(hfl != INVALID_HANDLE_VALUE)
	    {
            GetDiskFreeSpaceEx(cfile,&a,&b,&a);   
            sprintf(size,"%llu\0",b.QuadPart);
            AddData(0,cImage,cfile,size,"","DIR",0);   
            FindClose(hfl);
        }
      }
      EnableWindow(hListView,1);
      //SetWindowText(ComboEdit, "Computer\\");
      DefaultComboView("Computer\\");
      memset(CurrentDirListView,0,MAX_PATH);
      ResetListValue();
    //  SetFocus(hListView);
   //   ListView_SetItemState(hListView,0,LVIS_SELECTED,LVIS_SELECTED);
return;     
}
void ExploreDrive()
{
    UCHAR AllDrives[MAX_PATH]; 
    ULARGE_INTEGER a,b; int index=-1;
    UCHAR size[MAX_PATH]; UCHAR cfile[]="A:\0";

    EnableWindow(hListView,0);
    SendMessage(hListView,LVM_DELETEALLITEMS,0,0); 
    
    if(GetLogicalDriveStrings(MAX_PATH,AllDrives) > 0)
    {    
         UCHAR *ch = AllDrives; 
         int iImage;
         while(ch[0])
         {
              cfile[0]=ch[0]; 
              memset(&a,0,sizeof(ULARGE_INTEGER));
              memset(&b,0,sizeof(ULARGE_INTEGER));
              if(GetDiskFreeSpaceEx(cfile,&a,&b,&a)==0)
              {
                   size[0]=0;
              }else{  sprintf(size,"%llu\0",b.QuadPart); }
              index=AddData(0,ch,cfile,size,"","DIR",index+1);
              //SetWindowText(MainHWND,ch);
               ch += 4;
        }
     }
     else
     {
        ExploreDrive_();
        return;
     }
     EnableWindow(hListView,1);
     DefaultComboView("Computer\\");
     memset(CurrentDirListView,0,MAX_PATH);
}

int GetSelectedOnList()
{
     int iSelected =~0;
     iSelected= ListView_GetNextItem(hListView,iSelected,LVNI_SELECTED);
	return iSelected;    
}
int FileExist(UCHAR *DirOrFile)
{
  //  HANDLE h;
//	WIN32_FIND_DATA info;
    int st = 0;
    DWORD a=GetFileAttributes(DirOrFile);
    if(a ==0xFFFFFFFF)
     return 0;
    if(a & FILE_ATTRIBUTE_DIRECTORY)
    {
        st = ThisIsDir;
    }
    else
    {
        st = ThisIsFile;
    } 
return st;
}
/*	h = FindFirstFile(DirOrFile, &info);
	if(h != INVALID_HANDLE_VALUE)
    { 
       FindClose(h);
       if(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
       {
            st = ThisIsDir;
       }
       else
       {
            st = ThisIsFile;
       }
    }
    return st;
}
*/
int GetSelectedInfoText(UCHAR *Root,UCHAR *FileName)
{ 

  int iSelected =~0;
  UCHAR temp[MAX_PATH];
  UCHAR ThisDir[MAX_PATH];
  int n = sprintf(ThisDir,"%s\0",Root);
  ThisDir[n] = 0;
      		
  iSelected=GetSelectedOnList();
  if(iSelected >= 0)
  {
      memset(temp,0,255);
      ListView_GetItemText(hListView,iSelected,0,temp,MAX_PATH);
							
      if(ThisDir[n-1] !='\\' && n> 2) strcat(ThisDir,"\\");
      strcat(ThisDir,temp);								
     //SetWindowText(MainHWND,temp);
      if(n<=3)
      {
	     sprintf(FileName,"%s\0",ThisDir); // SetWindowText(hwnd,ThisDir);
         return ThisIsDir;
      }
      int c=FileExist(ThisDir);
      if(c==ThisIsDir)
      {
	     sprintf(FileName,"%s\0",ThisDir); // SetWindowText(hwnd,ThisDir);
         return ThisIsDir;
      }
      else if(c==ThisIsFile)
      {
	     sprintf(FileName,"%s\0",ThisDir); // SetWindowText(hwnd,ThisDir);
         return ThisIsFile;
      }
      else
      {
          return -1;
      }  
   }
return 0;
}

    
void GetItemTextFromIndex(int index,UCHAR *FileName,UINT size)
{
      
      memset(FileName,0,size);
      ListView_GetItemText(hListView,index,0,FileName,size);
}
   
