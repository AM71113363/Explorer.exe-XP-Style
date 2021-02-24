#ifndef H_LISTVIEW_H
#define H_LISTVIEW_H


extern HWND hListView;

extern RECT ListRect;
void CreateListView(HWND hwnd);
void ExploreDir(UCHAR *ThisDir,UCHAR isBack);
void ExploreDrive();
extern UCHAR CurrentDirListView[MAX_PATH];

void ChangeListViewStyle(DWORD);
int GetSelectedOnList();
int GetSelectedInfoText(UCHAR *Root,UCHAR *FileName);
int AddData(UCHAR Replace,UCHAR *FullPath,UCHAR *hFile,UCHAR *size,UCHAR *modified,UCHAR *ext,int c);

void GetItemTextFromIndex(int index,UCHAR *FileName,UINT size);


#endif  //PROTECT
