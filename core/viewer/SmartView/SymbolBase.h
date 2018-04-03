// SymbolBase.h: interface for the CSymbol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYMBOLBASE_H__E3EE941E_188B_407E_94D1_01408A73C8C8__INCLUDED_)
#define AFX_SYMBOLBASE_H__E3EE941E_188B_407E_94D1_01408A73C8C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrBuffer2d.h"
#include "math.h"

#define SYMTYPE_NONE				0
#define SYMTYPE_CELL				1
#define SYMTYPE_DASHLINETYPE		2
#define SYMTYPE_CELLLINETYPE		3
#define SYMTYPE_COLORHATCH			4
#define SYMTYPE_CELLHATCH   		5
#define SYMTYPE_ANNOTATION			6
#define SYMTYPE_SCALELINETYPE		7
#define SYMTYPE_LINEHATCH   		8
#define SYMTYPE_DIAGONAL   			9

#define SYMTYPE_PARATYPE   			11
#define SYMTYPE_ANGBISECTORTYPE		12
#define SYMTYPE_SCALEARCTYPE		13
#define SYMTYPE_SCALETURNPLATETYPE	14
#define SYMTYPE_SCALECRANETYPE		15
#define SYMTYPE_SCALEFUNNELTYPE		16

#define SYMTYPE_CAD					17
#define SYMTYPE_COLORHATCH_COND		18


// 共用标签
#define XMLTAG_DATA			_T("Data")
#define XMLTAG_COLOR        _T("Color")
#define XMLTAG_TYPE         _T("Type")
#define XMLTAG_SIZE			_T("Size")
#define XMLTAG_FIELD		_T("Field")
#define XMLTAG_POINT		_T("Pt")
#define XMLTAG_WIDTH		_T("Width")

// 单元配置
#define XMLTAG_CELL			_T("_Cell")
// 图元定义
// GrLineString2d
#define XMLTAG_CELLDEFNAME		_T("CellDefName")
#define XMLTAG_NAME				_T("Name")
#define XMLTAG_CELLDEFWIDTH		_T("Width")
#define XMLTAG_CELLDEFGRDWID	_T("GrdWid")
#define XMLTAG_CELLDEFSTYLE  	_T("Style")
#define XMLTAG_CELLDEFGR		_T("_GR")
#define XMLTAG_CELLDEFGRVS		_T("GRVS")
#define XMLTAG_GRBUFFER2D		_T("GrBuffer2d")
#define XMLTAG_GRAPH2D			_T("Graph2d")
// GrPoint2d

#define XMLTAG_POINTCELL		_T("Cell")
#define XMLTAG_ISFLAT			_T("ISFLAT")

// GrPolygon2d
#define XMLTAG_POLYGONFILLTYPE	_T("FillType")
#define XMLTAG_POLYGONINDEX		_T("Index")
// GrText2d
#define XMLTAG_TEXT				_T("Text")
#define XMLTAG_TEXTFONTNAME		_T("FontName")
#define XMLTAG_TEXTHEIGHT		_T("CharHeight")
#define XMLTAG_TEXTWIDTHSCALE	_T("CharWidScale")
#define XMLTAG_TEXTINTVSCALE	_T("CharIntvScale")
#define XMLTAG_TEXTLNSPCESCALE	_T("LineSpacingScale")
#define XMLTAG_TEXTTEXTANGLE	_T("TextAngle")
#define XMLTAG_TEXTCHARANGLE	_T("CharAngle")
#define XMLTAG_TEXTALIGN    	_T("Align")
#define XMLTAG_TEXTINCLINETYPE	_T("InclineType")
#define XMLTAG_TEXTINCLINEANGLE	_T("InclineAngle")



// 线形配置
#define XMLTAG_LINETYPE		_T("_Linetype")
#define XMLTAG_LINENUM		_T("LineNum")
#define XMLTAG_LINELENGTH	_T("Length")

#define XMLTAG_LINEITEM		_T("LINEITEM")
#define XMLTAG_CELLITEM		_T("CELLITEM")
#define XMLTAG_DRAWTYPE		_T("DrawType")

// 填充配置
#define XMLTAG_HATCH		_T("_Hatch")
#define XMLTAG_ITEM			_T("ITEM")


#define SYMTYPE_DLGCELL			10
#define SYMTYPE_DLGLINETYPE		11
#define SYMTYPE_DLGHATCH		12


