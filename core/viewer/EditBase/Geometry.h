// EBGeometry.h: interface for the CGeometry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBGEOMETRY_H__BB263257_1BD7_4EDB_9066_433AC373BE76__INCLUDED_)
#define AFX_EBGEOMETRY_H__BB263257_1BD7_4EDB_9066_433AC373BE76__INCLUDED_

#include "SmartViewBaseType.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrBuffer.h"
#include "Permanent.h"
#include "SearchStruct.h"
#include "Linearizer.h"
#include "GeometryBase.h"

#define FIELDNAME_GEOCLASS			_T("GEOCLASS")
#define FIELDNAME_GEOCOLOR			_T("COLOR")
#define FIELDNAME_SHAPE				_T("SHAPE")
#define FIELDNAME_SYMBOLNAME		_T("SYMBOLNAME")
#define FIELDNAME_SYMBOLIZEFLAG		_T("SYMBOLIZEFLAG")

//是否有文字注记
#define SYMFLAG_TEXT						0

//不显示面边界
#define SYMFLAG_HIDE_SURFACE_BOUND			1

//不显示面的内边界
#define SYMFLAG_HIDE_SURFACE_INNER_BOUND	2



#define COLOUR_BYLAYER              -1

enum CoverType
{
	COVERTYPE_NONE = 0,
	COVERTYPE_RECT = 1,
	COVERTYPE_CIRCLE = 2
};


MyNameSpaceBegin

struct PT_KEYCTRL
{
	PT_KEYCTRL()
	{
		type = typeNone;
		index = -1;
	}
	bool IsValid()
	{
		return ( (type==typeKey||type==typeCtrl) && index>=0 );
	}
	enum
	{
		typeNone=0,
		typeKey=1,
		typeCtrl=2
	};
	int type;
	int index;
};

//标志数据，占用4个字节；可以支持24种标志位 + 255 种枚举值，应该足够表示多种符号化标志了
struct EXPORT_EDITBASE FlagBit
{
	FlagBit();
	FlagBit(long a);
	~FlagBit();

	operator long();

	void SetBit(int nBit, BOOL bEnable);
	BOOL GetBit(int nBit)const;
	void SetEnumValue(BYTE value);
	BYTE GetEnumValue()const;

	unsigned long m_nValue;
};

class CShapeLine;
class CSymbol;
class EXPORT_EDITBASE CGeometry : public CGeometryBase
{
	DECLARE_DYNAMIC(CGeometry)
public:
	virtual BOOL  GetCenter(PT_3D *ipt,PT_3D* opt);
	CGeometry();
	virtual ~CGeometry();

	virtual int GetClassType()const;
	virtual Envelope GetEnvelope(){return Envelope();};

	virtual int GetCtrlPointSum()const;
	virtual PT_3DEX GetCtrlPoint(int i)const;
	virtual BOOL SetCtrlPoint(int i,PT_3D pt);

	virtual BOOL CreateShape(const PT_3DEX *pts, int npt)=0;
	virtual int GetDataPointSum()const;	
	virtual void GetShape(CArray<PT_3DEX,PT_3DEX>& arrPts)const=0;
	virtual BOOL SetDataPoint(int i,PT_3DEX pt);
	virtual PT_3DEX GetDataPoint(int i)const;

	virtual BOOL CheckValid()const;
    virtual const CShapeLine *GetShape() {return NULL;}
	virtual CGeometry* Clone()const=0;
	virtual BOOL CopyFrom(const CGeometry *pObj);

	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0)=0;
	virtual void Transform(double matrix[16]);

	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab,int idx = 0);

	virtual void SetColor(long color);
	virtual long GetColor();

	void SetSymbolName(const char* name);
	const char* GetSymbolName() const;
	BOOL IsSymbolizeText()const;
	void EnableSymbolizeText(BOOL bSymbolized);

	BOOL IsHideSymbolizeSurfaceBound()const;
	BOOL IsHideSymbolizeSurfaceInnerBound()const;	

	void SetSymbolizeFlagBit(int nBit, BOOL bEnable);
	BOOL GetSymbolizeFlagBit(int nBit)const;

	void SetSymbolizeFlagEnum(BYTE value);
	BYTE GetSymbolizeFlagEnum()const;

	long GetSymbolizeFlag()const;
	void SetSymbolizeFlag(long flag);

	virtual CGeometry *Linearize(){return NULL;};
	virtual PT_KEYCTRL FindNearestKeyCtrlPt(PT_3D sch_pt, double r, CCoordSys *pCS, int flag=3);
	virtual int FindNearestKeyLine(PT_3D sch_pt, double r, CCoordSys *pCS);

	// search a line segment in an object
	virtual BOOL FindNearestBaseLine(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *ret1, PT_3D *ret2, double *mindis);
	
	// search a point in an object
	virtual BOOL FindNearestBasePt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *mindis);

	void ReversePoints();

	//点是否在内部,0外部，1边线上，2内部
	virtual int bPtIn(const PT_3D *pt);

protected:
	long m_nColor;

	// 支持# @ *标识的符号类别, #为初步打散的结果, @为图元名或线型名, *为打散的最终形态(线串)
	// %颜色%  支持面的填充色
	char m_symname[32];

	long m_nSymbolizeFlag;	
};

MyNameSpaceEnd

#endif // !defined(AFX_EBGEOMETRY_H__BB263257_1BD7_4EDB_9066_433AC373BE76__INCLUDED_)
