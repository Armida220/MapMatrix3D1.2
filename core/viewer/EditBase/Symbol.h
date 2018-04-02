// EBSymbol.h: interface for the CSymbol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBSYMBOL_H__E3EE941E_188B_407E_94D1_01408A73C8C8__INCLUDED_)
#define AFX_EBSYMBOL_H__E3EE941E_188B_407E_94D1_01408A73C8C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SymbolBase.h"
#include "Feature.h"
#include "GrBuffer2d.h"


#define SYMTYPE_SCALE_LiangChang			20
#define SYMTYPE_SCALE_YouGuan				21
#define SYMTYPE_SCALE_JianFangWu			22
#define SYMTYPE_SCALE_TongCheShuiZha		23
#define SYMTYPE_SCALE_BuTongCheShuiZha		24
#define SYMTYPE_SCALE_CellLinetype			25
#define SYMTYPE_SCALE_DiShangYaoDong		26
#define SYMTYPE_SCALE_ChuanSongDai          27
#define SYMTYPE_SCALE_ChuRuKou				28
#define SYMTYPE_SCALE_Cell					29
#define SYMTYPE_SCALE_OldDouya				30
#define SYMTYPE_SCALE_Xiepo					31
#define SYMTYPE_SCALE_WenShi				32
#define SYMTYPE_TIDALWATER					33		//潮水沟
#define SYMTYPE_CULVERTSURFACE1             35      //涵洞面1
#define SYMTYPE_CULVERTSURFACE2             36      //涵洞面2


#define SYMFLAG_ENUM_ZERO					0
#define SYMFLAG_ENUM_NODIANGAN1				1
#define SYMFLAG_ENUM_NODIANGAN2				2
#define SYMFLAG_ENUM_NODIANGAN3				3


MyNameSpaceBegin


extern DWORD g_nScale;
EXPORT_EDITBASE void SetDataSourceScale(DWORD nScale);
EXPORT_EDITBASE float GetSymbolDrawScale();
EXPORT_EDITBASE void SetSymbolDrawScale(float fDrawScale);
EXPORT_EDITBASE void SetSymbolAnnoScale(float fAnnoScale);

float GetSymbolAnnoScale();

int GGetPtFromLineWidth(PT_3DEX *pts, int num, PT_3DEX pt, double dWidth, PT_3DEX *dpt, BOOL bNextNode=TRUE);

BOOL FillLineWidth(PT_3DEX *pts, int nSum, float fFtrWidth, float ftrLinewidthScale, GrBuffer *pBuf, COLORREF clr, float fDrawScale);

void EXPORT_EDITBASE ConvertGrBufferToGeos(GrBuffer *pBuf, float fDrawScale, CGeoArray &arr);


EXPORT_EDITBASE void UpdateGlobalSymbolizeParams();

//符号
class EXPORT_EDITBASE CSymbol  
{
public:
	CSymbol();
	virtual ~CSymbol();

	int GetType()const{
		return m_nType;
	}
	
	virtual LPCTSTR GetName()const{
		return m_strName;
	}
	void SetName(LPCTSTR name){
		m_strName = name;
	}

	virtual void CopyFrom(CSymbol *pSymbol) {};

	virtual BOOL IsNeedAttrWhenDraw();

	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, CValueTable& tab, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType)=0;

	virtual BOOL Load(CString& strXML)=0;
	virtual void Save(CString& strXML)=0;

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	virtual BOOL Explode(const CFeature *pFt, CValueTable& tab, float fDrawScale, CGeoArray& arrPGeos);
	virtual BOOL ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	virtual BOOL ExplodeSimple(const CFeature *pFt, CValueTable& tab, float fDrawScale, CGeoArray& arrPGeos);

	static float m_tolerance;
	static BOOL m_bBreakCell;
	static BOOL m_bBreakLinetype;
protected:
	int m_nType;
	CString m_strName;
};


typedef CArray<CSymbol*,CSymbol*> CSymbolArray;

EXPORT_EDITBASE CSymbol *GCreateSymbol(int type);

EXPORT_EDITBASE BOOL GIsSymbolSelfColor(CSymbol *pSym);

