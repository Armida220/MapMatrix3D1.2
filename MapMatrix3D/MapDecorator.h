// MapDecorator.h: interface for the CMapDecorator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPDECORATOR_H__CABB10F7_308E_48D7_8F01_AEF6A54CECEF__INCLUDED_)
#define AFX_MAPDECORATOR_H__CABB10F7_308E_48D7_8F01_AEF6A54CECEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewBaseType.h"
#include "Feature.h"
#include "GeoCurve.h"
#include "GeoText.h"
#include "tm.h"
#include "UIParam.h"
#include "FtrLayer.h"

#undef UTM_a
#undef UTM_b
#undef UTM_Override

using namespace EditBase;

#define TK_MAPTYPE_RECT5050		0
#define TK_MAPTYPE_RECT5040		1
#define TK_MAPTYPE_RECT4040		2
#define TK_MAPTYPE_STD			3
#define TK_MAPTYPE_ARBITRARY	10


#define TK_MAPBORDER_4PT		0
#define TK_MAPBORDER_1PT		1
#define TK_MAPBORDER_MAPNUM		2

class CDlgDataSource;
class CDlgDoc;


namespace MapDecorate
{
class CMapDecorator;
class CMapDecoratorAssist;
static BOOL bMapDecorateNew = FALSE;		//区分旧版还是新版

class CBaseObj
{
public:
	CBaseObj(){
		m_nIndex = -1;
		m_bUsed = TRUE;
		m_pMain = NULL;
		m_pAssist = NULL;
	}
	virtual ~CBaseObj(){}

	virtual LPCTSTR Name(){
		return m_strName;
	}
	virtual LPCTSTR StringID(){
		return "";
	}
	//参数的显示和修改（参数是指层、字体、间距等参数）
	virtual void GetUIParams(CUIParam *pParam){}
	virtual void SetParams(CValueTable& tab){}
	virtual void GetParams(CValueTable& tab){}

	//数据内容的显示和修改（数据是指图名、图号、接图表中的图名图号等）
	virtual void GetUIParams_data(CUIParam *pParam){}
	virtual void SetData(CValueTable& tab){}
	virtual void GetData(CValueTable& tab){}

	void SetBaseData(CValueTable& tab)
	{
		const CVariantEx *pvar;
		if (tab.GetValue(0, "bUsed", pvar))
		{
			m_bUsed = (bool)(_variant_t)*pvar;
		}
	}

	void GetBaseData(CValueTable& tab)
	{
		_variant_t var = (bool)m_bUsed;
		tab.AddValue("bUsed", &CVariantEx(var));
	}

	virtual void MakeFtrs(){}

	CString m_strName;
	int m_nIndex;
	BOOL m_bUsed;