// 方案设计
// action.xml
#define XML_ACTION	         _T("Action")
#define XML_PLACEPARAM	     _T("PlaceParam")
// scheme.xml
#define XML_SCHEME	         _T("Scheme")
#define XML_SORTEDGROUPNAME _T("SortedGroupName")
#define XML_LAYER_DEFINE	 _T("LayerDefine")
#define XMLTAG_GROUPNAME     _T("GroupName")
#define XMLTAG_CODE			 _T("Code")
#define XMLTAG_LAYERNAME	 _T("LayerName")
#define XMLTAG_DEFAULT_GEOCLASS _T("DefaultGeoClass")
#define XMLTAG_DB_GEOCLASS		 _T("DbGeoClass")
#define XMLTAG_SUPPORT_GEOCLASS _T("SupportGeoClass")
#define XMLTAG_XATTRIBUTES      _T("XAttributes")
#define XMLTAG_XATTRIBUTE       _T("XAttribute")
#define XMLTAG_FIXEDATTRIBUTE   _T("FixedAttribute")
#define XMLTAG_VALUE_TYPE       _T("ValueType")
#define XMLTAG_VALUE_LENGTH     _T("ValueLength")
#define XMLTAG_DEFAULT_VALUE    _T("DefaultValue")
#define XMLTAG_SYMBOLS          _T("Symbols")
#define XMLTAG_SYMBOL           _T("Symbol")
#define XML_RECENT				 _T("Recent")
#define XMLTAG_LAYER	         _T("Layer")
#define XMLTAG_DBLAYER			_T("DBLayer")
//
#define XMLTAG_PLACETYPE		_T("PlaceType")
#define XMLTAG_ANGLE			_T("ANGLE")
#define XMLTAG_KX			_T("KX")
#define XMLTAG_KY			_T("KY")
#define XMLTAG_BASEOFF		_T("BaseOff")
#define XMLTAG_BASEXOFF		_T("BaseXOff")
#define XMLTAG_BASEYOFF		_T("BaseYOff")
#define XMLTAG_ASSISTYOFF	_T("AssistYOff")
#define XMLTAG_CYCLE		_T("Cycle")
// CCell
#define XMLTAG_DX			_T("DX")
#define XMLTAG_DY			_T("DY")
#define XMLTAG_FILLTYPE		_T("FillType")
#define XMLTAG_CENLINEMODE  _T("CenlineMode")
#define XMLTAG_COVERTYPE    _T("CoverType")
#define XMLTAG_EXTENDDIS    _T("ExtendDis")
#define XMLTAG_DIRWITHFIRSTLINE			_T("DirWithFistLine")
// CDashLinetype
#define XMLTAG_BASELINENAME  _T("BaseLineName")
#define XMLTAG_USESELFCOLOR  _T("UseSelfcolor")
#define XMLTAG_INDENTTYPE    _T("IndentType")
#define XMLTAG_INDENT        _T("Indent")
#define XMLTAG_STARTOFF      _T("StartOff")
// CCellLinetype
#define XMLTAG_XOFFSET		_T("XOFFSET")
#define XMLTAG_YOFFSET		_T("YOFFSET")
#define XMLTAG_PLACEPOS		_T("PlacePos")

// CColorHatch
#define XMLTAG_TRANSPARENCE			_T("Transparence")
// CCellHatch
#define XMLTAG_DDX					_T("Ddx")
#define XMLTAG_DDY					_T("Ddy")
#define XMLTAG_ACCURATE_CUTCELL		_T("AccurateCutCell")		
// CAnnotation
#define XMLTAG_ANNOTYPE				_T("AnnoType")
#define XMLTAG_DIGIT				_T("Digit")
#define XMLTAG_TEXTFONT				_T("Font")
#define XMLTAG_TEXTPLACETYPE		_T("TextPlaceType")
// #define XMLTAG_TEXTCHARWID			_T("CharWid")
// #define XMLTAG_TEXTCHARHEI			_T("CharHei")
// #define XMLTAG_TEXTCHARINTV			_T("CharIntv")
// #define XMLTAG_TEXTLINEINTV			_T("LineIntv")
// #define XMLTAG_TEXTPLACETYPE		_T("PlaceType")
// #define XMLTAG_TEXTANIGNTYPE		_T("AlignType")
// #define XMLTAG_TEXTINCLINETYPE		_T("IncineType")
// #define XMLTAG_TEXTINCLINEANGLE		_T("IncineAngle")
// #define XMLTAG_TEXTFONTNAME			_T("FontName")
// CScalLinetype
#define XMLTAG_FIRXOFFTYPE			_T("FXOffType")
#define XMLTAG_FIRXOFFSET			_T("FXOffSet")
#define XMLTAG_ENDXOFFTYPE			_T("EXOffType")
#define XMLTAG_ENDXOFFSET			_T("EXOffSet")

#define XMLTAG_FIRYOFFTYPE			_T("FYOffType")
#define XMLTAG_FIRYOFFSET			_T("FYOffSet")
#define XMLTAG_ENDYOFFTYPE			_T("EYOffType")
#define XMLTAG_ENDYOFFSET			_T("EYOffSet")
#define XMLTAG_AVERAGEDRAW          _T("AverageDraw")