//单元符号
class EXPORT_EDITBASE CCell : public CSymbol
{
public:
	//放置位置：0，首点，1，尾点，2，中点，3，中心点，
	enum PlaceType
	{
		Head = 0,
		Tail = 1,
		Middle = 2,
		Center = 3
	};

	enum FillType
	{
		BaseLine = 1, //基线
		AidLine = 2,  //辅助线（例如，平行线地物上的辅助线）
		MidLine = 3,  //中线
	};

	// 放在中心时线段的方向; 0,无连接 1,与首尾点相连 2,与主辅线的中点连接
	enum CenterLineDir
	{
		cenNULL	     = 0,
		cenHead2Tail = 1,
		cenMid2Mid   = 2
	};
	
	CCell();
	virtual ~CCell();

	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strCellDefName;
	}

	//彻底打散
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);

	//不彻底打散
	virtual BOOL ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
public:

	// 压盖类型: 0, 无压盖 1,矩形压盖 2,圆形压盖
	int     m_nCoverType;
	// 压盖外扩距离
	float   m_fExtendDis;

	//图元的缩放和旋转
	double m_kx;
	double m_ky;
	double m_angle;
	
	//放置位置
	int    m_nPlaceType;

	//基线还是辅助线
	int m_nFillType;

	// 仅当符号放在中心时有效
	int m_nCenterLineMode;
	// 中心点时方向是否随首段线
	bool m_bDirWithFirstLine;

	//偏移距离
	double m_dx;
	double m_dy;

	//线宽
	float m_fWidth;
	
	//图元名称
	CString m_strCellDefName;
};

class CGeoParallel;
//基本线型（虚线线型）
class EXPORT_EDITBASE CDashLinetype : public CSymbol
{
public:
	enum
	{
		BaseLine = 1, //基线
		AidLine = 2,  //辅助线（例如，平行线地物上的辅助线）
		MidLine = 3  //中线
	};
	enum IndentType
	{
		Node = 0,    // 节点前后缩进
		Mid = 1,     // 中点前后缩进
		HeadTail = 2	// 首尾缩进
	};
	// 与基线的距离
	float m_fBaseOffset;

	//线宽
	float m_fWidth;

	// 起始偏移
	float m_fXOffset0;

	// 缩进长度
	float m_fIndent;

	// 缩进类型：节点前后,中点前后
	int   m_nIndentType;

	//引用的虚线线型符号名
	CString m_strBaseLinetypeName;

	//渲染方式（BaseLine或者AidLine）
	int m_nPlaceType;

	// 是否使用独立颜色（缺省为否）
	BOOL m_bUseSelfcolor;

	// 颜色值
	DWORD m_nColor;

	//是否自动调整起点偏移，使得头尾是实线，中间是虚线
	BOOL m_bAdjustXOffset;

	BOOL m_bDashAlign;

public:
	CDashLinetype();
	virtual ~CDashLinetype();

	virtual void CopyFrom(CSymbol *pSymbol);

	//彻底打散
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);

	//不彻底打散
	virtual BOOL ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
public:	
	static BOOL GetAssistPtsWithHeight(PT_3DEX *pts, int nSum, float fAssistWid, CArray<PT_3DEX,PT_3DEX> &arrRet, CGeoParallel *pGeo=NULL);
	void DashLine(PT_3DEX *pts, int nSum, GrBuffer *pBuf, COLORREF clr, float width, float fViewDrawScale, BOOL bGrdWid, float fDrawScale, 
		float xoff, int dwStyleCount,const float *lpStyle, int nStyleIndex, BOOL bBreakLinetype,BOOL bAdjustXOffset,float fAssistWid,CGeometry *pGeo);

	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
};

//复杂线型（点符号填充）
class EXPORT_EDITBASE CCellLinetype : public CSymbol
{
public:
	enum PlacePos
	{
		Cycle = 1,			//等距离周期放置
		Vertex = 2,			//每个顶点放置一个
		VertexDouble = 3	//每个顶点放置前后两个，但是端点只放置一个（例如，电力线符号）
	};
	enum PlaceType
	{
		BaseLine = 1, //基线
		AidLine = 2,  //辅助线（例如，平行线地物上的辅助线）
		MidLine = 3,  //中线
	};

