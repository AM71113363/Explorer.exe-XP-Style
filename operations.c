#include <windows.h>
#include <stdio.h>
#include "statusbar.h"
#include "listview.h"
#include "main.h"
#include "resource.h"
//#include "mydebug.h"

extern HINSTANCE ins;


typedef struct _nfiles
{
    UCHAR type; //0=dir 1=file
    ULONGLONG size;
    UCHAR FileName[MAX_PATH];   
        
}NFILES;
typedef struct _data
{
     UCHAR operation; //1=delete 2=cut  3=copy, 0 maybe free data
     UINT nr;
     UCHAR SrcPath[MAX_PATH];
     UCHAR DestPath[MAX_PATH];
     NFILES *nfiles;   
}DATA;

DATA OP;

UINT ListViewBufferSize=0;
UINT ListViewNrElements=0;
UCHAR *ListViewSelection=NULL;
UINT selected=0;
UCHAR P[32];

UCHAR HandleOperation(UCHAR Del,UCHAR Cop, UCHAR *Ret);
UCHAR IsValidCutCopyOperation(UCHAR *Path);

void ResetListValue()
{
   selected=0;
   SMS(0,"Done");
   if(ListViewSelection==NULL)
      return;
   memset( ListViewSelection,0, ListViewBufferSize); 
}

void GrowList(UINT size)
{
     if(size>ListViewBufferSize)
     {
        ListViewBufferSize=size+1;
        ListViewSelection=(UCHAR*)realloc(ListViewSelection,ListViewBufferSize);
     }
     ListViewNrElements=size;
     ResetListValue();
}


void UpdateListValue(UINT index,UINT value)
{
    if(index>=ListViewNrElements)
      return;
    if(ListViewSelection==NULL)
      return;
    
    if(ListViewSelection[index]!=(UCHAR)value)
    {
        ListViewSelection[index]=(UCHAR)value; 
        if(value){ selected++;}else{selected--;}
        if(selected==0)
        {
            SMS(0,"Done");
        }
        else
        {
         sprintf(P,"Selected: %u\0",selected);   SMS(0,P);
        }
    }
}

UCHAR IsValidOperation()
{
    if(selected>0)
    {
       return YES;
     }
return NO;  
}

UCHAR SetDestPath(UCHAR *Path,int in)
{
     if(ListViewSelection[in])
     return NO;

return IsValidCutCopyOperation(Path);
}


UCHAR SetOperation(UCHAR o,UCHAR *Path)
{
     if(o>0)
     {
         if(ListViewSelection==NULL) //just in case
           return NO;
         if(strlen(Path)<3)
           return NO;
         UINT i; int index=0;
         sprintf(OP.SrcPath,"%s\0",Path);
         if(OP.nfiles) free(OP.nfiles);   
         OP.nfiles=(NFILES*)calloc(selected,sizeof(NFILES));
         if(!OP.nfiles)
         {
             return 0;
         }
         OP.operation=o;
         for(i=0;i<ListViewNrElements;i++)
         {
            if(ListViewSelection[i])
            {
                 GetItemTextFromIndex((int)i,OP.nfiles[index].FileName,MAX_PATH);
                 index++;                     
            }                                 
                                             
         }
         if(o==2){ sprintf(P,"[ Cut ]: %u\0",index); }
         else if(o==3){sprintf(P,"[ Copy ]: %u\0",index);}  
         OP.nr=index;
         SMS(0,P);  
     }
     else //freeAll
     {
        if(OP.nfiles) free(OP.nfiles); 
        memset(&OP,0,sizeof(DATA));
        selected=0;
        SMS(0,"Done");
     }
     return YES;
}


UCHAR IsValidCutCopyOperation(UCHAR *Path)
{
      if(OP.operation!=0)
      {
         if(Path[1] == ':')
         {
             sprintf(OP.DestPath,"%s\0",Path);
             return YES;
         }
      }
return NO;
}


void PutOnCenter(HWND hwnd,int W,int H)
{
     RECT rC, rD;
     SystemParametersInfo(SPI_GETWORKAREA, 0, &rD, 0);
     GetWindowRect(hwnd, &rC);
     int nX=((rD.right - rD.left) / 2) -((rC.right - rC.left) / 2);
     int nY=((rD.bottom - rD.top) / 2) -((rC.bottom - rC.top) / 2);
     SetWindowPos(hwnd, NULL, nX+20, nY+20, W, H,  0);
return;
}

HANDLE semafor=NULL;
HWND hMainProc,hBtnOK;
HWND hNfiles,hNfilesO,    pNfiles;
HWND hNfolders,hNfoldersO,pNfolders;
HWND hNsize,hNsizeO,      pNsize;

