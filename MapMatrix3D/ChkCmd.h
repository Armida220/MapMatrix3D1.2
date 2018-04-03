// ChkCmd.h: interface for the CChkCmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHKCMD_H__EA30E001_27F8_4D1E_A55C_9309C1FD3608__INCLUDED_)
#define AFX_CHKCMD_H__EA30E001_27F8_4D1E_A55C_9309C1FD3608__INCLUDED_
#include <vector>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CDlgDoc;
class CChkResManger;
class CChkCmd  
{
public:
	CChkCmd();
	virtual ~CChkCmd();
	inline void Init(CDlgDoc *pDoc){
		m_pDoc = pDoc;
	}
	void SetChkResMgr(CChkResManger *pChkResMgr){
		m_pChkResMgr = pChkResMgr;		
	}
	virtual BOOL CheckParamsValid(CString &reason){
		return TRUE;
	}
	virtual CString GetCheckCategory()const =0 ;
	virtual CString GetCheckName()const =0;
	virtual void GetParams(CValueTable& tab)const =0;
	virtual void SetParams(CValueTable& tab)=0;
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE)=0;
	virtual void Execute(BOOL bForSelection=FALSE)=0;
	virtual void GetAllReasons(CStringArray& reasons)=0;

	void MarkSelectionFtrs(int flag);
protected:
	CDlgDoc *m_pDoc;
	CChkResManger *m_pChkResMgr;

};

typedef CChkCmd* (*LPPROC_CHKCMDCREATE)();

#define  CHK_INT     _T("CHK_INT")
#define  CHK_DBL	 _T("CHK_DBL")
#define  CHK_STR	 _T("CHK_STR")
#define  CHK_BOOL    _T("CHK_BOOL")
#define  CHK_OPTION  _T("CHK_OPTION")
class CTestChkCmd:public CChkCmd
{
	enum
	{
		Option1=0,
		Option2,
		Option3
	};
public:
	CTestChkCmd();
	~CTestChkCmd();
	static CChkCmd* Create(){
		return new CTestChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason){
		return m_bBool;
	}
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	int m_nInt;
	double m_lfDouble;
	CString m_strString;
	BOOL m_bBool;
	int m_nOption;
};