	// 偏移
	float m_fXOffset;
	float m_fYOffset;	

	//图元的缩放和旋转
	float m_fAngle;
	float m_fkx;
	float m_fky;

	//基线还是辅助线
	int m_nPlaceType;

	// 相对于母线的垂直偏移
	float m_fBaseXOffset,m_fBaseYOffset;

	// 渲染类型: 周期循环, 顶点放置等等
	int m_nPlacePos;

	// 周期；只在周期循环类型时有用
	float m_fCycle;

	//线宽
	float m_fWidth;

	// 图元引用名
	CString m_strCellDefName;

	//是否需要对齐图元（像虚线线型一样）
	BOOL m_bCellAlign;

	//是否要求图元整体不超出基线范围（这时，需要考虑图元的宽度）
	BOOL m_bInsideBaseLine;

public:
	CCellLinetype();
	virtual ~CCellLinetype();

	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);

	void ConvertToDashLinetype(CDashLinetype *pLT);
	
public:		
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strCellDefName;
	}

protected:
	// arrindex为dpts在基线上最近的点索引
	void TransformLine(CArray<PT_3DEX,PT_3DEX> &pts,CArray<PT_3DEX,PT_3DEX> &dpts, CUIntArray &arrindex, float cycle, float fbaseXoff, float fbaseYoff, 
		float fAssitWid, float fDrawScale=1.0);

	
};

//颜色填充符号
class EXPORT_EDITBASE CColorHatch : public CSymbol
{
public:

	// 是否使用独立颜色（缺省为否）
	BOOL m_bUseSelfcolor;

	//颜色
	DWORD m_nColor;

	//透明度
	float m_fTransparence;

public:	
	CColorHatch();
	virtual ~CColorHatch();

	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
protected:
};



//关联属性的颜色填充符号
class EXPORT_EDITBASE CConditionColorHatch : public CSymbol
{
public:
	struct Condition
	{
		TCHAR  value[64];
		DWORD color;
		float fTransparence;
	};
		
	CString m_strFieldName;

	//颜色
	DWORD m_nDefColor;

	//透明度
	float m_fDefTransparence;	

	CArray<Condition,Condition> m_Conditions;

public:	
	CConditionColorHatch();
	virtual ~CConditionColorHatch();

	virtual BOOL IsNeedAttrWhenDraw();
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	virtual BOOL Explode(const CFeature *pFt, CValueTable &tab, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf,CValueTable &tab, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
protected:
};


//单元填充符号
class EXPORT_EDITBASE CCellHatch : public CSymbol
{
public:
	//图元的名称
	CString	m_strCellDefName;

	//线宽
	float m_fWidth;

	//图元的缩放和旋转
	float m_fAngle;
	float m_fkx;
	float m_fky;

	//填充的横向/纵向间距，以及行的横向偏差
	float m_fdx;
	float m_fdy;
	float m_fddx;
	float m_fddy;
	float m_fxoff;
	float m_fyoff;

	//是否精确裁剪图元
	int   m_bAccurateCutCell;
	//图元的角度为随机角度
	int	  m_bRandomAngle;

	static BOOL m_bAccuTrimSurface;
	static BOOL m_bkeepOneCell;

public:	
	CCellHatch();
	virtual ~CCellHatch();

	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strCellDefName;
	}
protected:

};

class CGeoText;
class EXPORT_EDITBASE CAnnotation : public CSymbol
{
public:	
	//注记类型
	enum AnnoType
	{
		Text = 0,		//文字
		Height = 1,		//高程
		DHeight = 2,	//比高
		Attribute = 3,	//属性
		LayerConfig = 4, //层配置
		Length = 5, //长度
		Area = 6 //面积
	};
	//放置位置：0，首点，1，尾点，2，中点，3，中心点，4，每个顶点，5，水平均匀排列，6，横向均匀排列
	//9, 闭合地物中心点，若地物不闭合，则不显示
	enum PlaceType
	{
		Head = 0,
		Tail = 1,
		Middle = 2,
		Center = 3,
		PerVertex = 4,
		ByLineHoriz = 5,
		ByLineHorizEven = 6,
		ByLineVert = 7,
		ByLineVertEven = 8,
		ByClosedFtrCenter = 9
	};

