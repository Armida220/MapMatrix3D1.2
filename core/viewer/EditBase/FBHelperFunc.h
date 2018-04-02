#pragma once


void EXPORT_EDITBASE GSetProgressHwnd(HWND hWnd);
void EXPORT_EDITBASE GProgressStart(int sum);
void EXPORT_EDITBASE GProgressStep(int step=1);
void EXPORT_EDITBASE GProgressEnd();
void EXPORT_EDITBASE GOutPut(LPCTSTR str, BOOL bNewLine=TRUE);