	CMapDecorator *m_pMain;
	CMapDecoratorAssist *m_pAssist;
};

//图名的设置
class CName : public CBaseObj
{
public:	
	CName();
	virtual ~CName();
	virtual LPCTSTR StringID(){
		return "Name";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);

	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char name[64];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float char_interv; //字符间距比例
	float dy;	//底边到外图廓的距离（mm）

};

//图号的设置
class CNumber : public CBaseObj
{
public:	
	CNumber();
	virtual ~CNumber();
	virtual LPCTSTR StringID(){
		return "Number";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);

	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char number[64];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float char_interv; //字符间距比例
	float dy;	//底边到外图廓的距离（mm）
	float int_num;	//图号的整数位数（矩形图幅时会用到）
	float float_num;	//图号的小数位数（矩形图幅时会用到）
};

//区域注记的设置
class CAreaDesc : public CBaseObj
{
public:	
	CAreaDesc();
	virtual ~CAreaDesc();
	virtual LPCTSTR StringID(){
		return "AreaDesc";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char area_desc[256];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float dy;	//底边到外图廓的距离（mm）
};

//接图表的设置
class CNearMap : public CBaseObj
{
public:	
	CNearMap();
	virtual ~CNearMap();
	virtual LPCTSTR StringID(){
		return "NearMap";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	struct maplist{
		char name1[64];
		char name2[64];
		char name3[64];
		char name4[64];
		char name5[64];
		char name6[64];
		char name7[64];
		char name8[64];
	}nearmap;

	char layer[64];
	char font[64];
	float width;	//总宽度
	float height;	//总高度
	float size;		//字体尺寸
	float wscale;	//字宽比例
	float dy;	//底边到外图廓的距离（mm）
};

//密级的设置
class CSecret : public CBaseObj
{
public:	
	CSecret();
	virtual ~CSecret();
	virtual LPCTSTR StringID(){
		return "Secret";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char secret[64];
	int  nSecret;

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float dy;	//底边到外图廓的距离（mm）
};

//外图廓的设置
class COuterBound : public CBaseObj
{
public:	
	COuterBound();
	virtual ~COuterBound();
	virtual LPCTSTR StringID(){
		return "OuterBound";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];
	float width;			//外图廓线宽度
	float extension;		//相对于内图廓的外扩距离（mm）
};

//内图廓的设置
class CInnerBound : public CBaseObj
{
public:	
	CInnerBound();
	virtual ~CInnerBound();
	virtual LPCTSTR StringID(){
		return "InnerBound";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];
	float width;			//内图廓线宽度
};

//格网的设置
class CGrid : public CBaseObj
{
public:	
	CGrid();
	virtual ~CGrid();
	virtual LPCTSTR StringID(){
		return "Grid";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];		
	int type;				//0,十字丝,1,格网线
	float cross_len;		//十字丝长度（仅当类型为十字丝时）
	int bFrameLine;			//是否带有图廓内侧短线
	float grid_wid;			//格网宽度（毫米）
	char kmBig_font[64];	//带号的字体名
	float kmBig_size;		//带号的字体尺寸
	float kmBig_wscale;		//带号的字宽比例
	float kmBig_dx;			//带号的边缘到外图廓侧边线的距离（mm）
	float kmBig_dy;			//带号的边缘到外图廓上下边线的距离（mm）

	char kmLit_font[64];	//公里数的字体名
	float kmLit_size;		//公里数的字体尺寸
	float kmLit_wscale;		//公里数的字宽比例
	float kmLit_dx;			//公里数的边缘到外图廓侧边线的距离（mm）
	float kmLit_dy;			//公里数的边缘到外图廓上下边线的距离（mm）

	int bGeoNumber;			//是否存在经纬度注记
	char geo_font[64];		//经纬度的字体名
	float geo_size;			//经纬度的字体尺寸
	float geo_wscale;		//经纬度的字宽比例
	float geo_dx;			//经纬度的边缘到内图廓横线的距离（mm）
	float geo_dy;			//经纬度的边缘到内图廓竖线的距离（mm）
};

//格网（25000/50000）
class CGrid_New : public CGrid
{
public:	
	CGrid_New();
	virtual ~CGrid_New();
	virtual LPCTSTR StringID(){
		return "Grid_New";
	}