	CAnnotation();
	virtual ~CAnnotation();

	virtual BOOL IsNeedAttrWhenDraw();

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	virtual BOOL Explode(const CFeature *pFt, CValueTable &tab, float fDrawScale, CGeoArray& arrPGeos);

	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf,CValueTable &tab, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);

	BOOL ExtractGeoText(const CFeature *pFtr, CPtrArray &geoText, CValueTable &tab, float fDrawScale=1.0, float fRotAngle=0);
	
	static BOOL m_bUpwardText;
	static BOOL m_bBreakText;
	static BOOL m_bAnnotToText;
	static BOOL m_bNotDisplayAnnot;

public:
	//注记类型
	int m_nAnnoType;
	
	//文字内容，仅当文字类型为Text时有效
	TCHAR m_strText[128];

	TCHAR m_strField[32];

	//0, 首点，1，尾部点，2，中点，3，中心点，4，每个顶点，5，水平均匀排列，6，横向均匀排列
	int m_nPlaceType;	
	
	//偏移
	float m_fXOff, m_fYOff;
	
	//注记小数点位数
	int m_nDigit;
	
	//注记文本的设置
	TextSettings  m_textSettings;

	// 压盖类型: 0, 无压盖 1,矩形压盖 2,圆形压盖
	int     m_nCoverType;
	// 压盖外扩距离
	float   m_fExtendDis;
};

class CGeoParallel;

//依比例线型（平行线的宽度方向）
class EXPORT_EDITBASE CScaleLinetype : public CSymbol
{
public:
	CScaleLinetype();
	virtual ~CScaleLinetype();

	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	virtual BOOL ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:
	// 将线dpt沿着基线pts的方向放置，ratatept为旋转中心
	double LayAlongLine(PT_3DEX *pts, int num, PT_3DEX *dpt, int dptnum, PT_3DEX *ratatept);

public:

	// 是否均匀过渡
	BOOL m_bAverageDraw;
	
	// 放置类型：0,固定周期, 1,依比例周期, 2,节点, 3,首端, 4,尾端, 5,首尾两端，6,中点
	int m_nPlaceType; 
	
	// 周期长度，仅当 placetype=0,1 时有效
	double m_lfCycle;

	// 起点水平偏移的长度，(固定长度的偏移) 
	double m_lfXOffset0;

	// 起点垂直偏移的长度，(固定长度的偏移) 
	double m_lfYOffset0;
	
	// 起点垂直偏移的依比例的系数
	double m_lfYOffsetType0;

	// 终点水平偏移的长度，(固定长度的偏移) 
	double m_lfXOffset1;

	// 终点垂直偏移的长度，(固定长度的偏移) 
	double m_lfYOffset1;
	
	// 终点垂直偏移的依比例的系数
	double m_lfYOffsetType1;

	// 线宽
	double m_lfWidth;
	
	// 基本线型名称
	CString	m_strBaseLinetypeName;
	
};


// 晕线填充符号
class EXPORT_EDITBASE CLineHatch : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;		
	// 角度
	float m_fAngle;
	// 间距
	float m_fIntv;
	
public:	
	CLineHatch();
	virtual ~CLineHatch();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:
	
};

// 对角线有关
class EXPORT_EDITBASE CDiagonal : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;	
	// 0，仅对角线；1，输电线入地口
	int m_nDiagonalType;
public:	
	CDiagonal();
	virtual ~CDiagonal();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:
	
};

// 平行阶梯
class EXPORT_EDITBASE CParaLinetype : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;
	//间距
	float m_fIntv;	
public:	
	CParaLinetype();
	virtual ~CParaLinetype();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:
	
};

// 角平分线
class EXPORT_EDITBASE CAngBisectortype : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;
	//长度
	float m_fLength;	
