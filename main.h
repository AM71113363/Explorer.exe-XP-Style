#ifndef H_MAIN_H
#define H_MAIN_H

HWND MainHWND;

void CenterOnScreen(HWND hnd);
extern HWND AngleFoto;
extern RECT AngleRect;
void CreateAngleFoto(HWND hwnd);

typedef struct _help
{
   UCHAR IsBack;
   UCHAR ThisDir[MAX_PATH];
}HELP;

static HELP HLP;

void ExploreDirThread(VOID *H);


#endif  //PROTECT
