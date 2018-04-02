// SymbolLib.cpp: implementation of the CSymbolLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
#include "resource.h"
#include "SymbolLib.h"
#include "Markup.h"
#include "ObjectXmlIO.h"
#include "FtrLayer.h"
#include "textfile.h"
#include "DataSourceEx.h"
#include "GeoParallel.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlaceConfig::CPlaceConfig()
{
	memset(m_strLayName,0,64);
	m_nCommandID = 0;
	memset(m_strCmdParams,0,1024);
	memset(m_strViewParams,0,128);
}


CPlaceConfig::~CPlaceConfig()
{
}

CPlaceConfig& CPlaceConfig::operator=(const CPlaceConfig &config)
{
	_tcscpy(m_strLayName,config.m_strLayName);
	m_nCommandID = config.m_nCommandID;
	_tcscpy(m_strCmdParams,config.m_strCmdParams);
	_tcscpy(m_strViewParams,config.m_strViewParams);
	return *this;
}

BOOL CPlaceConfig::WriteTo(CString &strXml)
{
	CMarkup xmlFile;
	xmlFile.AddElem(XMLTAG_PLACEPARAM);
	xmlFile.IntoElem();

	xmlFile.AddElem(XMLTAG_LAYERNAME,m_strLayName);

	CString strCmdId;
	strCmdId.Format(_T("%d"),m_nCommandID);
	xmlFile.AddElem(XMLTAG_COMMANDID,strCmdId);

	xmlFile.AddElem(XMLTAG_CMDPARAMS);
	xmlFile.IntoElem();
	xmlFile.AddSubDoc(m_strCmdParams);
	xmlFile.OutOfElem();

	xmlFile.AddElem(XMLTAG_VIEWPARAMS,m_strViewParams);

	xmlFile.OutOfElem();

	strXml = xmlFile.GetDoc();

	return TRUE;
}


BOOL CPlaceConfig::ReadFrom(CString &strXml)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXml);

// 	if( !xmlFile.FindElem(XMLTAG_ACTION,TRUE) )
// 		return FALSE;
// 
// 	xmlFile.IntoElem();
	
	CString tag,data;
	if ( xmlFile.FindElem(XMLTAG_PLACEPARAM) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_LAYERNAME) )
		{
			data = xmlFile.GetData();
			_tcscpy(m_strLayName,data);
		}
		if( xmlFile.FindElem(XMLTAG_COMMANDID) )
		{
			data = xmlFile.GetData();
			m_nCommandID = _ttoi(data);
/*
			if (m_nCommandID > ID_ELE_START && m_nCommandID < ID_ELE_END)
			{
				m_nCommandID -= 10074;
			}*/
		}
		if( xmlFile.FindElem(XMLTAG_CMDPARAMS) )
		{
			xmlFile.IntoElem();
			xmlFile.FindElem(NULL);
			data = xmlFile.GetSubDoc();
			_tcscpy(m_strCmdParams,data);
			xmlFile.OutOfElem();
		}
		if( xmlFile.FindElem(XMLTAG_VIEWPARAMS) )
		{
			data = xmlFile.GetData();
			_tcscpy(m_strViewParams,data);
			xmlFile.OutOfElem();
		}		

		xmlFile.OutOfElem();
	}

	xmlFile.OutOfElem();
	

	return TRUE;
}
//hcw,2012.7.31.
void CPlaceConfig::Copy( CPlaceConfig const& config )
{
	_tcscpy(this->m_strLayName,config.m_strLayName);
	this->m_nCommandID = config.m_nCommandID;
	_tcscpy(this->m_strCmdParams,config.m_strCmdParams);
	_tcscpy(this->m_strViewParams,config.m_strViewParams);
	return;
}

CCmdXMLParams::CCmdXMLParams()
{
	m_nCommandID = 0;
	memset(m_strCmdParams,0,1024);	
}

BOOL CCmdXMLParams::WriteTo(CString &strXml)
{
	CMarkup xmlFile;
	xmlFile.AddElem(XMLTAG_COMMANDITEM);
	xmlFile.IntoElem();
	
	CString strCmdId;
	strCmdId.Format(_T("%d"),m_nCommandID);
	xmlFile.AddElem(XMLTAG_COMMANDID,strCmdId);
	
	xmlFile.AddElem(XMLTAG_CMDPARAMS);
	xmlFile.IntoElem();
	xmlFile.AddSubDoc((LPCTSTR)m_strCmdParams);
	xmlFile.OutOfElem();
	
	xmlFile.OutOfElem();
	
	strXml = xmlFile.GetDoc();
	
	return TRUE;
}