public:	
	CAngBisectortype();
	virtual ~CAngBisectortype();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:
	
};

// 依比例圆弧
class EXPORT_EDITBASE CScaleArctype : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;
	// 露出弧长
	float m_fArcLength;	
	// 放置类型：0,首端, 1,尾端, 2,首尾两端
	int m_nPlaceType; 
public:	
	CScaleArctype();
	virtual ~CScaleArctype();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:
	
};

// 转盘
class EXPORT_EDITBASE CScaleTurnplatetype : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;

	//枕木线宽
	float m_fWidth;

	//枕木突出长度
	float m_fStickupLen;

	// 轨道宽度
	float m_fRoadWidth;	

	//轨道的边线线宽
	float m_fSideLineWidth;

	//中间填单线铁路（不依比例铁路）
	BOOL m_bSingleLine;
public:	
	CScaleTurnplatetype();
	virtual ~CScaleTurnplatetype();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:

};

// 吊车
class EXPORT_EDITBASE CScaleCranetype : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;
public:	
	CScaleCranetype();
	virtual ~CScaleCranetype();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:
	
};

// 国际新版：装卸漏斗（普通依比例和斗在坑内的（包括圆形和方形））
// 国际旧版：漏斗（斗在坑内的）
class EXPORT_EDITBASE CScaleFunneltype : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;
	// 国际新版：0，普通依比例；1，斗在坑内
	// 国际旧版：3，斗在坑内；
	int m_nFunnelType;
public:	
	CScaleFunneltype();
	virtual ~CScaleFunneltype();
	
	virtual void CopyFrom(CSymbol *pSymbol);

	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
protected:

	void GetShortLine(PT_3D pt1, PT_3D pt2, PT_3D pt3, double dis, double len, BOOL bClockwise, PT_3D& pt4, PT_3D& pt5);
	BOOL Draw_core(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	
};


// 只是仅仅只有线宽和线型名的程序类符号的基类，便于重用
class EXPORT_EDITBASE CProcSymbol_LT : public CSymbol
{
public:
	//虚线线型符号名
	CString m_strBaseLinetypeName;
	//线宽
	float m_fWidth;
	
	CProcSymbol_LT();
	virtual ~CProcSymbol_LT();
	
	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strBaseLinetypeName;
	}
};


// 依比例粮仓
class EXPORT_EDITBASE CScaleLiangCang : public CProcSymbol_LT
{
public:	
	CScaleLiangCang(){
		m_nType = SYMTYPE_SCALE_LiangChang;
	}
	virtual ~CScaleLiangCang(){}
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};

// 油罐
class EXPORT_EDITBASE CScaleYouGuan : public CProcSymbol_LT
{
public:	
	CScaleYouGuan(){
		m_nType = SYMTYPE_SCALE_YouGuan;
		m_bFilled = FALSE;
	}
	virtual ~CScaleYouGuan(){}
	virtual void CopyFrom(CSymbol *pSymbol);
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);

	BOOL m_bFilled;
};


// 简单房屋
class EXPORT_EDITBASE CScaleJianFangWu : public CProcSymbol_LT
{
public:	
	CScaleJianFangWu(){
		m_nType = SYMTYPE_SCALE_JianFangWu;
	}
	virtual ~CScaleJianFangWu(){}
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};

// 依比例地上窑洞
class EXPORT_EDITBASE CScaleDiShangYaoDong : public CProcSymbol_LT
{
public:	
	CScaleDiShangYaoDong(){
		m_nType = SYMTYPE_SCALE_DiShangYaoDong;
	}
	virtual ~CScaleDiShangYaoDong(){}
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};

// 通车水闸
class EXPORT_EDITBASE CScaleTongCheShuiZha : public CProcSymbol_LT
{
public:	
	CScaleTongCheShuiZha(){
		m_nType = SYMTYPE_SCALE_TongCheShuiZha;
	}
	virtual ~CScaleTongCheShuiZha(){}
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};


