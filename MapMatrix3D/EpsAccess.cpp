// EpsAccess.cpp: implementation of the CEpsAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "SymbolLib.h "
#include "EpsAccess.h"
#include "sunwayFunction.h "
#include "GeoText.h "
#include "DlgDataSource.h "
#include "Scheme.h "
#include "PlotWChar.h "
#include "Feature.h"


#ifndef _WIN64
static COLORREF StringTORGB(CString str)
{
	LPTSTR p = str.GetBuffer(0);
	long R = _ttol(p);
	int pos = str.Find(_T(','));
	long G = _ttol(p+pos+1);
	pos = str.Find(_T(','),pos+1);
	long B = _ttol(p+pos+1);
	return RGB(R,G,B);
}


static int ConvertAlignment(int type, BOOL fo_to_eps)
{
	if (fo_to_eps)
	{
		int nJustification = 0;
		switch(type)
		{
		case TAH_LEFT:
			nJustification = e_LeftTop;
			break;
		case TAH_LEFT|TAV_TOP:
			nJustification = e_LeftTop;
			break;
		case TAH_MID:
			nJustification = e_Centre;			
			break;
		case TAH_MID|TAV_TOP:
			nJustification = e_TopCentre;			
			break;
		case TAH_RIGHT:
			nJustification = e_RightTop;
			break;
		case TAH_RIGHT|TAV_TOP:
			nJustification = e_RightTop;			
			break;
		case TAH_LEFT|TAV_MID:
			nJustification = e_LeftCentre;
			break;
		case TAH_MID|TAV_MID:
			nJustification = e_Centre;		
			break;
		case TAH_RIGHT|TAV_MID:	
			nJustification = e_RightCentre;			
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			nJustification = e_LeftBottom;		
			break;
		case TAH_MID|TAV_BOTTOM:
			nJustification = e_BottomCentre;			
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			nJustification = e_RightBottom;
			break;
		default:
			nJustification = e_LeftTop;			
			break;
		}
		
		return nJustification;
	}
	else
	{
		int nAlign;
		switch(type)
		{
		case e_LeftTop:
			nAlign = TAH_LEFT|TAV_TOP;
			break;
		case e_TopCentre:
			nAlign = TAH_MID|TAV_TOP;			
			break;
		case e_RightTop:
			nAlign = TAH_RIGHT|TAV_TOP;			
			break;
		case e_LeftCentre:
			nAlign = TAH_LEFT|TAV_MID;
			break;
		case e_Centre:
			nAlign = TAH_MID|TAV_MID;		
			break;
		case e_RightCentre:	
			nAlign = TAH_RIGHT|TAV_MID;			
			break;
		case e_LeftBottom:
			nAlign = TAH_LEFT|TAV_BOTTOM;		
			break;
		case e_BottomCentre:
			nAlign = TAH_MID|TAV_BOTTOM;			
			break;
		case e_RightBottom:
			nAlign = TAH_RIGHT|TAV_BOTTOM;
			break;
		default:
			nAlign = TAH_LEFT;			
			break;
		}
		
		return nAlign;
	}
	
}



LCodeListManager::LCodeListManager()
{
	m_pDaoSmallLayerRs = NULL;
	m_pDaoNotesRs = NULL;
	m_bHasListFile = FALSE;
	m_pExternDaoDb = NULL;
	
//	m_tableName = _T("");
	m_pLocalDb = NULL;
}

LCodeListManager::~LCodeListManager()
{
	if (m_pDaoSmallLayerRs)
	{
		if (m_pDaoSmallLayerRs->IsOpen())
		{
			m_pDaoSmallLayerRs->Close();
			delete m_pDaoSmallLayerRs;
		}
	}
	if (m_pDaoNotesRs)
	{
		if (m_pDaoNotesRs->IsOpen())
		{
			m_pDaoNotesRs->Close();
			delete m_pDaoNotesRs;
		}
	}
	m_listFile.Clear();
}

BOOL LCodeListManager::LoadFile(LPCTSTR fileName)
{
	m_bHasListFile = m_listFile.Open(fileName);
	return m_bHasListFile;
}

void LCodeListManager::Clear()
{
	if (m_pDaoSmallLayerRs)
	{
		if (m_pDaoSmallLayerRs->IsOpen())
		{
			m_pDaoSmallLayerRs->Close();
			delete m_pDaoSmallLayerRs;
			m_pDaoSmallLayerRs = NULL;
		}
	}
	if (m_pDaoNotesRs)
	{
		if (m_pDaoNotesRs->IsOpen())
		{
			m_pDaoNotesRs->Close();
			delete m_pDaoNotesRs;
			m_pDaoNotesRs = NULL;
		}
	}
	m_listFile.Clear();
	m_pExternDaoDb = NULL;
	m_bHasListFile = FALSE;
//	m_tableName = _T("");
	m_pLocalDb = NULL;
}

BOOL LCodeListManager::SetDataSourse(CEpsAccess *pAccess, CDlgDataSource *pDS)
{
	ASSERT(m_bHasListFile==FALSE);
	ASSERT(pAccess!=NULL);
	ASSERT(pDS!=NULL);
	if (!pAccess->m_pDaoDb->IsOpen())
	{
		return FALSE;
	}
	if (pAccess->IsUseRefDb())
	{
		m_pExternDaoDb = pAccess->m_pDaoRefDb;
	}
	else
		m_pExternDaoDb = pAccess->m_pDaoDb;
	m_pDaoSmallLayerRs = new CDaoRecordset(m_pExternDaoDb);
	if(!m_pDaoSmallLayerRs)return FALSE;
	try
	{
		CString sql;
		sql.Format(_T("select * from %s;"),pAccess->m_strSmallLayer);
		m_pDaoSmallLayerRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
	}
	catch (CDaoException* e)
	{
		e->ReportError();
		e->Delete();
		delete m_pDaoSmallLayerRs;
		m_pDaoSmallLayerRs = NULL;
		return FALSE;
	}	

	m_pDaoNotesRs = new CDaoRecordset(m_pExternDaoDb);
	if(!m_pDaoNotesRs)return FALSE;
	try
	{
		CString sql;
		sql.Format(_T("select * from %s;"),pAccess->m_strNotesTable);
		m_pDaoNotesRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
	}
	catch (CDaoException* e)
	{
		e->ReportError();
		e->Delete();
		delete m_pDaoNotesRs;
		m_pDaoNotesRs = NULL;
		return FALSE;
	}
	m_pLocalDb = pDS;
	return TRUE;
}

//返回-1则没找到。
long LCodeListManager::LocLayNameToExternCode(LPCTSTR name)
{
	long ret = -1;
	CString sql;
	if (m_bHasListFile)
	{
		ret = m_listFile.FindIntItem(name,1,0,ret);	
	}
	
	if (ret == -1)
	{	
		sql.Format(_T("ObjectName = '%s'"),name);
		if(m_pDaoSmallLayerRs->FindFirst(sql))
		{
			COleVariant var;
			m_pDaoSmallLayerRs->GetFieldValue(_T("Code"),var);
			ret = var.lVal;
		}
		else
		{
			ret = -1;
		}
	}
	return ret;
}

CString LCodeListManager::ExternCodeToLocLayName(long code)
{
	CString ret = _T("");
	if (m_bHasListFile)
	{	
		CString name;
		name.Format(_T("%ld"),code);
		ret = m_listFile.FindTextItem(name,0,1,"");	
	}
	
	if (ret.IsEmpty())
	{		
		CString sql;
		sql.Format(_T("Code = %ld"),code);
		if(m_pDaoSmallLayerRs->FindFirst(sql))
		{
			COleVariant var;
			m_pDaoSmallLayerRs->GetFieldValue(_T("ObjectName"),var);
			ret = LPCTSTR(var.bstrVal);
			if(!m_pLocalDb->GetLocalFtrLayer(ret))
				ret = _T("");
		}		
	}	
	return ret;
}

int LCodeListManager::GetParaOutMode(LPCTSTR name)
{
	if (m_bHasListFile)
	{	
		CString ret = m_listFile.FindTextItem(name,1,2,"");	
		if (ret.CompareNoCase(_T("m")) == 0)
		{
			return 1;
		}
		else if (ret.CompareNoCase(_T("s")) == 0)
		{
			return 2;
		}
		else if (ret.CompareNoCase(_T("c")) == 0)
		{
			return 3;
		}
	}

	return 0;
}

int GeoTypeFrom(int src)
{
	int ret;
	switch(src)
	{
	case 0:
		ret = CLS_GEOPOINT;
		break;
	case 1:		
	case 2:	    
	case 3:
	case 4:
	case 6:
		ret = CLS_GEOCURVE;
		break;
	case 5:
		ret = CLS_GEOSURFACE;
		break;	
	default:
		ret = CLS_GEOCURVE;
	    break;
	}
	return ret;
}

static long LayerCodeForm(long code)
{
	return 0;
}
// close the database and destruct it
// IN/OUT:: ppDatabase--pointer to pointer to database to close and
// destruct
static void closeDatabase(CDaoDatabase **ppDatabase)
{
	// only process if the database object exists
	if (*ppDatabase != NULL)
	{
		if ((*ppDatabase)->IsOpen())
			(*ppDatabase)->Close();

		// closing doesn't delete the object
		delete *ppDatabase;
		*ppDatabase = NULL;
	}
}

// open the database from a file (close if necessary)
// IN/OUT: ppDatabase--pointer to pointer to database object
// IN: fileName--name of .mdb file to open
// IN: bReportNoOpen--TRUE by default, if true, if open fails
//     because the specified MDB file doesn't exist, report that
//     fact to the user.  Note: other errors are reported always
// RETURN: 1 if success, 0 if non-fatal failure, -1 if fatal failure
static int openDatabase(CDaoDatabase **ppDatabase, CString fileName,
				  BOOL bReportNoOpen /* = TRUE */)
{
	// initialize success indicator
	int nReturnCode = 1;

	// close and delete if necessary
	if (*ppDatabase != NULL)
	{
		if ((*ppDatabase)->IsOpen())
			closeDatabase(ppDatabase);
		delete *ppDatabase;
	}

	// construct new database
	*ppDatabase = new CDaoDatabase;

	// failed to allocate
	if (ppDatabase == NULL)
		return -1; // fatal error

	// now open the database object with error checking
	try
	{
		(*ppDatabase)->Open(fileName);
	}
	catch (CDaoException *e)
	{
		// special case--couldn't find the file, so it may be because
		// user specified a new file to open
		if (e->m_pErrorInfo->m_lErrorCode == 3024)
		{
			if (bReportNoOpen)
			{
				// create a message to display
				CString message = _T("Couldn't open database--Exception: ");
				message += e->m_pErrorInfo->m_strDescription;

				// output status
				AfxMessageBox(message);
			}

			// indicate failure but not fatal
			nReturnCode = 0;
		}
		else // other type of DAO exception--always report
		{
			// create a message to display
			CString message = _T("Couldn't open database--Exception: ");
			message += e->m_pErrorInfo->m_strDescription;

			// output status
			AfxMessageBox(message);

			// indicate fatal error
			nReturnCode = -1;
		}

		// not rethrowing, so delete exception
		e->Delete();

		delete *ppDatabase;
		*ppDatabase = NULL;
	}
	catch (CMemoryException *e)
	{
		// output status
		AfxMessageBox(_T("Failed to open database--Memory exception thrown."));

		// not rethrowing, so delete exception
		e->Delete();

		delete *ppDatabase;
		*ppDatabase = NULL;

		// indicate fatal error
		nReturnCode = -1;
	}

	return nReturnCode;
}

// create the database file (close any open database)
// IN/OUT: ppDatabase--pointer to pointer to database object
// IN: fileName--name of .mdb file to open
// IN: dwOptions--info like version and encryption settings
//     0 by default
// RETURN: TRUE if success, FALSE if failure
static BOOL createDatabase(CDaoDatabase **ppDatabase, CString fileName,
				  int dwOptions /* = 0 */)
{
	// initialize success indicator
	BOOL bSuccess = TRUE;

	// close and delete if necessary
	if (*ppDatabase != NULL)
	{
		if ((*ppDatabase)->IsOpen())
			closeDatabase(ppDatabase);
		delete *ppDatabase;
	}

	// construct new database
	*ppDatabase = new CDaoDatabase;

	// failed to allocate
	if (ppDatabase == NULL)
		return FALSE; // error

	// now create the database object with error checking
	try
	{
		// default language specified
		(*ppDatabase)->Create(fileName, dbLangGeneral, dwOptions);
	}
	catch (CDaoException *e)
	{
		// create a message to display
		CString message = _T("Couldn't create database--Exception: ");
		message += e->m_pErrorInfo->m_strDescription;

		// output status
		AfxMessageBox(message);

		// not rethrowing, so delete exception
		e->Delete();

		delete *ppDatabase;
		*ppDatabase = NULL;

		// failure
		bSuccess = FALSE;
	}
	catch (CMemoryException *e)
	{
		// output status
		AfxMessageBox(_T("Failed to create database--Memory exception thrown."));

		// not rethrowing, so delete exception
		e->Delete();

		delete *ppDatabase;
		*ppDatabase = NULL;

		// failure
		bSuccess = FALSE;
	}

	return bSuccess;
}


static BOOL  IsExistentTable(CDaoDatabase  *pDatabase,   CString  strTableName)   
{ 	
	if(pDatabase == NULL)   
		return FALSE;   
	BOOL bDuplicateTableName =  TRUE;   
	CDaoTableDefInfo tableInfo;   //   only   needed   for   the   call   
	TRY   
	{
		pDatabase->GetTableDefInfo(strTableName,  tableInfo);   
	}   
	CATCH(CDaoException,e)   
	{  	
		if(e->m_pErrorInfo->m_lErrorCode == 3265)   
			bDuplicateTableName  =  FALSE;   
	}   
	AND_CATCH(CMemoryException,e)   
	{   
		//   do   nothing   
		;   
	}   
	END_CATCH 		
	return bDuplicateTableName;   
    
}   
//判断域   
static BOOL   IsExistentField(CDaoTableDef   *pTableDef,   CString   strFieldName)   
{   
	//   if   the   tabledef   is   non-existent,   then   the   answer   is   obvious   
	if   (pTableDef   ==   NULL)   
		return   FALSE;   
    
	//   initialize   status   indicator   
	BOOL   bDuplicateFieldName   =   TRUE;     
	//   see  if  there  is  a  field  by  this  name  already--duplicate   
	//   named  fields  are  not  accepted   
	CDaoFieldInfo   fieldInfo;         //   only   needed   for   the   call   
    
	//   MFC   exception   handler   macros   used   
	TRY   
	{
		//   this   call   will   throw   an   exception   if   there   is   no   
		//   field   by   the   specified   name--test   for   duplication   
		pTableDef->GetFieldInfo(strFieldName,   fieldInfo);   
	}   
	CATCH   (CDaoException,   e)   
	{   
		//   if   this   is   an   'Item   not   found'   exception,   we   are   
		//   cleared   to   create   the   field   --   else   this   is   
		//   a   duplicate   field   name   and   we   got   another   exception   
		//   which   is   irrelevant   for   our   purposes   
		if   (e->m_pErrorInfo->m_lErrorCode   ==   3265)   
			bDuplicateFieldName   =   FALSE;   
	}   
	AND_CATCH   (CMemoryException,   e)   
	{   
		//   do   nothing--no   need   to   process   errors   since   this   is   
		//   just   a   duplicate   checker   
	}   
	END_CATCH 
	return   bDuplicateFieldName;   
}   

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEpsAccess::CEpsAccess()
{
	m_pDaoDb = NULL;
	m_pDaoRefDb = NULL;
	m_bUseRefTmpDb = FALSE;
	
//	m_pDaoTd = NULL;
//	m_pDaoQd = NULL;
	m_bValid = FALSE;	
	
	m_pDaoLayerRs = NULL;	
	
	m_pDaoFtrRs = NULL;

	m_pDaoTmpRs = NULL;
	m_lScale = 0;

//	m_pCodeList = NULL;
	m_strSmallLayer = _T("");
	m_strBigLayer = _T("");
	m_strSymbolsTable = _T("");
	m_strNotesTable = _T("");

//	m_pScheme = NULL;
	m_lObjID = 0;
	m_nParaOutMode = modeSeparate;
}