#define    CHK_LINE_2PT				_T("chk_line_2pt")
#define    CHK_SURFACE_3PT			_T("chk_surface_3pt")
#define    CHK_REPEATEDPT			_T("chk_repeatedpt")
#define	   CHK_TOLER_REPEATEDPT		_T("chk_toler_repeatedpt")
#define	   CHK_TURNROUNDLINE		_T("chk_turnroundline")
class CFtrSpaceLogicChkCmd:public CChkCmd
{
public:
	CFtrSpaceLogicChkCmd();
	~CFtrSpaceLogicChkCmd();
	static CChkCmd* Create(){
		return new CFtrSpaceLogicChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLays;
	BOOL m_bCheckLineForLess2Pt;
	BOOL m_bCheckSurfaceLess3Pt;
	BOOL m_bCheckRepeatedPt;
	double m_lfTolerForRepeatedPt;
	BOOL m_bTurnRoundLine;
};


#define   CHK_OVERLAP_OBJTYPE			_T("chk_overLap_ObjType")
#define   CHK_OVERLAP_TOLER				_T("chk_overLap_Toler")
#define   CHK_OVERLAP_SAMELAYCODE	    _T("chk_overLap_SameLaycode")

class COverlapChkCmd:public CChkCmd
{
	enum
	{
		Option_Pnt=0,
		Option_Line,
		Option_Surface,
		Option_Txt,
		Option_ALL
	};
public:
	COverlapChkCmd();
	~COverlapChkCmd();
	static CChkCmd* Create(){
		return new COverlapChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLays;
	BOOL m_bSameLayerCode;//�Ƿ�ͬ����
	int m_geotypelimit;
	int m_nMode;//0, �ڵ�һ�£� 1 ͼ��һ��
};

#define     CHK_SELF_INTERSECT_CODES    _T("chk_SelfIntersection_codes")
class CSelfIntersectionChkCmd:public CChkCmd
{	
public:
	CSelfIntersectionChkCmd();
	~CSelfIntersectionChkCmd();
	static CChkCmd* Create(){
		return new CSelfIntersectionChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	int m_geotypelimit;
};

#define     CHK_INTERSECTCURVE_CODES    _T("chk_IntersectionCurve_codes")
class CIntersectCurveChkCmd:public CChkCmd
{	
public:
	CIntersectCurveChkCmd();
	~CIntersectCurveChkCmd();
	static CChkCmd* Create(){
		return new CIntersectCurveChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
};

#define   CHK_MIN_ANGLE    _T("chk_MinAngle")
//Ŀǰֻ�����ߺ���
class CMinObtuseAngleChkCmd:public CChkCmd
{	
public:
	CMinObtuseAngleChkCmd();
	~CMinObtuseAngleChkCmd();
	static CChkCmd* Create(){
		return new CMinObtuseAngleChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes; //ͼ��
	double m_lfMinAngle;//�Ƕ�

};

#define      CHK_TARGET_OBJ_CODE	_T("chk_target_obj_fCode")
#define      CHK_ADJACENT_OBJ_CODE	_T("chk_adjacent_obj_fCode")
#define		 CHK_ISOLATED_RADIUS	_T("chk_isolated_radius")

class CIsolatedObjChkCmd:public CChkCmd
{
public:
	CIsolatedObjChkCmd();
	~CIsolatedObjChkCmd();
	static CChkCmd* Create(){
		return new CIsolatedObjChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strTargetCode;
	CString m_strAdjCode;
	double m_lfSearchRadius;
protected:
	double GetDisOfPt2Ftr(PT_3DEX pt, CFeature* pFtr);	//����㵽�������̾���
};

#define  CHK_SURFACE_INTERSEC_LAYCODES   _T("chk_surface_intersect_codes")
class CSurfaceintersectChkCmd:public CChkCmd
{
public:
	CSurfaceintersectChkCmd();
	~CSurfaceintersectChkCmd();
	static CChkCmd* Create(){
		return new CSurfaceintersectChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
};


#define  CHK_SURFACE_INCLUDE_LAYCODES   _T("chk_surface_include_codes")
class CSurfaceincludeChkCmd:public CChkCmd
{
public:
	CSurfaceincludeChkCmd();
	~CSurfaceincludeChkCmd();
	static CChkCmd* Create(){
		return new CSurfaceincludeChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
};


#define     CHK_PARTOVERLAP_TOLER   _T("chk_part_overlap_toler")
class CPartOverlapLineWithTolChkCmd:public CChkCmd
{
public:
	CPartOverlapLineWithTolChkCmd();
	~CPartOverlapLineWithTolChkCmd();
	static CChkCmd* Create(){
		return new CPartOverlapLineWithTolChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes1;
	CString m_strLayerCodes2;
	double  m_lfToler;
	int     m_geotypelimit;
};
#define  CHK_GEOCURVE_CLOSED_LAYCODES   _T("chk_geocurve_closed_codes")
#define  CHK_GEOCURVE_CLOSED_CHKCLOSE   _T("chk_geocurve_closed_chkclosed")
class CGeoCurveClosedChkCmd:public CChkCmd
{
public:
	CGeoCurveClosedChkCmd();
	~CGeoCurveClosedChkCmd();
	static CChkCmd* Create(){
		return new CGeoCurveClosedChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	BOOL m_bChkClosed;
};
  
#define     CHK_INDEX_CONTOUR_CODE		_T("chk_index_contour_code")
#define     CHK_STD_CONTOUR_CODE		_T("chk_standard_contour_code")
#define     CHK_CONTOUR_INTERVAL     	_T("chk_contour_interval")
#define     CHK_STD_CONTOUR_CNT	        _T("chk_standard_contour_cnt")

struct ContourObj
{
	CArray<CFeature*,CFeature*> ftrs;
	double height;

	ContourObj() {}
	ContourObj(const ContourObj &contour){
			ftrs.Copy(contour.ftrs);
			height = contour.height;
	}