BOOL CCmdXMLParams::ReadFrom(CString &strXml)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXml);
	
	CString tag,data;
	if ( xmlFile.FindElem(XMLTAG_COMMANDITEM) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_COMMANDID) )
		{
			data = xmlFile.GetData();
			m_nCommandID = _ttoi(data);
/*
			if (m_nCommandID > ID_ELE_START && m_nCommandID < ID_ELE_END)
			{
				m_nCommandID -= 10074;
			}*/
		}
		if( xmlFile.FindElem(XMLTAG_CMDPARAMS) )
		{
			xmlFile.IntoElem();
			xmlFile.FindElem(NULL);
			data = xmlFile.GetSubDoc();
			_tcscpy(m_strCmdParams,data);
			xmlFile.OutOfElem();
		}
		
		xmlFile.OutOfElem();
	}
	
	xmlFile.OutOfElem();
	
	
	return TRUE;
}
//hcw,2012.7.31.
void CCmdXMLParams::Copy( CCmdXMLParams const& cmdParams )
{
	this->m_nCommandID = cmdParams.m_nCommandID;
	_tcscpy(this->m_strCmdParams, cmdParams.m_strCmdParams);
	return;
}


ConfigLibItem::ConfigLibItem(){
	scale = 0;
	pPlaceCfgLib = NULL;
	pScheme = NULL;
	pCellDefLib = NULL;
	pLinetypeLib = NULL;
	bLoaded = FALSE;
}


void ConfigLibItem::Create() 
{
	pPlaceCfgLib = new CPlaceConfigLib;
	pScheme = new CScheme;
	pCellDefLib = new CCellDefLib;
	pLinetypeLib = new CBaseLineTypeLib;
}

ConfigLibItem& ConfigLibItem::operator=(const ConfigLibItem &config)
{
	scale = config.scale;
	m_strPath = config.m_strPath;
	bLoaded = config.bLoaded;
	pPlaceCfgLib = config.pPlaceCfgLib;
	pScheme = config.pScheme;
	pCellDefLib = config.pCellDefLib;
	pLinetypeLib = config.pLinetypeLib;
	return *this;
}
//hcw,2012.7.27.
void ConfigLibItem::Copy(ConfigLibItem &config)
{

	this->scale = config.scale;
	this->m_strPath = config.m_strPath;
	this->bLoaded = config.bLoaded;
	
	//{hcw,2012.8.6,Modified.
	if (config.pPlaceCfgLib) 
	{
		if(!this->pPlaceCfgLib) 
		{
			this->pPlaceCfgLib = (CPlaceConfigLib *)new CPlaceConfig();
		}
		this->pPlaceCfgLib->Copy(*config.pPlaceCfgLib);

	}
	else
	{
		if (this->pPlaceCfgLib)
		{
			delete this->pPlaceCfgLib;
			this->pPlaceCfgLib = NULL;
		}
	}		 
    if (config.pScheme)
    {
		if(!this->pScheme)
		{
			this->pScheme = (CScheme*)new CScheme();	
		}
		*this->pScheme = *config.pScheme;	
    }
	else
	{
		if (this->pScheme)
		{
			 delete this->pScheme;
			 this->pScheme = NULL;
		}
	}
	if (config.pCellDefLib)
	{
		if(!this->pCellDefLib)
		{
			this->pCellDefLib = (CCellDefLib *)new CCellDefLib();
		}
		this->pCellDefLib->Clear();
		for (int nCellDefIndex=0; nCellDefIndex<config.pCellDefLib->GetCellDefCount(); nCellDefIndex++)
		{

			CellDef originalCellDef = config.pCellDefLib->GetCellDef(nCellDefIndex);
			CellDef tmpCellDef;
			tmpCellDef.CopyFrom(originalCellDef);
			this->pCellDefLib->AddCellDef(tmpCellDef);
		}
	}
	else
	{
		if (this->pCellDefLib)
		{
			delete this->pCellDefLib;
			this->pCellDefLib = NULL;
		}

	}

	if (config.pLinetypeLib)
	{
		if(!this->pLinetypeLib)
		{
			this->pLinetypeLib = (CBaseLineTypeLib*)new BaseLineType();
		}
		this->pLinetypeLib->Clear();
		for(int nLTypeIndex=0; nLTypeIndex<config.pLinetypeLib->GetBaseLineTypeCount();nLTypeIndex++)
		{
			BaseLineType tmpLineTypeDef = config.pLinetypeLib->GetBaseLineType(nLTypeIndex);
			this->pLinetypeLib->AddBaseLineType(tmpLineTypeDef);
		}
	}
	else
	{
		if (this->pLinetypeLib)
		{
			delete this->pLinetypeLib;
			this->pLinetypeLib = NULL;
		}
	}
	//}
	return;
}



void ConfigLibItem::Clear()
{
	if(pPlaceCfgLib)
	{
		delete pPlaceCfgLib;
		pPlaceCfgLib = NULL;
	}
	if(pScheme)
	{
		delete pScheme;
		pScheme = NULL;
	}
	if(pCellDefLib)
	{
		delete pCellDefLib;
		pCellDefLib = NULL;
	}
	if(pLinetypeLib)
	{
		delete pLinetypeLib;
		pLinetypeLib = NULL;
	}
	bLoaded = FALSE;
}