CEpsAccess::~CEpsAccess()
{	
	if(m_pDaoLayerRs)
	{
		if(m_pDaoLayerRs->IsOpen())
			m_pDaoLayerRs->Close();
		delete m_pDaoLayerRs;	
	}
	
	if(m_pDaoFtrRs)
	{
		if(m_pDaoFtrRs->IsOpen())
			m_pDaoFtrRs->Close();
		delete m_pDaoFtrRs;
	}
	if(m_pDaoTmpRs)
	{
		if(m_pDaoTmpRs->IsOpen())
			m_pDaoTmpRs->Close();
		delete m_pDaoTmpRs;
	}
	if(m_pDaoDb) 
	{
		if(m_pDaoDb->IsOpen())
			m_pDaoDb->Close();
		delete m_pDaoDb;
	}
	
	if(m_pDaoRefDb)
	{
		if(m_pDaoRefDb->IsOpen())
			m_pDaoRefDb->Close();
		delete m_pDaoRefDb;	
	}
//	if(m_pDaoTd) delete m_pDaoTd;
//	if(m_pDaoQd) delete m_pDaoQd;	
	


}

// void CEpsAccess::AddDataField()
// {
// 	CDaoTableDef tabdef(m_pDaoDb);
// 	//点表
// 	tabdef.Open(_T("GeoPointTB"));
// 	tabdef.CreateField(_T("Graphics"),dbLongBinary,0);
// 	tabdef.CreateField(_T("GraphicInfo"),dbLongBinary,0);
// 	tabdef.Close();
// 	//线表
// 	tabdef.Open(_T("GeoLineTB"));
// 	tabdef.CreateField(_T("Graphics"),dbLongBinary,0);
// 	tabdef.CreateField(_T("GraphicInfo"),dbLongBinary,0);
// 	tabdef.Close();	
// 	//面表
// 	tabdef.Open(_T("GeoAreaTB"));
// 	tabdef.CreateField(_T("Graphics"),dbLongBinary,0);
// 	tabdef.CreateField(_T("GraphicInfo"),dbLongBinary,0);
// 	tabdef.Close();	
// 	//注记
// 	tabdef.Open(_T("MarkNoteTB"));
// 	tabdef.CreateField(_T("Graphics"),dbLongBinary,0);
// 	tabdef.CreateField(_T("GraphicInfo"),dbLongBinary,0);
// 	tabdef.Close();
// }

void  CEpsAccess::Clear()
{
	if(m_pDaoLayerRs)
	{
		if(m_pDaoLayerRs->IsOpen())
			m_pDaoLayerRs->Close();
		delete m_pDaoLayerRs;	
	}
	
	if(m_pDaoFtrRs)
	{
		if(m_pDaoFtrRs->IsOpen())
			m_pDaoFtrRs->Close();
		delete m_pDaoFtrRs;
	}
	if(m_pDaoTmpRs)
	{
		if(m_pDaoTmpRs->IsOpen())
			m_pDaoTmpRs->Close();
		delete m_pDaoTmpRs;
	}
	if(m_pDaoDb) 
	{
		if(m_pDaoDb->IsOpen())
			m_pDaoDb->Close();
		delete m_pDaoDb;
	}
	if(m_pDaoRefDb)
	{
		if(m_pDaoRefDb->IsOpen())
			m_pDaoRefDb->Close();
		delete m_pDaoRefDb;	
	}
	m_pDaoDb = NULL;
	m_pDaoRefDb = NULL;
	m_bUseRefTmpDb = FALSE;
//	m_pDaoTd = NULL;
//	m_pDaoQd = NULL;
	m_bValid = FALSE;	
	
	m_pDaoLayerRs = NULL;	
	
	m_pDaoFtrRs = NULL;
	
	m_pDaoTmpRs = NULL;
	m_lScale = 0;
	m_strSmallLayer = _T("");
	m_strBigLayer = _T("");
	m_strSymbolsTable = _T("");
	m_strNotesTable = _T("");
//	m_pScheme = NULL;
}

// void CEpsAccess::SetScheme(CScheme * pSheme)
// {
// //	m_pScheme = pSheme;
// }

BOOL CEpsAccess::Attach(LPCTSTR fileName)
{
	if( fileName==NULL )
		return FALSE;
	if( strlen(fileName)<=0 )
		return FALSE;

// 	WORD path2[1024] = {0};
// 	char path3[1024] = {0};
// 	MultiByteToWideChar(CP_ACP,0,fileName,strlen(fileName),path2,1024);
// 	WideCharToMultiByte(CP_UTF8,0,path2,strlen(fileName),path3,1024,NULL,NULL);
	m_strReadName = m_strWriteName = fileName;
	if (m_pDaoDb)
	{
		Clear();
	}	
	
	if(openDatabase(&m_pDaoDb,fileName,TRUE)!=1)
		return FALSE;	
	
	m_bValid = TRUE;
	
	m_pDaoFtrRs = new CDaoRecordset(m_pDaoDb);
	m_pDaoLayerRs = new CDaoRecordset(m_pDaoDb);

	//判断预定的基表时候存在
// 	if( !m_db.isValid() )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("WorkSpace") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Point") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Line") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Surface") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Text") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("DLGVectorLayer") )
// 		m_bValid = FALSE;

	return m_bValid;
}


BOOL CEpsAccess::IsValid()
{
	return m_bValid;
}


BOOL CEpsAccess::Close()
{
	Clear();		
	return TRUE;
}

void CEpsAccess::SetDataSource(CDlgDataSource *pDS)
{
	m_pDataSource = pDS;
}

BOOL CEpsAccess::CreateFile(LPCTSTR fileName, LPCTSTR templateFile)
{
	if (templateFile!=NULL)
	{
		BOOL ret =  CreateSunwayEDBFile(templateFile,fileName);
		if (ret)
			m_strTemplatePath = templateFile;
		return ret;
	}
	DWORD lScale = m_pDataSource->GetScale();
	CString  strPath = GetConfigPath();
	char line[12];
	strPath += _T("\\");
	lScale = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager()->GetSuitedScale(lScale);
	strPath += _ultoa(lScale,line,10);
	strPath += _T("\\基础地理标准5hV5.mdt");
	BOOL ret =  CreateSunwayEDBFile(strPath,fileName);
	if (ret)
		m_strTemplatePath = strPath;
	return ret;
}


BOOL CEpsAccess::OpenRefTemplateDatabase()
{	
	if(!IsValid()||!m_pDaoDb->IsOpen())
		return FALSE;
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from IniInfoTB where KeyName = 'TemplateLabel';"),dbReadOnly);
	if (rs.IsEOF())
	{
		rs.Close();
		return FALSE;
	}
	COleVariant var;
	rs.GetFieldValue(_T("StringValue"),var);
	rs.Close();
	CString strValue = LPCTSTR(_bstr_t(*(var.pbstrVal)));
	if (!m_pDaoRefDb)
	{
		m_pDaoRefDb = new CDaoDatabase;
	}
	else
	{
		if (m_pDaoRefDb->IsOpen())
		{
			m_pDaoRefDb->Close();
		}
	}
	try
	{
		m_pDaoRefDb->Open(strValue);
	}
	catch (CDaoException* e)
	{
		e->Delete();
		delete m_pDaoRefDb;
		m_pDaoRefDb = NULL;
		return FALSE;
	}
	m_bUseRefTmpDb = TRUE;
	return TRUE;
}

BOOL CEpsAccess::CloseRefTemplateDatabase()
{	
	closeDatabase(&m_pDaoRefDb);
	m_bUseRefTmpDb = FALSE;
	return TRUE;
}


void CEpsAccess::ClearAttrTables()
{
	//清理扩展属性表中的DEL项	
// 	CppSQLite3Query  ftrQuery;
// 	ftrQuery = m_db.execQuery("select name from sqlite_master where type='table';");
// 	if (!ftrQuery.eof())
// 	{	
// 		const char* strTable;
// 		char line[1024];
// 		do
// 		{
// 			strTable = ftrQuery.getStringField("name");
// 			if(strncmp(strTable,"AttrTable_",10)==0)
// 			{
// 				sprintf(line,"delete from %s where DELETED = 1;",strTable);
// 				m_db.execDML(line);
// 			}
// 			ftrQuery.nextRow();
// 		}
// 		while(!ftrQuery.eof());
// 	}
// 	ftrQuery.finalize();
}

BOOL CEpsAccess::OpenRead(LPCTSTR filename)
{
	if( filename==NULL )
		return FALSE;
	if( strlen(filename)<=0 )
		return FALSE;
	

//	MultiByteToWideChar(CP_ACP,0,filename,strlen(filename),path2,1024);
//	WideCharToMultiByte(CP_UTF8,0,path2,strlen(filename),path3,1024,NULL,NULL);
	if (m_pDaoDb)
	{
		Clear();	
	}
// 	m_pDaoDb = new CDaoDatabase;
// 	
// 	m_pDaoDb->Open(path3,TRUE,TRUE);
// 	if(!m_pDaoDb->IsOpen())
// 	{
// 		::MessageBox(NULL,_T("Failed to open database!"));	
// 		return FALSE;
// 	}
	if(openDatabase(&m_pDaoDb,filename,TRUE)!=1)
		return FALSE;
	
	m_bValid = TRUE;

	m_pDaoFtrRs = new CDaoRecordset(m_pDaoDb);
	m_pDaoLayerRs = new CDaoRecordset(m_pDaoDb);
	

// 	
// 	if( !m_db.isValid() )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("WorkSpace") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Point") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Line") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Surface") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("Text") )
// 		m_bValid = FALSE;
// 	else if( !m_db.tableExists("DLGVectorLayer") )
// 		m_bValid = FALSE;
// 	
	return m_bValid;
}



BOOL CEpsAccess::OpenWrite(LPCTSTR filename)
{
	m_strWriteName = filename;
	return Attach(filename);
}


BOOL CEpsAccess::CloseRead()
{
	Clear();
	return TRUE;
}


BOOL CEpsAccess::CloseWrite()
{
	Clear();
	return TRUE;
}


//读取数据源信息
BOOL CEpsAccess::ReadDataSourceInfo(CDataSourceEx *pDS)
{
	CDaoRecordset  DaoTmpRs(m_pDaoDb);
	DaoTmpRs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * form ProjectTB where ID = 0;"),dbReadOnly);
	if (DaoTmpRs.IsEOF())
	{
		return FALSE;		
	}

	CValueTable tab;
	CVariantEx var;
	COleVariant oleVar;
	double value;

	DaoTmpRs.GetFieldValue(_T("Scale"),oleVar);
	value = oleVar.dblVal;

	tab.BeginAddValueItem();
	tab.AddValue(FIELDNAME_SCALE,&CVariantEx((_variant_t)(long)value));

	CArray<PT_3DEX,PT_3DEX> arrPts;
	arrPts.SetSize(4);
	PT_3DEX* pts = arrPts.GetData();

	DaoTmpRs.GetFieldValue(_T("MinX"),oleVar);
	value = oleVar.dblVal;
	pts[0].x = pts[3].x = value;
	DaoTmpRs.GetFieldValue(_T("MinY"),oleVar);
	value = oleVar.dblVal;
	pts[0].y = pts[1].y = value;
	DaoTmpRs.GetFieldValue(_T("MaxX"),oleVar);
	value = oleVar.dblVal;
	pts[1].x = pts[2].x = value;
	DaoTmpRs.GetFieldValue(_T("MaxY"),oleVar);
	value = oleVar.dblVal;
	pts[2].y = pts[3].y = value;
		
	var.SetAsShape(arrPts);		
	tab.AddValue(FIELDNAME_BOUND,&var);
	
	double zmin = -1000.0;
	tab.AddValue(FIELDNAME_ZMIN,&CVariantEx((_variant_t)zmin));

	double zmax = 1000.0;
	tab.AddValue(FIELDNAME_ZMAX,&CVariantEx((_variant_t)zmax));

	tab.EndAddValueItem();

	pDS->ReadFrom(tab);
	DaoTmpRs.Close();
	return TRUE;
}

BOOL CEpsAccess::InitImport()
{
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from ProjectTB where ID = 0;"),dbReadOnly);
	if (rs.IsEOF())
	{
		return FALSE;
	}
	COleVariant var;
	rs.GetFieldValue(_T("FeatureCodeTableName"),var);
	m_strSmallLayer = LPCTSTR((var.bstrVal));	
	rs.GetFieldValue(_T("UserLayerTableName"),var);
	m_strBigLayer = LPCTSTR((var.bstrVal));
	rs.GetFieldValue(_T("SymbolScriptTableName"),var);
	m_strSymbolsTable = LPCTSTR((var.bstrVal));
	rs.GetFieldValue(_T("NoteTemplateTableName"),var);
	m_strNotesTable = LPCTSTR((var.bstrVal));
	rs.GetFieldValue(_T("Scale"),var);
	m_lScale = var.lVal;
	rs.Close();	
	
	//此处有一个假定，4个表要么都存在，要么都不存在
// 	CString sql;
// 	sql.Format(_T("select * form %s;"),LPCTSTR(strValue));
// 	if(IsExistentTable(m_pDaoDb,m_strSmallLayer))
// 	{
// 		m_bUseRefTmpDb = FALSE;
// 	}
// 	else
	{
// 		rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from IniInfoTB where KeyName = 'TemplateLabel';"),dbReadOnly);
// 		if (rs.IsEOF())
// 		{
// 			rs.Close();
// 			return FALSE;
// 		}
// 		COleVariant var;
// 		rs.GetFieldValue(_T("StringValue"),var);
// 		rs.Close();
// 		CString dbfile = LPCTSTR(var.bstrVal);
		if (m_strTemplatePath.IsEmpty())
		{
			CString path = GetConfigPath();
			long  scale = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager()->GetSuitedScale(m_lScale);
			char line[12];
			path +=_T("\\");
			path += _ltoa(scale,line,10);
			path += _T("\\基础地理标准5hV5.mdt");

			m_strTemplatePath = path;
		}
		
		if(!TryUseRefTemplateDatabase(m_strTemplatePath,m_strSmallLayer))
			return FALSE;
		m_bUseRefTmpDb = TRUE;
	}
	return TRUE;	
}