	ContourObj& operator=(const ContourObj &contour){
		ftrs.Copy(contour.ftrs);
		height = contour.height;
		return *this;
	}

};

class CContourValidChkCmd:public CChkCmd
{	
public:
	CContourValidChkCmd();
	~CContourValidChkCmd();
	static CChkCmd* Create(){
		return new CContourValidChkCmd;
	}
	static bool CheckObjForZ( CGeometry *pObj);
	static bool CheckObjForZAndMarkPt( CGeometry *pObj,CArray<PT_3DEX,PT_3DEX> &arrPts);
	bool CheckObjBetweenTwoObjs(CFeature *pFtr1, CFeature *pFtr2, PT_3DEX *ret=NULL, double *len=NULL);
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strIdxContourCode;
	CString m_strStdContourCode;
	double m_lfContourInterval;
	int m_nStdContourCnt;
	bool m_bCheckLoseContour;

private:
	double GetDisOf2Obj(CFeature* line1, CFeature* line2, PT_3DEX& pt1, PT_3DEX& pt2);
	bool CheckObjBetweenTwoObjs(CFeature *pFtr1, CFeature *pFtr2, PT_3DEX pt1, PT_3DEX pt2);
};

class CDlgDataSource;
#define     CHK_PL_CONTOUR_CODE		    _T("chk_PL_contour_code")
#define     CHK_PL_TESTPT_CODE	    	_T("chk_PL_testpt_code")
#define     CHK_PL_CONTOUR_INTERVAL     _T("chk_PL_contour_interval")
#define     CHK_PL_MIN_HEI_DIFF	        _T("chk_PL_min_heidiff_cnt")
class CPLChkCmd:public CSearcherFilter, CChkCmd
{	
public:
	CPLChkCmd();
	~CPLChkCmd();
	static CChkCmd* Create(){
		return new CPLChkCmd;
	}
	static BOOL CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode);
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	static CStringArray m_strcodeArray;
	void CheckObj(CFeature *pFtr);
	BOOL FilterFeature(LONG_PTR id);
	CString m_strContourCode;
	CString m_strTestPtCode;
	double m_lfContourInterval;
	double m_lfMinHeightDiff;
};

#define     CHK_LL_CONTOUR_CODE		        _T("chk_LL_contour_code")
#define     CHK_LL_TESTLINE_CODE	    	_T("chk_LL_testline_code")
class CLLChkCmd: CChkCmd
{	
public:
	CLLChkCmd();
	~CLLChkCmd();
	static CChkCmd* Create(){
		return new CLLChkCmd;
	}
	static BOOL CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode);
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	static CStringArray m_strCodeArray;
	void GetIntersectObjs(PT_3D pt0, PT_3D pt1, CArray<FTR_HANDLE,FTR_HANDLE>& arr);
	void CheckObj(CFeature *pFtr);
	CString m_strContourCode;
	CString m_strTestLineCode;
};

//�̵߳�ע����ͬ��ì��
class CPPChkCmd:public CSearcherFilter, CChkCmd
{	
public:
	CPPChkCmd();
	~CPPChkCmd();
	static CChkCmd* Create(){
		return new CPPChkCmd;
	}
	BOOL CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode);
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
protected:
	void CheckObj(CFeature *pFtr);
	BOOL FilterFeature(LONG_PTR id);

