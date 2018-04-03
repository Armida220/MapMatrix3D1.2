#if !defined(AFX_DLGEXPORTARCGISMDB_H__5C8B4A8A_7111_4FFB_95B9_432CC24C7984__INCLUDED_)
#define AFX_DLGEXPORTARCGISMDB_H__5C8B4A8A_7111_4FFB_95B9_432CC24C7984__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportArcgisMdb.h : header file
//

class CDlgDoc;
class CDlgDataSource;

#include "ArcgisMdbCodeTab.h"

void GetTmpFilePath(char *path, char* filename);

enum mEsriFeatureType
{
	mEsriFTSimple = 1,
	mEsriFTSimpleJunction = 7,
	mEsriFTSimpleEdge = 8,
	mEsriFTComplexJunction = 9,
	mEsriFTComplexEdge = 10,
	mEsriFTAnnotation = 11,
	mEsriFTCoverageAnnotation = 12,
	mEsriFTDimension = 13,
	mEsriFTRasterCatalogItem = 14
};

enum mEsriFieldType
{
	mEsriFieldTypeSmallInteger = 0,
	mEsriFieldTypeInteger = 1,
	mEsriFieldTypeSingle = 2,
	mEsriFieldTypeDouble = 3,
	mEsriFieldTypeString = 4,
	mEsriFieldTypeDate = 5,
	mEsriFieldTypeOID = 6,
	mEsriFieldTypeGeometry = 7,
	mEsriFieldTypeBlob = 8,
	mEsriFieldTypeRaster = 9,
	mEsriFieldTypeGUID = 10,
	mEsriFieldTypeGlobalID = 11,
	mEsriFieldTypeXML = 12
};
enum mEsriGeometryType
{
	mEsriGeometryNull = 0,
	mEsriGeometryPoint = 1,
	mEsriGeometryMultipoint = 2,
	mEsriGeometryLine = 13,
	mEsriGeometryCircularArc = 14,
	mEsriGeometryEllipticArc = 16,
	mEsriGeometryBezier3Curve = 15,
	mEsriGeometryPath = 6,
	mEsriGeometryPolyline = 3,
	mEsriGeometryRing = 11,
	mEsriGeometryPolygon = 4,
	mEsriGeometryEnvelope = 5,
	mEsriGeometryAny = 7,
	mEsriGeometryBag = 17,
	mEsriGeometryMultiPatch = 9,
	mEsriGeometryTriangleStrip = 18,
	mEsriGeometryTriangleFan = 19,
	mEsriGeometryRay = 20,
	mEsriGeometrySphere = 21,
	mEsriGeometryTriangles = 22
};
inline int FieldTypeFrom(int esriType);
inline int esriFieldTypeFrom(int Type);
inline int XdefineTypeToEsri(int valuetype);
void Linearize_forExportArcgisMdb(CGeometry *pObj);

/////////////////////////////////////////////////////////////////////////////
// CDlgExportArcgisMdb dialog

class CDlgExportArcgisMdb : public CDialog
{
// Construction
public:
	CDlgExportArcgisMdb(CWnd* pParent = NULL);   // standard constructor
	CDlgExportArcgisMdb(UINT nIDTemplate, CWnd* pParentWnd = NULL)
		:CDialog(nIDTemplate, pParentWnd){}

// Dialog Data
	//{{AFX_DATA(CDlgExportArcgisMdb)
	enum { IDD = IDD_EXPORT_ARCGISMDB };
	CString	m_strFilePath;//���·��
	CString m_strFileName;//����ļ���
	CString m_strFileDir;//���Ŀ¼

	CString	m_strCodeList;//����ձ�
	CString	m_strTemplateFile;//�ο�ģ��
	CString m_strPrjFile;//����ϵ�ļ�
	CString m_strvPrjFile;//��ֱ����ϵ�ļ�

	BOOL	m_bBreakParallel;//ƽ���ߴ�ɢ
	BOOL    m_bGDB;
	BOOL    m_bReserveSurface;//������
	BOOL	m_bUseIndex; //ʹ��������
	BOOL	m_bCollect;		//�ɼ�
	BOOL	m_bMark;		//����
	BOOL	m_bEDB;			//���
	BOOL	m_bXAttrOnly;	//ֻ������չ����
	BOOL    m_bExportPointZ;//������߳�
	BOOL    m_bExportCurveZ;//�����߸߳�
	BOOL    m_bExportSurfaceZ;//������߳�
	BOOL    m_bClockWiseAngle;//�Ƕȱ�˳ʱ��
	BOOL    m_bExportEmptyLayer;//�����ղ�
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportArcgisMdb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CArcgisMdbCodeTab m_codeTable;

	// Generated message map functions
	//{{AFX_MSG(CDlgExportArcgisMdb)
	afx_msg void OnButtonIdBrowse();
	afx_msg void OnButtonEdBrowse2();
	virtual void OnOK();
	afx_msg void OnSetfocusEditEdFilepath2();
	afx_msg void OnKillfocusEditEdFilepath2();
	afx_msg void OnButtonBrowse3();
	afx_msg void OnButtonBrowse4();
	afx_msg void OnButtonBrowse5();
	afx_msg void OnSetfocusEditEdFilepath3();
	afx_msg void OnKillfocusEditEdFilepath3();
	afx_msg void OnSetfocusEditEdFilepath4();
	afx_msg void OnKillfocusEditEdFilepath4();
	afx_msg void OnSetfocusEditEdFilepath5();
	afx_msg void OnKillfocusEditEdFilepath5();
	afx_msg void OnCheckGDB();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDlgExportArcgisMdb_Batch dialog

class CDlgExportArcgisMdb_Batch : public CDlgExportArcgisMdb
{
	// Construction
public:
	CDlgExportArcgisMdb_Batch(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgExportArcgisMdb_Batch)
	enum { IDD = IDD_EXPORT_ARCGISMDB_BATCH };

	CString m_strFdbFiles;     //fdb�ļ�
	BOOL    m_bNonArcgis;   // ���������ܽ���ArcGIS������
	//}}AFX_DATA

	CStringArray m_arrFdbStr;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportArcgisMdb)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgExportArcgisMdb_Batch)
	afx_msg void OnBrowseFdb();
	afx_msg void OnBrowseFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTARCGISMDB_H__5C8B4A8A_7111_4FFB_95B9_432CC24C7984__INCLUDED_)