	virtual void MakeFtrs();
};

//左上角图名图号的设置
class CLTname : public CBaseObj
{
public:	
	CLTname();
	virtual ~CLTname();
	virtual LPCTSTR StringID(){
		return "LTname";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char name_layer[64];	//图名的层名
	char number_layer[64];	//图号的层名
	char name_font[64];
	char number_font[64];
	float name_size;		//图名文字尺寸
	float name_wscale;		//字宽比例
//	float name_dx;			//右边到外图廓边线的距离（mm）
	float name_dy;			//下边到外图廓边线的距离（mm）
	float number_size;		//图号文字尺寸
	float number_wscale;	//字宽比例
	float number_dx;		//右边到外图廓边线的距离（mm）
	float number_dy;		//下边到外图廓边线的距离（mm）
};

//左下角图名图号的设置
class CLBname : public CBaseObj
{
public:	
	CLBname();
	virtual ~CLBname();
	virtual LPCTSTR StringID(){
		return "LBname";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char name_layer[64];	//图名的层名
	char number_layer[64];	//图号的层名
	char name_font[64];
	char number_font[64];
	float name_size;		//图名文字尺寸
	float name_wscale;		//字宽比例
//	float name_dx;			//右边到外图廓边线的距离（mm）
	float name_dy;			//上边到外图廓边线的距离（mm）
	float number_size;		//图号文字尺寸
	float number_wscale;	//字宽比例
	float number_dx;		//右边到外图廓边线的距离（mm）
	float number_dy;		//上边到外图廓边线的距离（mm）
};

//右上角图名图号的设置
class CRTname : public CBaseObj
{
public:	
	CRTname();
	virtual ~CRTname();
	virtual LPCTSTR StringID(){
		return "RTname";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char name_layer[64];	//图名的层名
	char number_layer[64];	//图号的层名
	char name_font[64];
	char number_font[64];
	float name_size;		//图名文字尺寸
	float name_wscale;		//字宽比例
//	float name_dx;			//左边到外图廓边线的距离（mm）
	float name_dy;			//下边到外图廓边线的距离（mm）
	float number_size;		//图号文字尺寸
	float number_wscale;	//字宽比例
	float number_dx;			//左边到外图廓边线的距离（mm）
	float number_dy;			//下边到外图廓边线的距离（mm）
};

//右下角图名图号的设置
class CRBname : public CBaseObj
{
public:	
	CRBname();
	virtual ~CRBname();
	virtual LPCTSTR StringID(){
		return "RBname";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char name_layer[64];	//图名的层名
	char number_layer[64];	//图号的层名
	char name_font[64];
	char number_font[64];
	float name_size;		//图名文字尺寸
	float name_wscale;		//字宽比例
//	float name_dx;			//左边到外图廓边线的距离（mm）
	float name_dy;			//下边到外图廓边线的距离（mm）
	float number_size;		//图号文字尺寸
	float number_wscale;	//字宽比例
	float number_dx;			//左边到外图廓边线的距离（mm）
	float number_dy;			//上边到外图廓边线的距离（mm）
};

//图例
class CFigure : public CBaseObj
{
public:	
	CFigure();
	virtual ~CFigure();
	virtual LPCTSTR StringID(){
		return "Figure";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];
	float dx;		//左边到外图廓的距离（mm）
	float dy;		//顶边到外图廓的距离（mm）
};

//比例尺
class CScale : public CBaseObj
{
public:	
	CScale();
	virtual ~CScale();
	virtual LPCTSTR StringID(){
		return "Scale";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char scaleText[64];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float dy;		//顶边到外图廓的距离（mm）

	float dy_scaleRuler;
	char scaleLine_font[64]; //刻度线上文字的字体
	float scaleLine_size;		//刻度线上文字的尺寸
};

//坡度尺（5000-10000）
class CRuler : public CBaseObj
{
public:	
	CRuler();
	virtual ~CRuler();
	virtual LPCTSTR StringID(){
		return "Ruler";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];
	float dy;		//底边到外图廓的距离（mm）
	float dx;		//左边到内图廓的距离（mm）
	float dy2;		//顶边到外图廓的距离（mm）

	float x_interv;	//横轴间隔（内部使用）
	//标题
	char title_font[64];
	float title_size;
	float title_wscale;

	//注释
	char note_font[64];
	float note_size;
	float note_wscale;

	//刻度
	char cal_font[64];
	float cal_size;
	float cal_wscale;