BOOL ConfigLibItem::SetScalePath(CString strPath)
{
	m_strPath = strPath;
	int pos = strPath.ReverseFind('\\');
	if (pos < 0) return FALSE;
	scale = _ttoi(strPath.Right(strPath.GetLength()-pos-1));
	return TRUE;

}

BOOL ConfigLibItem::Load()
{
	if (bLoaded)  return TRUE;
	
	Create();
	bLoaded = TRUE;
	CString strCellPath = m_strPath + _T("\\symbol.xml");
	
	LoadCellLine(strCellPath);
	
	CString strDoc;
	CString strSchemePath = m_strPath + _T("\\scheme.xml");
	CTextFileRead rrfile(strSchemePath);
	rrfile.Read(strDoc);
	pScheme->SetPath(strSchemePath);
	pScheme->ReadFrom(strDoc);
	pScheme->LoadCADSymols();
	
	CString strPlacePath = m_strPath + _T("\\action.xml");
	pPlaceCfgLib->Load(strPlacePath);
	
	// 纠正action.xml中的无效命令id
	BOOL bModify = FALSE;
	for (int i=0; i<pPlaceCfgLib->GetConfigCount(); i++)
	{
		CPlaceConfig *pConfig = pPlaceCfgLib->GetConfig(i);
		if (!pConfig) continue;
		CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(pConfig->m_strLayName);
		if (!pLayer)
		{
			// 方案中无对应层时缺省为点命令
			if (pConfig->m_nCommandID<=ID_ELE_START || pConfig->m_nCommandID>=ID_ELE_END)
			{
				pConfig->m_nCommandID = ID_ELEMENT_DOT_DOT;
				//				conLibItem.pPlaceCfgLib->SetConfig(config.m_strLayName, config);
			}
			continue;
		}
		
		if (pConfig->m_nCommandID<=ID_ELE_START || pConfig->m_nCommandID>=ID_ELE_END)
		{
			bModify = TRUE;
			int nGeoClass = pLayer->GetGeoClass();
			if (nGeoClass == CLS_GEOPOINT)
			{
				pConfig->m_nCommandID = ID_ELEMENT_DOT_DOT;
			}
			else if (nGeoClass == CLS_GEODIRPOINT)
			{
				pConfig->m_nCommandID = ID_ELEMENT_DOT_VECTORDOT;
			}
			else if (nGeoClass == CLS_GEOCURVE)
			{
				pConfig->m_nCommandID = ID_ELEMENT_LINE_LINE;
			}
			else if (nGeoClass == CLS_GEODCURVE)
			{
				pConfig->m_nCommandID = ID_ELEMENT_LINE_DLINE;
			}
			else if (nGeoClass == CLS_GEOPARALLEL)
			{
				pConfig->m_nCommandID = ID_ELEMENT_LINE_PARALLEL;
			}
			else if (nGeoClass == CLS_GEOSURFACE)
			{
				pConfig->m_nCommandID = ID_ELEMENT_FACE_FACE;
			}
			else if (nGeoClass == CLS_GEOTEXT)
			{
				pConfig->m_nCommandID = ID_ELEMENT_TEXT;
			}
			
			//			conLibItem.pPlaceCfgLib->SetConfig(pConfig->m_strLayName, config);
			
		}
	}
	if (bModify)
	{
		pPlaceCfgLib->Save();
	}

	return TRUE;
}

BOOL ConfigLibItem::LoadCellLine(CString strPath)
{
	if (!strPath.IsEmpty())
	{
		CString pathb = strPath.Left(strPath.GetLength()-3) + "bin";
		
		pCellDefLib->SetPath(pathb);
		pLinetypeLib->SetPath(pathb);

		// 有二进制存储格式优先读取
		CFileFind finder;
		if (!finder.FindFile(pathb) && finder.FindFile(strPath))
		{
			// 兼容模式
			CMarkup rfile;
			rfile.Load(strPath);
			CString strDoc = rfile.GetDoc();
			
			pCellDefLib->ReadFrom(strDoc);
			pLinetypeLib->ReadFrom(strDoc);

			// symbol.bin不存在则读完后立即存储一次
			SaveCellLine();
			
			return TRUE;
		}
		finder.Close();
	}
	
	FILE *stream = _tfopen( LPCTSTR(pCellDefLib->GetPath()), _T("rb") );
	if (stream == NULL ) return FALSE;

	pCellDefLib->Load(stream);
	pLinetypeLib->Load(stream);
	fclose(stream);

	return TRUE;
}

