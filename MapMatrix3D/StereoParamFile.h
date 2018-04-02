// StereoParamFile.h: interface for the CStereoParamFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOPARAMFILE_H__528F6C73_4A74_4F09_BCE4_ED25C19A1252__INCLUDED_)
#define AFX_STEREOPARAMFILE_H__528F6C73_4A74_4F09_BCE4_ED25C19A1252__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CStereoParamFile  
{
public:
	struct StereoParamItem
	{
		CString id;
		CStringArray names;
		CStringArray values;
	};

	CStereoParamFile();
	virtual ~CStereoParamFile();

	int SetStereoID(LPCTSTR id);
	CString ReadParam(LPCTSTR name);
	double ReadFloatParam(LPCTSTR name);
	int ReadIntParam(LPCTSTR name);
	void WriteParam(LPCTSTR name, LPCTSTR value);
	void WriteParam(LPCTSTR name, double value);
	void WriteParam(LPCTSTR name, int value);

	void Load();
	void Save();

	CString m_path;
	CArray<StereoParamItem*,StereoParamItem*> m_arrPItems;
	int m_nCurItem;
};

#endif // !defined(AFX_STEREOPARAMFILE_H__528F6C73_4A74_4F09_BCE4_ED25C19A1252__INCLUDED_)