	float con_interv;
};

//三北方向
class CNorthPointer : public CBaseObj
{
public:	
	CNorthPointer();
	virtual ~CNorthPointer();
	virtual LPCTSTR StringID(){
		return "NorthPointer";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];
	float dx;		//左边到外图廓的距离（mm）
	float dy;		//顶边到外图廓的距离（mm）
};

//磁北线
class CMagline : public CBaseObj
{
public:	
	CMagline();
	virtual ~CMagline();
	virtual LPCTSTR StringID(){
		return "Magline";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	double magline;
	float angle;		//磁偏角

	char layer[64];
};

//测绘单位
class CMakerLeft : public CBaseObj
{
public:	
	CMakerLeft();
	virtual ~CMakerLeft();
	virtual LPCTSTR StringID(){
		return "MakerLeft";
	}	

	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char makerLeft[256];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float char_interv; //字符间距比例
	float line_interv; //行间距比例	
	float dx;	//右边到外图廓的距离（mm）
};

//主管单位
class CMakerRight : public CBaseObj
{
public:	
	CMakerRight();
	virtual ~CMakerRight();
	virtual LPCTSTR StringID(){
		return "MakerRight";
	}	
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char makerRight[64];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float char_interv; //字符间距比例
	float line_interv; //行间距比例
	float dx;		//左边到外图廓右边的距离（mm）
};

//资料说明
class CDataDesc : public CBaseObj
{
public:	
	CDataDesc();
	virtual ~CDataDesc();
	virtual LPCTSTR StringID(){
		return "DataDesc";
	}	
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char data_desc[256];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float char_interv; //字符间距比例
	float line_interv; //行间距比例	
	float dy;	//上边到外图廓的距离（mm）
};

//附注
class CNote : public CBaseObj
{
public:	
	CNote();
	virtual ~CNote();
	virtual LPCTSTR StringID(){
		return "Note";
	}	
			
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char note[256];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float char_interv; //字符间距比例
	float line_interv; //行间距比例
	float dx;		//左边到内图廓右边的距离（mm）
	float dy;		//顶边到外图廓的距离（mm）
};

//检查员
class CCheckman : public CBaseObj
{
public:	
	CCheckman();
	virtual ~CCheckman();
	virtual LPCTSTR StringID(){
		return "Checkman";
	}	
			
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();	

	char surveyman[64];
	char drawman[64];
	char checkman[64];	

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float char_interv; //字符间距比例
	float line_interv; //行间距比例
	float dx;		//左边到内图廓右边的距离（mm）
	float dy;		//顶边到外图廓的距离（mm）
};


//可以在一个 CDlgDoc 中创建图廓元素，也可以在 CDlgDataSource 中创建
class CMapDecoratorAssist
{
public:
	CMapDecoratorAssist();
	virtual ~CMapDecoratorAssist();

	void SetDoc(CDlgDoc *pDoc, int scale);
	void SetDlgDataSource(CDlgDataSource *pDS, int scale);

	CFtrLayer *GetLayer(LPCTSTR fid);
	CFeature *CreateCurve(LPCTSTR fid, CFtrLayer *&pLayer);
	CFeature *CreatePoint(LPCTSTR fid, CFtrLayer *&pLayer);
	CFeature *CreateText(LPCTSTR fid, CFtrLayer *&pLayer);
	void CreateCross(CGeoCurve *pGeo, PT_3D pt, int dir, float len);
	void SetText(CGeoText *pObj, PT_3D* pt3ds, int npt, LPCTSTR text, LPCTSTR font, float size, float wscale, int nAlign);
	void SetText(CGeoText *pObj, double lfTextAngle);
	void CreateHachure(LPCTSTR layname, PT_3D *pts, int npt, double ang, double interval, CFtrLayer*& pLayer, CFtrArray& arrFtrs); //创建晕线
	
	CGeoText* AddText(LPCTSTR fid, PT_3D pt, LPCTSTR text, LPCTSTR font, float size, float wscale, int nAlign, double ang=0);
	CGeoCurve* AddLine(LPCTSTR fid, PT_3D pt0, PT_3D pt1, float wid);
	
