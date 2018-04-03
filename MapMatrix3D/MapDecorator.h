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
static BOOL bMapDecorateNew = FALSE;		//���־ɰ滹���°�

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
	//��������ʾ���޸ģ�������ָ�㡢���塢���Ȳ�����
	virtual void GetUIParams(CUIParam *pParam){}
	virtual void SetParams(CValueTable& tab){}
	virtual void GetParams(CValueTable& tab){}

	//�������ݵ���ʾ���޸ģ�������ָͼ����ͼ�š���ͼ���е�ͼ��ͼ�ŵȣ�
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

//ͼ��������
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float char_interv; //�ַ�������
	float dy;	//�ױߵ���ͼ���ľ��루mm��

};

//ͼ�ŵ�����
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float char_interv; //�ַ�������
	float dy;	//�ױߵ���ͼ���ľ��루mm��
	float int_num;	//ͼ�ŵ�����λ��������ͼ��ʱ���õ���
	float float_num;	//ͼ�ŵ�С��λ��������ͼ��ʱ���õ���
};

//����ע�ǵ�����
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float dy;	//�ױߵ���ͼ���ľ��루mm��
};

//��ͼ�������
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
	float width;	//�ܿ��
	float height;	//�ܸ߶�
	float size;		//����ߴ�
	float wscale;	//�ֿ����
	float dy;	//�ױߵ���ͼ���ľ��루mm��
};

//�ܼ�������
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float dy;	//�ױߵ���ͼ���ľ��루mm��
};

//��ͼ��������
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
	float width;			//��ͼ���߿��
	float extension;		//�������ͼ�����������루mm��
};

//��ͼ��������
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
	float width;			//��ͼ���߿��
};

//����������
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
	int type;				//0,ʮ��˿,1,������
	float cross_len;		//ʮ��˿���ȣ���������Ϊʮ��˿ʱ��
	int bFrameLine;			//�Ƿ����ͼ���ڲ����
	float grid_wid;			//������ȣ����ף�
	char kmBig_font[64];	//���ŵ�������
	float kmBig_size;		//���ŵ�����ߴ�
	float kmBig_wscale;		//���ŵ��ֿ����
	float kmBig_dx;			//���ŵı�Ե����ͼ������ߵľ��루mm��
	float kmBig_dy;			//���ŵı�Ե����ͼ�����±��ߵľ��루mm��

	char kmLit_font[64];	//��������������
	float kmLit_size;		//������������ߴ�
	float kmLit_wscale;		//���������ֿ����
	float kmLit_dx;			//�������ı�Ե����ͼ������ߵľ��루mm��
	float kmLit_dy;			//�������ı�Ե����ͼ�����±��ߵľ��루mm��

	int bGeoNumber;			//�Ƿ���ھ�γ��ע��
	char geo_font[64];		//��γ�ȵ�������
	float geo_size;			//��γ�ȵ�����ߴ�
	float geo_wscale;		//��γ�ȵ��ֿ����
	float geo_dx;			//��γ�ȵı�Ե����ͼ�����ߵľ��루mm��
	float geo_dy;			//��γ�ȵı�Ե����ͼ�����ߵľ��루mm��
};

//������25000/50000��
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

//���Ͻ�ͼ��ͼ�ŵ�����
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

	char name_layer[64];	//ͼ���Ĳ���
	char number_layer[64];	//ͼ�ŵĲ���
	char name_font[64];
	char number_font[64];
	float name_size;		//ͼ�����ֳߴ�
	float name_wscale;		//�ֿ����
//	float name_dx;			//�ұߵ���ͼ�����ߵľ��루mm��
	float name_dy;			//�±ߵ���ͼ�����ߵľ��루mm��
	float number_size;		//ͼ�����ֳߴ�
	float number_wscale;	//�ֿ����
	float number_dx;		//�ұߵ���ͼ�����ߵľ��루mm��
	float number_dy;		//�±ߵ���ͼ�����ߵľ��루mm��
};

