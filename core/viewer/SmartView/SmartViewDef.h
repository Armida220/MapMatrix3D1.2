#if !defined(AFX_SMARTVIEWDEF_H_INCLUDED_)
#define AFX_SMARTVIEWDEF_H_INCLUDED_


#ifdef EXPORT_SMARTVIEW
#define EXPORT_SMARTVIEW __declspec(dllexport)
#else
#define EXPORT_SMARTVIEW __declspec(dllimport)
#endif

#define MyNameSpaceName				EditBase
#define MyNameSpaceBegin			namespace MyNameSpaceName{
#define MyNameSpaceEnd				}

#endif