BOOL CEpsAccess::InitExport()
{
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from ProjectTB where ID = 0;"),dbReadOnly);
	if (rs.IsEOF())
	{
		return FALSE;
	}
	COleVariant var;
	rs.GetFieldValue(_T("FeatureCodeTableName"),var);
	m_strSmallLayer = LPCTSTR((var.bstrVal));	
	rs.GetFieldValue(_T("UserLayerTableName"),var);
	m_strBigLayer = LPCTSTR((var.bstrVal));
	rs.GetFieldValue(_T("SymbolScriptTableName"),var);
	m_strSymbolsTable = LPCTSTR((var.bstrVal));
	rs.GetFieldValue(_T("NoteTemplateTableName"),var);
	m_strNotesTable = LPCTSTR((var.bstrVal));
	rs.GetFieldValue(_T("Scale"),var);
	m_lScale = var.dblVal;
	rs.Close();	
	
	//此处有一个假定，4个表要么都存在，要么都不存在
	// 	CString sql;
	// 	sql.Format(_T("select * form %s;"),LPCTSTR(strValue));
// 	if(IsExistentTable(m_pDaoDb,m_strSmallLayer))
// 	{
// 		m_bUseRefTmpDb = FALSE;
// 	}
// 	else
	{	
		if(!TryUseRefTemplateDatabase(m_strTemplatePath,m_strSmallLayer))
			return FALSE;
		m_bUseRefTmpDb = TRUE;
	}
	return TRUE;	
}


BOOL CEpsAccess::TryUseRefTemplateDatabase(LPCTSTR dbName, LPCTSTR tableName)
{
// 	TCHAR szPath[MAX_PATH];
// 	::GetModuleFileName( NULL, szPath, MAX_PATH );
// 	CString path = szPath;
// 	int pos = path.ReverseFind(_T('\\'));
// 	if( pos>0 )
// 	{
// 		path = path.Left(pos+1);				
// 	}
// 	path += dbName;
// 	path +=_T(".mdt");
	if(openDatabase(&m_pDaoRefDb,dbName,TRUE)!=1)
	{
		m_bUseRefTmpDb = FALSE;
		return FALSE;
	}
	if(IsExistentTable(m_pDaoRefDb,tableName))
	{
		m_bUseRefTmpDb = TRUE;
		m_strTemplatePath = dbName;
		return TRUE;
	}
	else
	{
		m_bUseRefTmpDb = FALSE;
		return FALSE;
	}
}
void CEpsAccess::OpenBigLayerTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{		
		if (m_bUseRefTmpDb)
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoRefDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strBigLayer);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strBigLayer);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
	else
	{
		if (m_bUseRefTmpDb)
		{
			pDaoRs->m_pDatabase = m_pDaoRefDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strBigLayer);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			pDaoRs->m_pDatabase = m_pDaoDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strBigLayer);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
}
void CEpsAccess::CloseBigLayerTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{
		if (m_pDaoTmpRs)
		{
			if(m_pDaoTmpRs->IsOpen())
				m_pDaoTmpRs->Close();
			delete m_pDaoTmpRs;
			m_pDaoTmpRs = NULL;
		}
	}
	else
	{
		if (pDaoRs->IsOpen())
		{
			pDaoRs->Close();
		}
	}	
}
void CEpsAccess::OpenSmallLayerTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{		
		if (m_bUseRefTmpDb)
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoRefDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSmallLayer);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSmallLayer);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
	else
	{
		if (m_bUseRefTmpDb)
		{
			pDaoRs->m_pDatabase = m_pDaoRefDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSmallLayer);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			pDaoRs->m_pDatabase = m_pDaoDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSmallLayer);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
}
void CEpsAccess::CloseSmallLayerTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{
		if (m_pDaoTmpRs)
		{
			if(m_pDaoTmpRs->IsOpen())
				m_pDaoTmpRs->Close();
			delete m_pDaoTmpRs;
			m_pDaoTmpRs = NULL;
		}
	}
	else
	{
		if (pDaoRs->IsOpen())
		{
			pDaoRs->Close();
		}
	}	
}

void CEpsAccess::OpenSymbolsTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{		
		if (m_bUseRefTmpDb)
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoRefDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSymbolsTable);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSymbolsTable);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
	else
	{
		if (m_bUseRefTmpDb)
		{
			pDaoRs->m_pDatabase = m_pDaoRefDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSymbolsTable);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			pDaoRs->m_pDatabase = m_pDaoDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strSymbolsTable);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
}

void CEpsAccess::CloseSymbolsTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{
		if (m_pDaoTmpRs)
		{
			if(m_pDaoTmpRs->IsOpen())
				m_pDaoTmpRs->Close();
			delete m_pDaoTmpRs;
			m_pDaoTmpRs = NULL;
		}
	}
	else
	{
		if (pDaoRs->IsOpen())
		{
			pDaoRs->Close();
		}
	}	
}

void CEpsAccess::OpenNotesTmpTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{		
		if (m_bUseRefTmpDb)
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoRefDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strNotesTable);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			m_pDaoTmpRs = new CDaoRecordset(m_pDaoDb);
			if (!m_pDaoTmpRs)return;
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strNotesTable);
			try
			{
				m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
	else
	{
		if (m_bUseRefTmpDb)
		{
			pDaoRs->m_pDatabase = m_pDaoRefDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strNotesTable);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
		else
		{
			pDaoRs->m_pDatabase = m_pDaoDb;			
			CString sql;
			sql.Format(_T("select * from %s;"),(LPCTSTR)m_strNotesTable);
			try
			{
				pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql,dbReadOnly);
			}
			catch (CDaoException* e)
			{
				e->Delete();
				return;
			}		
		}
	}
}

void CEpsAccess::CloseNotesTmpTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{
		if (m_pDaoTmpRs)
		{
			if(m_pDaoTmpRs->IsOpen())
				m_pDaoTmpRs->Close();
			delete m_pDaoTmpRs;
			m_pDaoTmpRs = NULL;
		}
	}
	else
	{
		if (pDaoRs->IsOpen())
		{
			pDaoRs->Close();
		}
	}	
}

void CEpsAccess::OpenTable(LPCTSTR tableName, CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{		
		m_pDaoTmpRs = new CDaoRecordset(m_pDaoDb);
		if (!m_pDaoTmpRs)return;
		CString sql;
		sql.Format(_T("select * from %s;"),tableName);
		try
		{
			m_pDaoTmpRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql);
		}
		catch (CDaoException* e)
		{
			e->Delete();
			return;
		}		
	}
	else
	{
		pDaoRs->m_pDatabase = m_pDaoDb;			
		CString sql;
		sql.Format(_T("select * from %s;"),(LPCTSTR)tableName);
		try
		{
			pDaoRs->Open(AFX_DAO_USE_DEFAULT_TYPE,sql);
		}
		catch (CDaoException* e)
		{
			e->Delete();
			return;
		}		
	}
}

void CEpsAccess::CloseTable(CDaoRecordset *pDaoRs)
{
	if (NULL==pDaoRs)
	{
		if (m_pDaoTmpRs)
		{
			if(m_pDaoTmpRs->IsOpen())
				m_pDaoTmpRs->Close();
			delete m_pDaoTmpRs;
			m_pDaoTmpRs = NULL;
		}
	}
	else
	{
		if (pDaoRs->IsOpen())
		{
			pDaoRs->Close();
		}
	}	
}
//读取层
CFtrLayer *CEpsAccess::GetFirstFtrLayer(long &idx)
{ 	
	if( m_pDaoTmpRs->IsEOF() )
		return NULL;
	
	CFtrLayer *pLayer = ReadFtrLayer();

	m_pDaoTmpRs->MoveNext();

	return pLayer;
}


CFtrLayer *CEpsAccess::GetNextFtrLayer(long &idx)
{
	if( m_pDaoTmpRs->IsEOF() )
		return NULL;
	
	CFtrLayer *pLayer = ReadFtrLayer();

	m_pDaoTmpRs->MoveNext();
	
	return pLayer;
}

CFtrLayer *CEpsAccess::ReadFtrLayer()
{
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
//	char line[256];
	long lValue;
	int nValue;
	const char* pstr;
	bool bValue;
	
	COleVariant oleVar;
 	m_pDaoTmpRs->GetFieldValue(_T("Code"),oleVar);
	m_lTmpCode = oleVar.lVal;
// 	__int64 lLocalCode = m_pCodeList->FindCode(m_lTmpCode,FALSE);
// 
// 	CString name;
// 	if(lLocalCode!=-1)
// 		m_pScheme->FindLayerIdx(TRUE,lLocalCode,name);

 
	m_pDaoTmpRs->GetFieldValue(_T("LineColor"),oleVar);
	lValue = oleVar.lVal;
 	tab.AddValue(FIELDNAME_LAYCOLOR,&(CVariantEx)(_variant_t)(long)lValue);
 	
// 	bValue = (m_layerQuery.getIntField("Locked")!=0);
// 	tab.AddValue(FIELDNAME_LAYLOCKED,&(CVariantEx)(_variant_t)bValue);
// 	
// 	bValue = (m_layerQuery.getIntField("Visible")!=0);
// 	tab.AddValue(FIELDNAME_LAYVISIBLE,&(CVariantEx)(_variant_t)bValue);
// 
// 	bValue = (m_layerQuery.getIntField("Inherent")!=0);
// 	tab.AddValue(FIELDNAME_LAYINHERENT,&(CVariantEx)(_variant_t)bValue);
// 	
	tab.EndAddValueItem();
	
	CFtrLayer *pLayer = new CFtrLayer();
	if( !pLayer )return NULL;
// 	if (name)
// 	{
// 		pLayer->SetName(name);
// 	}
// 	else
// 		pLayer->SetName(_T(""));
// 	
	pLayer->ReadFrom(tab);

	return pLayer;
}

long CEpsAccess::GetFirstFtrLayerCode()
{
	if( m_pDaoTmpRs->IsEOF() )
	{		
		return -1;
	}
	
	COleVariant oleVar;
	m_pDaoTmpRs->GetFieldValue(_T("Code"),oleVar);
	m_lTmpCode = oleVar.lVal;

	m_pDaoTmpRs->GetFieldValue(_T("Type"),oleVar);
	m_nTmpType = oleVar.intVal;


	m_pDaoTmpRs->MoveNext();

	return m_lTmpCode;
}

long CEpsAccess::GetNextFtrLayerCode()
{
	if( m_pDaoTmpRs->IsEOF() )
	{		
		return -1;
	}
	
	COleVariant oleVar;
	m_pDaoTmpRs->GetFieldValue(_T("Code"),oleVar);
	m_lTmpCode = oleVar.lVal;

	m_pDaoTmpRs->GetFieldValue(_T("Type"),oleVar);
	m_nTmpType = oleVar.intVal;


	m_pDaoTmpRs->MoveNext();

	return m_lTmpCode;
}

CFeature *CEpsAccess::GetFirstPoint(CValueTable *exAttriTab)
{
	if (m_pDaoFtrRs->IsOpen())
	{
		m_pDaoFtrRs->Close();
	}
	m_pDaoFtrRs->Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from GeoPointTB;"),dbReadOnly);
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;
	
	return ReadPoint(exAttriTab);
}


CFeature *CEpsAccess::GetNextPoint(CValueTable *exAttriTab)
{
	m_pDaoFtrRs->MoveNext();
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;

	return ReadPoint(exAttriTab);
}

CFeature *CEpsAccess::ReadPoint(CValueTable *exAttriTab)
{
	CValueTable tab;
	CVariantEx var;
	
	
	long lValue, ObjID;
	int nValue;
	const char* pstr;
	double lfValue = 0;
	float fValue;
	CString strValue,strLayer;
	COleVariant oleVar;

	m_pDaoFtrRs->GetFieldValue(_T("Mark"),oleVar);
	if(oleVar.intVal%2==0)
	{
		CFeature *pFtr = new CFeature;
		pFtr->SetToDeleted(TRUE);
		return pFtr;
	}

	tab.BeginAddValueItem();
	m_pDaoFtrRs->GetFieldValue(_T("ID"),oleVar);
	ObjID = oleVar.lVal;

	m_pDaoFtrRs->GetFieldValue(_T("Code"),oleVar);
	m_lTmpCode = oleVar.lVal;
//	m_lTmpCode = m_pCodeList->FindCode(code,FALSE);


	m_pDaoFtrRs->GetFieldValue(_T("LayerName"),oleVar);
	m_strLayerName = LPCTSTR(oleVar.bstrVal);
	
	m_pDaoFtrRs->GetFieldValue(_T("LineColor"),oleVar);
	lValue = oleVar.lVal;
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)lValue);

	
		
//	nValue = m_ftrQuery.getIntField("ClassType");
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)CLS_DLGFEATURE);
	
//	nValue = m_ftrQuery.getIntField("GeoClass");
// 	m_pDaoFtrRs->GetFieldValue(_T("LineType"),oleVar);
// 	nValue = GeoTypeFrom(oleVar.iVal);
// 	m_pDaoTmpRs->FindFirst()
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)CLS_GEOPOINT);

	m_pDaoFtrRs->GetFieldValue(_T("Reference"),oleVar);
	fValue = oleVar.dblVal;

	fValue = (fValue/*-fValue+PI/2*/)*180/PI;
	if (m_nParaOutMode == modeClock)
	{
		fValue += PI/2;
	}
	tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&(CVariantEx)(_variant_t)fValue);