//���½�ͼ��ͼ�ŵ�����
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

	char name_layer[64];	//ͼ���Ĳ���
	char number_layer[64];	//ͼ�ŵĲ���
	char name_font[64];
	char number_font[64];
	float name_size;		//ͼ�����ֳߴ�
	float name_wscale;		//�ֿ����
//	float name_dx;			//�ұߵ���ͼ�����ߵľ��루mm��
	float name_dy;			//�ϱߵ���ͼ�����ߵľ��루mm��
	float number_size;		//ͼ�����ֳߴ�
	float number_wscale;	//�ֿ����
	float number_dx;		//�ұߵ���ͼ�����ߵľ��루mm��
	float number_dy;		//�ϱߵ���ͼ�����ߵľ��루mm��
};

//���Ͻ�ͼ��ͼ�ŵ�����
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

	char name_layer[64];	//ͼ���Ĳ���
	char number_layer[64];	//ͼ�ŵĲ���
	char name_font[64];
	char number_font[64];
	float name_size;		//ͼ�����ֳߴ�
	float name_wscale;		//�ֿ����
//	float name_dx;			//��ߵ���ͼ�����ߵľ��루mm��
	float name_dy;			//�±ߵ���ͼ�����ߵľ��루mm��
	float number_size;		//ͼ�����ֳߴ�
	float number_wscale;	//�ֿ����
	float number_dx;			//��ߵ���ͼ�����ߵľ��루mm��
	float number_dy;			//�±ߵ���ͼ�����ߵľ��루mm��
};

//���½�ͼ��ͼ�ŵ�����
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

	char name_layer[64];	//ͼ���Ĳ���
	char number_layer[64];	//ͼ�ŵĲ���
	char name_font[64];
	char number_font[64];
	float name_size;		//ͼ�����ֳߴ�
	float name_wscale;		//�ֿ����
//	float name_dx;			//��ߵ���ͼ�����ߵľ��루mm��
	float name_dy;			//�±ߵ���ͼ�����ߵľ��루mm��
	float number_size;		//ͼ�����ֳߴ�
	float number_wscale;	//�ֿ����
	float number_dx;			//��ߵ���ͼ�����ߵľ��루mm��
	float number_dy;			//�ϱߵ���ͼ�����ߵľ��루mm��
};

//ͼ��
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
	float dx;		//��ߵ���ͼ���ľ��루mm��
	float dy;		//���ߵ���ͼ���ľ��루mm��
};

//������
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float dy;		//���ߵ���ͼ���ľ��루mm��

	float dy_scaleRuler;
	char scaleLine_font[64]; //�̶��������ֵ�����
	float scaleLine_size;		//�̶��������ֵĳߴ�
};

//�¶ȳߣ�5000-10000��
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
	float dy;		//�ױߵ���ͼ���ľ��루mm��
	float dx;		//��ߵ���ͼ���ľ��루mm��
	float dy2;		//���ߵ���ͼ���ľ��루mm��

	float x_interv;	//���������ڲ�ʹ�ã�
	//����
	char title_font[64];
	float title_size;
	float title_wscale;

	//ע��
	char note_font[64];
	float note_size;
	float note_wscale;

	//�̶�
	char cal_font[64];
	float cal_size;
	float cal_wscale;

	float con_interv;
};

//��������
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
	float dx;		//��ߵ���ͼ���ľ��루mm��
	float dy;		//���ߵ���ͼ���ľ��루mm��
};

//�ű���
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
	float angle;		//��ƫ��

	char layer[64];
};

//��浥λ
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float char_interv; //�ַ�������
	float line_interv; //�м�����	
	float dx;	//�ұߵ���ͼ���ľ��루mm��
};

//���ܵ�λ
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float char_interv; //�ַ�������
	float line_interv; //�м�����
	float dx;		//��ߵ���ͼ���ұߵľ��루mm��
};