	BOOL SplitMultipleLineText(CFeature *pObj, double interval, CFtrArray& arrFtrs);

	double Unit();

	Envelope GetTextEnvelope(CGeoText *pText);

	BOOL AddObject(CFeature *pFt, int layid=-1);

	CDlgDataSource *GetDlgDataSource();

	BOOL ImportXMLtoDS(LPCTSTR path, CFtrLayer *pLayer, double angle, double x0, double y0, double z0);

	CArray<FTR_HANDLE,FTR_HANDLE>* m_pArrNewObjHandles;

protected:

	CDlgDoc *m_pDoc;
	CDlgDataSource *m_pDS; //m_pDoc,m_pDS 只用其一
	long m_scale;
};

//可以在一个 CDlgDoc 中创建图廓元素，也可以在 CDlgDataSource 中创建
class CMapDecorator  
{
public:
	//旋转图廓后，得到的几个参数
	struct XBound
	{
		//角度及其正余弦值
		double cosa, sina, ang;

		//基于上述角度旋转图廓后得到的新图廓
		PT_3D innerBound[4], outerBound[4];
	};

	CMapDecorator();
	virtual ~CMapDecorator();

	static CString GetFigurePath(long scale);

	void InitData(CDlgDataSource *pDS=NULL);
	void Clear();
	
	//根据mapType 计算mapWid,mapHei
	void CalcMapSize();
	BOOL CalcMapBound();
	
	CString CalcRectMapNumber(PT_3D pt, long scale);
	void CalcRectNearmap(PT_3D pt, long scale);
	
	void CalcSmallScaleNearmap();	
	BOOL CalcSmallScaleMapNumber(CDlgDataSource *pDS, PT_3D pt);	
	void CalcMapNumberAndNearmap(CDlgDataSource *pDS, PT_3D pt);

	void LoadParams(long scale);
	void LoadParams(LPCTSTR path);
	void SaveParams(LPCTSTR path);
	//在内存中保存图廓整饰的参数
	void SaveParamsMem();
	//恢复参数
	void RestoreParamsMem();

	//load和save数据
	void LoadDatas(LPCTSTR path);
	void SaveDatas(LPCTSTR path);

	CBaseObj *GetObj(LPCTSTR id);
	
	BOOL Build(CDlgDoc *pDoc, CDlgDataSource *pDS, int scale, BOOL bSilence=FALSE, BOOL bSortBound=TRUE);

protected:
	void CreateObj(LPCTSTR id);
	void CreateObjs(long scale);

public:

	PT_3D GaussToGeo(PT_3D pt);
	PT_3D GeoToGauss(PT_3D pt);

	BOOL ImportXML(LPCTSTR path, CFtrLayer *pLayer, PT_3D move_pt, PT_3D rotate_pt, double angle);

	void SortBoundPoints(PT_3D pts[4]);

	void RotateBound(PT_3D pt0, PT_3D pt1, XBound& xb);

	CMapDecoratorAssist m_assist;

	CDlgDoc *m_pDoc;
	CDlgDataSource *m_pDS; //m_pDoc,m_pDS 只用其一
	long m_scale;

	PT_3D m_innerBound[4];
	PT_3D m_outerBound[4];

	//分幅类型
	int m_nMapType; //0 矩形图幅 50cmX50cm，1 矩形图幅 50cmX40cm，2 小比例尺标准图幅，3 任意图幅
	//图幅尺寸
	float m_fMapWid, m_fMapHei;  //单位为cm
	int m_nMapBorderType; //0 选择内图廓线, 1 选择内图廓左下角点, 2 用图号计算
	//内图廓范围
	PT_3D m_SelectedBoundPts[4]; //当mapBorderType==0时有效
	PT_3D m_SelectBoundCorner; //当mapBorderType==1时有效
	//投影信息
	CTM m_tm;
	//是否去掉带号
	BOOL m_bCutStripNum;

	BOOL m_bSilence;
	BOOL m_bSortBound;