DWORD NumberOfFolders,NumberOfFiles;
UINT64 TotalSize; 

void IncrementFolders()
{
     NumberOfFolders++;
     sprintf(P,"%d\0",NumberOfFolders);
     SetWindowText(pNfolders,P);
}
void IncrementFiles()
{
     NumberOfFiles++;
     sprintf(P,"%d\0",NumberOfFiles);
     SetWindowText(pNfiles,P);
}
void IncrementSize(ULONGLONG QuadPart)
{
     TotalSize+=QuadPart;
     sprintf(P,"%I64u\0",TotalSize);
     SetWindowText(pNsize,P);
}

UCHAR DoMore(UCHAR *ThisDir)
{
    UCHAR DirExp[MAX_PATH];
    sprintf(DirExp,"%s\\*.*\0",ThisDir);
    HANDLE h;
	WIN32_FIND_DATA info; ULARGE_INTEGER a;
	h = FindFirstFile(DirExp, &info);
	if(h == INVALID_HANDLE_VALUE) 
         return YES;   
    do
    {
      if(WaitForSingleObject(semafor,0)==0){ return NO; }
        
      if(!(strcmp(info.cFileName, ".") == 0 || strcmp(info.cFileName, "..") == 0))
      {
        if(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	    {        
           IncrementFolders();
           sprintf(DirExp,"%s\\%s\0",ThisDir,info.cFileName); 
           if(DoMore(DirExp)==NO)
           {
               FindClose(h);
               return NO;
           }
        }
        else
        {
           IncrementFiles();
           a.LowPart = info.nFileSizeLow;
           a.HighPart = info.nFileSizeHigh;
           IncrementSize(a.QuadPart);
        } 
         
      }
    } while(FindNextFile(h, &info) != 0);
    FindClose(h); 
  return YES;    
}

UCHAR CollectData(UCHAR *ret)
{
     UCHAR DirExp[MAX_PATH]; UCHAR temp[32];
     HANDLE h; UINT i;
	 WIN32_FIND_DATA info;  ULARGE_INTEGER a;   
     NumberOfFiles=NumberOfFolders=0;
     TotalSize=0;
     
     for(i=0;i<OP.nr;i++)
     {
          if(WaitForSingleObject(semafor,0)==0){ *ret=1; return NO; }
          sprintf(DirExp,"%s%s\0",OP.SrcPath,OP.nfiles[i].FileName);

	      h = FindFirstFile(DirExp, &info);
	      if(h == INVALID_HANDLE_VALUE)
	      {
             return NO;
          }
          FindClose(h); 
          if(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	      { 
                OP.nfiles[i].type=0;
                IncrementFolders();
                if(DoMore(DirExp)==NO)
                {
                   *ret=1;
                   return NO;
                }
          }
          else
          {
                OP.nfiles[i].type=1;
                IncrementFiles();                
                a.LowPart = info.nFileSizeLow;
                a.HighPart = info.nFileSizeHigh;
                OP.nfiles[i].size=a.QuadPart;
                IncrementSize(a.QuadPart);  
          }
    }  
      
   return YES;   
}

void OP_DEL()
{    
     UCHAR Ret=0;UCHAR i=NO;
     ResetEvent(semafor);
     SetWindowText(hMainProc,"DELETE...");
     i=CollectData(&Ret);
     if(i==NO) //someting Wrong
     {
         if(Ret==1) //canceled by event
         { 
            SMS(0,"Operation: Canceled");
         }
         else
         {
             SMS(0,"Operation: Error[ CollectData ]");
         }
     }
     else
     { 
         i=HandleOperation(1,0,&Ret);
         if(i==NO) //someting Wrong
         {
             if(Ret==1) //canceled by event
             { 
                SMS(0,"Operation: Canceled[ Partial Files Deleted]");
             }
             else
             {
                 SMS(0,"Operation: Error[ HandleOperation ]");
             }
         }
         else
         { 
            SMS(0,"Done");
         }

     } 
     EnableWindow(hBtnOK,1);
     CloseHandle(semafor); 
     SNDMSG(MainHWND,WM_COMMAND,MAKEWPARAM(MENU_RIGHT_UPDATE,0),0);
     ResetListValue();
}
void OP_CUT()
{     
     UCHAR Ret=0;UCHAR i=NO;
     ResetEvent(semafor);
     SetWindowText(hMainProc,"MOVE...");
     i=CollectData(&Ret);
     if(i==NO) //someting Wrong
     {
         if(Ret==1) //canceled by event
         { 
            SMS(0,"Operation: Canceled");
         }
         else
         {
             SMS(0,"Operation: Error[ CollectData ]");
         }
     }
     else
     { 
         i=HandleOperation(1,1,&Ret);
         if(i==NO) //someting Wrong
         {
             if(Ret==1) //canceled by event
             { 
                SMS(0,"Operation: Canceled[ Partial Files Moved]");
             }
             else
             {
                 SMS(0,"Operation: Error[ HandleOperation ]");
             }
         }
         else
         { 
            SMS(0,"Done");
         }

     } 
     EnableWindow(hBtnOK,1);
     CloseHandle(semafor); 
     SNDMSG(MainHWND,WM_COMMAND,MAKEWPARAM(MENU_RIGHT_UPDATE,0),0);
     ResetListValue();
}
void OP_COPY()
{     
     UCHAR Ret=0;UCHAR i=NO;
     ResetEvent(semafor);
     SetWindowText(hMainProc,"COPY...");
     i=CollectData(&Ret);
     if(i==NO) //someting Wrong
     {
         if(Ret==1) //canceled by event
         { 
            SMS(0,"Operation: Canceled");
         }
         else
         {
             SMS(0,"Operation: Error[ CollectData ]");
         }
     }
     else
     { 
         i=HandleOperation(0,1,&Ret);
         if(i==NO) //someting Wrong
         {
             if(Ret==1) //canceled by event
             { 
                SMS(0,"Operation: Canceled[ Partial Files Copied]");
             }
             else
             {
                 SMS(0,"Operation: Error[ HandleOperation ]");
             }
         }
         else
         { 
            SMS(0,"Done");
         }

     } 
     EnableWindow(hBtnOK,1);
     CloseHandle(semafor); 
     SNDMSG(MainHWND,WM_COMMAND,MAKEWPARAM(MENU_RIGHT_UPDATE,0),0);
}

BOOL CALLBACK DlgProc(HWND hnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
       case WM_INITDIALOG:
       {
              HFONT hFont; HWND s1,s2,s3,s1O,s2O,s3O;
              PutOnCenter(hnd,543,230);
              hMainProc=hnd;
              semafor = CreateEvent(NULL, TRUE, FALSE, "ODMC");
              CreateWindow("BUTTON","Selected",WS_CHILD|WS_VISIBLE| BS_GROUPBOX| BS_CENTER,2,0,263,167,hnd,NULL,ins,NULL);
              CreateWindow("BUTTON","Status",WS_CHILD|WS_VISIBLE| BS_GROUPBOX| BS_CENTER,272,0,263,167,hnd,NULL,ins,NULL);
                
              s1=CreateWindow("BUTTON","Folders:",WS_CHILD|WS_VISIBLE| BS_GROUPBOX,8,15,250,47,hnd,NULL,ins,NULL);
              s1O=CreateWindow("BUTTON","Folders:",WS_CHILD|WS_VISIBLE| BS_GROUPBOX,278,15,250,47,hnd,NULL,ins,NULL);
              hNfolders=CreateWindow("STATIC","0",WS_CHILD|WS_VISIBLE,17,30,235,28,hnd,NULL,ins,NULL);
	          hNfoldersO=CreateWindow("STATIC","0",WS_CHILD|WS_VISIBLE,287,30,235,28,hnd,NULL,ins,NULL);
		      pNfolders=hNfolders;
              s2=CreateWindow("BUTTON","Files:",WS_CHILD|WS_VISIBLE| BS_GROUPBOX,8,63,250,47,hnd,NULL,ins,NULL);
              s2O=CreateWindow("BUTTON","Files:",WS_CHILD|WS_VISIBLE| BS_GROUPBOX,278,63,250,47,hnd,NULL,ins,NULL);
              hNfiles=CreateWindow("STATIC","0",WS_CHILD|WS_VISIBLE,17,78,235,28,hnd,NULL,ins,NULL);
              hNfilesO=CreateWindow("STATIC","0",WS_CHILD|WS_VISIBLE,287,78,235,28,hnd,NULL,ins,NULL);
              pNfiles=hNfiles;
              s3=CreateWindow("BUTTON","Size:",WS_CHILD|WS_VISIBLE| BS_GROUPBOX,8,111,250,47,hnd,NULL,ins,NULL);
              s3O=CreateWindow("BUTTON","Size:",WS_CHILD|WS_VISIBLE| BS_GROUPBOX,278,111,250,47,hnd,NULL,ins,NULL);
              hNsize=CreateWindow("STATIC","0",WS_CHILD|WS_VISIBLE,17,126,235,28,hnd,NULL,ins,NULL);
              hNsizeO=CreateWindow("STATIC","0",WS_CHILD|WS_VISIBLE,287,126,235,28,hnd,NULL,ins,NULL);
              pNsize=hNsize;
              hFont = CreateFont(28, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "ARIAL");
			  SNDMSG(hNfolders, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hNfoldersO, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hNfiles, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hNfilesO, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
              SNDMSG(hNsize, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hNsizeO, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));

			  hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
			  SNDMSG(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
			  SNDMSG(s1O, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
			  SNDMSG(s2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
			  SNDMSG(s2O, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
			  SNDMSG(s3, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
			  SNDMSG(s3O, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));  
              hBtnOK=CreateWindow("BUTTON","OK",WS_CHILD|WS_VISIBLE|WS_DISABLED,2,170,533,25,hnd,(HMENU)7777,ins,NULL);
              
              
              int w=(int)lParam;
              if(w==1) //delete
              {
                 CreateThread(0,0,(LPTHREAD_START_ROUTINE)OP_DEL,0,0,0);   
              } 
              else if(w==2) //cut
              {
                 CreateThread(0,0,(LPTHREAD_START_ROUTINE)OP_CUT,0,0,0);   
              } 
              if(w==3) //copy
              {
                 CreateThread(0,0,(LPTHREAD_START_ROUTINE)OP_COPY,0,0,0);   
              } 
                   
         }			
       break;
       case WM_COMMAND:
         if(LOWORD(wParam)==7777)
         {
             EndDialog(hnd,0);                    
         }
         break;
       case WM_CLOSE:
       {
           SetEvent(semafor); 
           EndDialog(hnd,0); 
       }
       break;
    }
    return FALSE;
}

void ExecuteOperation()
{
     if(OP.operation==1) //delete
     {
       DialogBoxParam(ins,MAKEINTRESOURCE(2222),MainHWND,(DLGPROC)DlgProc,1);
     }
     else if(OP.operation==2) //cut
     {
       DialogBoxParam(ins,MAKEINTRESOURCE(2222),MainHWND,(DLGPROC)DlgProc,2);
     }
     else if(OP.operation==3) //copy
     {
       DialogBoxParam(ins,MAKEINTRESOURCE(2222),MainHWND,(DLGPROC)DlgProc,3);
     }
     else
     {
      //   LogStatus(NULL,"#ERROR: ExecuteOperation(OP.operation==0)\r\n",NULL,1);
     }
}


DWORD CreateDirectoryFromPath (UCHAR *szPath)
{
    UCHAR   szLocalPath[MAX_PATH];
    UCHAR   *szEnd;
    DWORD   lReturn = 0;

    sprintf(szLocalPath,"%s\0", szPath);

    szEnd = &szLocalPath[3]; //A:\\

    if (*szEnd != 0)
    {
        // then there are sub dirs to create
        while (*szEnd != 0)
        {
            // go to next backslash
            while ((*szEnd != '\\') && (*szEnd != 0)) szEnd++;
            if (*szEnd == '\\')
            {
                // terminate path here and create directory
                *szEnd = 0;

                if (!CreateDirectory(szLocalPath, NULL))
                {
                    // see what the error was and "adjust" it if necessary
                    if (GetLastError() == ERROR_ALREADY_EXISTS)
                    {
                        // this is OK
                        SetLastError (ERROR_SUCCESS);
                    } else {
                        lReturn = 0;
                    }
                } 
                else
                {
                    // directory created successfully so update count
                    lReturn++;
                }
                // replace backslash and go to next dir
                *szEnd++ = '\\';
            }
        }
        // create last dir in path now
        if (!CreateDirectory(szLocalPath, NULL))
        {
            // see what the error was and "adjust" it if necessary
            if (GetLastError() == ERROR_ALREADY_EXISTS)
            {
                // this is OK
                SetLastError (ERROR_SUCCESS);
                lReturn++;
            } else {
                lReturn = 0;
            }
        } 
        else 
        {
            // directory created successfully
            lReturn++;
        }
    } 
    else 
    {
        // else this is a root dir only so return success.
        lReturn = 1;
    }
    return lReturn;
}

BOOL CopyDir(UCHAR *SrcPath,UCHAR *DestPath,UCHAR Del,UCHAR Cop,UCHAR *Ret)
{
	HANDLE h;
	WIN32_FIND_DATA info;   ULARGE_INTEGER a;
    UCHAR SearchPath[MAX_PATH]; 
    UCHAR FullSrcPath[MAX_PATH];  UCHAR *pFullSrcPath;
    UCHAR FullDestPath[MAX_PATH]; UCHAR *pFullDestPath;
    BOOL ret;
	int len;

    len=sprintf(FullSrcPath,"%s\0",SrcPath); 
    if (FullSrcPath[len-1] != '\\') strcat(FullSrcPath, "\\\0");
    pFullSrcPath = FullSrcPath + strlen(FullSrcPath);
    
    if(Cop==YES)
    {
       len=sprintf(FullDestPath,"%s\0",DestPath); 
       if (FullDestPath[len-1] != '\\') strcat(FullDestPath, "\\\0");
       pFullDestPath = FullDestPath + strlen(FullDestPath);
    
       if(CreateDirectoryFromPath(DestPath)==0)
       {
           return FALSE;
       }
    IncrementFolders();    
    }
    len=sprintf(SearchPath,"%s*.*\0",FullSrcPath);
    
	h = FindFirstFile(SearchPath, &info);
	if(h != INVALID_HANDLE_VALUE)
	{
		while(FindNextFile(h, &info) != 0)
		{
			 if(strcmp(info.cFileName, ".") == 0){  continue; }
             if(strcmp(info.cFileName, "..") == 0){ continue; }
        
             if(WaitForSingleObject(semafor,0)==0){ *Ret=1; return FALSE; }
             sprintf(pFullSrcPath, "%s\0",info.cFileName);
             if(Cop==YES) sprintf(pFullDestPath, "%s\0",info.cFileName);
             
            if(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
			       ret=CopyDir(FullSrcPath,FullDestPath,Del,Cop,Ret);
		           if(ret == FALSE)
		           {
                      return FALSE;
                   }
            }
            else//it is a file
            {
                   if(Cop==YES)
                   {
                        ret=CopyFile(FullSrcPath,FullDestPath, FALSE);
                        if(ret==0)
                        {
                           return FALSE;
                        }
                   }
                   if(Del==YES)
                   {
                      SetFileAttributes(FullSrcPath,FILE_ATTRIBUTE_NORMAL);
                      ret=DeleteFile(FullSrcPath);
			          if(ret==0)
			          {
                         return FALSE; 
                      }
                   }
                   IncrementFiles();
                   a.LowPart = info.nFileSizeLow;
                   a.HighPart = info.nFileSizeHigh;
                   IncrementSize(a.QuadPart);
            }  
        }
    }
    FindClose(h); //end of scan
    if(Del==YES)
    {
        SearchPath[len-4]=0; //clean wildchars and slash
       SetFileAttributes(SearchPath,FILE_ATTRIBUTE_NORMAL);
       if(RemoveDirectory(SearchPath) == 0)
       {
           return FALSE;
       }
       IncrementFolders();
    }
    return TRUE;
}


UCHAR HandleOperation(UCHAR Del,UCHAR Cop, UCHAR *Ret)
{     
     UINT i; BOOL ret; UCHAR Path[MAX_PATH],Dest[MAX_PATH]; int len;
     pNfiles=hNfilesO;
     pNfolders=hNfoldersO;
     pNsize=hNsizeO;
     NumberOfFolders=NumberOfFiles=0;
     TotalSize=0; 
     
 
     for(i=0;i<OP.nr;i++)
     {
          if(WaitForSingleObject(semafor,0)==0){ *Ret=1; return NO; }
          
          memset(Path,0,MAX_PATH);
          len=sprintf(Path,"%s\0",OP.SrcPath);
          if(Path[len-1]!='\\') strcat(Path,"\\");
          strcat(Path,OP.nfiles[i].FileName);
          memset(Dest,0,MAX_PATH);
          len=sprintf(Dest,"%s\0",OP.DestPath);
          if(Dest[len-1]!='\\') strcat(Dest,"\\");
          strcat(Dest,OP.nfiles[i].FileName);

          if(OP.nfiles[i].type==0)
          {
             ret=CopyDir(Path,Dest,Del,Cop,Ret);
             if(ret==FALSE)
             {
                 return NO;
             } 
          }
          else //isfile
          {
               if(Cop==YES)
               {
              /*      memset(Dest,0,MAX_PATH);
                    len=sprintf(Dest,"%s\0",OP.DestPath);
                    if(Dest[len-1]!='\\') strcat(Dest,"\\");
                    strcat(Dest,OP.nfiles[i].FileName);*/
                    ret=CopyFile(Path,Dest, FALSE);
                    if(ret==0)
                    {
                          return NO;
                    }
               }
               if(Del==YES)
               {
                      SetFileAttributes(Path,FILE_ATTRIBUTE_NORMAL);
                      ret=DeleteFile(Path);
			          if(ret==0)
			          {
                         return NO; 
                      }
               }
               IncrementFiles();
               IncrementSize(OP.nfiles[i].size);
            }  
        }
 return YES;    
}
