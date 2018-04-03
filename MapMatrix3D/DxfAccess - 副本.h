// DxfAccess.h: interface for the CDxfRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXFACCESS_H__F7810F9B_9B46_4146_9D90_8F9DE01DFC27__INCLUDED_)
#define AFX_DXFACCESS_H__F7810F9B_9B46_4146_9D90_8F9DE01DFC27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CadLib.h"
#include "ListFile.h"
#include "Access.h"
#include "DlgImportDxf.h"
#include "DlgExportDxf.h"
#include "UndoAction.h"
#include "DlgDataSource.h"

class CDlgDataSource;
class CDlgDoc;

#import "../core/DwgIO/DwgIO/Release/DwgIO.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids

//ͨ�����������������ļ�
class CFontFileFind
{
public:
	CFontFileFind();
	~CFontFileFind();
	
	CString FindFontFile(LPCTSTR font);
	CString FindFontName(LPCTSTR file);
	
private:
	void Load();
	void CutTailChars(char *buf);
	
	BOOL m_bLoad;
	CStringArray m_fontList;
	CStringArray m_fileList;
};



class CDxfRead 
{
public:
	CDxfRead(CDlgDoc *pDoc=NULL, BOOL bUndo=FALSE);
	~CDxfRead();

	BOOL OpenRead(CString dxfPath);
	BOOL CloseRead();

	BOOL OnImportDxf(CDlgDataSource *pDS, BOOL bSilence=FALSE, CString path="");

	BOOL OnImportMultiDxf(CDlgDataSource *pDS);
	BOOL ConvertDxf2FDB(CScheme* pScheme, CStringArray& org_path, CString& des_folder);

	int GetFtrLayers(CPtrArray &FtrLays);
	BOOL ReadFeatures();

	void ImportLidarDxf();
protected:
	IAcadLayer* FindDwgLayer(CString name);

	void ReadBlocks_dwg();
	void ReadBlocks_dxf();
	BOOL ReadFeatures_dwg();
	CString ReadTextFontName_dxf(OBJHANDLE hStyle);
	CString ReadTextFontName_dwg(BSTR styleName);

	void ConvertCADSpline(CArray<PT_3DEX,PT_3DEX>& arrPts);

private:
	CString			m_strFileName;
	CDxfDrawing		m_DxfDraw;
	CMultiListFile	m_lstFile;

	CString			m_CurLayerName;
	CDlgImportDxf	m_dlgDxfImport;
	CDlgDataSource	*m_pDS;

	// ��¼��ӵĵ����ͼԪ���������ͼԪʱʹ��
	CPtrArray       m_arrFtrs;

	//dwg
#ifdef _WIN64
	IDwgReadPtr m_dwg;
#else
	IOdaHostApp      *m_iHost;
	IAcadApplication *m_iApplication;
    IAcadDocument    *m_pCurDocument;
	IAcadDatabase   *m_pDb;
	IAcadModelSpace *m_pMS;
	IAcadLayers		*m_pLayers;
#endif // _WIN64

	BOOL              m_bImportDWG;
	CString           m_curLayerName;

	CUndoFtrs       *m_pUndo;

	CDlgDoc			*m_pDoc;

	CStringArray	m_arrSysFontNames;

	CFontFileFind   m_fontFileFinder;

};


class CDxfWrite  
{
public:
	struct SubSymbolItem
	{
		SubSymbolItem();

		int index;			//�ӷ��ű�ţ�-1 ��ʾû����д�ӷ��ţ�ֻ���ǵ������壻0 ��ʾĸ�ߣ���Щ��û�з��ţ����Խ�ĸ�ߵ����Ҵ���ʵ�߷��ţ�
							// >=1��ʾ�ӷ��ű�ţ�
		int type;			//������ͣ�0�Զ��жϣ�1�飬2���ͣ�3��䣩
		char layer[100];	//dwg�еĲ���
		char name[100];		//dwg�еķ�������Ϊ*ʱ����ʾ���ӷ��������
		char cassCode[32];	//cass�е�ʵ�����
	};

	typedef CArray<SubSymbolItem,SubSymbolItem> SubSymbols;

	CDxfWrite();
	~CDxfWrite();

	BOOL OpenWrite(LPCTSTR fileName);
	BOOL CloseWrite();
	//���²�
	BOOL SaveFtrLayer(CFtrLayer *pLayer);
	BOOL SaveFeature(CFeature *pFt, BOOL bDem=FALSE, LPCTSTR layName=NULL);

	//��������DEMʱ
	BOOL SilenceExportDxf(CDlgDoc *pDoc,CString pathName);

	//�����������˵��µ�DXF/DWG����
	BOOL OnExportDxf();

	//���߲˵��µ���������
	BOOL OnExportDxf_Batch();

	void ObjectsToDxf(CPtrArray& arrFtrs, CPtrArray &arrLayer, LPCTSTR strDxf);
	void ObjectsToDwg(CPtrArray& arrFtrs, CPtrArray &arrLayer, LPCTSTR strDxf);
	void GrBuffer2dToDxf(GrBuffer2d *buf,LPCTSTR strDxf);

	void SetDlgDataSource(CDlgDataSource *pDS);