	CString m_strSavePath;

public:
	CArray<CBaseObj*,CBaseObj*> m_arrPObjs;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrNewObjHandles;

private:
	CArray<CValueTable*,CValueTable*> m_arrParamTabs;
public:
	BOOL m_bNew;		//新增一个变量用于识别新版或旧版
};


//新版5000/10000比例尺的资料说明（25000-50000也适用）
class CDataDesc_5KN : public CBaseObj
{
public:	
	CDataDesc_5KN();
	virtual ~CDataDesc_5KN();
	virtual LPCTSTR StringID(){
		return "DataDesc_5KN";
	}	
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();	

	char layer[64];

	float dx;			//字的左侧距外图廓的距离

	//第一部分参数
	char data_desc1[256];
	char font1[64];
	float size1;		//字体大小
	float wscale1;		//字宽比例
	float char_interv1; //字符间距比例
	float line_interv1; //行间距比例	

	//第二部分参数
	char data_desc2[256];
	char font2[64];
	float size2;		//字体大小
	float wscale2;		//字宽比例
	float char_interv2; //字符间距比例
	float line_interv2; //行间距比例
};

//政区略图与邻接图表
class CAdminRegion_NearMap : public CNearMap
{
public:	
	CAdminRegion_NearMap();
	virtual ~CAdminRegion_NearMap();
	virtual LPCTSTR StringID(){
		return "AdminRegion_NearMap";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	float dx;				//左边到外图廓的距离（mm）
	CString src_layers;		//政区略图中地物的原始图层（可为多个）

	//标题的参数
	char title_font[64];
	float title_size;		//字体尺寸
	float title_wscale;		//字宽比例

protected:
	void DrawAdminRegion(double matrix1[16], double matrixA[16], double matrixB[16]);
	bool IsFeatureInBound(CFeature* pFtr);
};

//左下角版权机关
class CMakerBottom : public CBaseObj
{
public:	
	CMakerBottom();
	virtual ~CMakerBottom();
	virtual LPCTSTR StringID(){
		return "MakerBottom";
	}

	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char makerBottom[256];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float dy;		//顶部到外图廓的距离（mm）
};

//数字比例尺
class CDigitalScale : public CBaseObj
{
public:	
	CDigitalScale();
	virtual ~CDigitalScale();
	virtual LPCTSTR StringID(){
		return "DigitalScale";
	}	

	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char layer[64];

	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float dy;		//底部到外图廓的距离（mm）

	//标题（中华人民共和国基本比例尺地形图）
	char title_font[64];
	float title_size;
	float title_wscale;
};
//


//三北方向（供新版25000-50000比例尺使用）
class CNorthPointer_25KN : public CBaseObj
{
public:	
	CNorthPointer_25KN();
	virtual ~CNorthPointer_25KN();
	virtual LPCTSTR StringID(){
		return "NorthPointer_25KN";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);

	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char layer[64];
	float dx;		//真子午线到内图廓的距离（mm）
	float dy;		//顶边到外图廓的距离（mm）

	//线名
	char font1[64];
	float size1;
	float wscale1;	
	
	//角度注记
	char font2[64];
	float size2;
	float wscale2;
	
