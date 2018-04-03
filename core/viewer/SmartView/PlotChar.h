// PlotChar.h: interface for the CPlotChar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOTCHAR_H__57B5C8EE_F7FA_4286_B7D9_2CB4599E345D__INCLUDED_)
#define AFX_PLOTCHAR_H__57B5C8EE_F7FA_4286_B7D9_2CB4599E345D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrBuffer.h "
#define PC_SIZE				100

MyNameSpaceBegin

class GrBuffer;
class EXPORT_SMARTVIEW CPlotChar  
{
public:
	CPlotChar();
	virtual ~CPlotChar();
	void SetEnChar(char c);
	void SetCnChar(char c1, char c2);
	BOOL Plot(CArray<GrVertex,GrVertex> *pts, CDC *pDC);
private:
	double FixedToDouble(FIXED fxy);
	FIXED FloatToFixed( double d );
	void DrawOutLine(CArray<GrVertex,GrVertex> *pts,int wType,int cpfx,POINTFX apfx[],POINTFX &pfxStart);

public:
	//是否解释样条
	BOOL m_bPlotSpline;
private:
	int m_nChar;
};

MyNameSpaceEnd

#endif // !defined(AFX_PLOTCHAR_H__57B5C8EE_F7FA_4286_B7D9_2CB4599E345D__INCLUDED_)