// 	lfValue = m_ftrQuery.getFloatField("Angle");
// 	tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&(CVariantEx)(_variant_t)lfValue);
	
	m_pDaoFtrRs->GetFieldValue(_T("Graphics"),oleVar);
	SSPoint3DLIST ssPts;
	if(!RestoreEdbGraphics(ssPts, oleVar))
		return FALSE;
	int nPt = ssPts.GetSize();
	if( nPt>0 )
	{
		CSSPoint3D* p = ssPts.GetData();
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.SetSize(nPt);
		PT_3DEX expt;
		expt.pencode = penLine;
		for (int i=0;i<nPt;i++)
		{
			COPY_3DPT(expt,*p);			
			arrPts.SetAt(i,expt);
			p++;
		}
		
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}
	
	tab.EndAddValueItem();

	CFeature *pFt = (CFeature*)CreateObject(tab);

	//扩展属性信息
	if (exAttriTab)
	{
		exAttriTab->BeginAddValueItem();
		CDaoRecordset rs;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),strLayer);
		if(rs.FindFirst(sql))
		{
			CDaoFieldInfo fieldinfo;
			COleVariant oleVar;
			COleDateTime ole;
			rs.GetFieldValue(_T("InnerAttrTbl_p"),oleVar);
			strValue = LPCTSTR(oleVar.bstrVal);
			CloseBigLayerTable(&rs);
			OpenTable(strValue,&rs);
			sql.Format(_T("ID = %ld"),ObjID);
			if(rs.FindFirst(sql))
			{
				int nsz = rs.GetFieldCount();
				for (int i=0;i<nsz;i++)
				{
					rs.GetFieldInfo(i,fieldinfo);
					rs.GetFieldValue(i,oleVar);
					switch(fieldinfo.m_nType)
					{
					case dbBoolean:	
						lValue = oleVar.boolVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbLong:
						lValue = oleVar.lVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbByte:
						lValue = oleVar.bVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbInteger:
						lValue = oleVar.intVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbSingle:
						fValue = oleVar.fltVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)fValue);
					case dbDouble:
						lfValue = oleVar.dblVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lfValue);
						break;
					case dbText:
						strValue = (LPCTSTR)oleVar.bstrVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
					    break;
					case dbMemo:
//						strValue = (LPCTSTR)oleVar.bstrVal;
//						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
					    break;
					case dbDate:
						ole = oleVar;
						strValue.Format(_T("%d,%d,%d,%d"),ole.GetYear(),ole.GetMonth(),ole.GetDay(),ole.GetHour());
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					default:
					    break;
					}				
				}
				CloseTable(&rs);
			}
			else
				CloseTable(&rs);

		}
		else
			CloseBigLayerTable(&rs);
		
		exAttriTab->EndAddValueItem();
	}
	
	return pFt;
}

void CEpsAccess::UpdateLayerName(int nFtrType)
{
	CString sql;
	sql.Format(_T("Code = %ld"),m_lTmpCode);
	if ( m_pDaoTmpRs->FindFirst(sql) )
	{
		COleVariant oleVar;
		m_pDaoTmpRs->GetFieldValue(_T("LayerName"),oleVar);
		m_strLayerName = (LPCTSTR)oleVar.bstrVal;
		m_pDaoTmpRs->GetFieldValue(_T("Byname"),oleVar);
		m_strByname = (LPCTSTR)oleVar.bstrVal;
		m_pDaoTmpRs->GetFieldValue(_T("Type"),oleVar);
		m_nTmpType = oleVar.intVal;
	}
	else
	{
// 		m_strLayerName = _T("Default");
// 		m_strByname = "";
// 		if( nFtrType==CLS_GEOPOINT || nFtrType==CLS_GEODIRPOINT || nFtrType==CLS_GEOMULTIPOINT || nFtrType==CLS_GEOTEXT)
// 			m_nTmpType = 0;
// 		else if( nFtrType==CLS_GEOCURVE || nFtrType==CLS_GEOPARALLEL )
// 			m_nTmpType = 4;
// 		else if( nFtrType==CLS_GEOSURFACE ||nFtrType==CLS_GEOMULTISURFACE )
// 			m_nTmpType = 5;	
		m_lTmpCode = -1;

	}
	
}

BOOL CEpsAccess::MatchDataType(const CDaoFieldInfo *info, const CVariantEx *pVar,COleVariant *pOleVar)
{
	int type = pVar->GetType();
	BOOL ret = FALSE;
	switch(type)
	{
	case VT_R4:
		{
			if (info->m_nType==dbSingle)
			{
				*pOleVar = (float)(_variant_t)(*pVar);
				ret = TRUE;
			}
			else if (info->m_nType==dbDouble)
			{
				*pOleVar = (double)(float)(_variant_t)(*pVar);
				ret = TRUE;
			}
		}		
		break;
	case VT_R8:
		{
			if (info->m_nType==dbSingle)
			{
				*pOleVar = (float)(double)(_variant_t)(*pVar);
				ret = TRUE;
			}
			else if (info->m_nType==dbDouble)
			{
				*pOleVar = (double)(_variant_t)(*pVar);
				ret = TRUE;
			}
		}		
		break;
	case VT_I4:
		{
			if (info->m_nType==dbLong)
			{
				*pOleVar = (long)(_variant_t)(*pVar);
				ret = TRUE;
			}
			else if (info->m_nType==dbInteger)
			{
				*pOleVar = (long)(_variant_t)(*pVar);
				ret = TRUE;
			}		
		}
		break;
	case VT_I2:
		{
			if (info->m_nType==dbLong)
			{
				*pOleVar = (long)(short)(_variant_t)(*pVar);
				ret = TRUE;
			}
			else if (info->m_nType==dbInteger)
			{
				*pOleVar = (long)(short)(_variant_t)(*pVar);
				ret = TRUE;
			}		
		}
		break;
	case VT_BOOL:
		{
			if (info->m_nType==dbLong)
			{
				*pOleVar = (long)(bool)(_variant_t)(*pVar);
				ret = TRUE;
			}
			else if (info->m_nType==dbInteger)
			{
				*pOleVar = (long)(bool)(_variant_t)(*pVar);
				ret = TRUE;
			}	
			else if (info->m_nType==dbBoolean)
			{
				*pOleVar = (long)(bool)(_variant_t)(*pVar);
				ret = TRUE;
			}		
		}
		break;
	case VT_BSTR:
		{
			if (info->m_nType==dbText)
			{
				(*pOleVar).SetString((LPCTSTR)(_bstr_t)(_variant_t)(*pVar),VT_BSTRT);
				ret = TRUE;
			}
		
		}
		break;
	default:
		break;
	}

	return ret;	
}

void CEpsAccess::SavePoint(CFeature *pFtr, CValueTable *exAttriTab)
{

	if (m_lTmpCode==-1)
	{
		m_lTmpCode = 0;
		UpdateLayerName();
	}
	int nGeoCls = GeoTypeFrom(m_nTmpType);
	if (CLS_GEOPOINT!=nGeoCls)
	{
//		m_lTmpCode = -1;
		if( nGeoCls==CLS_GEOPOINT || nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT)
			SavePoint(pFtr,exAttriTab);
		else
			SaveLine(pFtr,exAttriTab);
		return;
	}
	long ID = ++m_lObjID;

	long LineType = 0;
	long LineColor = pFtr->GetGeometry()->GetColor();

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pFtr->GetGeometry()->GetShape(arrPts);
	Envelope  e = pFtr->GetGeometry()->GetEnvelope();
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from GeoPointTB;"),dbAppendOnly);
	rs.AddNew();
	rs.SetFieldValueNull(_T("Memo"));


	rs.SetFieldValueNull(_T("UserID"));
	rs.SetFieldValueNull(_T("Relation"));
	
	rs.SetFieldValueNull(_T("ObjectName"));
	rs.SetFieldValueNull(_T("SymbolRef"));
	rs.SetFieldValueNull(_T("GraphicInfo"));
	COleVariant oleVar;
	oleVar = ID;
	rs.SetFieldValue(_T("ID"),oleVar);
	oleVar = m_lScale;
	rs.SetFieldValue(_T("Scale"),oleVar);
	
	oleVar = m_lTmpCode;
	rs.SetFieldValue(_T("Code"),oleVar);
	oleVar = long(1);
	rs.SetFieldValue(_T("Mark"),oleVar);

	oleVar = long(0);
	rs.SetFieldValue(_T("FillColor"),oleVar);
	if (pFtr->GetGeometry()->GetClassType() == CLS_GEODIRPOINT)
	{
		float angle = ((CGeoDirPoint*)pFtr->GetGeometry())->GetDirection()*PI/180;
// 		angle -= PI/2;
// 		oleVar = -angle;
		if (m_nParaOutMode == modeClock)
		{
			angle = PI - angle;
			angle = PI/2 - angle;
		}		
		oleVar = angle;
	}
	rs.SetFieldValue(_T("Reference"),oleVar);
//	oleVar = LPCTSTR(m_strLayerName);
	rs.SetFieldValue(_T("LayerName"),LPCTSTR(m_strLayerName));
	if (m_strByname.IsEmpty())
	{
		rs.SetFieldValueNull(_T("Byname"));
	}
	else
		rs.SetFieldValue(_T("Byname"),LPCTSTR(m_strByname));
	oleVar = (long)0;
	rs.SetFieldValue(_T("LineType"),oleVar);
	rs.SetFieldValue(_T("GroupID"),oleVar);

	CString sql0;
	sql0.Format(_T("Code = %ld"),m_lTmpCode);
	m_pDaoTmpRs->FindFirst(sql0);
	m_pDaoTmpRs->GetFieldValue(_T("LineColor"),oleVar);
	LineColor = oleVar.lVal;
	CDaoRecordset rs0;
	OpenTable(_T("ColorInfoTB"),&rs0);
	sql0.Format(_T("ID = %ld"),LineColor);
	rs0.FindFirst(sql0);
	rs0.GetFieldValue(_T("ColorValue"),oleVar);
	sql0 = (LPCTSTR)oleVar.pbstrVal;
	LineColor = StringTORGB(sql0);
	rs.SetFieldValue(_T("LineColor"),LineColor);
	CloseTable(&rs0);

	oleVar = (long)10;
	rs.SetFieldValue(_T("LineWidth"),oleVar);
	
	oleVar = e.m_xl;
	rs.SetFieldValue(_T("MinX"),oleVar);
	oleVar = e.m_yl;
	rs.SetFieldValue(_T("MinY"),oleVar);
	oleVar = e.m_xh;
	rs.SetFieldValue(_T("MaxX"),oleVar);
	oleVar = e.m_yh;
	rs.SetFieldValue(_T("MaxY"),oleVar);
	
	int  nsz = arrPts.GetSize();
	SSPoint3DLIST pointArr;
	pointArr.SetSize(nsz);

	// 高程注记和比高注记
	BOOL bHeight = FALSE, bDHeight = FALSE;
	int iHeight = 0, iDHeight = 0;
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	if (!pCfgLibManager)  return;
	if (!m_pDataSource) return;
	CScheme *pScheme = pCfgLibManager->GetScheme(m_pDataSource->GetScale());
	if (!pScheme)  return;
	
	CSchemeLayerDefine *pLayerDefine = pScheme->GetLayerDefine(m_strSchemeLayerName);
	if (pLayerDefine)
	{
		CPtrArray arrAnnoPtr;
		for(int i=0; i<pLayerDefine->GetSymbolCount(); i++)
		{
			CSymbol *pSymbol = pLayerDefine->GetSymbol(i);
			if (!pSymbol || pSymbol->GetType() != SYMTYPE_ANNOTATION)  continue;	
			
			if (((CAnnotation*)pSymbol)->m_nAnnoType == CAnnotation::AnnoType::Height)
			{
				bHeight = TRUE;
				if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Head)
				{
					iHeight = 0;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Tail)
				{
					iHeight = nsz - 1;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Middle)
				{
					iHeight = nsz/2;
				}
			}

			if (((CAnnotation*)pSymbol)->m_nAnnoType == CAnnotation::AnnoType::DHeight)
			{
				bDHeight = TRUE;
				if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Head)
				{
					iDHeight = 0;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Tail)
				{
					iDHeight = nsz - 1;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Middle)
				{
					iDHeight = nsz/2;
				}
			}
			
		}
	}

	for (int i=0;i<nsz;i++)
	{
		COPY_3DPT(pointArr[i],arrPts[i]);
		if ((bHeight && i == iHeight ) || (bDHeight && i == iDHeight))
		{
			pointArr[i].nType = e_SS_Height_Mark;
		}
		else
			pointArr[i].nType = 0;
	}

	

	if (bDHeight)
	{
		
		if (nsz > 1)
		{
			pointArr[iDHeight].z = pointArr[1].z - pointArr[0].z;			
		}
		else
		{
			pointArr[iDHeight].z = 0;
		}
		
	}

	StoreEdbGraphics(pointArr,oleVar);
	rs.SetFieldValue(_T("Graphics"),oleVar);
	rs.Update();
	rs.Close();
/*	if(exAttriTab)
	{
		int nCnt = exAttriTab->GetFieldCount();
		if (nCnt<=0)return;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),(LPCTSTR)m_strLayerName);
		rs.FindFirst(sql);
		rs.GetFieldValue(_T("InnerAttrTbl_p"),oleVar);
		CString attriTable = (LPCTSTR)oleVar.bstrVal;
		CloseBigLayerTable(&rs);
		OpenTable(attriTable,&rs);
		rs.AddNew();
		short colNum = rs.GetFieldCount();
		CDaoFieldInfo info;
		const CVariantEx *var;
		oleVar = ID;
		rs.SetFieldValue(_T("ID"),oleVar);
		for (int j=1;j<colNum;j++)
		{
			rs.GetFieldInfo(j,info);
			if(!exAttriTab->GetValue(0,info.m_strName,var))
				continue; 			
			if(MatchDataType(&info,var,&oleVar))		
				rs.SetFieldValue(j,oleVar);			
		}
		rs.Update();
		rs.Close();	
	}
*/
}


CFeature *CEpsAccess::GetFirstLine(CValueTable *exAttriTab)
{
	if (m_pDaoFtrRs->IsOpen())
	{
		m_pDaoFtrRs->Close();
	}
	m_pDaoFtrRs->Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from GeoLineTB;"),dbReadOnly);
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;
	
	return ReadLine(exAttriTab);
}


CFeature *CEpsAccess::GetNextLine(CValueTable *exAttriTab)
{
	m_pDaoFtrRs->MoveNext();
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;
	
	return ReadLine(exAttriTab);
}

CFeature *CEpsAccess::ReadLine(CValueTable *exAttriTab)
{
	CValueTable tab;
	CVariantEx var;
	
	long lValue, ObjID;
	int nValue;
	const char* pstr;
	float fValue;
	double lfValue = 0;
	CString strValue, strLayer;
	COleVariant oleVar;

	m_pDaoFtrRs->GetFieldValue(_T("Mark"),oleVar);
	if(oleVar.intVal%2==0)
	{
		CFeature *pFtr = new CFeature;
		pFtr->SetToDeleted(TRUE);
		return pFtr;
	}

	tab.BeginAddValueItem();

	m_pDaoFtrRs->GetFieldValue(_T("ID"),oleVar);
	ObjID = oleVar.lVal;
	
	m_pDaoFtrRs->GetFieldValue(_T("Code"),oleVar);
	m_lTmpCode = oleVar.lVal;
/*	m_lTmpCode = m_pCodeList->FindCode(m_lTmpCode,FALSE);*/
	
	m_pDaoFtrRs->GetFieldValue(_T("LayerName"),oleVar);
	m_strLayerName = LPCTSTR(oleVar.bstrVal);

	m_pDaoFtrRs->GetFieldValue(_T("LineColor"),oleVar);
	lValue = oleVar.lVal;
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)lValue);

	
	//	nValue = m_ftrQuery.getIntField("ClassType");
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)CLS_DLGFEATURE);
	
	long nLineType = e_Beeline;
	m_pDaoFtrRs->GetFieldValue(_T("LineType"),oleVar);
	nLineType = oleVar.lVal;
	