// CLineHatch
#define XMLTAG_INTV					_T("Interval")

#define XMLTAG_ANGLENTGH			_T("AngLength")
// CScaleArctype
#define XMLTAG_ARCLENTGH			_T("ArcLength")
// CScaleTurnplatetype
#define  XMLTAG_ROADWIDTH           _T("RoadWidth")
// CScaleFunneltype
#define  XMLTAG_FUNNELTYPE			_T("FunnelType")
// CDiagonal
#define  XMLTAG_DIAGONALTYPE		_T("DiagonalType")

#define	XMLTAG_DX2					_T("DX2")


MyNameSpaceBegin


// 图元定义，单元，线形和填充都会引用它
struct EXPORT_SMARTVIEW CellDef
{
	CellDef()
	{
		m_pgr = NULL;
		m_name[0] = 0;
		m_nSourceID = 0;
		m_bCircular = FALSE;
	}
	void Create() { if(m_pgr == NULL) m_pgr = new GrBuffer2d; }
	void Clear() { if(m_pgr) delete m_pgr; m_pgr=NULL; }
	void RefreshEnvelope(){
		if( m_pgr )
		{
			m_evlp = m_pgr->GetEnvelope();
			m_bCircular = IsCircular();
		}
	}
	void CopyFrom(const CellDef& a)
	{
		m_evlp = a.m_evlp;
		memcpy(m_name,a.m_name,sizeof(m_name));
		m_nSourceID = a.m_nSourceID;
		if( a.m_pgr )
		{
			if( !m_pgr )m_pgr = new GrBuffer2d;
			if( m_pgr )
				m_pgr->CopyFrom(a.m_pgr);
		}
		else
		{
			if(m_pgr)delete m_pgr; 
			m_pgr = NULL;
		}
	}
	//是否为圆形，圆形的图元就可以不用考虑旋转了，从而可以优化速度
	BOOL IsCircular();

	// 图形定义部分
	GrBuffer2d *m_pgr;
	Envelope m_evlp; //不需要存储，加载符号库之后，实时计算
	
	// 图元定义名称
	char m_name[_MAX_FNAME];
	DWORD_PTR  m_nSourceID;	//来源ID，为0表示来自配置文件，不为0则来源于某个FDB文件(CDlgDataSource对象的地址)

	BOOL m_bCircular;
};

typedef CArray<CellDef,CellDef> CArrayCellDef;

template<class T>
class CRecentCache
{
public:
	CRecentCache(int nMax=10){
		m_nMaxNum = nMax;
	}
	~CRecentCache(){}

	BOOL Find(LPCTSTR name, T &t)
	{
		for( int i=m_arrNames.GetSize()-1; i>=0; i--)
		{
			if( _tcsicmp(m_arrNames[i],name)==0 )
			{
				t = m_arrCache[i];
				
				m_arrCache.Add(t);
				m_arrCache.RemoveAt(i);

				m_arrNames.Add(CString(m_arrNames[i]));
				m_arrNames.RemoveAt(i);
				return TRUE;
			}
		}
		return FALSE;
	}

	void AddToRecent(LPCTSTR name, T &t)
	{
		m_arrNames.Add(name);
		m_arrCache.Add(t);
		if( m_arrCache.GetSize()>m_nMaxNum )
		{
			m_arrCache.RemoveAt(0);
			m_arrNames.RemoveAt(0);
		}
	}
	void Clear()
	{
		m_arrCache.RemoveAll();
		m_arrNames.RemoveAll();
	}

	CStringArray m_arrNames;
	CArray<T,T> m_arrCache;
	int m_nMaxNum;
};

//图元库
class EXPORT_SMARTVIEW CCellDefLib
{
public:
	CCellDefLib();
	~CCellDefLib();

	void Clear();

	BOOL	ReadFrom(CString& strXML);
	BOOL	WriteTo(CString& strXML);

 	BOOL	Load(FILE* stream);
 	BOOL	Save(FILE* stream);	
	
	int		AddCellDef(CellDef def);
	int		DelCellDef(LPCTSTR name);
	CellDef GetCellDef(LPCTSTR name);
	CellDef GetCellDef(int idx);
	BOOL	SetCellDef(int idx, CellDef def);
	BOOL	SetCellDef(LPCTSTR name, CellDef def);
	int		GetCellDefCount();
	int     GetCellDefIndex(LPCTSTR name);
	void    SetPath(CString strPath){ m_strCellPath = strPath; };
	CString GetPath() { return m_strCellPath; }
	void	DelAllFromSourceID(DWORD_PTR id);
		
private:
	CArray<CellDef,CellDef> m_arrCellDefs;
	CString m_strCellPath;

