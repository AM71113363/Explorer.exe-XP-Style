#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "statusbar.h"
#include "resource.h"
//#include "mydebug.h"

extern HINSTANCE ins;
HWND hTreeView=NULL;
RECT TreeRect ={ 0,90,224,396};

int proc_ImlTreeView(unsigned char *Path)
{
    SHFILEINFO sfi;
    HIMAGELIST himl;
 	ZeroMemory(&sfi,sizeof(SHFILEINFO));
    himl=(HIMAGELIST)SHGetFileInfo(Path,0,&sfi,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	if(himl == 0)
    {   
       return -1;
    }    
    SNDMSG(hTreeView,TVM_SETIMAGELIST,TVSIL_NORMAL,(LPARAM)himl);
	return sfi.iIcon;
}


HTREEITEM InsertTreeviewItem(HWND hTreeView, UCHAR *pszText, int iImage, HTREEITEM htiParent,HTREEITEM s)
{
    TVITEM tvi = {0};
    tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvi.pszText = pszText;
    tvi.cchTextMax = strlen(pszText);
    tvi.iImage = iImage;

    TVINSERTSTRUCT tvis = {0};
    tvi.iSelectedImage = iImage;
    tvis.item = tvi; 
    tvis.hInsertAfter = s; 
    tvis.hParent = htiParent;

    return (HTREEITEM)SNDMSG(hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvis);
}


void AddFaKe(UCHAR *newroot, HTREEITEM hParent)
{ 
     WIN32_FIND_DATA info;    
     HANDLE h;
     // add fake only if exit at least one folder
     h = FindFirstFile(newroot, &info);
     if(h == INVALID_HANDLE_VALUE) return; 
     do
     {
      if(!(strcmp(info.cFileName, ".") == 0 || strcmp(info.cFileName, "..") == 0))
      {
        if(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	    {        
           InsertTreeviewItem(hTreeView,"#FAKE FOLDER",-2, hParent,0);
           break;
        }  
      }
    } while(FindNextFile(h, &info) != 0);
    FindClose(h); 
 return;    
}
void ResetTreeView()
{
    UCHAR temp[MAX_PATH];
    UCHAR buf[MAX_PATH];
    HTREEITEM hParent;
    UCHAR DriveName[MAX_PATH];
    if(hTreeView==NULL)
    {
        return;
    }                       
    TreeView_DeleteAllItems(hTreeView);
    if(GetLogicalDriveStrings(MAX_PATH,temp) > 0)
    {    
         UCHAR *ch = temp; 
         int iImage;
         while(ch[0])
         {
           iImage = proc_ImlTreeView(ch);
           memset(DriveName,0,MAX_PATH);
           GetVolumeInformation(ch, DriveName, MAX_PATH, 0,0,0,0,0);
           sprintf(buf,"%s (%c:)\0",DriveName,ch[0]);
           
            hParent = InsertTreeviewItem(hTreeView, buf, iImage, TVI_ROOT,0);
            sprintf(buf,"%s\\*\0",ch);
            AddFaKe(buf, hParent);
		    ch += strlen(ch)+1;
        }
     }   
     
}

void CreateTreeView(HWND hwnd)
{
	hTreeView = CreateWindow(WC_TREEVIEW, 0,
						WS_CHILD | WS_VISIBLE |  WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS| 
						TVS_HASLINES |  TVS_LINESATROOT | WS_CLIPSIBLINGS |TVS_TRACKSELECT| WS_TABSTOP|TVS_SHOWSELALWAYS,
    					TreeRect.left, TreeRect.top, TreeRect.right, TreeRect.bottom,  
						hwnd,(HMENU)ID_TREE_VIEW, ins, NULL);
    
ResetTreeView();
}      

UCHAR *GetFullPath_(HTREEITEM hselect)
{
     static UCHAR ThisDir[MAX_PATH];      memset(ThisDir,0,MAX_PATH);
     UCHAR tmp[MAX_PATH];
     UCHAR temp[MAX_PATH];
	
     TV_ITEM tvi;
	 tvi.mask = TVIF_TEXT;
	 tvi.pszText = temp;
	 tvi.cchTextMax = MAX_PATH;
	 while(1)
	 {
		tvi.hItem = hselect;
		memset(temp,0,MAX_PATH);
		if(!SendMessage(hTreeView, TVM_GETITEM,(WPARAM) TVGN_CARET,(LPARAM)(TV_ITEM*)&tvi))
        return 0;
		if(temp[1] == ':') temp[2] = 0;
		sprintf(tmp,"%s\0",ThisDir);
		sprintf(ThisDir,"%s\\%s\0",temp, tmp);
	    if(temp[1] == ':') break;
		hselect = (HTREEITEM) SendMessage(hTreeView, TVM_GETNEXTITEM, TVGN_PARENT, (long)hselect);
		if(!hselect) break;
	}
return ThisDir;
}

UCHAR *GetFullPath(HTREEITEM hParent)
{

    TV_ITEM tvi; HTREEITEM hselect; int len;
    UCHAR text[MAX_PATH], NewDir[MAX_PATH], buf[MAX_PATH];
    static UCHAR ThisDir[MAX_PATH];      memset(ThisDir,0,MAX_PATH);
    
    tvi.mask = TVIF_TEXT;
    tvi.pszText = text;
    tvi.cchTextMax = MAX_PATH;
    tvi.hItem = hParent;
    tvi.state = 0;

    memset(NewDir, 0, MAX_PATH);
    hselect = hParent;

    while(1)
    {
          tvi.hItem = hselect;
          memset(text, 0, MAX_PATH);
          if(TreeView_GetItem(hTreeView, &tvi)==FALSE)
               return NULL; //list empty

          hselect = TreeView_GetParent(hTreeView,hselect);
          if (hselect != NULL)
          {
             strcpy(buf, NewDir);
             snprintf(NewDir,MAX_PATH,"%s\\%s\0", text, buf); 
          }
          else{ break; }
    }

    len = strlen(text);
    snprintf(ThisDir, MAX_PATH,"%c:\\%s\0",text[len-3],NewDir);
return ThisDir;
}

void ScanForFolders(HTREEITEM hParent)
{
    HTREEITEM hselect = hParent;
	UCHAR DirExp[MAX_PATH];
    UCHAR *ThisDir=GetFullPath(hselect);
    if(ThisDir == NULL)
    {
       return;
    }    
    EnableWindow(hTreeView,0);
    while (1)
    {
	    hselect = (HTREEITEM) SendMessage(hTreeView, TVM_GETNEXTITEM,(WPARAM) TVGN_CHILD,(LPARAM)(HTREEITEM)hParent);
		if(hselect == 0)  break;
		SendMessage(hTreeView,TVM_DELETEITEM,0,(LPARAM)(HTREEITEM)hselect);
    }
    HANDLE h;
	WIN32_FIND_DATA info;
	sprintf(DirExp,"%s*.*\0",ThisDir);

	h = FindFirstFile(DirExp, &info);
	if(h == INVALID_HANDLE_VALUE){EnableWindow(hTreeView,1); return; }
	UCHAR buf[MAX_PATH];
    do
    {
      if(!(strcmp(info.cFileName, ".") == 0 || strcmp(info.cFileName, "..") == 0))
      {
        if(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	    {        
	      
           sprintf(buf,"%s%s\0",ThisDir,info.cFileName);
           hselect = InsertTreeviewItem(hTreeView, info.cFileName,proc_ImlTreeView(buf), hParent,TVI_SORT); 
           sprintf(buf,"%s%s\\*\0",ThisDir,info.cFileName);
           AddFaKe(buf, hselect);
        }  
      }
    } while(FindNextFile(h, &info) != 0);
    FindClose(h); 
    EnableWindow(hTreeView,1);
return;
}


void DeleteAllChildWindow(HTREEITEM lParam)
{
     HTREEITEM htrit;
     UCHAR buf[MAX_PATH];
     while (1)
	 {
		htrit = (HTREEITEM) SendMessage(hTreeView, TVM_GETNEXTITEM,(WPARAM) TVGN_CHILD,(LPARAM)(HTREEITEM)lParam);
		if(htrit == 0)  break;
		SendMessage(hTreeView,TVM_DELETEITEM,0,(LPARAM)(HTREEITEM)htrit); //TreeView_DeleteItem(hTree, htrit);
     }
     sprintf(buf,"%s*\0",GetFullPath(lParam));
     AddFaKe(buf, lParam);
    
return;     
}

void RefreshTree(NM_TREEVIEW *lParam)
{
   switch( lParam->action)
   {
       case 1: // collapse // close
       { 
          DeleteAllChildWindow(lParam->itemNew.hItem);     
       }
       break;
       
       case 2:// expand  // open
       {            
         ScanForFolders(lParam->itemNew.hItem); 
         SetFocus(hTreeView); 
       }
       break;
   }
return;
}
  
