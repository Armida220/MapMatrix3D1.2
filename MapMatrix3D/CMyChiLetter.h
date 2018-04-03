
#if !defined(AFX_MYCHILETTER_H__3B396C17_CF90_43FC_7777_7F7EB028B011__INCLUDED_)
#define AFX_MYCHILETTER_H__3B396C17_CF90_43FC_7777_7F7EB028B011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMyChiToLetter
{
public:
	CMyChiToLetter();
	~CMyChiToLetter();

	void SetFlags(BOOL bSimple, BOOL bFirstBig, BOOL bAllBiG, BOOL bLetterEnd);
	CString GetLetter(LPCTSTR strText);

protected:	
	CString FindLetter(int nCode);

    BOOL m_blnSimple;    //首字母简拼，
    BOOL m_blnFirstBig;  //首字母大写
    BOOL m_blnAllBiG;    //全字母大写
    BOOL m_LetterEnd;    //用单引号隔开拼音
};


#endif