#ifndef H_TOOLBAR_H
#define H_TOOLBAR_H

extern RECT ToolRect;
extern HWND hToolBar;
void CreateToolbar(HWND hwnd);
void ToolBarButtonDropDown(HWND hwnd, LPNMHDR lpnm, LPNMTOOLBAR lpnmTB, int nIndex);
void Func_Up();
void ToolBarFuncDELETE();
void ToolBarFuncCUT();
void ToolBarFuncCOPY();
void ToolBarFuncPASTE();
void Back_Forw_status(UCHAR b,UCHAR f);

#endif  //PROTECT
