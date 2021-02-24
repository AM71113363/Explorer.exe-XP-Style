#ifndef H_MENUBAR_H
#define H_MENUBAR_H


extern HWND hMenuBar;
extern RECT MenuRect;
void CreateMenubar(HWND hwnd);
void MenuBarDropDown(HWND hwnd, UINT ID, int nIndex);

void SetMenuCheck(UINT n); //only use for dropmenu listeview style
void SetItemCheck(UCHAR w,UINT id);//w=0 unchek w=1 check

#endif  //PROTECT
