#ifndef H_ADDRESSBAR_H
#define H_ADDRESSBAR_H


extern HWND hAddressBar;
extern HWND hComboBox;
extern HWND ComboEdit;
extern RECT AddressRect;
extern RECT ComboRect;

void CreateAddressbar(HWND hwnd);
void DefaultComboView(UCHAR *Path);
void AddComboView(UCHAR *Path);
int GoComboEdit(UCHAR IsBack);

void Func_Back();
void Func_Forw();

#endif  //PROTECT
