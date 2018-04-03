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

//通过字体名查找字体文件
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

	// 记录添加的地物，在图元浏览中新增图元时使用
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

		int index;			//子符号编号（-1 表示没有填写子符号，只考虑地物整体；0 表示母线，有些层没有符号，可以将母线导出且带上实线符号；
							// >=1表示子符号编号）
		int type;			//输出类型（0自动判断，1块，2线型，3填充）
		char layer[100];	//dwg中的层名
		char name[100];		//dwg中的符号名，为*时，表示该子符号项不导出
		char cassCode[32];	//cass中的实体编码
	};

	typedef CArray<SubSymbolItem,SubSymbolItem> SubSymbols;

	CDxfWrite();
	~CDxfWrite();

	BOOL OpenWrite(LPCTSTR fileName);
	BOOL CloseWrite();
	//更新层
	BOOL SaveFtrLayer(CFtrLayer *pLayer);
	BOOL SaveFeature(CFeature *pFt, BOOL bDem=FALSE, LPCTSTR layName=NULL);

	//用在生成DEM时
	BOOL SilenceExportDxf(CDlgDoc *pDoc,CString pathName);

	//工作区导出菜单下的DXF/DWG导出
	BOOL OnExportDxf();

	//工具菜单下的批量导出
	BOOL OnExportDxf_Batch();

	void ObjectsToDxf(CPtrArray& arrFtrs, CPtrArray &arrLayer, LPCTSTR strDxf);
	void ObjectsToDwg(CPtrArray& arrFtrs, CPtrArray &arrLayer, LPCTSTR strDxf);
	void GrBuffer2dToDxf(GrBuffer2d *buf,LPCTSTR strDxf);

	void SetDlgDataSource(CDlgDataSource *pDS);

	//修改输出的设置为导出母线的方式
	void SetAsExportBaselines();
	void SetAsExportSymbols();

protected:
	void ObjectsToDxf0(CPtrArray& arr, CPtrArray &arrLayer, LPCTSTR strDxf);
	BOOL SaveLayer(LPCTSTR layName, COLORREF color, int lineWeight);

	//这几个函数只保存地物，可能会引用到符号名，但不生成CAD符号
	void SavePoint(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR blkName=NULL, LPCTSTR cassCode=NULL);
	void SaveCurveOrSurface(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR linetype_name=NULL, float wid=-1, float thickness=-1, LPCTSTR cassCode=NULL);
	void SaveText(CGeoText *pGeoObj, LPCTSTR dxfLayerName,LPCTSTR cassCode=NULL);
	void SaveText_core(CGeoText *pGeoObj, LPCTSTR dxfLayerName,LPCTSTR cassCode=NULL);  //SaveText是入口函数， SaveText调用 SaveText_core
	void SaveHatch(IAcadBlock *pBlk, CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR linetype_name=NULL, float wid=-1, float thickness=-1, LPCTSTR cassCode=NULL);
	void SaveDCurveOrParallel(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR linetype_name=NULL, float wid=-1, float thickness=-1, LPCTSTR cassCode=NULL);

	//生成块，如果块已经存在，就插入其引用点
	void SetBlock(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, BOOL bNewBlock, LPCTSTR blkName=NULL, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	
	void SetBlock_dwg(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, BOOL bNewBlock, LPCTSTR blkName=NULL, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	

	//不生成块，生成散的地物时，使用下面两个函数
	void SetBlock_exploded(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	
	void SetBlock_exploded_dwg(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, CFtrLayer *pLayer=NULL, LPCTSTR cassCode=NULL);	

	//保存地物，同时生成CAD符号
	void SaveCurveSubSymbol(CFeature *pFtr, SubSymbolItem *pItem, CSymbolArray& arrPSyms);
	void SavePointSubSymbol(CFeature *pFtr, SubSymbolItem *pItem, CSymbolArray& arrPSyms);

	BOOL AddSolidLinetype(LPCTSTR name);
	BOOL AddDashLinetype(LPCTSTR name,CDashLinetype *pLT);
	CString AddTextStyle(LPCTSTR name,LPCTSTR font,float fWidthFactor, int inclinedtype, float fAngle);

	//由于CAD中的块参照不能有不同的颜色，所以颜色不同时，块名也应不同；
	CString GetExportedBlockName(CFtrLayer *pLayer, long color_ftr);
	CString GetExportedLayerName(LPCTSTR name, int nSymIndex);	

	//从层对照表中提取要输出的子符号信息
	void GetExportedSubsymbols(LPCTSTR layname, SubSymbols& arr);
	int  GetValidNumOfSubsymbols(SubSymbols& arr);

	void GetSymbols(CFeature *pFtr, CSymbolArray& arrAllSyms, CSymbolArray& arrAnnoSyms, CSymbolArray& arrNotAnnoSyms);
	void KickoffSymbols(CFeature *pFtr, CSymbolArray& arrPSyms, SubSymbols& arr);
	void ClassifySymbols(CSymbolArray& arrAllSyms, CSymbolArray& arrAnnoSyms, CSymbolArray& arrNotAnnoSyms);
	long ConvertColor(COLORREF clr, CFtrLayer *pLayer);

	void AddDefaultLayer_dxf();

	BOOL IsSolidLineSymbol(CSymbolArray* arrPSyms);		//判断是否为实线符号

public:
	CDlgExportDxf	m_dlgDxfExport;

private:
	CString			m_strFileName;
	CDxfDrawing		m_DxfDraw;
	CMultiListFile	m_lstFile;
	CListFileEx     m_lstFileEx;//颜色对照表	
	CDlgDataSource	*m_pDS;
	int				 m_nBlockNo;

	//以下变量供SaveFtrLayer()和SaveFeature()使用
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

	//这两个变量只是缓存 GetExportedSubsymbols 的结果，加快执行速度
	SubSymbols		  m_SubSymbols;
	CString			  m_FdbLayerNameOfSubSymbols;

	CFontFileFind	  m_fontFileFinder;

	CStringArray	  m_arrNewBlockNames;
	CStringArray	  m_arrDWGBlockNames;
};


#endif // !defined(AFX_DXFACCESS_H__F7810F9B_9B46_4146_9D90_8F9DE01DFC27__INCLUDED_)