	//�޸����������Ϊ����ĸ�ߵķ�ʽ
	void SetAsExportBaselines();
	void SetAsExportSymbols();

protected:
	void ObjectsToDxf0(CPtrArray& arr, CPtrArray &arrLayer, LPCTSTR strDxf);
	BOOL SaveLayer(LPCTSTR layName, COLORREF color, int lineWeight);

	//�⼸������ֻ���������ܻ����õ�����������������CAD����
	void SavePoint(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR blkName=NULL, LPCTSTR cassCode=NULL);
	void SaveCurveOrSurface(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR linetype_name=NULL, float wid=-1, float thickness=-1, LPCTSTR cassCode=NULL);
	void SaveText(CGeoText *pGeoObj, LPCTSTR dxfLayerName,LPCTSTR cassCode=NULL);
	void SaveText_core(CGeoText *pGeoObj, LPCTSTR dxfLayerName,LPCTSTR cassCode=NULL);  //SaveText����ں����� SaveText���� SaveText_core
	void SaveHatch(IAcadBlock *pBlk, CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR linetype_name=NULL, float wid=-1, float thickness=-1, LPCTSTR cassCode=NULL);
	void SaveDCurveOrParallel(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR linetype_name=NULL, float wid=-1, float thickness=-1, LPCTSTR cassCode=NULL);

	//���ɿ飬������Ѿ����ڣ��Ͳ��������õ�
	void SetBlock(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, BOOL bNewBlock, LPCTSTR blkName=NULL, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	
	void SetBlock_dwg(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, BOOL bNewBlock, LPCTSTR blkName=NULL, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	

	//�����ɿ飬����ɢ�ĵ���ʱ��ʹ��������������
	void SetBlock_exploded(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	
	void SetBlock_exploded_dwg(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	

	//������ͬʱ����CAD����
	void SaveCurveSubSymbol(CFeature *pFtr, SubSymbolItem *pItem, CSymbolArray& arrPSyms);
	void SavePointSubSymbol(CFeature *pFtr, SubSymbolItem *pItem, CSymbolArray& arrPSyms);

	BOOL AddSolidLinetype(LPCTSTR name);
	BOOL AddDashLinetype(LPCTSTR name,CDashLinetype *pLT);
	CString AddTextStyle(LPCTSTR name,LPCTSTR font,float fWidthFactor, int inclinedtype, float fAngle);

	//����CAD�еĿ���ղ����в�ͬ����ɫ��������ɫ��ͬʱ������ҲӦ��ͬ��
	CString GetExportedBlockName(CFtrLayer *pLayer, long color_ftr);
	CString GetExportedLayerName(LPCTSTR name, int nSymIndex);	

	//�Ӳ���ձ�����ȡҪ������ӷ�����Ϣ
	void GetExportedSubsymbols(LPCTSTR layname, SubSymbols& arr);
	int  GetValidNumOfSubsymbols(SubSymbols& arr);

	void GetSymbols(CFeature *pFtr, CSymbolArray& arrAllSyms, CSymbolArray& arrAnnoSyms, CSymbolArray& arrNotAnnoSyms);
	void KickoffSymbols(CFeature *pFtr, CSymbolArray& arrPSyms, SubSymbols& arr);
	void ClassifySymbols(CSymbolArray& arrAllSyms, CSymbolArray& arrAnnoSyms, CSymbolArray& arrNotAnnoSyms);
	long ConvertColor(COLORREF clr, CFtrLayer *pLayer);

	void AddDefaultLayer_dxf();

	BOOL IsSolidLineSymbol(CSymbolArray* arrPSyms);		//�ж��Ƿ�Ϊʵ�߷���

public:
	CDlgExportDxf	m_dlgDxfExport;

private:
	CString			m_strFileName;
	CDxfDrawing		m_DxfDraw;
	CMultiListFile	m_lstFile;
	CListFileEx     m_lstFileEx;//��ɫ���ձ�	
	CDlgDataSource	*m_pDS;
	int				 m_nBlockNo;

	//���±�����SaveFtrLayer()��SaveFeature()ʹ��
	OBJHANDLE		m_hDefLinetype;
	OBJHANDLE		m_hDefLayer;

	CString m_docName;

	//dwg
	IOdaHostApp      *m_iHost;
	IAcadApplication *m_iApplication;
    IAcadDocument    *m_pCurDocument;
	IAcadDatabase   *m_pDb;
	IAcadModelSpace *m_pMS;
	BOOL              m_bExportDWG;

	CString           m_curFdbLayerName;
	CFtrLayer		 *m_pCurFdbLayer;

	//����������ֻ�ǻ��� GetExportedSubsymbols �Ľ�����ӿ�ִ���ٶ�
	SubSymbols		  m_SubSymbols;
	CString			  m_FdbLayerNameOfSubSymbols;

	CFontFileFind	  m_fontFileFinder;

	CStringArray	  m_arrNewBlockNames;
	CStringArray	  m_arrDWGBlockNames;
};


#endif // !defined(AFX_DXFACCESS_H__F7810F9B_9B46_4146_9D90_8F9DE01DFC27__INCLUDED_)