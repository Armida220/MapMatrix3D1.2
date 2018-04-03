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
#define SYMTYPE_TIDALWATER					33		//��ˮ��
#define SYMTYPE_CULVERTSURFACE1             35      //������1
#define SYMTYPE_CULVERTSURFACE2             36      //������2


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

//����
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

//��Ԫ����
class EXPORT_EDITBASE CCell : public CSymbol
{
public:
	//����λ�ã�0���׵㣬1��β�㣬2���е㣬3�����ĵ㣬
	enum PlaceType
	{
		Head = 0,
		Tail = 1,
		Middle = 2,
		Center = 3
	};

	enum FillType
	{
		BaseLine = 1, //����
		AidLine = 2,  //�����ߣ����磬ƽ���ߵ����ϵĸ����ߣ�
		MidLine = 3,  //����
	};

	// ��������ʱ�߶εķ���; 0,������ 1,����β������ 2,�������ߵ��е�����
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

	//���״�ɢ
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);

	//�����״�ɢ
	virtual BOOL ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);
	
public:

	// ѹ������: 0, ��ѹ�� 1,����ѹ�� 2,Բ��ѹ��
	int     m_nCoverType;
	// ѹ����������
	float   m_fExtendDis;

	//ͼԪ�����ź���ת
	double m_kx;
	double m_ky;
	double m_angle;
	
	//����λ��
	int    m_nPlaceType;

	//���߻��Ǹ�����
	int m_nFillType;

	// �������ŷ�������ʱ��Ч
	int m_nCenterLineMode;
	// ���ĵ�ʱ�����Ƿ����׶���
	bool m_bDirWithFirstLine;

	//ƫ�ƾ���
	double m_dx;
	double m_dy;

	//�߿�
	float m_fWidth;
	
	//ͼԪ����
	CString m_strCellDefName;
};

class CGeoParallel;
//�������ͣ��������ͣ�
class EXPORT_EDITBASE CDashLinetype : public CSymbol
{
public:
	enum
	{
		BaseLine = 1, //����
		AidLine = 2,  //�����ߣ����磬ƽ���ߵ����ϵĸ����ߣ�
		MidLine = 3  //����
	};
	enum IndentType
	{
		Node = 0,    // �ڵ�ǰ������
		Mid = 1,     // �е�ǰ������
		HeadTail = 2	// ��β����
	};
	// ����ߵľ���
	float m_fBaseOffset;

	//�߿�
	float m_fWidth;

	// ��ʼƫ��
	float m_fXOffset0;

	// ��������
	float m_fIndent;

	// �������ͣ��ڵ�ǰ��,�е�ǰ��
	int   m_nIndentType;

	//���õ��������ͷ�����
	CString m_strBaseLinetypeName;

	//��Ⱦ��ʽ��BaseLine����AidLine��
	int m_nPlaceType;

	// �Ƿ�ʹ�ö�����ɫ��ȱʡΪ��
	BOOL m_bUseSelfcolor;

	// ��ɫֵ
	DWORD m_nColor;

	//�Ƿ��Զ��������ƫ�ƣ�ʹ��ͷβ��ʵ�ߣ��м�������
	BOOL m_bAdjustXOffset;

	BOOL m_bDashAlign;

public:
	CDashLinetype();
	virtual ~CDashLinetype();

	virtual void CopyFrom(CSymbol *pSymbol);

	//���״�ɢ
	virtual BOOL Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos);

	//�����״�ɢ
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

//�������ͣ��������䣩
class EXPORT_EDITBASE CCellLinetype : public CSymbol
{
public:
	enum PlacePos
	{
		Cycle = 1,			//�Ⱦ������ڷ���
		Vertex = 2,			//ÿ���������һ��
		VertexDouble = 3	//ÿ���������ǰ�����������Ƕ˵�ֻ����һ�������磬�����߷��ţ�
	};
	enum PlaceType
	{
		BaseLine = 1, //����
		AidLine = 2,  //�����ߣ����磬ƽ���ߵ����ϵĸ����ߣ�
		MidLine = 3,  //����
	};

	// ƫ��
	float m_fXOffset;
	float m_fYOffset;	

	//ͼԪ�����ź���ת
	float m_fAngle;
	float m_fkx;
	float m_fky;

	//���߻��Ǹ�����
	int m_nPlaceType;

	// �����ĸ�ߵĴ�ֱƫ��
	float m_fBaseXOffset,m_fBaseYOffset;

	// ��Ⱦ����: ����ѭ��, ������õȵ�
	int m_nPlacePos;

	// ���ڣ�ֻ������ѭ������ʱ����
	float m_fCycle;

	//�߿�
	float m_fWidth;

	// ͼԪ������
	CString m_strCellDefName;

	//�Ƿ���Ҫ����ͼԪ������������һ����
	BOOL m_bCellAlign;