	CStringArray m_strcodeArray;
	CString m_strTestPtCode;
	double m_lfDistance;
};


#define    CHK_RIVERZ_VALID_CODE     _T("chk_riverZ_valid_codes")
#define    CHK_RIVERZ_DESC			 _T("chk_riverZ_desc")
class CRiverZChkCmd:public CChkCmd
{
public:
	CRiverZChkCmd();
	~CRiverZChkCmd();
	static CChkCmd* Create(){
		return new CRiverZChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	double m_lfTolerZ;
	CString m_strLayerCodes;
	BOOL m_bRiverZIncValid;
	BOOL m_bChkMonotony;
};

#define    CHK_CLOCK_DIR_CODE     _T("chk_clock_dir_codes")
#define    CHK_CLOCKWISE 		  _T("chk_clockwise")
class CClockDirChkCmd:public CChkCmd
{
public:
	CClockDirChkCmd();
	~CClockDirChkCmd();
	static CChkCmd* Create(){
		return new CClockDirChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	BOOL m_bClockwise;
};

#define    CHK_LEN_CODE			_T("chk_length_codes")
#define    CHK_LEN_MIN			_T("chk_length_min")
#define    CHK_LEN_MAX			_T("chk_length_max")
class CLengthChkCmd:public CChkCmd
{
public:
	CLengthChkCmd();
	~CLengthChkCmd();
	static CChkCmd* Create(){
		return new CLengthChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	double m_lfMinlen;
	double m_lfMaxLen;
};


#define    CHK_AREA_CODE		_T("chk_area_codes")
#define    CHK_AREA_MIN			_T("chk_area_min")
#define    CHK_AREA_MAX			_T("chk_area_max")
class CAreaChkCmd:public CChkCmd
{
public:
	CAreaChkCmd();
	~CAreaChkCmd();
	static CChkCmd* Create(){
		return new CAreaChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	double m_lfMinArea;
	double m_lfMaxArea;
};

#define    CHK_Z_CODE		    _T("chk_z_codes")
#define    CHK_Z_MIN			_T("chk_z_min")
#define    CHK_Z_MAX			_T("chk_z_max")
class CZChkCmd:public CChkCmd
{
public:
	CZChkCmd();
	~CZChkCmd();
	static CChkCmd* Create(){
		return new CZChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	double m_lfMinZ;
	double m_lfMaxZ;
	BOOL m_bOnlyContour;
protected:
	BOOL GetZValue(CFeature* pFtr, double* pZValue);
};

//hcw,2013.1.24,��������һ���Լ��
#define CHK_GEOTYPE_CONSISTENCY_CODES   _T("chk_geotype_consistency_codes")
class CGeoTypeConsistencyChkCmd:public CChkCmd
{
public:
	CGeoTypeConsistencyChkCmd();
	~CGeoTypeConsistencyChkCmd();
	static CChkCmd* Create(){
		return new CGeoTypeConsistencyChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	BOOL CheckGeoTypeConsistency(CFeature* pFtr, CString &strObjType); //����������һ����
};

//hcw,2013.1.25,ע����������ߵ�һ���Լ��
#define CHK_ANNO_LAYCODES						_T("chk_anno_laycodes")
#define CHK_ANNO_TEXT							_T("chk_anno_text")
#define CHK_SURFACE_CLOSEDCURVE_LAYCODES		_T("chk_surface_closedcurve")
class CAnnoandSurfaceConsistencyChkCmd: public CChkCmd
{
public: 
	CAnnoandSurfaceConsistencyChkCmd();
	~CAnnoandSurfaceConsistencyChkCmd();
	static CChkCmd* Create(){
		return new CAnnoandSurfaceConsistencyChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strAnnoLayerCodes;
    CString m_strAnnoText;
	CString m_strSurfaceorClosedCurveLayerCodes;
	BOOL CheckAnnoandSurfaceConsistency(CFeature* pFtr, vector<CFeature*>& textContainer, UINT& nGeoType);//����
	void RemoveElem(CFeature* pFtr, vector<CFeature*>& container);
	void GetMatchedAnnoLayInClosedCurve(CGeoCurveBase* pClosedCurveFtr, vector<CFeature*>& container, vector<CFeature*>& subContainer); //��ȡ����߻����е��ı�ע�ǵ��
};


//hcw,2013.1.30,����������ߵ�һ���Լ��
#define CHK_PTS_LAYCODES							_T("chk_pts_laycodes")

class CPtsandSurfaceConsistencyChkCmd: public CChkCmd
{
public:
	CPtsandSurfaceConsistencyChkCmd();
	~CPtsandSurfaceConsistencyChkCmd();
	static CChkCmd* Create(){
		return new CPtsandSurfaceConsistencyChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
    CString m_strSurfaceorClosedCurveLayerCodes;
	BOOL CheckPtsandSurfaceConsistency(CFeature* pFtr, vector<CFeature*>& ptsContainer, UINT& nGeoType);//����
};

//hcw,2013.1.31,�߳�ע��һ���Լ��
#define CHK_Z_SEARCH_RADIUS				_T("chk_z_search_radius")
class CZandAnnoConsistencyChkCmd: public CChkCmd
{
public:
	CZandAnnoConsistencyChkCmd();
	~CZandAnnoConsistencyChkCmd();
	static CChkCmd* Create(){
		return new CZandAnnoConsistencyChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strZLayerCodes;    //�̵߳����
	CString m_strAnnoLayerCodes; //ע�ǲ���
	double m_lfSearchRadius;     //�̵߳������뾶
	double m_lfMinZ;			 //�߳�����
	double m_lfMaxZ;			 //�߳�����
	BOOL CheckZandAnnoConsistency(CFeature* pFtr, vector<CFeature*>& zTextContainer, double lfSearchRadius);//���̵߳����pFtr��ע��zTextContainer�Ƿ�һ�¡�
	void LoadDataSourceinZRange(CDlgDataSource* pDS, CUIntArray& clsArr, vector<CFeature*>& zPtsContainer, CString strZLayerCodes, double lfMinZ, double lfMaxZ);//�����ض���Χ�ڵĵ����
};


//�����ӹ���---2013.09.18
//1.����λ�ø�����ȱʧ�ļ��
#define CHK_CROSS_CODE _T("corss_code")
#define CHK_CROSSED_CODE _T("corssed_code")
#define CHK_APPENDANT_CODE _T("appendant_code")

class CAppendantLoseChkCmd:public CChkCmd
{
public:
	CAppendantLoseChkCmd();
	~CAppendantLoseChkCmd();
	static CChkCmd* Create(){
		return new CAppendantLoseChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_cross_layer;
	CString m_crossed_layer;
	CString m_appendant_layer;
	double sch_radius;
protected:
	BOOL CheckPt(PT_3DEX pt);//����0˵������ȱʧ
	BOOL GetAllIntersectPts(CFeature* pFtr1, CFeature* pFtr2, CArray<PT_3DEX,PT_3DEX>& arrPts);	//����������Ľ���
	void ExtractPts(CFeature* pFtr, CArray<PT_3DEX,PT_3DEX>& pts1, CArray<PT_3DEX,PT_3DEX>& pts2);//������������ȡ���������У���Ҫ���˫�ߡ�ƽ���ߺͶ���
	void GetPtsOfIntersect( CArray<PT_3DEX,PT_3DEX>* ptsLine1, CArray<PT_3DEX,PT_3DEX>* ptsLine2, CArray<PT_3DEX,PT_3DEX>& ptsOut); //�������ߴ��Ľ���
};


//2.�������Ҽ��
#define CHK_PLS_POINT_CODE _T("chekc_pls_point_code")
#define CHK_PLS_LINE_CODE _T("check_pls_line_code")
#define CHK_PLS_PSEUDO_SECT _T("check_pls_virtual_intersect")

class CPLSuspendChkCmd:public CChkCmd
{
public:
	CPLSuspendChkCmd();
	~CPLSuspendChkCmd();
	static CChkCmd* Create(){
		return new CPLSuspendChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_point_layer;
	CString m_line_layer;
	BOOL m_bAllow_pseudo_intersect;
protected:
	//
	int IsNodeOfLine(PT_3DEX pt, CFeature* pFtr);
	BOOL CheckPoint(PT_3DEX pt, CFeature* pFtr);	//����0˵������
};

//3.�������Ҽ��
#define CHK_LLS_LAYER_CODE _T("chekc_llsuspend_code")
#define CHK_LLS_PSEUDO_SECT _T("check_lls_pseudo_intersect")

class CLLSuspendChkCmd:public CChkCmd
{
public:
	CLLSuspendChkCmd();
	~CLLSuspendChkCmd();
	static CChkCmd* Create(){
		return new CLLSuspendChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	BOOL m_bAllow_pseudo_intersect;
protected:
	BOOL CheckPoint(PT_3DEX pt, CFeature* pFtr);	//����0˵������
	int IsNodeOfLine(PT_3DEX pt, CFeature* pFtr);
};

//4.��������߼��
#define CHK_URBL_LAYER_CODE _T("check_unreasonable_bl_code")
#define CHK_URBL_INTERVAL _T("check_unreasonable_bl_interval")

class CUnreasonableBLChkCmd:public CChkCmd
{
public:
	CUnreasonableBLChkCmd();
	~CUnreasonableBLChkCmd();
	static CChkCmd* Create(){
		return new CUnreasonableBLChkCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	double m_lfInterval;
protected:
	void CheckFeature(CFeature* pFtr, CFtrArray* pFtrArr, int index);	//indexΪpFtr������pFtrArr�е�����
};


//�����ɲۣ�˫�ߣ�����ʵ����
class CNoIntersectInsideTunnelCmd:public CChkCmd
{
public:
	CNoIntersectInsideTunnelCmd();
	~CNoIntersectInsideTunnelCmd();
	static CChkCmd* Create(){
		return new CNoIntersectInsideTunnelCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerTunnel;
protected:
	void CheckFeature(CFeature* pFtr);
	void GetSolidIntersect(CGeoCurveBase* pGeo1, CGeoCurveBase* pGeo2, CArray<PT_3D,PT_3D>& arrPts);
};


//�̵߳���ˮ�������ݵļ��
class CHeiPointOnHouseCmd:public CChkCmd
{
public:
	CHeiPointOnHouseCmd();
	~CHeiPointOnHouseCmd();
	static CChkCmd* Create(){
		return new CHeiPointOnHouseCmd;
	}
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual BOOL CheckParamsValid(CString &reason);
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerHeiPoint;
	CString m_strLayerHouse;
protected:
	void CheckFeature(CFeature* pFtr);	//indexΪpFtr������pFtrArr�е�����
};


//�ߴ����պϵ���
#define CHK_CURVE_CODE    _T("chk_curve_code")
#define CHK_SURFACE_CODE  _T("chk_surface_code")
class CCurveAcrossSurfaceChkCmd:public CChkCmd
{	
public:
	CCurveAcrossSurfaceChkCmd();
	~CCurveAcrossSurfaceChkCmd();
	static CChkCmd* Create(){
		return new CCurveAcrossSurfaceChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);

protected:
	CString m_strCurveCode;
	CString m_strSurfaceCode;
};

//�ȸ��߲�����ˮϵ�����ཻ
class CContourRiverChkCmd:public CCurveAcrossSurfaceChkCmd
{	
public:
	CContourRiverChkCmd();
	~CContourRiverChkCmd();
	static CChkCmd* Create(){
		return new CContourRiverChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
};

//���ݲ������ߴ���
class CCurveAcrossHouseChkCmd:public CCurveAcrossSurfaceChkCmd
{	
public:
	CCurveAcrossHouseChkCmd();
	~CCurveAcrossHouseChkCmd();
	static CChkCmd* Create(){
		return new CCurveAcrossHouseChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
};

#define CHK_CONTOUR_CODE _T("chk_contour_code")
#define CHK_RIVER_CODE   _T("chk_river_code")
//������ȸ��ߴ�Խ����ȷ��
class CContourRiverIntersectChkCmd:public CChkCmd
{	
public:
	CContourRiverIntersectChkCmd();
	~CContourRiverIntersectChkCmd();
	static CChkCmd* Create(){
		return new CContourRiverIntersectChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
	
	BOOL IsCurveIntersect( CArray<PT_3DEX,PT_3DEX>& pts1, CArray<PT_3DEX,PT_3DEX>& pts2, CArray<PT_3DEX,PT_3DEX>& retpts);
private:
	CString m_strContourCode;
	CString m_strRiverCode;
};

//�̵߳���ȶȼ��
class CElevPtNumChkCmd:public CChkCmd
{	
public:
	CElevPtNumChkCmd();
	~CElevPtNumChkCmd();
	static CChkCmd* Create(){
		return new CElevPtNumChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);

protected:	
	CString m_strTKCode;
	CString m_strElevPtCode;
	double  m_lfDistance;
	int     m_nLimitPt;
};

//����Ϸ��Լ��
class CLayerChkCmd:public CChkCmd
{
public:
	CLayerChkCmd();
	~CLayerChkCmd();
	static CChkCmd* Create(){
		return new CLayerChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayerCodes;
	CString m_strField;
};

//������
class CSlpoeChkCmd:public CChkCmd
{
public:
	CSlpoeChkCmd();
	~CSlpoeChkCmd();
	static CChkCmd* Create(){
		return new CSlpoeChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strlayer1;
	CString m_strlayer2;

	BOOL CheckLoopAndDir(CFeature *pFtr, CFeature *pFtr2);
};

#include "editbasedoc.h"

//ͼ����ӱ߼��
class CMapMatchChkCmd:public CChkCmd
{
public:
	struct OBJ_ITEM1
	{
		OBJ_ITEM1(){
			pLayer = NULL;
			pFtr = NULL;
			segid = -1;
			pds = NULL;
			position = 0;
		}
		CFtrLayer *pLayer;
		CFeature *pFtr;
		PT_3DEX pt;
		int segid;
		int position;//1ͼ�����Ҳ࣬-1ͼ������࣬2ͼ�����ϲ࣬-2ͼ�����²�
		CDlgDataSource *pds;
	};
	struct OBJ_ITEM2 : public OBJ_ITEM1
	{
		OBJ_ITEM2(){
			pLayer = NULL;
			pFtr = NULL;
			segid = -1;
			pds = NULL;
			position = 0;
		}
		PT_3DEX pt1;
	};
public:
	CMapMatchChkCmd();
	~CMapMatchChkCmd();
	static CChkCmd* Create(){
		return new CMapMatchChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	void ExecuteSingleDS(BOOL bForSelection=FALSE);
	void ExecuteMutiDS(BOOL bForSelection=FALSE);
	CString m_strTKLayerCode;
	double m_lfToler;
	BOOL m_bMatchColor;
	BOOL m_bMatchAttr;
};

//�����ཻ
class CCurveIntersectCurveChkCmd:public CChkCmd
{
public:
	CCurveIntersectCurveChkCmd();
	~CCurveIntersectCurveChkCmd();
	static CChkCmd* Create(){
		return new CCurveIntersectCurveChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_lineLayer1;
	CString m_lineLayer2;
};

//���϶���
class CSurfaceGapChkCmd:public CChkCmd
{
public:
	CSurfaceGapChkCmd();
	~CSurfaceGapChkCmd();
	static CChkCmd* Create(){
		return new CSurfaceGapChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	double m_lfArea;
};

//�ߴ�ҧ���Լ��
class CCurveSnapChkCmd:public CChkCmd
{
public:
	CCurveSnapChkCmd();
	~CCurveSnapChkCmd();
	static CChkCmd* Create(){
		return new CCurveSnapChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
protected:
	void CheckSnap(CArray<PT_3DEX, PT_3DEX>& pts1, CArray<PT_3DEX, PT_3DEX>& pts2, CArray<PT_3D,PT_3D>& ret);
private:
	CString m_strLayers;
	double m_lfRange;
};


//�������ͼ��
class CCulvertLineTypeChkCmd:public CChkCmd
{
public:
	CCulvertLineTypeChkCmd();
	~CCulvertLineTypeChkCmd();
	static CChkCmd* Create(){
		return new CCulvertLineTypeChkCmd;
	}
	
	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection=FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strCulvertLayer;
	CString m_strLayers;
	int m_nLineType;
};

//����ֱ�ǻ����
class CHouseRectangularChkCmd :public CChkCmd
{
public:
	CHouseRectangularChkCmd();
	~CHouseRectangularChkCmd();
	static CChkCmd* Create(){
		return new CHouseRectangularChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection = FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strHouseLayers;
	double m_lfToler1;
	//double m_lfToler2;
	double m_lfAreaToler;
};

//�̵߳��غ����ľ��ȼ��
class CHeiPointAcrossCurveChkCmd :public CChkCmd
{
public:
	CHeiPointAcrossCurveChkCmd();
	~CHeiPointAcrossCurveChkCmd();
	static CChkCmd* Create(){
		return new CHeiPointAcrossCurveChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection = FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strHeiLayer;//�̵߳�ͼ��
	CString m_strLayers;//��ͼ��
	double m_lfToler1;//�������
	double m_lfToler2;//�������
};

//��������
class CNarrowSurfaceChkCmd :public CChkCmd
{
public:
	CNarrowSurfaceChkCmd();
	~CNarrowSurfaceChkCmd();
	static CChkCmd* Create(){
		return new CNarrowSurfaceChkCmd;
	}

	CString GetCheckCategory()const;
	virtual CString GetCheckName()const;
	virtual void GetParams(CValueTable& tab)const;
	virtual void SetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad);
	virtual void Execute(BOOL bForSelection = FALSE);
	virtual void GetAllReasons(CStringArray& reasons);
private:
	CString m_strLayers;
	double m_lfToler;
};

#endif // !defined(AFX_CHKCMD_H__EA30E001_27F8_4D1E_A55C_9309C1FD3608__INCLUDED_)