BOOL ConfigLibItem::SaveCellLine(CString path)
{
	if (!path.IsEmpty())
	{
		pCellDefLib->SetPath(path);
		pLinetypeLib->SetPath(path);
	}
	FILE *stream = _tfopen( LPCTSTR(pCellDefLib->GetPath()), _T("w+b") );
	if (stream == NULL ) return FALSE;

	pCellDefLib->Save(stream);
	pLinetypeLib->Save(stream);

	fclose(stream);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlaceConfigLib::CPlaceConfigLib()
{
}



CPlaceConfigLib::~CPlaceConfigLib()
{
	Clear();
}

void CPlaceConfigLib::Clear()
{
	m_arrItems.RemoveAll();
}

BOOL CPlaceConfigLib::Load(LPCTSTR pathname)
{
	Clear();
	
	m_strCfgPath = pathname;
	
	CMarkup xmlFile;

// 	if( !Xml_ReadTextFile(xmlFile,pathname,FALSE) )
// 		return FALSE;

	xmlFile.Load(m_strCfgPath);
	CString strDoc = xmlFile.GetDoc();
// 	CTextFileRead rfile(m_strCfgPath);
// 	rfile.Read(strDoc);
// 	xmlFile.SetDoc(strDoc);

	if( xmlFile.FindElem(XMLTAG_ACTION,TRUE) )
	{

		xmlFile.IntoElem();

		CString data;
		while ( xmlFile.FindElem(XMLTAG_PLACEPARAM) )
		{
			data = xmlFile.GetSubDoc();
			CPlaceConfig configitem;
			configitem.ReadFrom(data);
			m_arrItems.Add(configitem);

		}

		if( xmlFile.FindElem(XMLTAG_COMMAND,TRUE) )
		{
			xmlFile.IntoElem();
			
			CString data;
			while ( xmlFile.FindElem(XMLTAG_COMMANDITEM) )
			{
				data = xmlFile.GetSubDoc();
				CCmdXMLParams item;
				item.ReadFrom(data);
				m_arrCmdParams.Add(item);
				
			}
			
			xmlFile.OutOfElem();
		}

		xmlFile.OutOfElem();
	}

//	Save();
	
	return TRUE;
}


void CPlaceConfigLib::AddCmdXMLParams(int id, LPCTSTR params)
{
	CCmdXMLParams *p = GetCmdXMLParams(id);
	if( p==NULL )
	{
		CCmdXMLParams item;
		item.m_nCommandID = id;
		_tcsncpy(item.m_strCmdParams,params,sizeof(item.m_strCmdParams));
		m_arrCmdParams.Add(item);
	}
	else
	{
		p->m_nCommandID = id;
		_tcsncpy(p->m_strCmdParams,params,sizeof(p->m_strCmdParams));
	}
}

BOOL CPlaceConfigLib::Save(LPCTSTR pathname)const
{
	CString path = m_strCfgPath;
	if (pathname != NULL)
	{
		path = pathname;
	}
	int nsz = m_arrItems.GetSize();
	if( nsz<=0 )return TRUE;

	CMarkup xmlFile;

	xmlFile.AddElem(XMLTAG_ACTION);
	xmlFile.IntoElem();
	
	CString data;
	for (int i=0; i<nsz; i++)
	{
		CPlaceConfig config = m_arrItems.GetAt(i);
		config.WriteTo(data);
		xmlFile.AddSubDoc(data);
	}

	xmlFile.AddElem(XMLTAG_COMMAND);
	xmlFile.IntoElem();
	
	nsz = m_arrCmdParams.GetSize();
	for (i=0; i<nsz; i++)
	{
		CCmdXMLParams item = m_arrCmdParams.GetAt(i);
		item.WriteTo(data);
		xmlFile.AddSubDoc(data);
	}
	
	xmlFile.OutOfElem();
	
	xmlFile.OutOfElem();

//	Xml_WriteTextFile(xmlFile,path,FALSE);

	xmlFile.Save(path);
// 	CString strDoc = xmlFile.GetDoc();
// 	CTextFileWrite wfile(path, CTextFileBase::UNI16_LE);
// 	if (wfile.IsOpen())
// 	{
// 		wfile.SetCodePage(0);
// 		wfile<<strDoc;
// 	}

	return TRUE;
}

BOOL CPlaceConfigLib::AddConfig(const CPlaceConfig &config)
{
	m_arrItems.Add(config);
	return TRUE;
}

CPlaceConfig* CPlaceConfigLib::GetConfig(LPCTSTR strLayName)
{
	BOOL bFind = FALSE;
	for (int i=0; i<m_arrItems.GetSize(); i++)
	{
		if (_tcsicmp(m_arrItems.GetAt(i).m_strLayName,strLayName) == 0)
		{	
			bFind = TRUE;
			break;
		}
	}
	if (i > m_arrItems.GetSize()-1)
		return NULL;

	return &m_arrItems.ElementAt(i);
	
}

CCmdXMLParams* CPlaceConfigLib::GetCmdXMLParams(int id)
{
	BOOL bFind = FALSE;
	for (int i=0; i<m_arrCmdParams.GetSize(); i++)
	{
		if (m_arrCmdParams.GetAt(i).m_nCommandID==id)
		{	
			bFind = TRUE;
			break;
		}
	}
	if (i > m_arrCmdParams.GetSize()-1)
		return NULL;
	
	return &m_arrCmdParams.ElementAt(i);
}

CPlaceConfig* CPlaceConfigLib::GetConfig(int i)
{
	if (i>=0 && i<m_arrItems.GetSize())
	{
		return &m_arrItems.ElementAt(i); 
	}
	return NULL; 
}
//hcw,2012.7.31.
void CPlaceConfigLib::Copy( CPlaceConfigLib const& config )
{
	this->m_strCfgPath = config.m_strCfgPath;
	this->m_arrItems.RemoveAll();
	for (int i=0; i<m_arrItems.GetSize(); i++)
	{
		this->m_arrItems.Add(config.m_arrItems[i]);
	}
	this->m_arrCmdParams.RemoveAll();
	for (int j=0; j<m_arrCmdParams.GetSize();j++)
	{
		this->m_arrCmdParams.Add(config.m_arrCmdParams[j]);
	}
	return;
}

BOOL CPlaceConfigLib::DeleteConfig(LPCTSTR strLayName)
{
	for (int i=m_arrItems.GetSize()-1; i>=0; i--)
	{
		if (_tcsicmp(m_arrItems.GetAt(i).m_strLayName,strLayName) == 0)
		{	
			m_arrItems.RemoveAt(i);
			return TRUE;
		}
	}
	return FALSE;
}

/*
BOOL CPlaceConfigLib::SetConfig(LPCTSTR strLayName, CPlaceConfig item)
{
	BOOL bModifyOrAdd = FALSE;
	for (int i=0; i<m_arrItems.GetSize(); i++)
	{
		if (stricmp(m_arrItems.GetAt(i).m_strLayName,strLayName) == 0)
		{	
			CPlaceConfig &config = m_arrItems.ElementAt(i);
			strcpy(config.m_strLayName, item.m_strLayName);
			config.m_nCommandID = item.m_nCommandID;
			strcpy(config.m_strCmdParams, item.m_strCmdParams);
			strcpy(config.m_strViewParams, item.m_strViewParams);
			bModifyOrAdd = TRUE;
			break;
		}
	}
	if (!bModifyOrAdd)
	{
		CPlaceConfig config;
		strcpy(config.m_strLayName, item.m_strLayName);
		config.m_nCommandID = item.m_nCommandID;
		strcpy(config.m_strCmdParams, item.m_strCmdParams);
		strcpy(config.m_strViewParams, item.m_strViewParams);
		m_arrItems.Add(config);

	}
	return bModifyOrAdd;
}

*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigLibManager::CConfigLibManager()
{
}


CConfigLibManager::~CConfigLibManager()
{
	Clear();
}

void CConfigLibManager::Clear()
{
	ConfigLibItem item;
	int nsz = m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		item = m_arrItems.GetAt(i);
		item.Clear();
	}
	m_arrItems.RemoveAll();
	m_strPath.Empty();
}


//判断字符串strItem是否为数字
BOOL IsDigital(const CString& strItem)
{
	if (strItem.GetLength() == 0)
		return FALSE;

    for(int i= 0; i<strItem.GetLength(); i++)
    {
        // check"+"、"-"正负号
//         if (i== 0 && (strItem.GetAt(i)==0x2B || strItem.GetAt(i)==0x2D))
//             return FALSE; 

		if (i== 0 && (strItem.GetAt(i)!=0x2B && strItem.GetAt(i)!=0x2D && !isdigit(strItem.GetAt(i))))
            return FALSE; 

		if (i == 0)  continue;
		
        // check char
        if ( !isdigit(strItem.GetAt(i)) && strItem.GetAt(i)!= L'.')
            return FALSE;
    }
    // check 小数点
    if (strItem.Find(L'.')!= strItem.ReverseFind(L'.'))
        return FALSE;
    return TRUE;
} 

// 检查一个比例尺目录是否有效，包含那三个文件则为有效
BOOL IsValidScaleFile(CString path)
{
	CString strPath = path + "\\scheme.xml";
	CFileFind finder;
	if (!finder.FindFile(strPath)) return FALSE;
	
//  	strPath = path + "\\action.xml";
//  	if (!finder.FindFile(strPath)) return FALSE;
	
	strPath = path + "\\symbol.xml";
	CString strPath1 =  path + "\\symbol.bin";
	if (!finder.FindFile(strPath) && !finder.FindFile(strPath1)) return FALSE;
	finder.Close();
	return TRUE;
    
}

int CConfigLibManager::LoadScale(CString strPath, BOOL bLoaded)
{
//	if (!IsValidScaleFile(strPath)) return -1;

	int pos = -1;
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		if(m_arrItems.GetAt(i).GetPath().CompareNoCase(strPath) == 0)
		{
			pos = i;
			break;
		}
	}
	if (pos < 0)
	{
		ConfigLibItem conLibItem;
		conLibItem.SetScalePath(strPath);
		
		pos = m_arrItems.Add(conLibItem);
	}

	// 不加载数据模式
	if (!bLoaded)
	{
		return pos;		
	}

	// 加载数据模式
	ConfigLibItem &conLibItem = m_arrItems.ElementAt(pos);
	conLibItem.Load();
	return pos;
}

BOOL CConfigLibManager::LoadConfig(LPCTSTR strConfigPath, BOOL bRefConfig)
{
	if (m_strPath.CompareNoCase(strConfigPath) == 0) return TRUE;

	// 备份方案配置
	if (bRefConfig)
	{
		if (m_strBakPath.IsEmpty())
		{
			m_strBakPath = m_strPath;
			m_arrBakItems.Copy(m_arrItems);
			m_strPath.Empty();
			m_arrItems.RemoveAll();
		}
		else
		{
			Clear();
		}
	}
	else
	{
		Clear();
	}

	m_strPath = strConfigPath;

	//检查路径是否有效
	BOOL bValid = FALSE;

	CFileFind finder;
	if (!finder.FindFile( LPCTSTR((CString)strConfigPath)) )
	{
		CreateDirectory(strConfigPath,NULL);
	}

	BOOL bWorking = finder.FindFile( LPCTSTR(m_strPath+"\\*.*") );   
    while(bWorking)   
    {   
        bWorking = finder.FindNextFile(); 
        if(finder.IsDots())   continue;	
		
        if(finder.IsDirectory())	
        { 
			CString strScale = finder.GetFileTitle();
			if( !IsDigital(strScale) )
				continue;

			LoadScale(finder.GetFilePath());		
            
        } 
        
	}

	finder.Close();
	return (m_arrItems.GetSize()>0);
}

void CConfigLibManager::ResetConfig()
{
	if (!m_strBakPath.IsEmpty())
	{
		Clear();

		m_strPath = m_strBakPath;
		m_arrItems.Copy(m_arrBakItems);

		m_strBakPath.Empty();
		m_arrBakItems.RemoveAll();
	}
}

BOOL CConfigLibManager::SaveScale(int scale)
{
	//若为0,保存所有比例尺
	if (scale == 0)
	{
		int ntmpScale;
		for(int i=0; i<m_arrItems.GetSize(); i++)
		{
			ntmpScale = m_arrItems.GetAt(i).GetScale();
			CString strScale;
			strScale.Format(_T("%d"),ntmpScale);
			
			//symbol.xml
//			CString strCellPath = m_strPath + strScale + "\\symbol.xml";
			m_arrItems[i].SaveCellLine();

			//scheme.xml
			m_arrItems[i].pScheme->Save();

			m_arrItems[i].pPlaceCfgLib->Save();
		}
		return TRUE;
		

	}
	else
	{
		for(int i=0; i<m_arrItems.GetSize(); i++)
		{
			if(m_arrItems.GetAt(i).GetScale() == scale)
				break;
		}
		if(i > m_arrItems.GetSize()-1)
			return FALSE;
		CString strScale;
		strScale.Format(_T("%d"),scale);
		
		//symbol.xml
//		CString strCellPath = m_strPath + "\\" + strScale + "\\symbol.xml";
		m_arrItems[i].SaveCellLine();

		//scheme.xml
		m_arrItems[i].pScheme->Save();

		//placeconfig
		m_arrItems[i].pPlaceCfgLib->Save();

		return TRUE;

	}
	

}

DWORD CConfigLibManager::GetSuitedScale(DWORD scale)
{
	CArray<DWORD,DWORD> arrDirs;
	for (int i=0; i<m_arrItems.GetSize(); i++)
	{
		DWORD tmpScale = m_arrItems.GetAt(i).GetScale();
		int num = arrDirs.GetSize();
		for(int j=0; j<num; j++)
		{
			if (tmpScale < arrDirs.GetAt(j))
				break;
		}

		if (j < num)
			arrDirs.InsertAt(j,tmpScale);
		else 
			arrDirs.Add(tmpScale);
	}

	int num = arrDirs.GetSize();
	if (num <= 0)  return 0;
	
	//找到最合适的比例尺
	long target = 0;	
	for (i=0; i<num; i++)
	{
		if (scale < arrDirs.GetAt(i))break;
	}
	if (i == 0)
		target = arrDirs.GetAt(0);
	else if(i < num)
		target = arrDirs.GetAt(i-1);
	else 
		target = arrDirs.GetAt(num-1);

	return target;
}

CPlaceConfigLib *CConfigLibManager::GetPlaceConfigLib(DWORD scale)
{
	ConfigLibItem item = FindItem(scale);
	if (item.GetScale() == 0)   return NULL;

	return item.pPlaceCfgLib;
}


CCellDefLib *CConfigLibManager::GetCellDefLib(DWORD scale)
{
	ConfigLibItem item = FindItem(scale);
	if (item.GetScale() == 0)   return NULL;
	
	return item.pCellDefLib;
}

CScheme *CConfigLibManager::GetScheme(DWORD scale)
{
	if(this==NULL)
		return NULL;
	ConfigLibItem item = FindItem(scale);
	if (item.GetScale() == 0)   return NULL;
	
	return item.pScheme;
}


CBaseLineTypeLib *CConfigLibManager::GetBaseLineTypeLib(DWORD scale)
{
	ConfigLibItem item = FindItem(scale);
	if (item.GetScale() == 0)   return NULL;

	return item.pLinetypeLib;
}

ConfigLibItem CConfigLibManager::GetConfigLibItemByScale(int scale)
{
	return FindItem(scale);
}

DWORD CConfigLibManager::GetScaleByScheme(CScheme *pScheme)
{
	if (NULL==pScheme) return 0;
	for (int i=m_arrItems.GetSize()-1;i>=0;i--)
	{
		if(m_arrItems[i].pScheme==pScheme)
			return m_arrItems[i].GetScale();
	}
	return 0;
}
ConfigLibItem CConfigLibManager::GetConfigLibItem(int i)
{
	if (i>=0 && i<m_arrItems.GetSize())
	{
		ConfigLibItem &item = m_arrItems.ElementAt(i);
		if (!item.bLoaded)
		{
			item.Load();
		}
		return item;
	}
	return ConfigLibItem();
}

ConfigLibItem CConfigLibManager::FindItem(DWORD scale)
{
	DWORD nScale = GetSuitedScale(scale);
	if (nScale == 0)
	{
		return ConfigLibItem();
	}
	
	int nsz = m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		ConfigLibItem &item = m_arrItems.ElementAt(i);
		if( item.GetScale()==nScale )
		{
			if (!item.bLoaded)
			{
				item.Load();
			}
			return item;
		}
	}

	return ConfigLibItem();
}


BOOL CConfigLibManager::GetSymbol(CDataSourceEx *pSource, CFeature *pFtr, CPtrArray& arr, LPCTSTR strLayerNameAppointed)
{
	if (!pSource || !pFtr)  return FALSE;

	CGeometry *pGeo = pFtr->GetGeometry();

	long scale = pSource->GetScale();

	CScheme *pScheme = GetScheme(scale);
	
	if (!pScheme)  return FALSE;

	CString strSymName = pFtr->GetGeometry()->GetSymbolName();

	int nCount = strSymName.GetLength();

	int index = -1;
	if( nCount>0 && strSymName[0]=='%' )
		return FALSE;

	if( strSymName.CompareNoCase("*")==0 )
		return FALSE;
	else if ((index=strSymName.Find('@')) >= 0)
	{
		BOOL bSearchCells = TRUE;
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			bSearchCells = FALSE;
		}

		// 单个图元
		if (bSearchCells)
		{			
			CString celldef = strSymName.Right(nCount-index-1);
			CCell *pCell = new CCell;
			if (!pCell) return FALSE;

			pCell->m_strCellDefName = celldef;

			arr.Add(pCell);
		}
		// 单个线型
		else
		{
			CString linename = strSymName.Right(nCount-index-1);
			CDashLinetype *pDash = new CDashLinetype;
			if (!pDash) return FALSE;
			
			pDash->m_strBaseLinetypeName = linename;

			arr.Add(pDash);
		}
	}
	else if ((index=strSymName.Find('#')) >= 0)
	{
		CString strLayerName, strSymLayerName;

		if (index > 0)
		{
			strSymLayerName = strSymName.Left(index);
		}

		CString strSymIndex = strSymName.Right(nCount-index-1);
		if (!IsDigital(strSymIndex)) return FALSE;

		int symIndex = _ttoi(strSymIndex);

		if (strSymLayerName.IsEmpty())
		{
			CFtrLayer *pFtrLayer = NULL;
			
			if (strLayerNameAppointed != NULL)
			{
				pFtrLayer = pSource->GetFtrLayer(strLayerNameAppointed);
			}
			else
			{
				pFtrLayer = pSource->GetFtrLayerOfObject(pFtr);
			}

			if (!pFtrLayer)  return FALSE;
			
			CPtrArray arr1;
			pFtrLayer->GetSymbols(arr1);
			
			// 如果层缓存中有符号，则返回
			if (arr1.GetSize() > 0)
			{
				if (symIndex >=0 && symIndex < arr1.GetSize())
				{
					CSymbol *pNewSym = GCreateSymbol(((CSymbol*)arr1[symIndex])->GetType());
					if (pNewSym)
					{
						pNewSym->CopyFrom(((CSymbol*)arr1[symIndex]));
						arr.Add(pNewSym);
					}
					return TRUE;
				}
				else
					return FALSE;
				
			}
			
			strLayerName = pFtrLayer->GetName();
		}
		else
		{
			strLayerName = strSymName;
		}		
		
		CSchemeLayerDefine *define = pScheme->GetLayerDefine(strLayerName);		
		if (!define) 
		{
			define = pScheme->GetLayerDefine(strLayerName,FALSE,0,TRUE);
			if (!define)
			{
				return FALSE;
			}
					
		}
		
		CSymbol *pSym = define->GetSymbol(symIndex);
		if (pSym)
		{
			CSymbol *pNewSym = GCreateSymbol(pSym->GetType());
			if (pNewSym)
			{
				pNewSym->CopyFrom(pSym);
				arr.Add(pNewSym);
			}
			
		}		
	}	
	// 一般情况(使用本层或其他层的符号配置)
	else
	{
		CString strLayerName;
		
		if (strSymName.IsEmpty())
		{
			CFtrLayer *pFtrLayer = NULL;
			
			if (strLayerNameAppointed != NULL)
			{
				pFtrLayer = pSource->GetFtrLayer(strLayerNameAppointed);
			}
			else
			{
				pFtrLayer = pSource->GetFtrLayerOfObject(pFtr);
			}

			if (!pFtrLayer)  return FALSE;
			
			CPtrArray arr2;
			pFtrLayer->GetSymbols(arr2);

			for (int i=0; i<arr2.GetSize(); i++)
			{
				CSymbol *pNewSym = GCreateSymbol(((CSymbol*)arr2[i])->GetType());
				if (pNewSym)
				{
					pNewSym->CopyFrom(((CSymbol*)arr2[i]));
					arr.Add(pNewSym);
				}
			}
			
			return TRUE;
			// 如果层缓存中有符号，则返回
			if (arr.GetSize() > 0) return TRUE;
			
			strLayerName = pFtrLayer->GetName();
		}
		else
		{
			strLayerName = strSymName;
		}		
		
		CSchemeLayerDefine *define = pScheme->GetLayerDefine(strLayerName);	
		if (!define) 
		{
			define = pScheme->GetLayerDefine(strLayerName,FALSE,0,TRUE);
			if (!define)
			{
				return FALSE;
			}
			
		}

		CArray<CSymbol*,CSymbol*> arrSyms;
		define->GetSymbols(arrSyms);

		for (int i=0; i<arrSyms.GetSize(); i++)
		{
			CSymbol *pNewSym = GCreateSymbol(arrSyms[i]->GetType());
			if (pNewSym)
			{
				pNewSym->CopyFrom(arrSyms[i]);
				arr.Add(pNewSym);
			}
		}

	}

	return TRUE;

}

//hcw,2012.7.31.
void CConfigLibManager::Copy(CConfigLibManager *const pCfgLibMan )
{
	if (!pCfgLibMan) //hcw,2012.8.3
	{
		return;
	}
	this->m_strPath = pCfgLibMan->m_strPath;
	this->m_strBakPath = pCfgLibMan->m_strBakPath;
    
	for (int i=0; i<pCfgLibMan->GetConfigLibCount();i++)
	{
 		ConfigLibItem tmpLibItem;	
		tmpLibItem.Create();
		tmpLibItem.Copy(pCfgLibMan->GetConfigLibItem(i));
 		this->AddConfigItem(tmpLibItem);
	}	
}


// 将path1配置合并到path
void CombinConfig(CString path, CString path1)
{
	ConfigLibItem item, item1;
	item.SetScalePath(path);
	item.Load();
	
	item1.SetScalePath(path1);
	item1.Load();
	
	// action.xml
	for (int i=item1.pPlaceCfgLib->GetConfigCount()-1 ;i>=0; i--)
	{
		CPlaceConfig *pConfig1 = item1.pPlaceCfgLib->GetConfig(i);
		
		if (pConfig1)
		{
			CString name = pConfig1->m_strLayName;
			CSchemeLayerDefine *pLayer = item1.pScheme->GetLayerDefine(name);
			if (!pLayer) continue;
			
			CString strGroupName = pLayer->GetGroupName();
			if (strGroupName.CompareNoCase(_T("1测量控制")) == 0 || 
			strGroupName.CompareNoCase(_T("2居民地")) == 0 || 
			strGroupName.CompareNoCase(_T("3工矿和公共")) == 0 || 
			strGroupName.CompareNoCase(_T("4道路和附属")) == 0 ||
			strGroupName.CompareNoCase(_T("5管线墙栅栏")) == 0 )
			{
				CPlaceConfig *pConfig = item.pPlaceCfgLib->GetConfig(name);
				if (pConfig)
				{
					pConfig = pConfig1;
				}
				else
				{
					item.pPlaceCfgLib->AddConfig(*pConfig1);
				}
			}		
			
		}
	}
	
	// symbol.bin
/*	for (i=0; i<item1.pCellDefLib->GetCellDefCount(); i++)
	{
		CellDef def = item1.pCellDefLib->GetCellDef(i);
		CString name = def.m_name;
		if (item.pCellDefLib->GetCellDefIndex(name) == -1)
		{
			item.pCellDefLib->AddCellDef(def);
		}
	}
	
	for (i=0; i<item1.pLinetypeLib->GetBaseLineTypeCount(); i++)
	{
		BaseLineType def = item1.pLinetypeLib->GetBaseLineType(i);
		CString name = def.m_name;
		if (item.pLinetypeLib->GetBaseLineTypeIndex(name) == -1)
		{
			item.pLinetypeLib->AddBaseLineType(def);
		}
	}
	
	item.pScheme->Save();*/
	item.pPlaceCfgLib->Save();
	
	item.Clear();
}


CConfigLibManager *gpConfigLibManager = NULL;

CConfigLibManager *GetConfigLibManager()
{
	return gpConfigLibManager;
}


void SetConfigLibManager(CConfigLibManager *pLib)
{
	gpConfigLibManager = pLib;
}


MyNameSpaceEnd