//	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)CLS_GEOCURVE);

	m_pDaoFtrRs->GetFieldValue(_T("LineWidth"),oleVar);
	float fLineWid = oleVar.lVal*0.01;
	if( fLineWid==0.1f )
		fLineWid = -1;

	tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTH,&(CVariantEx)(_variant_t)fLineWid);
	
	int nLen = 0;
	m_pDaoFtrRs->GetFieldValue(_T("Graphics"),oleVar);
	SSPoint3DLIST ssPts;
	if(!RestoreEdbGraphics(ssPts, oleVar))
		return FALSE;

	int geo = CLS_GEOCURVE;

	int nPt = ssPts.GetSize();
	if( nPt>0 )
	{
		CSSPoint3D* p = ssPts.GetData();
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.SetSize(nPt);
		PT_3DEX expt;
		if (nLineType == e_Beeline)
		{
			expt.pencode = penLine;
		}
		else if (nLineType == e_Arc)
		{
			expt.pencode = pen3PArc;
		}
		else if (nLineType == e_Curve)
		{
			expt.pencode = penSpline;
		}
		else
		{
			expt.pencode = penLine;
		}
		
		for (int i=0;i<nPt;i++)
		{
			COPY_3DPT(expt,*p);
			// 双线
			if (i-1 != 0 && (*(p-1)).nType == e_SS_Turn_Point)
			{
				geo = CLS_GEODCURVE;
				PT_3DEX expt1 = expt;
				expt1.pencode = penMove;
				arrPts.SetAt(i,expt1);
			}
			else
			{
				arrPts.SetAt(i,expt);
			}
			p++;
		}
		
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}

	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)geo);
	
	tab.EndAddValueItem();
	
	CFeature *pFt = (CFeature*)CreateObject(tab);

	//扩展属性信息
	if (exAttriTab)
	{
		exAttriTab->BeginAddValueItem();
		CDaoRecordset rs;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),strLayer);
		if(rs.FindFirst(sql))
		{
			CDaoFieldInfo fieldinfo;
			COleVariant oleVar;
			COleDateTime ole;
			rs.GetFieldValue(_T("InnerAttrTbl_l"),oleVar);
			strValue = LPCTSTR(oleVar.bstrVal);
			CloseBigLayerTable(&rs);
			OpenTable(strValue,&rs);
			sql.Format(_T("ID = %ld"),ObjID);
			if(rs.FindFirst(sql))
			{
				int nsz = rs.GetFieldCount();
				for (int i=0;i<nsz;i++)
				{
					rs.GetFieldInfo(i,fieldinfo);
					rs.GetFieldValue(i,oleVar);
					switch(fieldinfo.m_nType)
					{
					case dbBoolean:	
						lValue = oleVar.boolVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbLong:
						lValue = oleVar.lVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbByte:
						lValue = oleVar.bVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbInteger:
						lValue = oleVar.intVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbSingle:
						fValue = oleVar.fltVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)fValue);
					case dbDouble:
						lfValue = oleVar.dblVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lfValue);
						break;
					case dbText:
						strValue = (LPCTSTR)oleVar.bstrVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					case dbMemo:
						//						strValue = (LPCTSTR)oleVar.bstrVal;
						//						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					case dbDate:
						ole = oleVar;
						strValue.Format(_T("%d,%d,%d,%d"),ole.GetYear(),ole.GetMonth(),ole.GetDay(),ole.GetHour());
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					default:
						break;
					}				
				}
				CloseTable(&rs);
			}
			else
				CloseTable(&rs);
			
		}
		else
			CloseBigLayerTable(&rs);
		
		exAttriTab->EndAddValueItem();
	}
	
	return pFt;
}


void CEpsAccess::SaveLine(CFeature *pFtr, CValueTable *exAttriTab)
{
	//ID,Scale,Code,LayerName,LineType,LineColor,Min,Reference(0),Graphics	
	if (m_lTmpCode==-1)
	{
		m_lTmpCode = 1;
		UpdateLayerName();
	}
	int nGeoCls = GeoTypeFrom(m_nTmpType);
	if (CLS_GEOCURVE!=nGeoCls)
	{
//		m_lTmpCode = -1;
		if( nGeoCls==CLS_GEOSURFACE ||nGeoCls==CLS_GEOMULTISURFACE )
		{
			SaveSurface(pFtr,exAttriTab);
			return;
		}
	}	

	CGeometry *pGeo = pFtr->GetGeometry();

	long ID = ++m_lObjID;
	//	long Scale = m_lScale;
	long LineType = 0;
	long LineColor = pGeo->GetColor();
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	Envelope  e = pGeo->GetEnvelope();
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from GeoLineTB;"),dbAppendOnly);
	rs.AddNew();
	rs.SetFieldValueNull(_T("Memo"));
	rs.SetFieldValueNull(_T("UserID"));
	rs.SetFieldValueNull(_T("Relation"));
	
	rs.SetFieldValueNull(_T("ObjectName"));
	rs.SetFieldValueNull(_T("SymbolRef"));
	rs.SetFieldValueNull(_T("GraphicInfo"));
	COleVariant oleVar;
	oleVar = ID;
	rs.SetFieldValue(_T("ID"),oleVar);
	oleVar = m_lScale;
	rs.SetFieldValue(_T("Scale"),oleVar);

	oleVar = m_lTmpCode;
	rs.SetFieldValue(_T("Code"),oleVar);
	oleVar = long(1);
	rs.SetFieldValue(_T("Mark"),oleVar);
	oleVar = long(0);
	rs.SetFieldValue(_T("FillColor"),oleVar);
	rs.SetFieldValue(_T("Reference"),oleVar);
//	oleVar = (_bstr_t)m_strLayerName;
	rs.SetFieldValue(_T("LayerName"),LPCTSTR(m_strLayerName));
	if (m_strByname.IsEmpty())
	{
		rs.SetFieldValueNull(_T("Byname"));
	}
	else
		rs.SetFieldValue(_T("Byname"),LPCTSTR(m_strByname));
	oleVar = (long)1;
	rs.SetFieldValue(_T("LineType"),oleVar);
	oleVar = (long)0;
	rs.SetFieldValue(_T("GroupID"),oleVar);

	CString sql0;
	sql0.Format(_T("Code = %ld"),m_lTmpCode);
	m_pDaoTmpRs->FindFirst(sql0);
	m_pDaoTmpRs->GetFieldValue(_T("LineColor"),oleVar);
	LineColor = oleVar.lVal;
	CDaoRecordset rs0;
	OpenTable(_T("ColorInfoTB"),&rs0);
	sql0.Format(_T("ID = %ld"),LineColor);
	rs0.FindFirst(sql0);
	rs0.GetFieldValue(_T("ColorValue"),oleVar);
	sql0 = (LPCTSTR)oleVar.pbstrVal;
	LineColor = StringTORGB(sql0);
	rs.SetFieldValue(_T("LineColor"),LineColor);
	CloseTable(&rs0);

	oleVar = (long)10;
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		float fLineWid = ((CGeoCurveBase*)pFtr->GetGeometry())->m_fLineWidth;
		if( fLineWid>0 )
			oleVar = (long)(fLineWid*100);
	}
	rs.SetFieldValue(_T("LineWidth"),oleVar);
	
	oleVar = e.m_xl;
	rs.SetFieldValue(_T("MinX"),oleVar);
	oleVar = e.m_yl;
	rs.SetFieldValue(_T("MinY"),oleVar);
	oleVar = e.m_xh;
	rs.SetFieldValue(_T("MaxX"),oleVar);
	oleVar = e.m_yh;
	rs.SetFieldValue(_T("MaxY"),oleVar);
	
	int  nsz = arrPts.GetSize();

	// 点
	if (nsz < 2)
	{
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
		{
			oleVar = (long)e_Beeline;
			rs.SetFieldValue(_T("LineType"),oleVar);

			PT_3DEX expt = pGeo->GetCtrlPoint(0);
			expt.pencode = penLine;
			arrPts.Add(expt);

			nsz = arrPts.GetSize();
		}
		else
		{
			oleVar = (long)e_Point;	
			rs.SetFieldValue(_T("LineType"),oleVar);
		}
	}
	else
	{
		// 普通圆弧或若数据节点线型不统一地物都线串化后导出
		int code = arrPts[1].pencode;
		for (int i=2;i<nsz;i++)
		{		
			if (arrPts[i].pencode != penMove && arrPts[i].pencode != code)
			{
				break;
			}
		}
		
		// 普通圆弧打散导出
		if (i < nsz || arrPts[1].pencode == penArc || arrPts[1].pencode == penStream)
		{
			CGeometry *pGeo = pFtr->GetGeometry()->Linearize();
			pGeo->GetShape(arrPts);
			nsz = arrPts.GetSize();
		}
		else
		{
			
			if (arrPts[1].pencode == penSpline)
			{
				oleVar = (long)e_Curve;
			}
			// 三点弧导出为圆弧
			else if (arrPts[1].pencode == pen3PArc)
			{
				oleVar = (long)e_Arc;
			}
			else
			{
				oleVar = (long)e_Beeline;
			}
			rs.SetFieldValue(_T("LineType"),oleVar);
		}
	}	

	SSPoint3DLIST pointArr;
	pointArr.SetSize(nsz);

	// 高程注记和比高注记
	BOOL bHeight = FALSE, bDHeight = FALSE;
	int iHeight = 0 ,iDHeight = 0;
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	if (!pCfgLibManager)  return;
	if (!m_pDataSource) return;
	CScheme *pScheme = pCfgLibManager->GetScheme(m_pDataSource->GetScale());
	if (!pScheme)  return;
	
	CSchemeLayerDefine *pLayerDefine = pScheme->GetLayerDefine(m_strSchemeLayerName);
	if (pLayerDefine)
	{
		CPtrArray arrAnnoPtr;
		for(int i=0; i<pLayerDefine->GetSymbolCount(); i++)
		{
			CSymbol *pSymbol = pLayerDefine->GetSymbol(i);
			if (!pSymbol || pSymbol->GetType() != SYMTYPE_ANNOTATION)  continue;	
			
			if (((CAnnotation*)pSymbol)->m_nAnnoType == CAnnotation::AnnoType::Height)
			{
				bHeight = TRUE;
				if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Head)
				{
					iHeight = 0;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Tail)
				{
					iHeight = nsz - 1;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Middle)
				{
					iHeight = nsz/2;
				}
			}
			
			if (((CAnnotation*)pSymbol)->m_nAnnoType == CAnnotation::AnnoType::DHeight)
			{
				bDHeight = TRUE;
				if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Head)
				{
					iDHeight = 0;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Tail)
				{
					iDHeight = nsz - 1;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Middle)
				{
					iDHeight = nsz/2;
				}
			}

		}
	}

	for (int i=0;i<nsz;i++)
	{
		COPY_3DPT(pointArr[i],arrPts[i]);

		if (i != 0 && arrPts[i].pencode == penMove)
		{
			pointArr[i-1].nType = e_SS_Turn_Point;
		}
		else
		{
			if ( (bHeight && i == iHeight) || (bDHeight && i == iDHeight) )
			{
				pointArr[i].nType = e_SS_Height_Mark;
			}
			else
			{
				pointArr[i].nType = 0;
			}
		}
			
	}

	if (bDHeight)
	{
		
		if (nsz > 1)
		{
			pointArr[iDHeight].z = pointArr[1].z - pointArr[0].z;			
		}
		else
		{
			pointArr[iDHeight].z = 0;
		}
		
	}

	int nGeoClass = pFtr->GetGeometry()->GetClassType();
	if (nGeoClass == CLS_GEOSURFACE)
	{
		CSSPoint3D sspt;
		COPY_3DPT(sspt ,arrPts[0]);
		sspt.nType = 0;
		pointArr.Add(sspt);
	}
	StoreEdbGraphics(pointArr,oleVar);
	rs.SetFieldValue(_T("Graphics"),oleVar);
	rs.Update();
	rs.Close();
/*	if(exAttriTab)
	{
		int nCnt = exAttriTab->GetFieldCount();
		if (nCnt<=0)return;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),(LPCTSTR)m_strLayerName);
		rs.FindFirst(sql);
		rs.GetFieldValue(_T("InnerAttrTbl_l"),oleVar);
		CString attriTable = (LPCTSTR)oleVar.bstrVal;
		CloseBigLayerTable(&rs);
		OpenTable(attriTable,&rs);
		rs.AddNew();
		short colNum = rs.GetFieldCount();
		CDaoFieldInfo info;
		const CVariantEx *var;
		oleVar = ID;
		rs.SetFieldValue(_T("ID"),oleVar);
		for (int j=1;j<colNum;j++)
		{			
			rs.GetFieldInfo(j,info);
			if(!exAttriTab->GetValue(0,info.m_strName,var))
				continue; 		
			if(MatchDataType(&info,var,&oleVar))
				rs.SetFieldValue(j,oleVar);

		}
		rs.Update();
		rs.Close();	
	}*/
}


CFeature *CEpsAccess::GetFirstSurface(CValueTable *exAttriTab)
{
	if (m_pDaoFtrRs->IsOpen())
	{
		m_pDaoFtrRs->Close();
	}
	m_pDaoFtrRs->Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from GeoAreaTB;"),dbReadOnly);
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;
	
	return ReadSurface(exAttriTab);
}


CFeature *CEpsAccess::GetNextSurface(CValueTable *exAttriTab)
{
	m_pDaoFtrRs->MoveNext();
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;
	
	return ReadSurface(exAttriTab);
}

CFeature *CEpsAccess::ReadSurface(CValueTable *exAttriTab)
{
	CValueTable tab;
	CVariantEx var;
	
	
	long lValue, ObjID;
	int nValue;
	const char* pstr;
	float fValue;
	double lfValue = 0;
	CString strValue, strLayer;
	COleVariant oleVar;

	m_pDaoFtrRs->GetFieldValue(_T("Mark"),oleVar);
	if(oleVar.intVal%2==0)
	{
		CFeature *pFtr = new CFeature;
		pFtr->SetToDeleted(TRUE);
		return pFtr;
	}

	tab.BeginAddValueItem();

	m_pDaoFtrRs->GetFieldValue(_T("ID"),oleVar);
	ObjID = oleVar.lVal;
	
	m_pDaoFtrRs->GetFieldValue(_T("Code"),oleVar);
	m_lTmpCode = oleVar.lVal;
// 	m_lTmpCode = m_pCodeList->FindCode(m_lTmpCode,FALSE);

	m_pDaoFtrRs->GetFieldValue(_T("LayerName"),oleVar);
	m_strLayerName = LPCTSTR(oleVar.bstrVal);

	m_pDaoFtrRs->GetFieldValue(_T("LineColor"),oleVar);
	lValue = oleVar.lVal;
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)lValue);


	
	//	nValue = m_ftrQuery.getIntField("ClassType");
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)CLS_DLGFEATURE);
	
	long nLineType = e_Beeline;
	m_pDaoFtrRs->GetFieldValue(_T("LineType"),oleVar);
	nLineType = oleVar.lVal;

