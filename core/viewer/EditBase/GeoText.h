// GeoText.h: interface for the CGeoText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOTEXT_H__B0F92617_4428_418C_A85D_DEDFB6031EBC__INCLUDED_)
#define AFX_GEOTEXT_H__B0F92617_4428_418C_A85D_DEDFB6031EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "Geometry.h"
#include "Linearizer.h"
#include "Feature.h"

#define FIELDNAME_GEOTEXT_CONTENT		_T("Content")
#define FIELDNAME_GEOTEXT_FONT			_T("Font")
#define FIELDNAME_GEOTEXT_CHARWIDTHS	_T("CharWidthRatio")
#define FIELDNAME_GEOTEXT_CHARHEIGHT	_T("CharHeight")
#define FIELDNAME_GEOTEXT_CHARINTVS		_T("CharIntvRatio")
#define FIELDNAME_GEOTEXT_LINEINTVS		_T("LineIntvRatio")
#define FIELDNAME_GEOTEXT_ALIGNTYPE		_T("AlignType")
#define FIELDNAME_GEOTEXT_INCLINE		_T("Incline")
#define FIELDNAME_GEOTEXT_INCLINEANGLE	_T("InclineAngle")
#define FIELDNAME_GEOTEXT_PLACETYPE		_T("PlaceType")
#define FIELDNAME_GEOTEXT_OTHERFLAG		_T("OtherFlag")
#define FIELDNAME_GEOTEXT_COVERTYPE	    _T("CoverType")
#define FIELDNAME_GEOTEXT_EXTENDDIS     _T("ExtendDis")
#define FIELDNAME_GEOTEXT_TEXTANGLE		_T("TextAngle")
#define FIELDNAME_GEOTEXT_CHARANGLE		_T("CharAngle")


MyNameSpaceBegin


//约定：文字对象的字体名优先按照文字样式名来理解，如果找不到对应的文字样式，就按照字体名来理解；
//例如：当字体名为“正等线体”时，优先从文字样式列表中去找，如果找不到，则把它当作系统的字体名；
//当按照文字样式来理解时，字宽比例和倾斜角度就从样式参数去获取；
//当按照系统字体名来理解时，字宽比例和倾斜角度就使用文字对象本身的参数；
class EXPORT_EDITBASE CGeoText : public CGeometry  
{
	DECLARE_DYNCREATE(CGeoText)
public:
	CGeoText();
	virtual ~CGeoText();
	
	virtual Envelope GetEnvelope();
	virtual int GetClassType()const;
	virtual int GetDataPointSum()const;
	void CreateSearchReg(GrBuffer *pBuf);
	virtual BOOL SetDataPoint(int i,PT_3DEX pt);
	virtual PT_3DEX GetDataPoint(int i)const;
	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);
	virtual void GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const;
	
	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);

	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab,int idx = 0);
	
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);
	void SeparateText(GrBuffer *pBuf, float fDrawScale=1.0);
	void ExplodeAsGrBuffer(GrBuffer *pBuf, float fDrawScale=1.0);

	void SetText(LPCTSTR text);
	CString GetText()const;
	void SetSettings(TEXT_SETTINGS0 *settings);
	void GetSettings(TEXT_SETTINGS0 *settings)const;
	double GetTextAngle()const;

	virtual const CShapeLine *GetShape();
	
protected:
	CShapeLine m_shape;
	TEXT_SETTINGS0 m_settings;
	CString	 m_charList;

	OUID m_id;  
public:
	// 压盖类型: 0, 无压盖 1,矩形压盖 2,圆形压盖
	int     m_nCoverType;
	// 压盖外扩距离
	float   m_fExtendDis;
};



//文字样式
struct EXPORT_EDITBASE TextStyle
{
	TextStyle()
	{
		memset(name,0,sizeof(name));
		memset(font,0,sizeof(font));
		fWidScale = 1.0f;
		fInclinedAngle = 0;
		nInclineType = 0;
		bBold = FALSE;
	}
	BOOL IsValid();
	TCHAR name[128]; //样式名
	TCHAR font[128]; //字体名
	float fWidScale; //宽度比例
	int	  nInclineType;  //倾斜类型
	float fInclinedAngle;  //倾斜角度
	BOOL  bBold;		//粗体
};

typedef CArray<TextStyle,TextStyle> TextStyles;


class EXPORT_EDITBASE CUsedTextStyles
{
public:
	CUsedTextStyles();
	~CUsedTextStyles();
	
	void GetTextStyles(TextStyles& styles);
	TextStyle GetTextStyleByName(LPCTSTR name);
	void GetTextStyleNames(CStringArray& names);
	void Save(const TextStyles& fonts);
	void Load();
	
protected:
	void CreatePath();
	
	BOOL m_bLoad;
	CString m_strPath;
	TextStyles m_arrTextStyles;
};


EXPORT_EDITBASE CUsedTextStyles *GetUsedTextStyles();


MyNameSpaceEnd

#endif // !defined(AFX_GEOTEXT_H__B0F92617_4428_418C_A85D_DEDFB6031EBC__INCLUDED_)