	//�Ƿ�Ҫ��ͼԪ���岻�������߷�Χ����ʱ����Ҫ����ͼԪ�Ŀ�ȣ�
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
	// arrindexΪdpts�ڻ���������ĵ�����
	void TransformLine(CArray<PT_3DEX,PT_3DEX> &pts,CArray<PT_3DEX,PT_3DEX> &dpts, CUIntArray &arrindex, float cycle, float fbaseXoff, float fbaseYoff, 
		float fAssitWid, float fDrawScale=1.0);

	
};

//��ɫ������
class EXPORT_EDITBASE CColorHatch : public CSymbol
{
public:

	// �Ƿ�ʹ�ö�����ɫ��ȱʡΪ��
	BOOL m_bUseSelfcolor;

	//��ɫ
	DWORD m_nColor;

	//͸����
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



//�������Ե���ɫ������
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

	//��ɫ
	DWORD m_nDefColor;

	//͸����
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


//��Ԫ������
class EXPORT_EDITBASE CCellHatch : public CSymbol
{
public:
	//ͼԪ������
	CString	m_strCellDefName;

	//�߿�
	float m_fWidth;

	//ͼԪ�����ź���ת
	float m_fAngle;
	float m_fkx;
	float m_fky;

	//���ĺ���/�����࣬�Լ��еĺ���ƫ��
	float m_fdx;
	float m_fdy;
	float m_fddx;
	float m_fddy;
	float m_fxoff;
	float m_fyoff;

	//�Ƿ�ȷ�ü�ͼԪ
	int   m_bAccurateCutCell;
	//ͼԪ�ĽǶ�Ϊ����Ƕ�
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
	//ע������
	enum AnnoType
	{
		Text = 0,		//����
		Height = 1,		//�߳�
		DHeight = 2,	//�ȸ�
		Attribute = 3,	//����
		LayerConfig = 4, //������
		Length = 5, //����
		Area = 6 //���
	};
	//����λ�ã�0���׵㣬1��β�㣬2���е㣬3�����ĵ㣬4��ÿ�����㣬5��ˮƽ�������У�6�������������
	//9, �պϵ������ĵ㣬�����ﲻ�պϣ�����ʾ
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
	//ע������
	int m_nAnnoType;
	
	//�������ݣ�������������ΪTextʱ��Ч
	TCHAR m_strText[128];

	TCHAR m_strField[32];

	//0, �׵㣬1��β���㣬2���е㣬3�����ĵ㣬4��ÿ�����㣬5��ˮƽ�������У�6�������������
	int m_nPlaceType;	
	
	//ƫ��
	float m_fXOff, m_fYOff;
	
	//ע��С����λ��
	int m_nDigit;
	
	//ע���ı�������
	TextSettings  m_textSettings;

	// ѹ������: 0, ��ѹ�� 1,����ѹ�� 2,Բ��ѹ��
	int     m_nCoverType;
	// ѹ����������
	float   m_fExtendDis;
};

class CGeoParallel;

//���������ͣ�ƽ���ߵĿ�ȷ���
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
	// ����dpt���Ż���pts�ķ�����ã�ratateptΪ��ת����
	double LayAlongLine(PT_3DEX *pts, int num, PT_3DEX *dpt, int dptnum, PT_3DEX *ratatept);

public:

	// �Ƿ���ȹ���
	BOOL m_bAverageDraw;
	
	// �������ͣ�0,�̶�����, 1,����������, 2,�ڵ�, 3,�׶�, 4,β��, 5,��β���ˣ�6,�е�
	int m_nPlaceType; 
	
	// ���ڳ��ȣ����� placetype=0,1 ʱ��Ч
	double m_lfCycle;

	// ���ˮƽƫ�Ƶĳ��ȣ�(�̶����ȵ�ƫ��) 
	double m_lfXOffset0;

	// ��㴹ֱƫ�Ƶĳ��ȣ�(�̶����ȵ�ƫ��) 
	double m_lfYOffset0;
	
	// ��㴹ֱƫ�Ƶ���������ϵ��
	double m_lfYOffsetType0;

	// �յ�ˮƽƫ�Ƶĳ��ȣ�(�̶����ȵ�ƫ��) 
	double m_lfXOffset1;

	// �յ㴹ֱƫ�Ƶĳ��ȣ�(�̶����ȵ�ƫ��) 
	double m_lfYOffset1;
	
	// �յ㴹ֱƫ�Ƶ���������ϵ��
	double m_lfYOffsetType1;

	// �߿�
	double m_lfWidth;
	
	// ������������
	CString	m_strBaseLinetypeName;
	
};


// ����������
class EXPORT_EDITBASE CLineHatch : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
	float m_fWidth;		
	// �Ƕ�
	float m_fAngle;
	// ���
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

// �Խ����й�
class EXPORT_EDITBASE CDiagonal : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
	float m_fWidth;	
	// 0�����Խ��ߣ�1���������ؿ�
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

// ƽ�н���
class EXPORT_EDITBASE CParaLinetype : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
	float m_fWidth;
	//���
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

// ��ƽ����
class EXPORT_EDITBASE CAngBisectortype : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
	float m_fWidth;
	//����
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

