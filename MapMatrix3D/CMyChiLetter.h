
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

    BOOL m_blnSimple;    //����ĸ��ƴ��
    BOOL m_blnFirstBig;  //����ĸ��д
    BOOL m_blnAllBiG;    //ȫ��ĸ��д
    BOOL m_LetterEnd;    //�õ����Ÿ���ƴ��
};


#endif