	CMap<DWORD_PTR, DWORD_PTR, CArrayCellDef*, CArrayCellDef*> m_mapSourceCellDefs;
	//最近调用的缓冲
	CRecentCache<int> m_CacheCell;
};


//基本线型（虚线线型）
struct EXPORT_SMARTVIEW BaseLineType
{
	BaseLineType():m_nNum(0){
		m_nSourceID = 0;
	}

	BOOL IsSolidLine()
	{
		if( m_nNum<1 )
			return FALSE;
		
		float alllen = 0, hidelen = 0;
		for (int i=0; i<m_nNum && i<8; i++)
		{
			alllen += fabs(m_fLens[i]);
			if( (i%2)==1 )
			{
				hidelen += fabs(m_fLens[i]);
			}
		}
		
		if ( hidelen<0.0001f && alllen>=0.0001f )
			return TRUE;
		
		return FALSE;
	}

	
	BOOL IsEmptyLine()
	{
		if( m_nNum<1 )
			return TRUE;
		
		float alllen = 0, hidelen = 0;
		for (int i=0; i<m_nNum && i<8; i++)
		{
			alllen += fabs(m_fLens[i]);
			if( (i%2)==1 )
			{
				hidelen += fabs(m_fLens[i]);
			}
		}
		
		if ( hidelen<0.0001f && alllen<0.0001f )
			return TRUE;
		
		return FALSE;
	}

	//定义名称
	char m_name[_MAX_FNAME];
	
	//虚线段数目（少于8）
	int m_nNum;
	
	//虚线段长度数据（正数为实线，负数为虚线）
	float m_fLens[8];
	DWORD_PTR  m_nSourceID;	//来源ID，为0表示来自配置文件，不为0则来源于某个FDB文件(CDlgDataSource对象的地址)
};


typedef CArray<BaseLineType,BaseLineType> CArrayBaseLineType;


//基本线型库
class EXPORT_SMARTVIEW CBaseLineTypeLib
{
public:
	CBaseLineTypeLib();
	~CBaseLineTypeLib();
	
	BOOL	ReadFrom(CString& strXML);
	BOOL	WriteTo(CString& strXML);

// 	BOOL	Load(CString& strXML);
// 	BOOL	Save(CString& strXML);

	BOOL Load(FILE *stream);
 	BOOL Save(FILE *stream);
	
	int		AddBaseLineType(BaseLineType def);
	int		DelBaseLineType(LPCTSTR name);
	void    Clear();
	BaseLineType GetBaseLineType(LPCTSTR name);
	BaseLineType GetBaseLineType(int idx);
	int		GetBaseLineTypeCount();
	int     GetBaseLineTypeIndex(LPCTSTR name);
	BOOL	SetBaseLineType(int idx, BaseLineType line);
	BOOL	SetBaseLineType(LPCTSTR name, BaseLineType line);
	void SetPath(CString strPath){ m_strLinePath = strPath; };
	CString GetPath() { return m_strLinePath; }

	void	DelAllFromSourceID(DWORD_PTR id);
	
private:

	CArray<BaseLineType,BaseLineType> m_arrItems;
	CString m_strLinePath;

	CMap<DWORD_PTR, DWORD_PTR, CArrayBaseLineType*, CArrayBaseLineType*> m_mapSourceBaseLineTypes;
	//最近调用的缓冲
	CRecentCache<int> m_CacheLType;
};



EXPORT_SMARTVIEW BOOL IsExist(LPCTSTR name, CStringArray &arrItems);
EXPORT_SMARTVIEW BOOL IsCellExist(LPCTSTR name, CCellDefLib *pCellDef=NULL);
EXPORT_SMARTVIEW BOOL IsLineExist(LPCTSTR name, CBaseLineTypeLib *pBaseLine=NULL);
EXPORT_SMARTVIEW CCellDefLib* GetCellDefLib();
EXPORT_SMARTVIEW CBaseLineTypeLib* GetBaseLineTypeLib();
EXPORT_SMARTVIEW void SetCellDefLib(CCellDefLib* pLib);
EXPORT_SMARTVIEW void SetBaseLineTypeLib(CBaseLineTypeLib* pLib);
EXPORT_SMARTVIEW void SetLibSourceID(DWORD_PTR id);
EXPORT_SMARTVIEW DWORD_PTR GetLibSourceID();
EXPORT_SMARTVIEW CString ExtractPath(LPCTSTR pathname);
EXPORT_SMARTVIEW CString ExtractFileName(LPCTSTR pathname);
EXPORT_SMARTVIEW CString ExtractFileNameExt(LPCTSTR pathname);
MyNameSpaceEnd



#endif // !defined(AFX_EBSYMBOL_H__E3EE941E_188B_407E_94D1_01408A73C8C8__INCLUDED_)