// 不通车水闸
class EXPORT_EDITBASE CScaleBuTongCheShuiZha : public CProcSymbol_LT
{
public:	
	CScaleBuTongCheShuiZha(){
		m_nType = SYMTYPE_SCALE_BuTongCheShuiZha;
	}
	virtual ~CScaleBuTongCheShuiZha(){}
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};


#define XMLTAG_CARRYTYPE		_T("CarryType")

// 传送带
class EXPORT_EDITBASE CScaleChuanSongDai : public CProcSymbol_LT
{
public:	
	CScaleChuanSongDai(){
		m_nCarryType = 0;
		m_nType = SYMTYPE_SCALE_ChuanSongDai;
	}
	virtual ~CScaleChuanSongDai(){}

	virtual void CopyFrom(CSymbol *pSymbol);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	

	// 0，地上；1，地下
	int m_nCarryType;
};
// 依比例地表出入口
class EXPORT_EDITBASE CScaleChuRuKou : public CProcSymbol_LT
{
public:	
	CScaleChuRuKou(){
		m_nType = SYMTYPE_SCALE_ChuRuKou;
	}
	virtual ~CScaleChuRuKou(){}
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};


// 新版依比例温室
class EXPORT_EDITBASE CScaleWenShi : public CProcSymbol_LT
{
public:	
	CScaleWenShi(){
		m_nType = SYMTYPE_SCALE_WenShi;
	}
	virtual ~CScaleWenShi(){}
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};


//依比例图元符号，两点控制横向比例，第三点控制纵向比例
class EXPORT_EDITBASE CScaleCell : public CSymbol
{
public:
	// 图元引用名
	CString m_strCellDefName;

	//横轴控制点的起点X坐标、落点X坐标
	float m_fX1, m_fX2;

	//纵轴控制点的Y坐标的绝对值
	float m_fY1;

	//线宽
	float m_fWidth;

public:
	CScaleCell();
	virtual ~CScaleCell();
	
	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
public:		
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strCellDefName;
	}
};

class EXPORT_EDITBASE CScaleCellLinetype : public CSymbol
{
public:	
	
	//图元的缩放
	float m_fkx;
		
	// 相对于母线的垂直偏移
	float m_fBaseXOffset,m_fBaseYOffset;

	// 图元与辅助线的间距
	float m_fAssistYOffset;
		
	// 周期；
	float m_fCycle;
	
	//线宽
	float m_fWidth;
	
	// 图元引用名
	CString m_strCellDefName;
	
public:
	CScaleCellLinetype();
	virtual ~CScaleCellLinetype();
	
	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
		
public:		
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strCellDefName;
	}
	
protected:

};


//旧版依比例陡崖
class EXPORT_EDITBASE CScaleOldDouya : public CSymbol
{
public:
	//周期长度
	float m_fCycle;

	//最大齿长
	float m_fMaxToothLen;
	
	// 齿间距
	float m_fInterval;
	
	//线宽
	float m_fWidth;
	
public:
	CScaleOldDouya();
	virtual ~CScaleOldDouya();
	
	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
public:		
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return _T("OldDouya");
	}
	
protected:
	
};


//依比例斜坡
class EXPORT_EDITBASE CScaleXiepo : public CSymbol
{
public:		
	//垂直与母线方向的偏移
	float m_fBaseYOffset;

	// 齿间距
	float m_fInterval;

	//短齿长度
	float m_fToothLen;
	
	//线宽
	float m_fWidth;

	//是否加固
	BOOL m_bFasten;

	//加固点符号的尺寸
	float m_fPointSize;

	//加固点与短齿的间距
	float m_fPointInterval;

	//角度均匀过渡
	BOOL m_bAverageDraw;
	
public:
	CScaleXiepo();
	virtual ~CScaleXiepo();
	
	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
public:		
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return _T("Xiepo");
	}
	
protected:
	
};


//潮水沟
class EXPORT_EDITBASE CTidalWaterSymbol : public CSymbol
{
public:	
	// 图元引用名
	CString m_strCellDefName;

	//图元的缩放系数范围
	float m_fkxMin, m_fkxMax;

	//图元间距
	float m_fInterval;