//	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)CLS_GEOSURFACE);
	
	
	// 	lfValue = m_ftrQuery.getFloatField("Angle");
	// 	tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&(CVariantEx)(_variant_t)lfValue);
	
	int nLen = 0;
	m_pDaoFtrRs->GetFieldValue(_T("Graphics"),oleVar);
	SSPoint3DLIST ssPts;
	if(!RestoreEdbGraphics(ssPts, oleVar))
		return FALSE;

	int geo = CLS_GEOCURVE;
	
	BOOL bColse = TRUE;
	int nPt = ssPts.GetSize();
	if( nPt>0 )
	{
		CSSPoint3D* p = ssPts.GetData();
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.SetSize(nPt);
		PT_3DEX expt;
		if (nLineType == e_Beeline)
		{
			expt.pencode = penLine;
		}
		else if (nLineType == e_Arc)
		{
			expt.pencode = pen3PArc;
		}
		else if (nLineType == e_Curve)
		{
			expt.pencode = penSpline;
		}
		else
		{
			expt.pencode = penLine;
		}
		for (int i=0;i<nPt;i++)
		{
			COPY_3DPT(expt,*p);
			// 双线
			if (i-1 != 0 && (*(p-1)).nType == e_SS_Turn_Point)
			{
				geo = CLS_GEODCURVE;
				PT_3DEX expt1 = expt;
				expt1.pencode = penMove;
				arrPts.SetAt(i,expt1);
			}
			else
			{
				arrPts.SetAt(i,expt);
			}
			p++;
		}

		if (nPt >= 3)
		{
			PT_3DEX pt0,pt1;
		
			pt0 = arrPts[0]; pt1 = arrPts[nPt-1];
			if( fabs(pt0.x-pt1.x)>1e-4 || fabs(pt0.y-pt1.y)>1e-4 || fabs(pt0.z-pt1.z)>1e-4 )
				bColse = FALSE;
		}

		if (geo == CLS_GEODCURVE)
		{
			tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)geo);
		}
		else if (!bColse || nPt < 3)
		{
			tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)CLS_GEOCURVE);
			
		}
		else
		{
			tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)CLS_GEOSURFACE);
			if (arrPts.GetSize() > 3 && bColse)
			{
				//arrPts.RemoveAt(arrPts.GetSize()-1);
			}
			
		}

		var.SetAsShape(arrPts);
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}
	
	tab.EndAddValueItem();
	
	CFeature *pFt = (CFeature*)CreateObject(tab);

	//扩展属性信息
	if (exAttriTab)
	{
		exAttriTab->BeginAddValueItem();
		CDaoRecordset rs;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),strLayer);
		if(rs.FindFirst(sql))
		{
			CDaoFieldInfo fieldinfo;
			COleVariant oleVar;
			COleDateTime ole;
			rs.GetFieldValue(_T("InnerAttrTbl_a"),oleVar);
			strValue = LPCTSTR(oleVar.bstrVal);
			CloseBigLayerTable(&rs);
			OpenTable(strValue,&rs);
			sql.Format(_T("ID = %ld"),ObjID);
			if(rs.FindFirst(sql))
			{
				int nsz = rs.GetFieldCount();
				for (int i=0;i<nsz;i++)
				{
					rs.GetFieldInfo(i,fieldinfo);
					rs.GetFieldValue(i,oleVar);
					switch(fieldinfo.m_nType)
					{
					case dbBoolean:	
						lValue = oleVar.boolVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbLong:
						lValue = oleVar.lVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbByte:
						lValue = oleVar.bVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbInteger:
						lValue = oleVar.intVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbSingle:
						fValue = oleVar.fltVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)fValue);
					case dbDouble:
						lfValue = oleVar.dblVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lfValue);
						break;
					case dbText:
						strValue = (LPCTSTR)oleVar.bstrVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					case dbMemo:
						//						strValue = (LPCTSTR)oleVar.bstrVal;
						//						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					case dbDate:
						ole = oleVar;
						strValue.Format(_T("%d,%d,%d,%d"),ole.GetYear(),ole.GetMonth(),ole.GetDay(),ole.GetHour());
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					default:
						break;
					}				
				}
				CloseTable(&rs);
			}
			else
				CloseTable(&rs);
			
		}
		else
			CloseBigLayerTable(&rs);
		
		exAttriTab->EndAddValueItem();
	}
	
	return pFt;
}


void CEpsAccess::SaveSurface(CFeature *pFtr, CValueTable *exAttriTab)
{
	if (m_lTmpCode==-1)
	{
		m_lTmpCode = 3;
		UpdateLayerName();
	}
	int nGeoCls = GeoTypeFrom(m_nTmpType);
	if (CLS_GEOSURFACE!=nGeoCls)
	{
//		m_lTmpCode = -1;

		if( nGeoCls==CLS_GEOCURVE ||nGeoCls==CLS_GEODCURVE || nGeoCls==CLS_GEOPARALLEL )
		{
			SaveLine(pFtr,exAttriTab); 
			return;
		}		
	}
	
	long ID = ++m_lObjID;
	//	long Scale = m_lScale;
	long LineType = 0;
	long LineColor = pFtr->GetGeometry()->GetColor();
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pFtr->GetGeometry()->GetShape(arrPts);
	Envelope  e = pFtr->GetGeometry()->GetEnvelope();
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from GeoAreaTB;"),dbAppendOnly);
	rs.AddNew();
	rs.SetFieldValueNull(_T("Memo"));

	
	rs.SetFieldValueNull(_T("UserID"));
	rs.SetFieldValueNull(_T("Relation"));
	
	rs.SetFieldValueNull(_T("ObjectName"));
	rs.SetFieldValueNull(_T("SymbolRef"));
	rs.SetFieldValueNull(_T("GraphicInfo"));
	COleVariant oleVar;
	oleVar = ID;
	rs.SetFieldValue(_T("ID"),oleVar);
	oleVar = m_lScale;
	rs.SetFieldValue(_T("Scale"),oleVar);

	oleVar = m_lTmpCode;
	rs.SetFieldValue(_T("Code"),oleVar);
	oleVar = long(1);
	rs.SetFieldValue(_T("Mark"),oleVar);

	oleVar = long(0);
	rs.SetFieldValue(_T("FillColor"),oleVar);

	rs.SetFieldValue(_T("Reference"),oleVar);
//	oleVar = LPCTSTR(m_strLayerName);
	rs.SetFieldValue(_T("LayerName"),LPCTSTR(m_strLayerName));
	if (m_strByname.IsEmpty())
	{
		rs.SetFieldValueNull(_T("Byname"));
	}
	else
		rs.SetFieldValue(_T("Byname"),LPCTSTR(m_strByname));
	oleVar = (long)1;
	rs.SetFieldValue(_T("LineType"),oleVar);
	oleVar = (long)0;
	rs.SetFieldValue(_T("GroupID"),oleVar);
	
	CString sql0;
	sql0.Format(_T("Code = %ld"),m_lTmpCode);
	m_pDaoTmpRs->FindFirst(sql0);
	m_pDaoTmpRs->GetFieldValue(_T("LineColor"),oleVar);
	LineColor = oleVar.lVal;
	CDaoRecordset rs0;
	OpenTable(_T("ColorInfoTB"),&rs0);
	sql0.Format(_T("ID = %ld"),LineColor);
	rs0.FindFirst(sql0);
	rs0.GetFieldValue(_T("ColorValue"),oleVar);
	sql0 = (LPCTSTR)oleVar.pbstrVal;
	LineColor = StringTORGB(sql0);
	rs.SetFieldValue(_T("LineColor"),LineColor);
	CloseTable(&rs0);

	oleVar = (long)10;
	if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		float fLineWid = ((CGeoCurveBase*)pFtr->GetGeometry())->m_fLineWidth;
		if( fLineWid>0 )
			oleVar = (long)(fLineWid*100);
	}
	rs.SetFieldValue(_T("LineWidth"),oleVar);
	
	oleVar = e.m_xl;
	rs.SetFieldValue(_T("MinX"),oleVar);
	oleVar = e.m_yl;
	rs.SetFieldValue(_T("MinY"),oleVar);
	oleVar = e.m_xh;
	rs.SetFieldValue(_T("MaxX"),oleVar);
	oleVar = e.m_yh;
	rs.SetFieldValue(_T("MaxY"),oleVar);
	
	int  nsz = arrPts.GetSize();
	
	// 点
	if (nsz < 2)
	{
		oleVar = (long)e_Point;	
		rs.SetFieldValue(_T("LineType"),oleVar);
	}
	else 
	{
		// 普通圆弧或若数据节点线型不统一地物都线串化后导出
		int code = arrPts[1].pencode;
		for (int i=2;i<nsz;i++)
		{		
			if (arrPts[i].pencode != penMove && arrPts[i].pencode != code)
			{
				break;
			}
		}
		
		// 普通圆弧打散导出
		if (i < nsz || arrPts[1].pencode == penArc || arrPts[1].pencode == penStream)
		{
			CGeometry *pGeo = pFtr->GetGeometry()->Linearize();
			pGeo->GetShape(arrPts);
			nsz = arrPts.GetSize();
		}
		else
		{
			
			if (arrPts[1].pencode == penSpline)
			{
				oleVar = (long)e_Curve;
			}
			// 三点弧导出为圆弧
			else if (arrPts[1].pencode == pen3PArc)
			{
				oleVar = (long)e_Arc;
			}
			else
			{
				oleVar = (long)e_Beeline;
			}
			rs.SetFieldValue(_T("LineType"),oleVar);
		}

	}	

	SSPoint3DLIST pointArr;
	pointArr.SetSize(nsz);
	
	// 高程注记和比高注记
	BOOL bHeight = FALSE, bDHeight = FALSE;
	int iHeight = 0, iDHeight = 0;
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	if (!pCfgLibManager)  return;
	if (!m_pDataSource) return;
	CScheme *pScheme = pCfgLibManager->GetScheme(m_pDataSource->GetScale());
	if (!pScheme)  return;
	
	CSchemeLayerDefine *pLayerDefine = pScheme->GetLayerDefine(m_strSchemeLayerName);
	if (pLayerDefine)
	{
		CPtrArray arrAnnoPtr;
		for(int i=0; i<pLayerDefine->GetSymbolCount(); i++)
		{
			CSymbol *pSymbol = pLayerDefine->GetSymbol(i);
			if (!pSymbol || pSymbol->GetType() != SYMTYPE_ANNOTATION)  continue;	
			
			if (((CAnnotation*)pSymbol)->m_nAnnoType == CAnnotation::AnnoType::Height)
			{
				bHeight = TRUE;
				if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Head)
				{
					iHeight = 0;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Tail)
				{
					iHeight = nsz - 1;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Middle)
				{
					iHeight = nsz/2;
				}
			}
			
			if (((CAnnotation*)pSymbol)->m_nAnnoType == CAnnotation::AnnoType::DHeight)
			{
				bDHeight = TRUE;
				if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Head)
				{
					iDHeight = 0;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Tail)
				{
					iDHeight = nsz - 1;
				}
				else if (((CAnnotation*)pSymbol)->m_nPlaceType == CAnnotation::PlaceType::Middle)
				{
					iDHeight = nsz/2;
				}
			}

		}
	}
	
	for (int i=0;i<nsz;i++)
	{
		COPY_3DPT(pointArr[i],arrPts[i]);
		
		if (i != 0 && arrPts[i].pencode == penMove)
		{
			pointArr[i-1].nType = e_SS_Turn_Point;
		}
		else
		{
			if ( (bHeight && i == iHeight) || (bDHeight && i == iDHeight) )
			{
				pointArr[i].nType = e_SS_Height_Mark;
			}
			else
			{
				pointArr[i].nType = 0;
			}
			
						
		}
		
	}

	if (bDHeight)
	{
		
		if (nsz > 1)
		{
			pointArr[iDHeight].z = pointArr[1].z - pointArr[0].z;			
		}
		else
		{
			pointArr[iDHeight].z = 0;
		}
		
	}

	int nGeoClass = pFtr->GetGeometry()->GetClassType();
	if (nGeoClass == CLS_GEOSURFACE)
	{
		CSSPoint3D sspt;
		COPY_3DPT(sspt ,arrPts[0]);
		sspt.nType = 0;
		pointArr.Add(sspt);
	}	
	StoreEdbGraphics(pointArr,oleVar);
	rs.SetFieldValue(_T("Graphics"),oleVar);
	rs.Update();
	rs.Close();
/*	if(exAttriTab)
	{
		int nCnt = exAttriTab->GetFieldCount();
		if (nCnt<=0)return;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),(LPCTSTR)m_strLayerName);
		rs.FindFirst(sql);
		rs.GetFieldValue(_T("InnerAttrTbl_a"),oleVar);
		CString attriTable = (LPCTSTR)oleVar.bstrVal;
		CloseBigLayerTable(&rs);
		OpenTable(attriTable,&rs);
		rs.AddNew();
		short colNum = rs.GetFieldCount();
		CDaoFieldInfo info;
		const CVariantEx *var;
		oleVar = ID;
		rs.SetFieldValue(_T("ID"),oleVar);
		for (int j=1;j<colNum;j++)
		{
			rs.GetFieldInfo(j,info);
			if(!exAttriTab->GetValue(0,info.m_strName,var))
				continue; 			
			if(MatchDataType(&info,var,&oleVar))
				rs.SetFieldValue(j,oleVar);			
			
		}
		rs.Update();
		rs.Close();	
	}*/
}


CFeature *CEpsAccess::GetFirstText(CValueTable *exAttriTab)
{
	if (m_pDaoFtrRs->IsOpen())
	{
		m_pDaoFtrRs->Close();
	}
	m_pDaoFtrRs->Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from MarkNoteTB;"),dbReadOnly);
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;
	
	return ReadText(exAttriTab);
}


CFeature *CEpsAccess::GetNextText(CValueTable *exAttriTab)
{
	m_pDaoFtrRs->MoveNext();
	if( m_pDaoFtrRs->IsEOF() )
		return NULL;
	
	return ReadText(exAttriTab);
}

