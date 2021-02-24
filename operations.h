#ifndef H_OPERATIONS_H
#define H_OPERATIONS_H


void GrowList(UINT size); //must call it after listview update list
void ResetListValue();   //just memset, secure if ptr=NULL
void UpdateListValue(UINT index,UINT value);
UCHAR IsValidOperation();   //return if there are selected items
UCHAR SetOperation(UCHAR o,UCHAR *Path);
UCHAR SetDestPath(UCHAR *Path,int in);  //set destPath for drag drop
UCHAR IsValidCutCopyOperation(UCHAR *Path);//return YES and Path is copied to DestPath


void ExecuteOperation();//

#endif  //PROTECT
