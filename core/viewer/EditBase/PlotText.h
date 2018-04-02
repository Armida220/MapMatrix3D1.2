// PlotText.h: interface for the CPlotText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOTTEXT_H__6E704628_D4A3_4FCC_8B09_C5D542C1B670__INCLUDED_)
#define AFX_PLOTTEXT_H__6E704628_D4A3_4FCC_8B09_C5D542C1B670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


extern DWORD gdwCharSet;

#define INIT_CHARSET 0xffffffff
#define AUTO_CHARSET (PRIMARYLANGID(GetThreadLocale())==LANG_CHINESE?GB2312_CHARSET:ANSI_CHARSET)
#define MY_CHARSET	((gdwCharSet!=INIT_CHARSET)?gdwCharSet:ANSI_CHARSET)

MyNameSpaceBegin

class GrBuffer;
class CShapeLine;


// 对CPlotText的重写，使用实心，抗锯齿文字
//为简单起见，统一处理宽字符
class EXPORT_EDITBASE CPlotTextEx  
{
public:
	struct CharWH
	{
		CharWH(){
			nextPos = wid = hei = 0;
		}
		double nextPos;
		double wid;
		double hei;
	};

	CPlotTextEx();
	virtual ~CPlotTextEx();
	void SetText(LPCTSTR text);
	void SetSettings(TEXT_SETTINGS0 *settings);
	void SetShape(const CShapeLine *pSL);
	void GetAddConstGrBuffer(GrBuffer *pBuf);
	void GetConstGrBuffer(GrBuffer *pBuf);
	void GetVariantGrBuffer(GrBuffer *pBuf, int varPt = -1);
	void GetOutLineBuf(GrBuffer* pBuf);
	BOOL Plot(LPCTSTR strText, PT_3D *bpts, int ptsnum, TEXT_SETTINGS0 *setting,BOOL bGrdSize, GrBuffer* pBuf, BOOL bSepMode=FALSE);
protected:
	void DrawText(GrBuffer *pBuf);
	void CreateCharWH();
private:
	CPtrArray m_arrpPts;
	CArray<CharWH,CharWH> m_arrWHs;
	int m_npPts;	
	double m_lfAllCharLen;//沿曲线均匀排列时使用
	const CShapeLine *m_pShape;
	TEXT_SETTINGS0 m_settings;
	CString	 m_charList;
};

MyNameSpaceEnd

#endif // !defined(AFX_PLOTTEXT_H__6E704628_D4A3_4FCC_8B09_C5D542C1B670__INCLUDED_)