CFeature *CEpsAccess::ReadText(CValueTable *exAttriTab)
{
	CValueTable tab;
	CVariantEx var;	
	
	float fValue;
	int nValue;
	long lValue, ObjID;
	double lfValue = 0;
	CString strValue, strLayer;
	COleVariant oleVar;	

	m_pDaoFtrRs->GetFieldValue(_T("Mark"),oleVar);
	if(oleVar.intVal%2==0)
	{
		CFeature *pFtr = new CFeature;
		pFtr->SetToDeleted(TRUE);
		return pFtr;
	}

	tab.BeginAddValueItem();

	m_pDaoFtrRs->GetFieldValue(_T("ID"),oleVar);
	ObjID = oleVar.lVal;
	
	m_pDaoFtrRs->GetFieldValue(_T("LayerName"),oleVar);
	m_strLayerName = LPCTSTR(oleVar.bstrVal);
	m_lTmpCode = -1;//特殊处理的标志
	
	nValue = CLS_DLGFEATURE;
	tab.AddValue(FIELDNAME_CLSTYPE,&(CVariantEx)(_variant_t)(long)nValue);
	
	m_pDaoFtrRs->GetFieldValue(_T("LineType"),oleVar);
	nValue = GeoTypeFrom(oleVar.iVal);	
	tab.AddValue(FIELDNAME_GEOCLASS,&(CVariantEx)(_variant_t)(long)CLS_GEOTEXT);
	
	m_pDaoFtrRs->GetFieldValue(_T("FontColor"),oleVar);
	lValue = oleVar.lVal;
	tab.AddValue(FIELDNAME_GEOCOLOR,&(CVariantEx)(_variant_t)lValue);
	
// 	nValue = m_ftrQuery.getIntField("LayerID");
// 	m_nCurLayID = nValue;

	m_pDaoFtrRs->GetFieldValue(_T("StringText"),oleVar);
	strValue = LPCTSTR(oleVar.bstrVal);
	tab.AddValue(FIELDNAME_GEOTEXT_CONTENT,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
	int charNum = strValue.GetLength();

	m_pDaoFtrRs->GetFieldValue(_T("FontName"),oleVar);
	strValue = LPCTSTR(oleVar.bstrVal);
	tab.AddValue(FIELDNAME_GEOTEXT_FONT,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);

	m_pDaoFtrRs->GetFieldValue(_T("FontHeight"),oleVar);
	nValue = oleVar.intVal;//0.1mm
	tab.AddValue(FIELDNAME_GEOTEXT_CHARHEIGHT,&(CVariantEx)(_variant_t)(nValue*0.01));
	float charHeight = nValue*0.01;

	m_pDaoFtrRs->GetFieldValue(_T("FontWidth"),oleVar);
	nValue = oleVar.intVal;//0.1mm	
	tab.AddValue(FIELDNAME_GEOTEXT_CHARWIDTHS,&(CVariantEx)(_variant_t)(nValue*0.01/charHeight));
	int charwidth = nValue*0.01;
	m_pDaoFtrRs->GetFieldValue(_T("FontInterval"),oleVar);
	nValue = oleVar.intVal;//0.1mm
	tab.AddValue(FIELDNAME_GEOTEXT_CHARINTVS,&(CVariantEx)(_variant_t)(double)0);

// 	lfValue = m_ftrQuery.getFloatField("LineIntv");
// 	tab.AddValue(FIELDNAME_GEOTEXT_LINEINTV,&(CVariantEx)(_variant_t)lfValue);

	m_pDaoFtrRs->GetFieldValue(_T("Alignment"),oleVar);
	nValue = oleVar.intVal;//0.1mm
	tab.AddValue(FIELDNAME_GEOTEXT_ALIGNTYPE,&(CVariantEx)(_variant_t)(long)ConvertAlignment(nValue,FALSE));
	int aligntype = nValue;
// 	nValue = m_ftrQuery.getIntField("Incline");
// 	tab.AddValue(FIELDNAME_GEOTEXT_INCLINE,&(CVariantEx)(_variant_t)(long)nValue);

	m_pDaoFtrRs->GetFieldValue(_T("DownAngle"),oleVar);
	fValue = oleVar.fltVal;//0.1mm
	tab.AddValue(FIELDNAME_GEOTEXT_INCLINEANGLE,&(CVariantEx)(_variant_t)(double)fValue);

// 	nValue = m_ftrQuery.getIntField("PlaceType");
// 	tab.AddValue(FIELDNAME_GEOTEXT_PLACETYPE,&(CVariantEx)(_variant_t)(long)nValue);
	
	m_pDaoFtrRs->GetFieldValue(_T("Graphics"),oleVar);
	SSPoint3DLIST ssPts;
	if(!RestoreEdbGraphics(ssPts, oleVar))
		return FALSE;
	int nPt = ssPts.GetSize();
	if( nPt>0 )
	{
		CSSPoint3D* p = ssPts.GetData();
		CArray<PT_3DEX,PT_3DEX> arrPts;
		arrPts.SetSize(nPt);
		PT_3DEX expt;
		expt.pencode = penLine;
		for (int i=0;i<nPt;i++)
		{
			COPY_3DPT(expt,*p);
			arrPts.SetAt(i,expt);
			p++;
		}
		var.SetAsShape(arrPts);
		
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}
	
	tab.EndAddValueItem();
	
	CFeature *pFt = (CFeature*)CreateObject(tab);

	//扩展属性信息
	if (exAttriTab)
	{
		exAttriTab->BeginAddValueItem();
		CDaoRecordset rs;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),strLayer);
		if(rs.FindFirst(sql))
		{
			CDaoFieldInfo fieldinfo;
			COleVariant oleVar;
			COleDateTime ole;
			rs.GetFieldValue(_T("InnerAttrTbl_n"),oleVar);
			strValue = LPCTSTR(oleVar.bstrVal);
			CloseBigLayerTable(&rs);
			OpenTable(strValue,&rs);
			sql.Format(_T("ID = %ld"),ObjID);
			if(rs.FindFirst(sql))
			{
				int nsz = rs.GetFieldCount();
				for (int i=0;i<nsz;i++)
				{
					rs.GetFieldInfo(i,fieldinfo);
					rs.GetFieldValue(i,oleVar);
					switch(fieldinfo.m_nType)
					{
					case dbBoolean:	
						lValue = oleVar.boolVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbLong:
						lValue = oleVar.lVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbByte:
						lValue = oleVar.bVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbInteger:
						lValue = oleVar.intVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lValue);
						break;
					case dbSingle:
						fValue = oleVar.fltVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)fValue);
					case dbDouble:
						lfValue = oleVar.dblVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)lfValue);
						break;
					case dbText:
						strValue = (LPCTSTR)oleVar.bstrVal;
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					case dbMemo:
						//						strValue = (LPCTSTR)oleVar.bstrVal;
						//						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					case dbDate:
						ole = oleVar;
						strValue.Format(_T("%d,%d,%d,%d"),ole.GetYear(),ole.GetMonth(),ole.GetDay(),ole.GetHour());
						exAttriTab->AddValue(fieldinfo.m_strName,&(CVariantEx)(_variant_t)(LPCTSTR)strValue);
						break;
					default:
						break;
					}				
				}
				CloseTable(&rs);
			}
			else
				CloseTable(&rs);
			
		}
		else
			CloseBigLayerTable(&rs);
		
		exAttriTab->EndAddValueItem();
	}
	
	return pFt;
}


void CEpsAccess::SaveText(CFeature *pFtr, CValueTable *exAttriTab)
{
	long ID = ++m_lObjID;
	//	long Scale = m_lScale;
	long LineType = 0;
	long LineColor = pFtr->GetGeometry()->GetColor();
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pFtr->GetGeometry()->GetShape(arrPts);
	Envelope  e = pFtr->GetGeometry()->GetEnvelope();
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from MarkNoteTB;"),dbAppendOnly);
	rs.AddNew();
	rs.SetFieldValueNull(_T("Memo"));
	rs.SetFieldValueNull(_T("UserID"));

// 	if (m_strByname.IsEmpty())
// 	{
		rs.SetFieldValueNull(_T("Byname"));
//	}
// 	else
// 		rs.SetFieldValue(_T("Byname"),LPCTSTR(m_strByname));

	rs.SetFieldValueNull(_T("GraphicInfo"));

	COleVariant oleVar;
	oleVar = 0L;
	rs.SetFieldValue(_T("BorderColor"),oleVar);
	oleVar = 0L;
	rs.SetFieldValue(_T("BorderStyle"),oleVar);
	oleVar = (long)16777215;
	rs.SetFieldValue(_T("FillColor"),oleVar);
	oleVar = (long)0;
	rs.SetFieldValue(_T("Thickness"),oleVar);
	oleVar = long(1);
	rs.SetFieldValue(_T("Mark"),oleVar);
	// 填充数据项
	CValueTable tab;
	const CVariantEx *pvar;
	
	tab.BeginAddValueItem();
	pFtr->WriteTo(tab);
	tab.EndAddValueItem();
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CONTENT,pvar) )
	{
		rs.SetFieldValue(_T("StringText"),(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
	}
	else
	{
		rs.SetFieldValueNull(_T("StringText"));
	}

	CString FontName;
	float FontHeight;
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_FONT,pvar) )
	{
		FontName = (LPCTSTR)(_bstr_t)(_variant_t)*pvar;
		rs.SetFieldValue(_T("FontName"),(LPCTSTR)FontName);
	}
	else
	{
		rs.SetFieldValueNull(_T("FontName"));
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARHEIGHT,pvar) )
	{
		FontHeight = 100*(double)(_variant_t)*pvar;
		rs.SetFieldValue(_T("FontHeight"),100*(double)(_variant_t)*pvar);
	}
	else
	{
		rs.SetFieldValueNull(_T("FontHeight"));
	}

	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARWIDTHS,pvar) )
	{
		oleVar = (double)(FontHeight*(double)(_variant_t)*pvar);
		rs.SetFieldValue(_T("FontWidth"),oleVar);
	}	
	else
	{
		rs.SetFieldValueNull(_T("FontWidth"));
	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_CHARINTVS,pvar) )
	{
		short tmp = 0;
		oleVar = (short)(tmp>10000?10000:tmp);
		rs.SetFieldValue(_T("FontInterval"),oleVar);
	}
	else
	{
		rs.SetFieldValueNull(_T("FontInterval"));
	}
	
	oleVar = (long)400;
	rs.SetFieldValue(_T("FontWeight"),oleVar);

 	if( tab.GetValue(0,FIELDNAME_GEOTEXT_ALIGNTYPE,pvar) )
 	{
 		oleVar = (BYTE)ConvertAlignment((long)(_variant_t)*pvar,TRUE);
 		rs.SetFieldValue(_T("Alignment"),oleVar);
 	}
 	else
 	{
		oleVar = (BYTE)e_LeftBottom;
 		rs.SetFieldValue(_T("Alignment"),oleVar);
 	}
	
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINE,pvar) )
	{
;//		m_stm.bind(idx++,(long)(_variant_t)*pvar);
	}
	else
	{
;//		m_stm.bindNull(idx++);
	}
	
	oleVar = 0.0f;
	if( tab.GetValue(0,FIELDNAME_GEOTEXT_INCLINEANGLE,pvar) )
	{
;//		m_stm.bind(idx++,(double)(_variant_t)*pvar);
	}
	else
	{
;//		m_stm.bindNull(idx++);
	}

	oleVar = 0.0f;
	rs.SetFieldValue(_T("IlaticAngle"),oleVar);
	
	oleVar = 0.0f;
	rs.SetFieldValue(_T("WordAngle"),oleVar);
	rs.SetFieldValue(_T("DownAngle"),oleVar);
	oleVar = (long)0;
	rs.SetFieldValue(_T("Underline"),oleVar);

	oleVar = ID;
	rs.SetFieldValue(_T("ID"),oleVar);
	oleVar = m_lScale;
	rs.SetFieldValue(_T("Scale"),oleVar);
	oleVar = (long)0;
	rs.SetFieldValue(_T("FontClass"),oleVar);
//	oleVar = LPCTSTR(m_strLayerName);
	rs.SetFieldValue(_T("LayerName"),/*LPCTSTR(m_strLayerName)*/_T("DEFAULT"));
	oleVar = (long)16;
	rs.SetFieldValue(_T("LineType"),oleVar);
	oleVar = (long)0;
	rs.SetFieldValue(_T("GroupID"),oleVar);
	if (LineColor==-1)
	{
		oleVar = (long)16777215;
	}
	else
		oleVar = LineColor;
	rs.SetFieldValue(_T("FontColor"),oleVar);
	oleVar = (long)20;
	rs.SetFieldValue(_T("LineWidth"),oleVar);
	
	oleVar = e.m_xl;
	rs.SetFieldValue(_T("MinX"),oleVar);
	oleVar = e.m_yl;
	rs.SetFieldValue(_T("MinY"),oleVar);
	oleVar = e.m_xh;
	rs.SetFieldValue(_T("MaxX"),oleVar);
	oleVar = e.m_yh;
	rs.SetFieldValue(_T("MaxY"),oleVar);
	
	int  nsz = arrPts.GetSize();
	SSPoint3DLIST pointArr;
	pointArr.SetSize(nsz);
	for (int i=0;i<nsz;i++)
	{
		COPY_3DPT(pointArr[i],arrPts[i]);
		pointArr[i].nType = 0;
	}
	StoreEdbGraphics(pointArr,oleVar);
	rs.SetFieldValue(_T("Graphics"),oleVar);
	rs.Update();
	rs.Close();
/*	if(exAttriTab)
	{
		int nCnt = exAttriTab->GetFieldCount();
		if (nCnt<=0)return;
		OpenBigLayerTable(&rs);
		CString sql;
		sql.Format(_T("LayerName = '%s'"),(LPCTSTR)m_strLayerName);
		rs.FindFirst(sql);
		rs.GetFieldValue(_T("InnerAttrTbl_n"),oleVar);
		CString attriTable = (LPCTSTR)oleVar.bstrVal;
		CloseBigLayerTable(&rs);
		OpenTable(attriTable,&rs);
		rs.AddNew();
		short colNum = rs.GetFieldCount();
		CDaoFieldInfo info;
		const CVariantEx *var;
		oleVar = ID;
		rs.SetFieldValue(_T("ID"),oleVar);
		for (int j=1;j<colNum;j++)
		{
			rs.GetFieldInfo(j,info);
			if(!exAttriTab->GetValue(0,info.m_strName,var))
				continue; 			
			if(MatchDataType(&info,var,&oleVar))
				rs.SetFieldValue(j,oleVar);		
		}
		rs.Update();
		rs.Close();	
	}*/
}

int CEpsAccess::GetFtrLayerCount()
{
// 	CppSQLite3Query query = m_db.execQuery("select count(*) from DLGVectorLayer;");
// 	return query.getIntField(0);
	return 0;
}

int CEpsAccess::GetFeatureCount()
{	
	CDaoTableDef tabledef(m_pDaoDb);
	
	int sum = 0;	
	tabledef.Open(_T("GeoPointTB"));
	sum += tabledef.GetRecordCount();
	tabledef.Close();
	tabledef.Open(_T("GeoLineTB"));
	sum += tabledef.GetRecordCount();
	tabledef.Close();
	tabledef.Open(_T("GeoAreaTB"));
	sum += tabledef.GetRecordCount();
	tabledef.Close();
	tabledef.Open(_T("MarkNoteTB"));
	sum += tabledef.GetRecordCount();
	tabledef.Close();	
	return sum;
}

