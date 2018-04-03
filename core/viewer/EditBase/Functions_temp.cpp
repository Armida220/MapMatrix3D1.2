#include <stdafx.h>

#include "Functions_temp.h"
#include <float.h>
#include <math.h>
#include "matrix.h"
#include "SmartViewFunctions.h"
#include "License.h"

#include "FBHelperFunc.h"

#define LICEDU_OBJLIMIT			300

extern AFX_EXTENSION_MODULE EditBaseDLL;

BOOL LoadMyString(CString &str, UINT uID)
{
	if( !EditBaseDLL.hResource )return FALSE;
	TCHAR tmpstr[256];
	
	if (::LoadString(EditBaseDLL.hResource,uID,tmpstr,sizeof(tmpstr)-1) == 0)
		return FALSE;
	
	str = tmpstr;
	return TRUE;
}

CString StrFromLocalResID(UINT id)
{
	CString str;
	if (!LoadMyString(str,id))
	{
		str = StrFromResID(id);
	}
	
	return str;
}

#ifndef TRIAL_VERSION
void CheckLoadPath(void *path_in, void *path_out, int len_out)
#else
void CheckLoadPath(void *path_out, int len_out, void *path_in)
#endif

{
	char *p1 = (char*)path_in;
	char *p2 = (char*)path_out;

#ifdef _LICENSE
	char input[256] = {0};
	strcpy(input,p1);
	if( CheckLicense(1) )
	{
		ExecuteLicense(0,LOADDATA,0,input, strlen(input), p2, len_out); 
		strcpy(p2, p1);
	}
#endif

}

void Matrix_reverse31(double *m, double *r);

#ifndef TRIAL_VERSION
void UpdateMatrix(void *m1, void *m2, int isInit)
#else
void UpdateMatrix(void *m2, int isInit, void *m1)
#endif
{
	double *p1 = (double*)m1;
	double *p2 = (double*)m2;

	if( isInit )CreateMatrix3(p1);
	
#ifdef _LICENSE
	//¼ÓÃÜ
	//if( CheckLicense(1) )
	{
		ExecuteLicense(0,MATRIX_REV,0,p1,sizeof(double)*9,m2,sizeof(double)*9); 
		Matrix_reverse31(p1, p2);
	}
#endif
}


BOOL CheckObjectNum(int num)
{
#if defined(_LICENSE)&&defined(_LICEDU)
	if( num>LICEDU_OBJLIMIT )
		return FALSE;

#endif
	return TRUE;
}