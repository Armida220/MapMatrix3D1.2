#if !defined(AFX_License_H__1260A711_4153_43F3_ADAF_952FAD30AC46__INCLUDED_)
#define AFX_License_H__1260A711_4153_43F3_ADAF_952FAD30AC46__INCLUDED_

//functions in license
#define LOADDATA		1
#define CREATEOBJ		2
#define MATRIX_REV		3
#define LIC_INIT		4


void EXPORT_EDITBASE ClearLicense(int idx);
void EXPORT_EDITBASE InitLicense();
BOOL EXPORT_EDITBASE CheckLicense(int idx);
BOOL EXPORT_EDITBASE ExecuteLicense(int idx, int fun, int msg_id, LPVOID in, DWORD nIn, LPVOID out, DWORD nOut, LPDWORD pnret=NULL);
BOOL EXPORT_EDITBASE CreateMatrix3(double *m);

EXPORT_EDITBASE void *GetLicenseContext(int idx);

#endif