	float angle1;			//磁子午线偏角
	float angle2;			//纵坐标线偏角
protected:
	void AddArcAndNote( PT_3DEX pts[3], float included_angle, float text_angle);
	float AddAngle( float angle1, float angle2 );
};


//磁北线（供新版25000-50000比例尺使用）
class CMagline_25KN : public CBaseObj
{
public:	
	CMagline_25KN();
	virtual ~CMagline_25KN();
	virtual LPCTSTR StringID(){
		return "Magline_25KN";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	float angle;		//磁偏角

	char layer[64];

	char font[64];
	float size;
	float wscale;
};


//比例尺（供新版5000-10000比例尺使用）
class CScale_5KN : public CBaseObj
{
public:	
	CScale_5KN();
	virtual ~CScale_5KN();
	virtual LPCTSTR StringID(){
		return "Scale_5KN";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char scaleText[64];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float dy;		//顶边到外图廓的距离（mm）
	float dx;		//右边到内图廓的距离（mm）

	float dy_scaleRuler;
	char scaleLine_font[64]; //刻度线上文字的字体
	float scaleLine_size;		//刻度线上文字的尺寸

	float dy_text;			//底边文字到比例尺的距离
	char text_font[64];
	float text_size;
	float text_wscale;
};



//比例尺（供新版25000-50000比例尺使用）
class CScale_25KN : public CBaseObj
{
public:	
	CScale_25KN();
	virtual ~CScale_25KN();
	virtual LPCTSTR StringID(){
		return "Scale_25KN";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char scaleText[64];

	char layer[64];
	char font[64];
	float size;		//文字尺寸
	float wscale;	//字宽比例
	float dy;		//顶边到外图廓的距离（mm）
	float dx;		//左边到坡度尺的距离（mm）

	float dy_scaleRuler;
	char scaleLine_font[64]; //刻度线上文字的字体
	float scaleLine_size;		//刻度线上文字的尺寸

	float dy_text;			//底边文字到比例尺的距离
	char text_font[64];
	float text_size;
	float text_wscale;
};



//坡度尺（供新版25000-50000比例尺使用）
class CRuler_25KN : public CBaseObj
{
public:	
	CRuler_25KN();
	virtual ~CRuler_25KN();
	virtual LPCTSTR StringID(){
		return "Ruler_25KN";
	}
		
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];
	float dy;		//底边到外图廓的距离（mm）
	float dx;		//左边到外图廓的距离（mm）
	float dy2;		//顶边（标题）到外图廓的距离（mm）


	float x_interv;	//坡度尺横轴间距（mm），内部使用
	//标题
	char title_font[64];
	float title_size;
	float title_wscale;

	//注释
	char note_font[64];
	float note_size;
	float note_wscale;

	//刻度
	char cal_font[64];
	float cal_size;
	float cal_wscale;

	float con_interv;		//等高距为5(m)
};
//


//外图廓装饰线
class COuterBoundDecorator : public CBaseObj
{
public:	
	COuterBoundDecorator();
	virtual ~COuterBoundDecorator();
	virtual LPCTSTR StringID(){
		return "OuterBoundDecorator";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	virtual void MakeFtrs();

	char layer[64];
	float width;			//装饰线宽度
	float extension;		//相对于外图廓的内扩距离（mm）
	float interv;			//针线的间隔（mm）
};


//政区略图
class CAdminRegion : public CBaseObj
{
public:
	CAdminRegion();
	virtual ~CAdminRegion();
	virtual LPCTSTR StringID(){
		return "AdminRegion";
	}
	
	virtual void GetUIParams(CUIParam *pParam);
	virtual void SetParams(CValueTable& tab);
	virtual void GetParams(CValueTable& tab);
	
	virtual void GetUIParams_data(CUIParam *pParam);
	virtual void SetData(CValueTable& tab);
	virtual void GetData(CValueTable& tab);

	virtual void MakeFtrs();

	char layer[64];

	float width;	//总宽度
	float height;	//总高度

	float dx;				//左边到外图廓的距离（mm）
	float dy;				//底边到内图廓的距离（mm）
	CString src_layers;

	//标题的参数
	char font[64];
	float size;		//字体尺寸
	float wscale;		//字宽比例

protected:
	void DrawAdminRegion(double matrix1[16], double matrixA[16], double matrixB[16]);
	bool IsFeatureInBound(CFeature* pFtr);		//判断地物是否在内图廓内
};
//
}

#endif // !defined(AFX_MAPDECORATOR_H__CABB10F7_308E_48D7_8F01_AEF6A54CECEF__INCLUDED_)