//����˵��
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float char_interv; //�ַ�������
	float line_interv; //�м�����	
	float dy;	//�ϱߵ���ͼ���ľ��루mm��
};

//��ע
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float char_interv; //�ַ�������
	float line_interv; //�м�����
	float dx;		//��ߵ���ͼ���ұߵľ��루mm��
	float dy;		//���ߵ���ͼ���ľ��루mm��
};

//���Ա
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float char_interv; //�ַ�������
	float line_interv; //�м�����
	float dx;		//��ߵ���ͼ���ұߵľ��루mm��
	float dy;		//���ߵ���ͼ���ľ��루mm��
};


//������һ�� CDlgDoc �д���ͼ��Ԫ�أ�Ҳ������ CDlgDataSource �д���
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
	void CreateHachure(LPCTSTR layname, PT_3D *pts, int npt, double ang, double interval, CFtrLayer*& pLayer, CFtrArray& arrFtrs); //��������
	
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
	CDlgDataSource *m_pDS; //m_pDoc,m_pDS ֻ����һ
	long m_scale;
};

//������һ�� CDlgDoc �д���ͼ��Ԫ�أ�Ҳ������ CDlgDataSource �д���
class CMapDecorator  
{
public:
	//��תͼ���󣬵õ��ļ�������
	struct XBound
	{
		//�Ƕȼ���������ֵ
		double cosa, sina, ang;

		//���������Ƕ���תͼ����õ�����ͼ��
		PT_3D innerBound[4], outerBound[4];
	};

	CMapDecorator();
	virtual ~CMapDecorator();

	static CString GetFigurePath(long scale);

	void InitData(CDlgDataSource *pDS=NULL);
	void Clear();
	
	//����mapType ����mapWid,mapHei
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
	//���ڴ��б���ͼ�����εĲ���
	void SaveParamsMem();
	//�ָ�����
	void RestoreParamsMem();

	//load��save����
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
	CDlgDataSource *m_pDS; //m_pDoc,m_pDS ֻ����һ
	long m_scale;

	PT_3D m_innerBound[4];
	PT_3D m_outerBound[4];

	//�ַ�����
	int m_nMapType; //0 ����ͼ�� 50cmX50cm��1 ����ͼ�� 50cmX40cm��2 С�����߱�׼ͼ����3 ����ͼ��
	//ͼ���ߴ�
	float m_fMapWid, m_fMapHei;  //��λΪcm
	int m_nMapBorderType; //0 ѡ����ͼ����, 1 ѡ����ͼ�����½ǵ�, 2 ��ͼ�ż���
	//��ͼ����Χ
	PT_3D m_SelectedBoundPts[4]; //��mapBorderType==0ʱ��Ч
	PT_3D m_SelectBoundCorner; //��mapBorderType==1ʱ��Ч
	//ͶӰ��Ϣ
	CTM m_tm;
	//�Ƿ�ȥ������
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
	BOOL m_bNew;		//����һ����������ʶ���°��ɰ�
};


//�°�5000/10000�����ߵ�����˵����25000-50000Ҳ���ã�
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

	float dx;			//�ֵ�������ͼ���ľ���

	//��һ���ֲ���
	char data_desc1[256];
	char font1[64];
	float size1;		//�����С
	float wscale1;		//�ֿ����
	float char_interv1; //�ַ�������
	float line_interv1; //�м�����	

	//�ڶ����ֲ���
	char data_desc2[256];
	char font2[64];
	float size2;		//�����С
	float wscale2;		//�ֿ����
	float char_interv2; //�ַ�������
	float line_interv2; //�м�����
};

//������ͼ���ڽ�ͼ��
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

	float dx;				//��ߵ���ͼ���ľ��루mm��
	CString src_layers;		//������ͼ�е����ԭʼͼ�㣨��Ϊ�����

	//����Ĳ���
	char title_font[64];
	float title_size;		//����ߴ�
	float title_wscale;		//�ֿ����

protected:
	void DrawAdminRegion(double matrix1[16], double matrixA[16], double matrixB[16]);
	bool IsFeatureInBound(CFeature* pFtr);
};

//���½ǰ�Ȩ����
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float dy;		//��������ͼ���ľ��루mm��
};

//���ֱ�����
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float dy;		//�ײ�����ͼ���ľ��루mm��

	//���⣨�л����񹲺͹����������ߵ���ͼ��
	char title_font[64];
	float title_size;
	float title_wscale;
};
//


//�������򣨹��°�25000-50000������ʹ�ã�
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
	float dx;		//�������ߵ���ͼ���ľ��루mm��
	float dy;		//���ߵ���ͼ���ľ��루mm��

	//����
	char font1[64];
	float size1;
	float wscale1;	
	
	//�Ƕ�ע��
	char font2[64];
	float size2;
	float wscale2;
	
	float angle1;			//��������ƫ��
	float angle2;			//��������ƫ��
protected:
	void AddArcAndNote( PT_3DEX pts[3], float included_angle, float text_angle);
	float AddAngle( float angle1, float angle2 );
};


//�ű��ߣ����°�25000-50000������ʹ�ã�
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

	float angle;		//��ƫ��

	char layer[64];

	char font[64];
	float size;
	float wscale;
};


//�����ߣ����°�5000-10000������ʹ�ã�
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float dy;		//���ߵ���ͼ���ľ��루mm��
	float dx;		//�ұߵ���ͼ���ľ��루mm��

	float dy_scaleRuler;
	char scaleLine_font[64]; //�̶��������ֵ�����
	float scaleLine_size;		//�̶��������ֵĳߴ�

	float dy_text;			//�ױ����ֵ������ߵľ���
	char text_font[64];
	float text_size;
	float text_wscale;
};



//�����ߣ����°�25000-50000������ʹ�ã�
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
	float size;		//���ֳߴ�
	float wscale;	//�ֿ����
	float dy;		//���ߵ���ͼ���ľ��루mm��
	float dx;		//��ߵ��¶ȳߵľ��루mm��

	float dy_scaleRuler;
	char scaleLine_font[64]; //�̶��������ֵ�����
	float scaleLine_size;		//�̶��������ֵĳߴ�

	float dy_text;			//�ױ����ֵ������ߵľ���
	char text_font[64];
	float text_size;
	float text_wscale;
};



//�¶ȳߣ����°�25000-50000������ʹ�ã�
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
	float dy;		//�ױߵ���ͼ���ľ��루mm��
	float dx;		//��ߵ���ͼ���ľ��루mm��
	float dy2;		//���ߣ����⣩����ͼ���ľ��루mm��


	float x_interv;	//�¶ȳߺ����ࣨmm�����ڲ�ʹ��
	//����
	char title_font[64];
	float title_size;
	float title_wscale;

	//ע��
	char note_font[64];
	float note_size;
	float note_wscale;

	//�̶�
	char cal_font[64];
	float cal_size;
	float cal_wscale;

	float con_interv;		//�ȸ߾�Ϊ5(m)
};
//


//��ͼ��װ����
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
	float width;			//װ���߿��
	float extension;		//�������ͼ�����������루mm��
	float interv;			//���ߵļ����mm��
};


//������ͼ
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

	float width;	//�ܿ��
	float height;	//�ܸ߶�

	float dx;				//��ߵ���ͼ���ľ��루mm��
	float dy;				//�ױߵ���ͼ���ľ��루mm��
	CString src_layers;

	//����Ĳ���
	char font[64];
	float size;		//����ߴ�
	float wscale;		//�ֿ����

protected:
	void DrawAdminRegion(double matrix1[16], double matrixA[16], double matrixB[16]);
	bool IsFeatureInBound(CFeature* pFtr);		//�жϵ����Ƿ�����ͼ����
};
//
}

#endif // !defined(AFX_MAPDECORATOR_H__CABB10F7_308E_48D7_8F01_AEF6A54CECEF__INCLUDED_)
