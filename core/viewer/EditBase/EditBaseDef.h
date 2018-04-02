#if !defined(AFX_EDITBASEDEF_H_INCLUDED_)
#define AFX_EDITBASEDEF_H_INCLUDED_


#ifdef EXPORT_EDITBASE
#define EXPORT_EDITBASE __declspec(dllexport)
#else
#define EXPORT_EDITBASE __declspec(dllimport)
#endif

#endif
