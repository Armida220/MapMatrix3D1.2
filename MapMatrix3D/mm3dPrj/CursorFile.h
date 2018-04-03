// CursorFile.h: interface for the CCursorFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURSORFILE_H__603082AC_E3C3_46ED_A403_52F25B951A46__INCLUDED_)
#define AFX_CURSORFILE_H__603082AC_E3C3_46ED_A403_52F25B951A46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

MyNameSpaceBegin

class CCursorFile
{
public:
	CCursorFile();
	virtual ~CCursorFile();
	bool LoadCursor(const char* name);
	bool IsValid();
	int GetTypeSum();
	CSize GetSize();
	int GetCursorBitWid();
	int GetType();
	void SetType(int type);
	char *GetTypeData();

private:
	int m_nSum;
	int m_nCol;
	int m_nRow;
	int m_nCurType;

	int m_nBitWid;
	char *m_pCursorData;
	char *m_pCurData;
	bool m_bValid;
};

MyNameSpaceEnd

#endif // !defined(AFX_CURSORFILE_H__603082AC_E3C3_46ED_A403_52F25B951A46__INCLUDED_)