	//线宽
	float m_fWidth;

public:
	CTidalWaterSymbol();
	virtual ~CTidalWaterSymbol();
	
	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
public:		
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual LPCTSTR GetName()const{
		return m_strCellDefName;
	}
	
protected:
};


//涵洞-面-1
class EXPORT_EDITBASE CCulvertSurface1Symbol : public CSymbol
{
public:
	CString m_strLinetypeName;
	//线宽
	float m_fWidth;
public:	
	CCulvertSurface1Symbol()
	{
		m_nType = SYMTYPE_CULVERTSURFACE1;
		m_strName = "HanDongMian1";
		m_fWidth = 0;
	}
	virtual ~CCulvertSurface1Symbol(){}
	virtual void CopyFrom(CSymbol *pSymbol);
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
public:
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};

//涵洞-面-2
class EXPORT_EDITBASE CCulvertSurface2Symbol : public CSymbol
{
public:
	CString m_strLinetypeName;
	//线宽
	float m_fWidth;
public:	
	CCulvertSurface2Symbol()
	{
		m_nType = SYMTYPE_CULVERTSURFACE2;
		m_strName = "HanDongMian2";
		m_fWidth = 0;
	}
	virtual ~CCulvertSurface2Symbol(){}
	virtual void CopyFrom(CSymbol *pSymbol);
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
public:
	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual BOOL IsSupportGeoType(int nGeoType);
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};


class CCADSymbolLib;
class EXPORT_EDITBASE CCADSymbol : public CSymbol
{
public:
	enum
	{
		typeNone = 0,
		typeLin = 1,
		typeHat = 2
	};
	CCADSymbol();
	virtual ~CCADSymbol();

	virtual void CopyFrom(CSymbol *pSymbol);
	
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);

	virtual BOOL Load(CString& strXML);
	virtual void Save(CString& strXML);	
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	virtual BOOL IsSupportGeoType(int nGeoType);

	void SetCADSymLib(CCADSymbolLib *p);
	CCADSymbolLib *GetCADSymLib();

	void SetCADSymType(int type);
	int GetCADSymType();

protected:
	void DrawLin(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
	void DrawPat(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);

protected:
	CCADSymbolLib *m_pCADSymLib;
	int m_nCADSymType;
};


class EXPORT_EDITBASE CSymbolFlag
{
public:
	enum{
		BreakCell = 0,
		BreakLinetype = 1,
		BreakText = 2,
		TextUpward = 3,
		Tolerance = 4
	};
	CSymbolFlag(int type, BOOL bEnable)
	{
		nType = type;
		switch( type )
		{
		case BreakCell:
			bSave = CSymbol::m_bBreakCell;
			CSymbol::m_bBreakCell = bEnable;
			break;
		case BreakLinetype:
			bSave = CSymbol::m_bBreakLinetype;
			CSymbol::m_bBreakLinetype = bEnable;
			break;
		case BreakText:
			bSave = CAnnotation::m_bBreakText;
			CAnnotation::m_bBreakText = bEnable;
			break;
		case TextUpward:
			bSave = CAnnotation::m_bUpwardText;
			CAnnotation::m_bUpwardText = bEnable;
			break;
		}
	}
	CSymbolFlag(int type, double toler)
	{
		nType = Tolerance;
		lfSave = CSymbol::m_tolerance;
		CSymbol::m_tolerance = toler;
	}
	~CSymbolFlag()
	{
		switch( nType )
		{
		case BreakCell:
			CSymbol::m_bBreakCell = bSave;
			break;
		case BreakLinetype:
			CSymbol::m_bBreakLinetype = bSave;
			break;
		case BreakText:
			CAnnotation::m_bBreakText = bSave;
			break;
		case TextUpward:
			CAnnotation::m_bUpwardText = bSave;
			break;
		case Tolerance:
			CSymbol::m_tolerance = lfSave;
			break;
		}
	}

	int nType;
	BOOL bSave;
	double lfSave;
};

MyNameSpaceEnd

#endif // !defined(AFX_EBSYMBOL_H__E3EE941E_188B_407E_94D1_01408A73C8C8__INCLUDED_)
