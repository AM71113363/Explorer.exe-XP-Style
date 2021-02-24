#ifndef H_TREEVIEW_H
#define H_TREEVIEW_H


extern HWND hTreeView;
extern RECT TreeRect;
void CreateTreeView(HWND hwnd);
void RefreshTree(NM_TREEVIEW *lParam);
UCHAR *GetFullPath(HTREEITEM hselect);
void ResetTreeView();

#endif  //PROTECT