// ������Բ��
class EXPORT_EDITBASE CScaleArctype : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
	float m_fWidth;
	// ¶������
	float m_fArcLength;	
	// �������ͣ�0,�׶�, 1,β��, 2,��β����
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

// ת��
class EXPORT_EDITBASE CScaleTurnplatetype : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;

	//��ľ�߿�
	float m_fWidth;

	//��ľͻ������
	float m_fStickupLen;

	// ������
	float m_fRoadWidth;	

	//����ı����߿�
	float m_fSideLineWidth;

	//�м������·������������·��
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

// ����
class EXPORT_EDITBASE CScaleCranetype : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
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

// �����°棺װж©������ͨ�������Ͷ��ڿ��ڵģ�����Բ�κͷ��Σ���
// ���ʾɰ棺©�������ڿ��ڵģ�
class EXPORT_EDITBASE CScaleFunneltype : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
	float m_fWidth;
	// �����°棺0����ͨ��������1�����ڿ���
	// ���ʾɰ棺3�����ڿ��ڣ�
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


// ֻ�ǽ���ֻ���߿���������ĳ�������ŵĻ��࣬��������
class EXPORT_EDITBASE CProcSymbol_LT : public CSymbol
{
public:
	//�������ͷ�����
	CString m_strBaseLinetypeName;
	//�߿�
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


// ����������
class EXPORT_EDITBASE CScaleLiangCang : public CProcSymbol_LT
{
public:	
	CScaleLiangCang(){
		m_nType = SYMTYPE_SCALE_LiangChang;
	}
	virtual ~CScaleLiangCang(){}
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};

// �͹�
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


// �򵥷���
class EXPORT_EDITBASE CScaleJianFangWu : public CProcSymbol_LT
{
public:	
	CScaleJianFangWu(){
		m_nType = SYMTYPE_SCALE_JianFangWu;
	}
	virtual ~CScaleJianFangWu(){}
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};

// ����������Ҥ��
class EXPORT_EDITBASE CScaleDiShangYaoDong : public CProcSymbol_LT
{
public:	
	CScaleDiShangYaoDong(){
		m_nType = SYMTYPE_SCALE_DiShangYaoDong;
	}
	virtual ~CScaleDiShangYaoDong(){}
	virtual void Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale=1.0, float fRotAngle=0, float fViewScale=1.0);
};

// ͨ��ˮբ
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


// ��ͨ��ˮբ
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

// ���ʹ�
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

	// 0�����ϣ�1������
	int m_nCarryType;
};
// �������ر�����
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


// �°�����������
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


//������ͼԪ���ţ�������ƺ������������������������
class EXPORT_EDITBASE CScaleCell : public CSymbol
{
public:
	// ͼԪ������
	CString m_strCellDefName;

	//������Ƶ�����X���ꡢ���X����
	float m_fX1, m_fX2;

	//������Ƶ��Y����ľ���ֵ
	float m_fY1;

	//�߿�
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
	
	//ͼԪ������
	float m_fkx;
		
	// �����ĸ�ߵĴ�ֱƫ��
	float m_fBaseXOffset,m_fBaseYOffset;

	// ͼԪ�븨���ߵļ��
	float m_fAssistYOffset;
		
	// ���ڣ�
	float m_fCycle;
	
	//�߿�
	float m_fWidth;
	
	// ͼԪ������
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


//�ɰ�����������
class EXPORT_EDITBASE CScaleOldDouya : public CSymbol
{
public:
	//���ڳ���
	float m_fCycle;

	//���ݳ�
	float m_fMaxToothLen;
	
	// �ݼ��
	float m_fInterval;
	
	//�߿�
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


//������б��
class EXPORT_EDITBASE CScaleXiepo : public CSymbol
{
public:		
	//��ֱ��ĸ�߷����ƫ��
	float m_fBaseYOffset;

	// �ݼ��
	float m_fInterval;

	//�̳ݳ���
	float m_fToothLen;
	
	//�߿�
	float m_fWidth;

	//�Ƿ�ӹ�
	BOOL m_bFasten;

	//�ӹ̵���ŵĳߴ�
	float m_fPointSize;

	//�ӹ̵���̳ݵļ��
	float m_fPointInterval;

	//�ǶȾ��ȹ���
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


//��ˮ��
class EXPORT_EDITBASE CTidalWaterSymbol : public CSymbol
{
public:	
	// ͼԪ������
	CString m_strCellDefName;

	//ͼԪ������ϵ����Χ
	float m_fkxMin, m_fkxMax;

	//ͼԪ���
	float m_fInterval;

	//�߿�
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


//����-��-1
class EXPORT_EDITBASE CCulvertSurface1Symbol : public CSymbol
{
public:
	CString m_strLinetypeName;
	//�߿�
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

//����-��-2
class EXPORT_EDITBASE CCulvertSurface2Symbol : public CSymbol
{
public:
	CString m_strLinetypeName;
	//�߿�
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