//读取地物
CFeature *CEpsAccess::GetFirstFeature(long &idx, CValueTable *exAttriTab)
{
	idx = 0;
	CFeature *pFtr = GetFirstPoint(exAttriTab);
	if( pFtr!=NULL )return pFtr;
	idx = 1;
	
	pFtr = GetFirstLine(exAttriTab);
	if( pFtr!=NULL )return pFtr;
	idx = 2;

	pFtr = GetFirstSurface(exAttriTab);
	if( pFtr!=NULL )return pFtr;
	idx = 3;

	pFtr = GetFirstText(exAttriTab);
	if( pFtr!=NULL )return pFtr;

	idx = -1;
	return NULL;
}


CFeature *CEpsAccess::GetNextFeature(long &idx, CValueTable *exAttriTab)
{
	CFeature *pFtr = NULL;

	if( idx<0 )return NULL;

	//上个是点数据
	if( idx==0 )
	{
		pFtr = GetNextPoint();
		if( pFtr!=NULL )return pFtr;
	}

	//上个是线数据
	if( idx==1 )
	{
		pFtr = GetNextLine();
		if( pFtr!=NULL )return pFtr;
	}
	else if( idx<1 )
	{
		idx = 1;
		pFtr = GetFirstLine();
		if( pFtr!=NULL )return pFtr;
	}

	//上个是面数据
	if( idx==2 )
	{
		pFtr = GetNextSurface();
		if( pFtr!=NULL )return pFtr;
	}
	else if( idx<2 )
	{
		idx = 2;
		pFtr = GetFirstSurface();
		if( pFtr!=NULL )return pFtr;
	}

	//上个是文本数据
	if( idx==3 )
	{
		pFtr = GetNextText();
		if( pFtr!=NULL )return pFtr;
	}
	else if( idx<3 )
	{
		idx = 3;
		pFtr = GetFirstText();
		if( pFtr!=NULL )return pFtr;
	}
	
	return NULL;
}


int CEpsAccess::GetCurFtrLayID()
{
	return m_nCurLayID;
}


int CEpsAccess::GetCurFtrGrpID()
{
	return 0;
}


//更新数据源信息
void CEpsAccess::WriteDataSourceInfo(CDataSourceEx *pDS)
{
	CDaoRecordset rs(m_pDaoDb);
	rs.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("select * from IniInfoTB;"));
	ASSERT(!rs.IsEOF());
	
	CValueTable tab;
	const CVariantEx *pvar;
	COleVariant oleVar;
	tab.BeginAddValueItem();
	pDS->WriteTo(tab);
	tab.EndAddValueItem();
	
	TCHAR buf[20];
	if( tab.GetValue(0,FIELDNAME_SCALE,pvar) )
	{
		long scale = (long)(_variant_t)*pvar;
		/*	oleVar.SetString(_ltot(scale,buf,10), VT_BSTR);*/
		if(rs.Find(AFX_DAO_FIRST,_T("KeyName = 'Scale'")))
		{
			rs.Edit();
			rs.SetFieldValue(_T("StringValue"),_ltot(scale,buf,10));	
			rs.Update();
		}
		else
		{
			rs.AddNew();
			rs.SetFieldValue(_T("Section"),_T("Project"));
			rs.SetFieldValue(_T("KeyName"),_T("Scale"));		
			rs.SetFieldValue(_T("StringValue"),_ltot(scale,buf,10));
			rs.Update();
		}
	
	}
	
	if( tab.GetValue(0,FIELDNAME_BOUND,pvar) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pvar->GetShape(arrPts);
		if( arrPts.GetSize()==4 )
		{
			Envelope e;
			e.CreateFromPts(arrPts.GetData(),4,sizeof(PT_3DEX));
			TCHAR Ch[128];
			_stprintf(Ch,_T("%lf,%lf,%lf,%lf"),e.m_yl,e.m_xl,e.m_yh,e.m_xh);					
			//			oleVar.SetString(Ch ,VT_BSTR);
			if(rs.Find(AFX_DAO_FIRST,_T("KeyName = 'WorkRange'")))
			{
				rs.Edit();
				rs.SetFieldValue(_T("StringValue"),Ch);
				rs.Update();
			}
			else
			{
				rs.AddNew();
				rs.SetFieldValue(_T("StringValue"),Ch);
				rs.SetFieldValue(_T("KeyName"),_T("WorkRange"));
				rs.SetFieldValue(_T("Section"),_T("Draw Set Main"));
				rs.Update();
			}			
		}
	}	
	rs.Close();
}



//更新层
BOOL CEpsAccess::SaveFtrLayer(CFtrLayer *pLayer)
{
// 	if (!pLayer)  return FALSE;
// 	char line[1024] = {0};
// 
// 	// 查询层是否存在
// 	sprintf(line,"select ID from DLGVectorLayer where ID=%d;",pLayer->GetID());
// 	
// 	CppSQLite3Query query = m_db.execQuery(line);
// 
// 	// 不存在，需要新建
// 	if( query.eof() )
// 	{
// 		sprintf(line,"insert into DLGVectorLayer ( ID, Name, MapName, Color, Visible, Locked, Inherent ) values ( %d, ?, ?, ?, ?, ? ,? ) ;",
// 			pLayer->GetID());
// 	}
// 	else
// 	{
// 		sprintf(line,"update DLGVectorLayer set Name=?, MapName=?, Color=?, Visible=?, Locked=?, Inherent=? where ID=%d;",pLayer->GetID());
// 	}
// 
// 	CppSQLite3Statement stm = m_db.compileStatement(line);
// 	
// 	CValueTable tab;
// 	const CVariantEx *pvar;
// 
// 	tab.BeginAddValueItem();
// 	pLayer->WriteTo(tab);
// 	tab.EndAddValueItem();
// 	
// 	if( tab.GetValue(0,FIELDNAME_LAYERNAME,pvar) )
// 	{
// 		stm.bind(1,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
// 	}
// 	
// 	if( tab.GetValue(0,FIELDNAME_LAYMAPNAME,pvar) )
// 	{
// 		stm.bind(2,(LPCTSTR)(_bstr_t)(_variant_t)*pvar);
// 	}
// 
// 	if( tab.GetValue(0,FIELDNAME_LAYCOLOR,pvar) )
// 	{
// 		stm.bind(3,(long)(_variant_t)*pvar);
// 	}
// 
// 	if( tab.GetValue(0,FIELDNAME_LAYVISIBLE,pvar) )
// 	{
// 		stm.bind(4,(long)(_variant_t)*pvar);
// 	}
// 
// 	if( tab.GetValue(0,FIELDNAME_LAYLOCKED,pvar) )
// 	{
// 		stm.bind(5,(long)(_variant_t)*pvar);
// 	}
// 
// 	if( tab.GetValue(0,FIELDNAME_LAYINHERENT,pvar) )
// 	{
// 		stm.bind(6,(long)(_variant_t)*pvar);
// 	}
// 	
// 	stm.execDML();
// 	stm.finalize();

	return TRUE;
}


BOOL CEpsAccess::DelFtrLayer(CFtrLayer *pLayer)
{
// 	char line[1024] = {0};
// 	sprintf(line,"select * from DLGVectorLayer where ID=%d;",pLayer->GetID());
// 
// 	CppSQLite3Query query = m_db.execQuery(line);
// 	if( query.eof() )
// 		return FALSE;
// 
// 	sprintf(line,"delete from DLGVectorLayer where ID=%d;",pLayer->GetID());
// 	m_db.execDML(line);

	return TRUE;
}



//更新地物
BOOL CEpsAccess::SetCurFtrLayID(int id)
{
	m_nCurLayID = id;
	return TRUE;
}


BOOL CEpsAccess::SetCurFtrGrpID(int id)
{
	return TRUE;
}




BOOL CEpsAccess::SaveFeature(CFeature *pFt, CValueTable *exAttriTab)
{
	int nGeoCls = pFt->GetGeometry()->GetClassType();

	if( nGeoCls==CLS_GEOPOINT || nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT)
		SavePoint(pFt,exAttriTab);
	else if (nGeoCls==CLS_GEOPARALLEL)
	{
		if (m_nParaOutMode == modeSeparate)
		{
			CGeometry *pGeo1,*pGeo2;
			if(((CGeoParallel*)pFt->GetGeometry())->Separate(pGeo1,pGeo2))
			{
				CFeature *pFtr = pFt->Clone();
				if(pGeo1)
				{
					pFtr->SetGeometry(pGeo1);
					SaveLine(pFtr,exAttriTab);	
					pFtr->SetGeometry(NULL);
				}

				if(pGeo2)
				{
					pFtr->SetGeometry(pGeo2);
					SaveLine(pFtr,exAttriTab);	
					pFtr->SetGeometry(NULL);
				}

				delete pFtr;
			}
			
		}
		// 平行线导出为面
		else if (m_nParaOutMode == modeSurface)
		{
			CArray<PT_3DEX,PT_3DEX> basepts;
			CGeoParallel *pParallel = (CGeoParallel*)pFt->GetGeometry();
			CArray<PT_3DEX,PT_3DEX> ptspar;
			pParallel->GetShape(basepts);
			pParallel->GetParallelShape(ptspar);
			
			for (int i=ptspar.GetSize()-1; i>=0; i--)
			{
				basepts.Add(ptspar.GetAt(i));
			}

			basepts.Add(basepts[0]);

			CFeature *pFtr = pFt->Clone();
			pFtr->GetGeometry()->CreateShape(basepts.GetData(),basepts.GetSize());

			SaveSurface(pFtr,exAttriTab);
			delete pFtr;
		}
		else if (m_nParaOutMode == modeSingle)
		{
			CArray<PT_3DEX,PT_3DEX> basepts, pts;
			CGeoParallel *pParallel = (CGeoParallel*)pFt->GetGeometry();
			pParallel->GetShape(basepts);

			if (pParallel->GetWidth() < 0)
			{
				for (int i=basepts.GetSize()-1; i>=0; i--)
				{
					pts.Add(basepts.GetAt(i));
				}
			}
			else
			{
				pts.Copy(basepts);
			}

			CFeature *pFtr = pFt->Clone();
			pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
			
			SaveLine(pFtr,exAttriTab);
			delete pFtr;
		}
		
		
	}
	else if( nGeoCls==CLS_GEOCURVE/* || nGeoCls==CLS_GEOPARALLEL */)
		SaveLine(pFt,exAttriTab);
	else if (nGeoCls == CLS_GEODCURVE)
	{
		if (m_nParaOutMode == modeSurface)
		{
			CGeoDCurve *pDGeo = (CGeoDCurve*)pFt->GetGeometry();
			CArray<PT_3DEX,PT_3DEX> arrBase, arrAssist;
			pDGeo->GetBaseShape(arrBase);
			pDGeo->GetAssistShape(arrAssist);
			
			int num = arrAssist.GetSize();
			if(num>0)
			{
				int pencode = arrAssist[0].pencode;
				arrAssist[0].pencode = arrAssist[num-1].pencode;
				arrAssist[num-1].pencode = pencode;	

				for (int i=num-1; i>=0; i--)
				{
					PT_3DEX pt = arrAssist[i];				
					arrBase.Add(pt);
				}
			}
			
			CFeature *pFtr = pFt->Clone();
			pFtr->GetGeometry()->CreateShape(arrBase.GetData(),arrBase.GetSize());
			
			SaveSurface(pFtr,exAttriTab);
			delete pFtr;

		}
		else
		{
			SaveSurface(pFt,exAttriTab);
		}
	}
	else if( nGeoCls==CLS_GEOSURFACE ||nGeoCls==CLS_GEOMULTISURFACE || nGeoCls==CLS_GEODCURVE)
		SaveSurface(pFt,exAttriTab);
	else if( nGeoCls==CLS_GEOTEXT )
		SaveText(pFt,exAttriTab);
	return TRUE;
}


BOOL CEpsAccess::DelFeature(CFeature *pFtr)
{
	// 查找地物所在的数据表名
// 	char line[1024] = {0};
// 	CppSQLite3Query query;
// 	
// 	CString strTable;
// 	int nGeoCls = pFtr->GetGeometry()->GetClassType();
// 	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT)
// 		strTable = "Point";
// 	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEOPARALLEL )
// 		strTable = "Line";
// 	else if( nGeoCls==CLS_GEOSURFACE|| nGeoCls==CLS_GEOMULTISURFACE)
// 		strTable = "Surface";
// 	else if( nGeoCls==CLS_GEOTEXT )
// 		strTable = "Text";
// 	
// 	if(strTable.IsEmpty()) return FALSE;
// 	// 查找地物是否存在
// 	sprintf(line,"delete from %s where FTRID='%s';",strTable,pFtr->GetID().ToString());
// 	int nRet = m_db.execDML(line);

	return (0);
}

BOOL CEpsAccess::DelXAttributes(CFeature *pFtr)
{
// 	char line[1024] = {0};
// 	CppSQLite3Query query;
// 	//首先获取地物所在的层ID
// 	CString strTable;
// 	int nGeoCls = pFtr->GetGeometry()->GetClassType();
// 	if( nGeoCls==CLS_GEOPOINT||nGeoCls==CLS_GEODIRPOINT || nGeoCls==CLS_GEOMULTIPOINT)
// 		strTable = "Point";
// 	else if( nGeoCls==CLS_GEOCURVE || nGeoCls==CLS_GEOPARALLEL )
// 		strTable = "Line";
// 	else if( nGeoCls==CLS_GEOSURFACE )
// 		strTable = "Surface";
// 	else if( nGeoCls==CLS_GEOTEXT )
// 		strTable = "Text";	
// 	if(strTable.IsEmpty()) return FALSE;
// 	sprintf(line,"select LayerID from %s where FTRID='%s';",strTable,pFtr->GetID().ToString());
// 	query = m_db.execQuery(line);
// 	if(query.eof()) return FALSE;
// 	int LayerID = query.getIntField(0);
// 	
// 	//通过层ID获取扩展属性表的名称
// 	strTable.Format("AttrTable_%d",LayerID);
// 	
// 	//扩展属性表不存在，则返回 
// 	if(!m_db.tableExists(LPCTSTR(strTable)))
// 		return FALSE;
// 	
// 	//在扩展属性表中更新地物的对应项的DEL标记
// 	sprintf(line,"update %s set DELETED='%d' where FTRID='%s';",
// 			strTable,1,pFtr->GetID().ToString());
// 	
// 	m_db.execDML(line);	
// 	query.finalize();
	return TRUE;
}

BOOL CEpsAccess::RestoreXAttributes(CFeature *pFtr)
{
	return TRUE;
}

extern void VariantToText(const CVariantEx& v, CString &text);

BOOL CEpsAccess::SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx)
{

    return TRUE;
}

void CEpsAccess::BatchUpdateBegin()
{
//	m_pDaoDb->m_pWorkspace.execDML("PRAGMA cache_size=10000;");
	m_pDaoDb->m_pWorkspace->BeginTrans();
}


void CEpsAccess::BatchUpdateEnd()
{
	m_pDaoDb->m_pWorkspace->CommitTrans();
}

void CEpsAccess::CreateDefaultLayer()
{
	//再创建几个缺省的层

}

BOOL CEpsAccess::Compress()
{
// 	m_db.execDML("VACUUM");
	return TRUE;
}

#endif
