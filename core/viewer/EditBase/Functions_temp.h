#ifndef FUNCTIONS_TEMP_123i124j1239423040239402349_
#define FUNCTIONS_TEMP_123i124j1239423040239402349_

#include "editbasedef.h"
#include "License.h"

CString EXPORT_EDITBASE StrFromLocalResID(UINT id);
BOOL EXPORT_EDITBASE CheckObjectNum(int num);

#define BeginCheck41License if(CheckLicense(54)){
#define EndCheck41License }

#ifndef TRIAL_VERSION

void EXPORT_EDITBASE CheckLoadPath(void *path_in, void *path_out, int len_out);
void EXPORT_EDITBASE UpdateMatrix(void *m1, void *m2, int flag);

#define _CheckLoadPath(a,b,c)		CheckLoadPath(a,b,c)
#define _UpdateMatrix(a,b,c)		UpdateMatrix(a,b,c)

#else

void EXPORT_EDITBASE CheckLoadPath(void *path_out, int len_out, void *path_in);
void EXPORT_EDITBASE UpdateMatrix(void *m2, int flag, void *m1);

#define _CheckLoadPath(a,b,c)		CheckLoadPath(b,c,a)
#define _UpdateMatrix(a,b,c)		UpdateMatrix(b,c,a)

#endif
#endif
