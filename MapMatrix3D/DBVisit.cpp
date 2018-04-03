/**************************************************************************************************
 This is a part of the Visiontek MapMatrix family.
 Copyright (C) 2005-2010 Visiontek Inc.
 All rights reserved.

 Module name:	���ݷ����м��ӿڶ���(GDMI)
 Author:		����ǿ
 Description:	���ڶԿռ����ݿ���з��ʵĽӿ�

**************************************************************************************************/
#include "stdafx.h"
#include "DBVisit.h"
#include <math.h>


//#include <Sddl.h>
 


/********************************************************
*                       ���ú���                        *
********************************************************/


GUID GUIDFromString(LPCTSTR text)
{
	GUID id;
	BYTE *buf = (BYTE*)&id;
	int i, num = sizeof(id);
	int len = strlen(text);
	
	for( i=0; i<num; i++)
	{
		buf[i] = 0;
		if( i<len )
		{
			if( text[(i<<1)]>='a' )
				buf[i] |= ((text[(i<<1)]-_T('a')+0xa)<<4);
			else
				buf[i] |= ((text[(i<<1)]-_T('0'))<<4);
			
			if( text[(i<<1)+1]>='a' )
				buf[i] |= ((text[(i<<1)+1]-_T('a')+0xa));
			else
				buf[i] |= ((text[(i<<1)+1]-_T('0')));
		}
	}
	
	return id;
}

CString GUIDToString(GUID id)
{
	TCHAR text[40] = {0};
	BYTE *buf = (BYTE*)&id;
	int i, num = sizeof(id);
	for( i=0; i<num; i++)
	{
		if( buf[i]>=0xa0 )
			text[(i<<1)] = (buf[i]>>4)-0xa + _T('a');
		else
			text[(i<<1)] = (buf[i]>>4) + _T('0');
		
		if( (buf[i]&0xf)>=0xa )
			text[(i<<1)+1] = (buf[i]&0xf)-0xa + _T('a');
		else
			text[(i<<1)+1] = (buf[i]&0xf) + _T('0');
	}
	return CString(text);
}


BOOL IsZeroGUID(GUID *id)
{
	BYTE *buf = (BYTE*)id;
	int i, num = sizeof(GUID);
	for( i=0; i<num; i++)
	{
		if( buf[i]!=0 )
			return FALSE;
	}
	return TRUE;
}


GUID NewGUID()
{
	GUID id;
	CoCreateGuid(&id);
	return id;
}



BYTE	byteAccessHeader[16] = {0x00,0x01,0x00,0x00,0x53,0x74,0x61,0x6e,
								0x64,0x61,0x72,0x64,0x20,0x4a,0x65,0x74};



//�ָ����ݿ��ļ�
BOOL ResumeDBFile(const char* szFile)
{
	FILE*	file = fopen(szFile,"r+b");
	if (!file)
		return FALSE;

	//����0 -- 512���ֽڵ�����������Ч����
	size_t s = fwrite(byteAccessHeader,1,16,file);

	fclose(file);

	if (s != 16)
		return FALSE;

	return TRUE;
}

//�������ݿ��ļ�
BOOL EncryptDBFile(const char* szFile)
{
	return TRUE;
	FILE*	file = fopen(szFile,"r+b");
	if (!file)
		return FALSE;

	BYTE b[16];
	srand((unsigned)time(NULL));

	for (int i = 0; i < 16; i++)
		b[i] = rand() % 256;
	

	//����0 -- 512���ֽڵ�����������Ч����
	size_t s = fwrite(b,1,16,file);

	fclose(file);
		
	if (s != 16)
		return FALSE;

	return TRUE;
}


//��������:�õ���ǰ����Ĵ�����Ϣ
//����˵��:  plm_handle_type:���ִ���ʱ��ʹ�õ�ODBC�������,ȡֵΪ:SQL_HANDLE_ENV,SQL_HANDLE_DBC,SQL_HANDLE_STMT
//           plm_handle		:���ִ���ʱ��ʹ�õ�ODBC���
//			 ConnInd		:ָ������Ƿ�ΪDBC���	
void ProcessLogMessages(SQLSMALLINT plm_handle_type,SQLHANDLE plm_handle, 
						BOOL ConnInd,char* szErrorMsg,UCHAR* szSqlState)

{

	RETCODE			plm_retcode					= SQL_SUCCESS;
	UCHAR			plm_szSqlState[255]			= "";
	UCHAR			plm_szErrorMsg[255]			= "";
	SDWORD			plm_pfNativeError			= 0L;
	SWORD			plm_pcbErrorMsg				= 0;
	SDWORD			plm_SS_MsgState				= 0;
	SDWORD			plm_SS_Severity				= 0;
	SQLINTEGER		plm_Rownumber				= 0;
	SQLSMALLINT		plm_cRecNmbr				= 1;
	SQLCHAR			plm_SS_Procname[255];
	SQLCHAR			plm_SS_Srvname[255];

 
	char	szTmp[512];
	memset(szTmp,0,sizeof(szTmp));

	

	while (plm_retcode != SQL_NO_DATA_FOUND) 
	{
		plm_retcode = SQLGetDiagRec(plm_handle_type, plm_handle,
									plm_cRecNmbr,plm_szSqlState,&plm_pfNativeError,
									plm_szErrorMsg,254,&plm_pcbErrorMsg);
 
		// Note that if the application has not yet made a
		// successful connection, the SQLGetDiagField
		// information has not yet been cached by ODBC
		// Driver Manager and these calls to SQLGetDiagField
		// will fail.

		if (plm_retcode != SQL_NO_DATA_FOUND) 
		{
			if (ConnInd) 
			{
				plm_retcode = SQLGetDiagField(plm_handle_type,plm_handle,plm_cRecNmbr,SQL_DIAG_ROW_NUMBER, 
					                          &plm_Rownumber,SQL_IS_INTEGER,NULL);

			}

			sprintf(szErrorMsg,"SqlState = %s\n pfNativeError = %d\n szErrorMsg = %s\n pcbErrorMsg = %d\n",
				    plm_szSqlState,plm_pfNativeError,plm_szErrorMsg,plm_pcbErrorMsg);
			strcpy((char*)szSqlState,(char*)plm_szSqlState);

			if (ConnInd)
			{
				sprintf(szTmp,"ODBCRowNumber = %d\n",
					    plm_Rownumber,plm_SS_MsgState,plm_SS_Severity,plm_SS_Procname,plm_SS_Srvname);
				strcat(szErrorMsg,szTmp);
			}
		}


		plm_cRecNmbr++; //Increment to next diagnostic record.

   } // End while.

}


//�жϷ���ֵ�Ƿ���ȷ
BOOL Check(SQLRETURN nRetCode)
{
	switch (nRetCode)
	{
		case SQL_SUCCESS_WITH_INFO:
			return false;
		case SQL_SUCCESS:
		case SQL_NO_DATA_FOUND:
			return true;
	}

	return true;
}


//��C���ֵĴ���ת��Ϊͼ����չ����,houkui,06.7.12
int LayerType_To_CType(int layerType)
{
    return layerType-17;
}

int CType_To_LayerType(int cType)
{
    return cType+17;
}

#define C_TYPE_TO_LAYER_TYPE(cType)			cType+17   


//���ڸ����ַ����ĺ���
inline void CopyStr(char** szDest,const char* szSource)
{
	if (!szDest || !szSource)
		return;

	*szDest = new char[strlen(szSource)+1];
	memset((*szDest),0,strlen(szSource)+1);
	strcpy(*szDest,szSource);
}

//�����ݿ��ʱ��ṹ�еõ�ʱ��
time_t	GetTimeFromDBStruct(TIMESTAMP_STRUCT* tss)
{
	if (!tss)
		return (time_t)0;

	time_t		tmRet	= 0;

	tm			tmTmp;
	memset(&tmTmp,0,sizeof(tm));

	tmTmp.tm_year		= tss->year-1900;
	tmTmp.tm_mon		= tss->month-1;
	tmTmp.tm_mday		= tss->day;
	tmTmp.tm_hour		= tss->hour;
	tmTmp.tm_min		= tss->minute;
	tmTmp.tm_sec		= tss->second;

	tmRet = mktime(&tmTmp);

	return tmRet;

}

//���´��ֶε�ֵ,��ORACLE�ھ�ΪBLOB���͵��ֶ� 
//����˵��:   hStmt -- ���ξ��		uIndex    -- Ҫ���µ��ֶε����
//			  buff  -- ���뻺����       iBuffLen  -- �������Ĵ�С
//Ŀǰ���ж����Ʋ�����һ����Ĵ�СΪ8192
BOOL SetLargerFieldValue(SQLHANDLE hConn,char* szSql,BYTE* buff,int iBuffLen)
{

	if (!hConn || !buff || iBuffLen <= 0)
		return false;

	SQLLEN			iSize		= 0;
	SQLHANDLE		hStmt		= NULL;
	SQLRETURN		retcode;
	SQLRETURN		retPutReturn;
	SQLPOINTER		pToken		= NULL;
	int				iRemain		= iBuffLen;


	SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hStmt);
	if (!hStmt)
		return false;
	
	retcode = SQLBindParameter(hStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,
		             SQL_LONGVARBINARY,iBuffLen,0,(SQLPOINTER)buff,
					 0,&iSize);
		             
	if (retcode != SQL_SUCCESS)
		return false;

	iSize	= SQL_LEN_DATA_AT_EXEC(iBuffLen);

	retcode = SQLExecDirect(hStmt,(SQLCHAR*)szSql,SQL_NTS);

		
	int				iNum		= 0;

	//д������
	while (retcode == SQL_NEED_DATA) 
	{
		retcode = SQLParamData(hStmt, &pToken);
		if (retcode == SQL_NEED_DATA) 
		{
			while (iRemain > 0)
			{
				if (iRemain > DEFAULT_WRITE_CHUNK)
				{
					retPutReturn = SQLPutData(hStmt,(SQLPOINTER)(buff+iNum*DEFAULT_WRITE_CHUNK), DEFAULT_WRITE_CHUNK);
					iRemain -= DEFAULT_WRITE_CHUNK;
					iNum++;
				}
				else
				{
					retPutReturn = SQLPutData(hStmt,(SQLPOINTER)(buff+iNum*DEFAULT_WRITE_CHUNK),iRemain);
					iRemain = 0;
				}

			}
		}
				
	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);

	if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA)
		return false;
	else
		return true;

}

//ֱ��ִ��һ��SQL���,��Ҫ����ִ��һЩ������CDpDatabase�Ĳ��Ҳ���Ҫ���ص�SQL���
BOOL ExecuteSqlDirect(SQLHANDLE hConn,char* szSql)
{
	if (!hConn || !szSql || strlen(szSql) < 1)
		return false;

	SQLHANDLE		hStmt	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hStmt) != SQL_SUCCESS)
		return false;

	SQLRETURN sRet = SQLExecDirect(hStmt,(SQLTCHAR*)szSql,strlen(szSql));

/*
	if (sRet == SQL_ERROR)
	{
		char		sz[1000];
		UCHAR		szSqlStates[100];
		memset(sz,0,sizeof(sz));
		memset(szSqlStates,0,sizeof(szSqlStates));
		ProcessLogMessages(SQL_HANDLE_STMT,hStmt,true,sz,szSqlStates);
		TRACE(sz);
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	}
*/

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);

	if (sRet == SQL_SUCCESS || sRet == SQL_NO_DATA)
		return true;
	else
		return false;

}




int	GetShapeDataSize(CDpShapeData* pData)
{
	if (!pData)
		return 0;
	return (pData->iNumParts * sizeof(int) + 
		    pData->iNumPoints * sizeof(CDpPoint) +
		    4 * sizeof(double) + 
			2 * sizeof(int));
}	



//��һ����ѯ��SQL����еõ�Ҫ��ѯ�ı���
BOOL GetTableNameFromSql(const char* szSql,char* szTableName)
{
	if (!szSql || !szTableName || strlen(szSql) <= 0)
		return false;

//	char	szTmp[2000];
	char*	szTmp = new char[strlen(szSql) + 1];
	memset(szTmp,0,strlen(szSql) + 1);
	strcpy(szTmp,szSql);

	char*	szAfterFrom		= NULL;
	int		iLen			= 0;
	BOOL	bStart			= false;
	int		iTableNameLen	= 0;
	int		i				= 0;

	//��SQLתΪ��д
	if (!_strupr(szTmp))
		goto ErrFun;

	szAfterFrom = strstr(szTmp,"FROM");
	if (!szAfterFrom)									//û���ҵ�"FROM"
		goto ErrFun;
	szAfterFrom += 4;									//��ָ���Ƶ�"FROM"��'M'���һ���ַ���λ��

	iLen			= strlen(szAfterFrom);
	bStart			= false;
	iTableNameLen	= 0;

	if (iLen <= 0)
		goto ErrFun;

	for (i = 0; i < iLen; i++)
	{
		szAfterFrom = szAfterFrom + i;
		if (*szAfterFrom != ' ')
			break;
	}

	iLen	= strlen(szAfterFrom);
	if (iLen <= 0)
		goto ErrFun;

	for (i = 0; i < iLen; i++)
	{
		if ((*(szAfterFrom+i)) == ',')			//�п����Ƕ����Ĳ�ѯ
			goto ErrFun;

		if ((*(szAfterFrom+i)) == ' ')
		{
			iTableNameLen = i;
			break;
		}

	}

	//���Ʊ���
	if (iTableNameLen > 0)
		memcpy(szTableName,szAfterFrom,iTableNameLen);
	else
		goto ErrFun;

	delete [] szTmp;
	szTmp = NULL;
	return true;
ErrFun:
	delete [] szTmp;
	szTmp = NULL;
	return false;

}

//����ʱ���SQL���
BOOL BuildDateTimeSql(enum DP_DATABASE_TYPE dbType,tm* tmDateTime,char* szDateTimeSql)
{
	if (!tmDateTime || !szDateTimeSql)
		return false;

	if (dbType == DP_ACCESS)			//���ݿ�ΪACCESS
		sprintf(szDateTimeSql,"DateValue('%04d-%02d-%02d')+TimeValue('%02d:%02d:%02d')",
			    tmDateTime->tm_year+1900,tmDateTime->tm_mon+1,tmDateTime->tm_mday,tmDateTime->tm_hour,
				tmDateTime->tm_min,tmDateTime->tm_sec);
	else if (dbType == DP_ORACLE)
		sprintf(szDateTimeSql,"TO_DATE('%04d%02d%02d%02d%02d%02d','YYYYMMDDHH24MISS')",
			    tmDateTime->tm_year+1900,tmDateTime->tm_mon+1,tmDateTime->tm_mday,tmDateTime->tm_hour,
				tmDateTime->tm_min,tmDateTime->tm_sec);
	else if (dbType == DP_SQLSERVER)
		sprintf(szDateTimeSql,"CAST('%04d-%02d-%02d %02d:%02d:%02d.000' AS DATETIME)",
			    tmDateTime->tm_year+1900,tmDateTime->tm_mon+1,tmDateTime->tm_mday,tmDateTime->tm_hour,
				tmDateTime->tm_min,tmDateTime->tm_sec);
	else
		return false;

	return true;
}


//����ʱ���SQL���
BOOL BuildDateTimeSql(enum DP_DATABASE_TYPE dbType,TIMESTAMP_STRUCT* tmDateTime,
					  char* szDateTimeSql)
{
	if (!tmDateTime || !szDateTimeSql)
		return false;

	if (dbType == DP_ACCESS)			//���ݿ�ΪACCESS
		sprintf(szDateTimeSql,"DateValue('%04d-%02d-%02d')+TimeValue('%02d:%02d:%02d')",
			    tmDateTime->year,tmDateTime->month,tmDateTime->day,
				tmDateTime->hour,tmDateTime->minute,tmDateTime->second);
	else if (dbType == DP_ORACLE)
		sprintf(szDateTimeSql,"TO_DATE('%04d%02d%02d%02d%02d%02d','YYYYMMDDHHMISS')",
			    tmDateTime->year,tmDateTime->month,tmDateTime->day,
				tmDateTime->hour,tmDateTime->minute,tmDateTime->second);
	else if (dbType == DP_SQLSERVER)
		sprintf(szDateTimeSql,"CAST('%04d-%02d-%02d %02d:%02d:%02d.000' AS DATETIME)",
			    tmDateTime->year,tmDateTime->month,tmDateTime->day,
				tmDateTime->hour,tmDateTime->minute,tmDateTime->second);
	else
		return false;

	return true;
}



//����UPDATE��SQL���
//����˵��:		[in]dbType       -- ���ݿ������			  [in]szTableName   -- Ҫ���µı������
//              [in]szFieldName  -- Ҫ���µ��ֶ���			  [in]szWhereClause -- Where�Ӿ�,����"Where"  
//              [in]SqlDataType  -- Ҫ���µ��ֶε�SQL�������� [in]var           -- Ҫ���µ�ֵ 
//              [out]szSql       -- �������ɵ�SQL���
BOOL BuildUpdateSql(enum DP_DATABASE_TYPE dbType,const char* szTableName,
					const char* szFieldName,const char* szWhereClause,
					SQLSMALLINT SqlDataType,CDpDBVariant var,
					char* szSql)
{
	

	if (dbType == DP_NULL || !szTableName || strlen(szTableName) <= 0 || 
	!szFieldName || strlen(szFieldName) <= 0 || !szSql)
	return false;

	if (SqlDataType == SQL_C_BINARY)									//�������ֶ�
	{
		sprintf(szSql,"Update %s set %s = ? %s",
				szTableName,szFieldName,szWhereClause);

	}
	else if (SqlDataType == SQL_C_CHAR)										//�ַ���
	{
		sprintf(szSql,"Update %s set %s = '%s' %s",szTableName,
				szFieldName,(LPCTSTR)var,szWhereClause);
	}
	else if (SqlDataType == SQL_C_TIMESTAMP)								//������
	{
		if (var.m_dwType != DPDBVT_DATE || !var.m_pdate)
			return false;

		char		szDateTimeSql[200];
		memset(szDateTimeSql,0,sizeof(szDateTimeSql));
		BuildDateTimeSql(dbType,var.m_pdate,szDateTimeSql);

		sprintf(szSql,"Update %s set %s = %s %s",szTableName,szFieldName,szDateTimeSql,szWhereClause);

	}
	else if (SqlDataType == SQL_C_FLOAT || SqlDataType == SQL_C_DOUBLE)	//��С����������
	{
		sprintf(szSql,"Update %s set %s = %f %s",
				szTableName,szFieldName,(double)var,szWhereClause);
	}
	else																//����
	{
		sprintf(szSql,"Update %s set %s = %d %s",
				szTableName,szFieldName,(int)var,szWhereClause);
	}

	return true;
}

//�жϸ�ͼ���ǲ���դ���
inline BOOL LAYER_IS_RASTER(DP_LAYER_TYPE LayType)
{
	return (LayType == GEO_Raster);
}


//����û����߿ո���ַ���ָ��
char* LeftTrim(const char* nDest)
{
	if (!nDest)
		return NULL;

	const char*	szTmp = nDest;
	int	iLen = strlen(nDest);
	if (iLen <= 0)
		return NULL;

	for (int i = 0; i < iLen; i++)
	{
		if (nDest[i] == ' ')
			szTmp++;
		else
			break;
	}

	return (char*)szTmp;
}
/*
BOOL GrantRight(const char* szUserName,const char* szPwd)
{
	// �������������̵ĵ��Լ���
	HANDLE hToken;
	TOKEN_PRIVILEGES tk;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tk.Privileges[0].Luid);
	tk.PrivilegeCount = 1;
	tk.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	BOOL b = AdjustTokenPrivileges(hToken, FALSE, &tk, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	PSID			sid		= NULL;
	if (!GetUserSID(szUserName,&sid))
		return false;

	//�õ�SID���ַ�������
	char*			szSID	= NULL;
	ConvertSidToStringSid(sid,&szSID);

    if(sid != NULL)
        HeapFree(GetProcessHeap(), 0, sid);

	//����Ȩ��
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE; 

	char pwszSD[512];
	sprintf(pwszSD,"D:(A;OICI;GA;;;%s)(A;OICI;GA;;;BA)",szSID);

	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
	    pwszSD,SDDL_REVISION_1,&(sa.lpSecurityDescriptor), 
	    (PULONG)NULL))
	{
		TRACE("��Ȩʧ��!");
		return FALSE;
	}
	return TRUE;
}
*/

//��Դ���ݿ��е�ָ���ı����������Ƽ�¼��Ŀ�����ݿ�����
//�ڸ���֮ǰ����where�Ӿ��Ƚ�Ŀ��������еļ�¼ɾ��
//����˵��: hConnDest				-- Ŀ�����ݿ��ODBC����
//			iRecordCountOfPerOper	-- �������������Ĺ����в����ļ�¼����Ŀ
//          szTableName				-- ָ��Ҫ���Ƽ�¼�ı���
//			szWhereClause			-- ���ڹ��˼�¼��WHERE�Ӿ�,����WHERE	
//			iDefaultBinaryBlockSize	-- ����������������ݵĻ�,��ȱʡ�Ķ��������ݵ��ڴ��Ĵ�С
BOOL BulkCopyRecordToDest(SQLHANDLE hConnSource,SQLHANDLE hConnDest,
						  int iRecordCountOfPerOper,char* szTableName,char* szWhereClause,
						  int iDefaultBinaryBlockSize)
{
	if (!hConnSource || !hConnDest || iRecordCountOfPerOper <= 0 || !szTableName || strlen(szTableName) <= 0)
		return false;

	BOOL		bRet		= false;
	SQLHANDLE	hStmtDest	= NULL;
	BYTE*		buff		= NULL;

	//��ɾ����¼
	char		szSql[1024];
	sprintf(szSql,"Delete From %s %s",szTableName,szWhereClause?szWhereClause:"");
	ExecuteSqlDirect(hConnDest,szSql);

	//����Դ���SQL�����
	SQLHANDLE	hStmtSource	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,hConnSource,&hStmtSource) != SQL_SUCCESS || 
		!hStmtSource)
		return false;
	sprintf(szSql,"Select * From %s %s",szTableName,szWhereClause?szWhereClause:"");

	if (SQLExecDirect(hStmtSource,(SQLTCHAR*)szSql,strlen(szSql)) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_DBC,hStmtSource);
		return false;
	}

	//������Դ���ݿ��иñ�Ľṹ���жϴ��󶨵ļ�¼�����ڴ�ṹ��С
	//�ȵõ��ֶ���
	SQLSMALLINT		iCount;
	SQLNumResultCols(hStmtSource,&iCount);
	SQLRETURN		sRet;

	if (iCount <= 0)
	{
		SQLFreeHandle(SQL_HANDLE_DBC,hStmtSource);
		return false;
	}
	
	_DpBindFieldInfo*	pField				= new _DpBindFieldInfo[iCount];
	memset(pField,0,sizeof(pField));
	
	int					iBindMemorySize		= 0;			//������¼������ڴ���С	

	SQLSMALLINT			iCbNameLen			= 0;

	//�󶨼�¼��״̬����	
	SQLUSMALLINT*		pRowStatusArray		= new SQLUSMALLINT[iRecordCountOfPerOper];
	//���صļ�¼��
	SQLUINTEGER			NumRowsFetched		= 0;

	for (int i = 0; i < iCount; i++)
	{
		SQLDescribeCol(hStmtSource,(SQLUSMALLINT)(i+1),(SQLCHAR*)(pField[i].info.m_strName),
			               sizeof(pField[i].info.m_strName),&iCbNameLen,
						   &(pField[i].info.m_nSQLType),
						   &(pField[i].info.m_iSize),
						   &(pField[i].info.m_nPrecision),
						   (SQLSMALLINT*)&(pField[i].info.m_bAllowNull));
		pField[i].iIndex		= i+1;
	}

	iBindMemorySize		= CleanUpBindInfo(pField,iCount,iDefaultBinaryBlockSize);
	if (iBindMemorySize == 0)
		goto FunEnd;
	
	//��������ݵ��ڴ��
	buff = new BYTE[iBindMemorySize * iRecordCountOfPerOper];
	memset(buff,0,iBindMemorySize * iRecordCountOfPerOper);

	//��ʼ�󶨵�Դ����Դ��,��ȡ����
	SQLSetStmtAttr(hStmtSource, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)iBindMemorySize, 0);
	SQLSetStmtAttr(hStmtSource, SQL_ATTR_ROW_ARRAY_SIZE,(SQLPOINTER)iRecordCountOfPerOper, 0);
	SQLSetStmtAttr(hStmtSource, SQL_ATTR_ROW_STATUS_PTR, pRowStatusArray, 0);
	SQLSetStmtAttr(hStmtSource, SQL_ATTR_ROWS_FETCHED_PTR, &NumRowsFetched, 0);

	//��Ŀ������Դ
	if (SQLAllocHandle(SQL_HANDLE_STMT,hConnDest,&hStmtDest) != SQL_SUCCESS || !hStmtDest)
		goto FunEnd;

	//��Ŀ���α���Ϊ��д
	SQLSetStmtAttr(hStmtDest,SQL_ATTR_CONCURRENCY,(SQLPOINTER)SQL_CONCUR_VALUES,0); 	
	SQLSetStmtAttr(hStmtDest, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, 0);

	sprintf(szSql,"Select * From %s",szTableName);
	sRet = SQLExecDirect(hStmtDest,(SQLTCHAR*)szSql,strlen(szSql));
	if (sRet != SQL_SUCCESS)
		goto FunEnd;

	for (i = 0; i < iCount; i++)
	{
		if (pField[i].bIsCanBind)
		{
			//�󶨵�Դ����Դ��
			SQLBindCol(hStmtSource,i+1,pField[i].nBindSqlType,(SQLPOINTER)(buff+pField[i].iPosInBlock),
				(SQLINTEGER)pField[i].iMemorySize, (SQLLEN*)(buff + pField[i].iLenPosInBlock));
			//���ֶε�Ŀ������Դ��
			SQLBindCol(hStmtDest,i+1,pField[i].nBindSqlType,(SQLPOINTER)(buff+pField[i].iPosInBlock),
				(SQLINTEGER)pField[i].iMemorySize, (SQLLEN*)(buff + pField[i].iLenPosInBlock));
		}

	}	

	bRet = true;

	//������Դ��ȡ���ݲ��������뵽Ŀ������Դ��
	while (1)
	{
		sRet = SQLFetch(hStmtSource);
			
		if (sRet == SQL_SUCCESS || sRet == SQL_SUCCESS_WITH_INFO)
		{
			SQLSetStmtAttr(hStmtDest, SQL_ATTR_ROW_ARRAY_SIZE,(SQLPOINTER)NumRowsFetched, 0);
			SQLSetStmtAttr(hStmtDest, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)iBindMemorySize, 0);
			SQLSetStmtAttr(hStmtDest, SQL_ATTR_ROW_STATUS_PTR, (SQLPOINTER)pRowStatusArray, 0);

			SQLRETURN sn;
			if (sn = SQLBulkOperations(hStmtDest,SQL_ADD) != SQL_SUCCESS)
			{
				#ifdef _DEBUG
					char		sz[1024];
					UCHAR		szSqlStates[256];
					memset(sz,0,sizeof(sz));
					memset(szSqlStates,0,sizeof(szSqlStates));
					ProcessLogMessages(SQL_HANDLE_STMT,hStmtDest,true,sz,szSqlStates);
				#endif
			}			
		}
		else
		{
			SQLCloseCursor(hStmtSource);
			break;
		}
		
	}
	
FunEnd:	
	if (hStmtSource)		SQLFreeHandle(SQL_HANDLE_STMT,hStmtSource);
	if (hStmtDest)			SQLFreeHandle(SQL_HANDLE_STMT,hStmtDest);
	if (pField)				
	{
		delete [] pField;
		pField = NULL;
	}

	if (pRowStatusArray)
	{
		delete [] pRowStatusArray;
		pRowStatusArray = NULL;
	}
	if (buff)
	{
		delete [] buff;
		buff = NULL;
	}

	return bRet;
}



//�ڵõ��ֶ����ͺ�,����¼����Ϣ��������
//����˵��: pBindInfo -- ���ֶ���Ϣ�ṹ������   iCount --�����size
//����ֵ:   ���ص�ǰ���������а󶨵��ֶε��ڴ��Ĵ�С(��������¼���ڴ���С)
int CleanUpBindInfo(_DpBindFieldInfo* pBindInfo,int iCount,int iDefaultBinaryBlockSize)
{
	if (!pBindInfo || iCount <= 0)
		return 0;

	int		iMemoryCount		= 0;

	for (int i = 0; i < iCount; i++)
	{
		switch (pBindInfo[i].info.m_nSQLType)
		{
			case SQL_BIT:
				pBindInfo[i].iMemorySize	= sizeof(short);
				pBindInfo[i].nBindSqlType	= SQL_C_BIT;
				break;

			case SQL_TINYINT:
				pBindInfo[i].iMemorySize	= sizeof(short);
				pBindInfo[i].nBindSqlType	= SQL_C_UTINYINT;
				break;

			case SQL_SMALLINT:
				pBindInfo[i].iMemorySize	= sizeof(short);
				pBindInfo[i].nBindSqlType	= SQL_C_SSHORT;
				break;

			case SQL_INTEGER:
				pBindInfo[i].iMemorySize	= sizeof(int);
				pBindInfo[i].nBindSqlType	= SQL_C_LONG;
				break;

			case SQL_REAL:
				pBindInfo[i].iMemorySize	= sizeof(double);
				pBindInfo[i].nBindSqlType	= SQL_C_FLOAT;
				break;

			case SQL_FLOAT:
			case SQL_DOUBLE:
				pBindInfo[i].iMemorySize	= sizeof(double);
				pBindInfo[i].nBindSqlType	= SQL_C_DOUBLE;
				break;

			case SQL_DATE:
			case SQL_TIME:
			case SQL_TIMESTAMP:
			case SQL_TYPE_DATE:
			case SQL_TYPE_TIME:
			case SQL_TYPE_TIMESTAMP:
				pBindInfo[i].iMemorySize	= sizeof(TIMESTAMP_STRUCT);
				pBindInfo[i].nBindSqlType	= SQL_C_TYPE_TIMESTAMP;
				break;

			case SQL_NUMERIC:
			case SQL_DECIMAL:
			case SQL_BIGINT:
			{
				if (pBindInfo[i].info.m_nPrecision > 0)
				{
					pBindInfo[i].iMemorySize	= sizeof(double);
					pBindInfo[i].nBindSqlType	= SQL_C_DOUBLE;
				}
				else
				{
					pBindInfo[i].iMemorySize	= sizeof(int);
					pBindInfo[i].nBindSqlType	= SQL_C_LONG;
				}

				break;
			}
			case SQL_CHAR:
			case SQL_VARCHAR:
			case SQL_LONGVARCHAR:
				pBindInfo[i].iMemorySize	= pBindInfo[i].info.m_iSize + 1;
				pBindInfo[i].nBindSqlType	= SQL_C_CHAR;
				break;

			//����������,ȱʡ����Ϊ65536,��64K
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
			{
				if (iDefaultBinaryBlockSize <= 0)
					pBindInfo[i].iMemorySize	= 4;
				else
					pBindInfo[i].iMemorySize	= iDefaultBinaryBlockSize;

				pBindInfo[i].nBindSqlType	= SQL_C_BINARY;
				break;		
			}
		}		

		if (pBindInfo[i].iMemorySize	== 0)
		{
			pBindInfo[i].bIsCanBind	= false;
			continue;
		}
		else
			pBindInfo[i].bIsCanBind	= true;

		iMemoryCount	+= pBindInfo[i].iMemorySize;
		iMemoryCount	+= sizeof(SQLINTEGER);				//����ָʾ��
	}

	if (iMemoryCount > 0)
	{
		int		iPreBindIndex	= -1;					//��һ���󶨵��ֶε����
		//����ÿ���ֶ����ڴ���еİ�λ��
		for (i = 0; i < iCount; i++)
		{
			if (pBindInfo[i].bIsCanBind)
			{
				if (iPreBindIndex == -1)
					pBindInfo[i].iPosInBlock		= 0;
				else
					pBindInfo[i].iPosInBlock		= pBindInfo[iPreBindIndex].iLenPosInBlock + sizeof(SQLINTEGER);

				pBindInfo[i].iLenPosInBlock			= pBindInfo[i].iPosInBlock + pBindInfo[i].iMemorySize;
				iPreBindIndex = i;
			}

		}
	}
	
	return iMemoryCount;

}


//�����ĳ�����һ����¼������ڴ��Ĵ�С
//����˵��:  hConn -- ���ݿ�����Ӿֱ�        szTableName -- Ҫ����󶨼�¼�ı������
//           iDefaultBinaryBlockSize -- ����ж������ֶ�,��������ֶε�ȷʡ���С
int GetBindTabRecordMemSize(SQLHANDLE hConn,const char* szTableName,int iDefaultBinaryBlockSize)
{
	if (!hConn || !szTableName || strlen(szTableName) <= 0 || iDefaultBinaryBlockSize < 1)
		return 0;

	//����SQL�����
	SQLHANDLE	hStmt	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hStmt) != SQL_SUCCESS || 
		!hStmt)
		return false;

	char	szSql[256];
	sprintf(szSql,"Select * From %s Where 1 = 2",szTableName);

	if (SQLExecDirect(hStmt,(SQLTCHAR*)szSql,strlen(szSql)) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_DBC,hStmt);
		return 0;
	}

	//�������ݿ��иñ�Ľṹ���жϴ��󶨵ļ�¼�����ڴ�ṹ��С
	//�ȵõ��ֶ���
	SQLSMALLINT		iCount;
	SQLNumResultCols(hStmt,&iCount);

	if (iCount <= 0)
	{
		SQLFreeHandle(SQL_HANDLE_DBC,hStmt);
		return 0;
	}
	
	_DpBindFieldInfo*	pField				= new _DpBindFieldInfo[iCount];
	memset(pField,0,sizeof(pField));
	
	int					iBindMemorySize		= 0;			//������¼������ڴ���С	

	SQLSMALLINT			iCbNameLen			= 0;

	for (int i = 0; i < iCount; i++)
	{
		SQLDescribeCol(hStmt,(SQLUSMALLINT)(i+1),(SQLCHAR*)(pField[i].info.m_strName),
			               sizeof(pField[i].info.m_strName),&iCbNameLen,
						   &(pField[i].info.m_nSQLType),
						   &(pField[i].info.m_iSize),
						   &(pField[i].info.m_nPrecision),
						   (SQLSMALLINT*)&(pField[i].info.m_bAllowNull));
		pField[i].iIndex		= i+1;
	}

	iBindMemorySize		= CleanUpBindInfo(pField,iCount,iDefaultBinaryBlockSize);

	if(pField)
	{
		delete [] pField;
		pField = NULL;
	}

	SQLFreeHandle(SQL_HANDLE_DBC,hStmt);

	return iBindMemorySize;
}

















//�ж��ļ��Ƿ����
BOOL DirIsExist(char* szDir)
{
	HANDLE hDir = NULL;
	hDir = CreateFile(szDir,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
							 NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if (hDir == INVALID_HANDLE_VALUE)
		return false;
	else
		CloseHandle(hDir);

	return true;

}

//��һ���ַ���תΪһ�����ַ��͵��ַ���,��ס������wstrRetҪ�ͷ�
BOOL GetBStrFromChar(const char* szString,WCHAR** wstrRet)
{
	if (strlen(szString) < 1)
		return false;

	int iwByteLen = MultiByteToWideChar(CP_ACP,0,(LPCSTR)szString,
		                                strlen(szString)+1,NULL,0);
	if (iwByteLen < 1)
		return false;
	*wstrRet = new WCHAR[iwByteLen];
	memset(*wstrRet,0,iwByteLen*sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)szString,strlen(szString)+1,
						*wstrRet,iwByteLen);

	return true;
}

//�õ���ǰWINDOWS�û���SID
BOOL GetUserSID(const char* szUserName,PSID* pSID)
{
	PSID			pSid = NULL;
    DWORD			cbSid		= 96;
    CHAR			RefDomain[DNLEN + 1];
    DWORD			cchDomain = DNLEN + 1;
    SID_NAME_USE	peUse;

    pSid = (PSID)HeapAlloc(GetProcessHeap(), 0, cbSid);
    if(pSid == NULL) 
		return false;

    if(!LookupAccountName(NULL,szUserName,pSid,&cbSid,     
						   RefDomain,&cchDomain,&peUse)) 
	{
        if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
            pSid = (PSID)HeapReAlloc(GetProcessHeap(),0,pSid,cbSid);

            if(pSid == NULL) 
                goto cleanup;

            cchDomain = DNLEN + 1;

            if(!LookupAccountName(NULL,szUserName,pSid,&cbSid,
								   RefDomain,&cchDomain,&peUse))
                goto cleanup;

			else
	            goto cleanup;
        }
		else
			return false;
    }

	*pSID = pSid;
	return true;

cleanup:
    if(pSid != NULL)
        HeapFree(GetProcessHeap(), 0, pSid);

    return false;
	
}




//����һ��WINDOWS�û�
BOOL CreateUserInWindow(const char* szUserName,const char* szPwd)
{
	//�ȴ����û�
	USER_INFO_1		u1;
	u1.usri1_password_age	= 0;
	u1.usri1_script_path	= NULL;
	u1.usri1_home_dir		= NULL;
	u1.usri1_comment		= NULL;

	GetBStrFromChar(szUserName,&(u1.usri1_name));
	GetBStrFromChar(szPwd,&(u1.usri1_password));
	u1.usri1_priv			= USER_PRIV_USER;
	u1.usri1_flags			= UF_PASSWD_CANT_CHANGE|UF_DONT_EXPIRE_PASSWD;

	DWORD			dw		= 0;

	NET_API_STATUS sRet = NetUserAdd(NULL,1,(LPBYTE)(&u1),&dw);
	
	if(u1.usri1_name)
	{
		delete [] u1.usri1_name;
		u1.usri1_name = NULL;
	}
	
	if(u1.usri1_password)
	{
		delete [] u1.usri1_password;
		u1.usri1_password = NULL;
	}
	
	if (sRet != NERR_Success && sRet != NERR_UserExists)
		return false;
	else
		return true;
}

//�õ�����������ʱ���������û�������
BOOL GetWksUserPwd(const char* szUserID,/*out*/char* szPwd)
{
	if (!szUserID || strlen(szUserID) <= 0 || !szPwd)
		return false;

	//�򵥵Ľ��û�����ǰ��Ի�
	int		iLen		= strlen(szUserID);
	int		iMid		= iLen / 2;

	char	szNewPwd[50];
	memset(szNewPwd,0,sizeof(szNewPwd));

	memcpy(szNewPwd,szUserID+iMid,iLen-iMid);
	memcpy(szNewPwd+(iLen-iMid),szUserID,iMid);
	
	strcpy(szPwd,szNewPwd);

	return true;
}

static char* AsWideString( const char* cszANSIstring )
{
	if( cszANSIstring == NULL )
	{
		return NULL;
	}
	
	int nBufSize = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, cszANSIstring, -1, NULL, 0 );
	WCHAR* wideString = new WCHAR[nBufSize+1];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, cszANSIstring, -1, wideString, nBufSize );
	
	// I think that better do one conversion here than convert at every assigment
	return	reinterpret_cast<char*>(wideString);
	
}

//���ý��̵İ�ȫ��������
BOOL SetProcessToken(LPCTSTR szName)
{
	return TRUE;
/*


	if(szName == NULL || strlen(szName) <= 0)
		return FALSE;
	
	BOOL fRet;
	PSID pSid = NULL; 
	DWORD sidBufferSize = 0;	
	char domainBuffer[256];	
	DWORD domainBufferSize = 256;
	SID_NAME_USE snu;

	//����һ���û�����ȡ���û���SID
	fRet = LookupAccountName(NULL,
							 szName, 
							 pSid,	
							 &sidBufferSize, 
							 domainBuffer,
							 &domainBufferSize, 
							 &snu);
	DWORD dwErr = GetLastError();
	if(dwErr == ERROR_INSUFFICIENT_BUFFER)
    {
        pSid = LocalAlloc(LPTR, sidBufferSize);
        if(!pSid)
        {
            return FALSE;
        }
        
		fRet = LookupAccountName(NULL,
								 szName, 
								 pSid,	
								 &sidBufferSize, 
								 domainBuffer,
								 &domainBufferSize, 
								 &snu);
		
        if(fRet == FALSE)
        {
            LocalFree(pSid);
			pSid = NULL;
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

	BYTE Buffer[1024];//��ʼ��һ��ACL
	PACL pAcl=(PACL)&Buffer;			
	InitializeAcl(pAcl, 1024, ACL_REVISION);

	//���ø��û���Access-Allowed��ACE����Ȩ��Ϊ������Ȩ�ޡ�	
    fRet = AddAccessAllowedAceEx(pAcl,
								 ACL_REVISION,
								 0,
								 GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
								 pSid);
	if(!fRet)
		return FALSE;

    HANDLE hToken;
	
    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS , &hToken))
        return FALSE;

	TOKEN_DEFAULT_DACL			tiTokenInfor;	
	DWORD						dTokenInforLength;
	ACL_SIZE_INFORMATION		asiAclSize;
	DWORD						dwBufLength;
	dwBufLength					= sizeof(asiAclSize);
	dTokenInforLength			= 0;
	tiTokenInfor.DefaultDacl	= pAcl;	
	
	fRet = GetAclInformation(pAcl, 
							(LPVOID)&asiAclSize,
							(DWORD)dwBufLength,
							(ACL_INFORMATION_CLASS)AclSizeInformation);
	dTokenInforLength = asiAclSize.AclBytesInUse;
	
	fRet = SetTokenInformation(hToken,
							   TokenDefaultDacl,
							   &tiTokenInfor,
							   dTokenInforLength);//sizeof(TOKEN_DEFAULT_DACL)

	if(!fRet)
		return FALSE;
	LocalFree(pSid);
	CloseHandle(hToken);
	
	
	return TRUE;*/

}

//ʹ��Windows��HeapAlloc�������ж�̬�ڴ����
#define myheapalloc(x) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, x))
#define myheapfree(x)  (HeapFree(GetProcessHeap(), 0, x))

typedef BOOL (WINAPI *SetSecurityDescriptorControlFnPtr)(
	IN PSECURITY_DESCRIPTOR pSecurityDescriptor,
	IN SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
	IN SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet );

typedef BOOL (WINAPI *AddAccessAllowedAceExFnPtr)(
	PACL pAcl,
	DWORD dwAceRevision,
	DWORD AceFlags,
	DWORD AccessMask,
	PSID pSid );

BOOL AddAccessRights(const TCHAR *lpszFileName, const TCHAR *lpszAccountName, 
      DWORD dwAccessMask) {

   // ����SID����
   SID_NAME_USE   snuType;

   // ������LookupAccountName��صı�����ע�⣬ȫΪ0��Ҫ�ڳ����ж�̬���䣩
   TCHAR *        szDomain       = NULL;
   DWORD          cbDomain       = 0;
   LPVOID         pUserSID       = NULL;
   DWORD          cbUserSID      = 0;

   // ���ļ���صİ�ȫ������ SD �ı���
   PSECURITY_DESCRIPTOR pFileSD  = NULL;     // �ṹ����
   DWORD          cbFileSD       = 0;        // SD��size

   // һ���µ�SD�ı��������ڹ����µ�ACL�������е�ACL����Ҫ�¼ӵ�ACL����������
   SECURITY_DESCRIPTOR  newSD;

   // ��ACL ��صı���
   PACL           pACL           = NULL;
   BOOL           fDaclPresent;
   BOOL           fDaclDefaulted;
   ACL_SIZE_INFORMATION AclInfo;

   // һ���µ� ACL ����
   PACL           pNewACL        = NULL;  //�ṹָ�����
   DWORD          cbNewACL       = 0;     //ACL��size

   // һ����ʱʹ�õ� ACE ����
   LPVOID         pTempAce       = NULL;
   UINT           CurrentAceIndex = 0;  //ACE��ACL�е�λ��

   UINT           newAceIndex = 0;  //�����ACE��ACL�е�λ��

   //API�����ķ���ֵ���������еĺ���������ʧ�ܡ�
   BOOL           fResult;
   BOOL           fAPISuccess;

   SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

   // ����������������µ�API����������Windows 2000���ϰ汾�Ĳ���ϵͳ֧�֡� 
   // �ڴ˽���Advapi32.dll�ļ��ж�̬���롣�����ʹ��VC++ 6.0������򣬶�������
   // ʹ�������������ľ�̬���ӡ�����Ϊ��ı�����ϣ�/D_WIN32_WINNT=0x0500
   // �ı������������ȷ�����SDK��ͷ�ļ���lib�ļ������µġ�
   SetSecurityDescriptorControlFnPtr _SetSecurityDescriptorControl = NULL;
   AddAccessAllowedAceExFnPtr _AddAccessAllowedAceEx = NULL; 

   __try {

      // 
      // STEP 1: ͨ���û���ȡ��SID
      //     ����һ����LookupAccountName���������������Σ���һ����ȡ������Ҫ
      // ���ڴ�Ĵ�С��Ȼ�󣬽����ڴ���䡣�ڶ��ε��ò���ȡ�����û����ʻ���Ϣ��
      // LookupAccountNameͬ������ȡ�����û������û������Ϣ������ο�MSDN��
      //

      fAPISuccess = LookupAccountName(NULL, lpszAccountName,
            pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);

      // ���ϵ���API��ʧ�ܣ�ʧ��ԭ�����ڴ治�㡣��������Ҫ���ڴ��С������
      // �����Ǵ�����ڴ治��Ĵ���

      if (fAPISuccess)
         __leave;
      else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
         _tprintf(TEXT("LookupAccountName() failed. Error %d\n"), 
               GetLastError());
         __leave;
      }

      pUserSID = myheapalloc(cbUserSID);
      if (!pUserSID) {
         _tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
         __leave;
      }

      szDomain = (TCHAR *) myheapalloc(cbDomain * sizeof(TCHAR));
      if (!szDomain) {
         _tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
         __leave;
      }

      fAPISuccess = LookupAccountName(NULL, lpszAccountName,
            pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);
      if (!fAPISuccess) {
         _tprintf(TEXT("LookupAccountName() failed. Error %d\n"), 
               GetLastError());
         __leave;
      }

      // 
      // STEP 2: ȡ���ļ���Ŀ¼����صİ�ȫ������SD
      //     ʹ��GetFileSecurity����ȡ��һ���ļ�SD�Ŀ�����ͬ�����������Ҳ
       // �Ǳ��������Σ���һ��ͬ����ȡSD���ڴ泤�ȡ�ע�⣬SD�����ָ�ʽ������ص�
       // ��self-relative���� ��ȫ�ģ�absolute����GetFileSecurityֻ��ȡ������
       // ��صġ�����SetFileSecurity����Ҫ��ȫ�ġ������Ϊʲô��Ҫһ���µ�SD��
       // ������ֱ����GetFileSecurity���ص�SD�Ͻ����޸ġ���Ϊ������صġ���Ϣ
       // �ǲ������ġ�

      fAPISuccess = GetFileSecurity(lpszFileName, 
            secInfo, pFileSD, 0, &cbFileSD);

      // ���ϵ���API��ʧ�ܣ�ʧ��ԭ�����ڴ治�㡣��������Ҫ���ڴ��С������
      // �����Ǵ�����ڴ治��Ĵ���
      if (fAPISuccess)
         __leave;
      else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
         _tprintf(TEXT("GetFileSecurity() failed. Error %d\n"), 
               GetLastError());
         __leave;
      }

      pFileSD = myheapalloc(cbFileSD);
      if (!pFileSD) {
         _tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
         __leave;
      }

      fAPISuccess = GetFileSecurity(lpszFileName, 
            secInfo, pFileSD, cbFileSD, &cbFileSD);
      if (!fAPISuccess) {
         _tprintf(TEXT("GetFileSecurity() failed. Error %d\n"), 
               GetLastError());
         __leave;
      }

      // 
      // STEP 3: ��ʼ��һ���µ�SD
      // 
      if (!InitializeSecurityDescriptor(&newSD, 
            SECURITY_DESCRIPTOR_REVISION)) {
         _tprintf(TEXT("InitializeSecurityDescriptor() failed.")
            TEXT("Error %d\n"), GetLastError());
         __leave;
      }

      // 
      // STEP 4: ��GetFileSecurity ���ص�SD��ȡDACL
      // 
      if (!GetSecurityDescriptorDacl(pFileSD, &fDaclPresent, &pACL,
            &fDaclDefaulted)) {
         _tprintf(TEXT("GetSecurityDescriptorDacl() failed. Error %d\n"),
               GetLastError());
         __leave;
      }

      // 
      // STEP 5: ȡ DACL���ڴ�size
      //     GetAclInformation�����ṩDACL���ڴ��С��ֻ����һ������Ϊ
      // ACL_SIZE_INFORMATION��structure�Ĳ�������DACL����Ϣ����Ϊ��
      // �������Ǳ������е�ACE��
      AclInfo.AceCount = 0; // Assume NULL DACL.
      AclInfo.AclBytesFree = 0;
      AclInfo.AclBytesInUse = sizeof(ACL);

      if (pACL == NULL)
         fDaclPresent = FALSE;

      // ���DACL��Ϊ�գ���ȡ����Ϣ�������������¡��Թ�������DACLΪ�գ�
      if (fDaclPresent) {            
         if (!GetAclInformation(pACL, &AclInfo, 
               sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) {
            _tprintf(TEXT("GetAclInformation() failed. Error %d\n"),
                  GetLastError());
            __leave;
         }
      }

      // 
      // STEP 6: �����µ�ACL��size
      //    ����Ĺ�ʽ�ǣ�ԭ�е�DACL��size������Ҫ��ӵ�һ��ACE��size����
      // ������һ����ACE��ص�SID��size������ȥ�����ֽ��Ի�þ�ȷ�Ĵ�С��
      cbNewACL = AclInfo.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) 
            + GetLengthSid(pUserSID) - sizeof(DWORD);


      // 
      // STEP 7: Ϊ�µ�ACL�����ڴ�
      // 
      pNewACL = (PACL) myheapalloc(cbNewACL);
      if (!pNewACL) {
         _tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
         __leave;
      }

      // 
      // STEP 8: ��ʼ���µ�ACL�ṹ
      // 
      if (!InitializeAcl(pNewACL, cbNewACL, ACL_REVISION2)) {
         _tprintf(TEXT("InitializeAcl() failed. Error %d\n"), 
               GetLastError());
         __leave;
      }

      // 
      // STEP 9  ����ļ���Ŀ¼�� DACL �����ݣ��������е�ACE���µ�DACL��
      // 
      //     ����Ĵ���������ȼ��ָ���ļ���Ŀ¼���Ƿ���ڵ�DACL������еĻ���
      // ��ô�Ϳ������е�ACE���µ�DACL�ṹ�У����ǿ��Կ���������ķ����ǲ���
      // ACL_SIZE_INFORMATION�ṹ�е�AceCount��Ա����ɵġ������ѭ���У�
      // �ᰴ��Ĭ�ϵ�ACE��˳�������п�����ACE��ACL�е�˳���Ǻܹؼ��ģ����ڿ�
      // �������У��ȿ����Ǽ̳е�ACE������֪��ACE����ϲ�Ŀ¼�м̳�������
      // 

      newAceIndex = 0;

      if (fDaclPresent && AclInfo.AceCount) {

         for (CurrentAceIndex = 0; 
               CurrentAceIndex < AclInfo.AceCount;
               CurrentAceIndex++) {

            // 
            // STEP 10: ��DACL��ȡACE
            // 
            if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
               _tprintf(TEXT("GetAce() failed. Error %d\n"), 
                     GetLastError());
               __leave;
            }

            // 
            // STEP 11: ����Ƿ��ǷǼ̳е�ACE
            //     �����ǰ��ACE��һ���Ӹ�Ŀ¼�̳�����ACE����ô���˳�ѭ����
            // ��Ϊ���̳е�ACE�����ڷǼ̳е�ACE֮�󣬶�������Ҫ��ӵ�ACE
            // Ӧ�������еķǼ̳е�ACE֮�����еļ̳е�ACE֮ǰ���˳�ѭ��
            // ����Ϊ��Ҫ���һ���µ�ACE���µ�DACL�У���������ٰѼ̳е�
            // ACE�������µ�DACL�С�
            //
            if (((ACCESS_ALLOWED_ACE *)pTempAce)->Header.AceFlags
               & INHERITED_ACE)
               break;

            // 
            // STEP 12: ���Ҫ������ACE��SID�Ƿ����Ҫ�����ACE��SIDһ����
            // ���һ������ô��Ӧ�÷ϵ��Ѵ��ڵ�ACE��Ҳ����˵��ͬһ���û��Ĵ�ȡ
            // Ȩ�޵����õ�ACE����DACL��Ӧ��Ψһ�������������ͬһ�û�������
            // �˵�ACE�����ǿ��������û���ACE��
            // 
            if (EqualSid(pUserSID,
               &(((ACCESS_ALLOWED_ACE *)pTempAce)->SidStart)))
               continue;

            // 
            // STEP 13: ��ACE���뵽�µ�DACL��
            //    ����Ĵ����У�ע�� AddAce �����ĵ����������������������˼�� 
            // ACL�е�����ֵ����ΪҪ��ACE�ӵ�ĳ����λ��֮�󣬲���MAXDWORD��
              // ��˼��ȷ����ǰ��ACE�Ǳ����뵽����λ�á�
            //
            if (!AddAce(pNewACL, ACL_REVISION, MAXDWORD, pTempAce,
                  ((PACE_HEADER) pTempAce)->AceSize)) {
               _tprintf(TEXT("AddAce() failed. Error %d\n"), 
                     GetLastError());
               __leave;
            }

            newAceIndex++;
         }
      }


	// 
    // STEP 14: ��һ�� access-allowed ��ACE ���뵽�µ�DACL��
    //     ǰ���ѭ�����������еķǼ̳���SIDΪ�����û���ACE���˳�ѭ���ĵ�һ����
    // ���Ǽ�������ָ����ACE����ע�������ȶ�̬װ����һ��AddAccessAllowedAceEx
    // ��API���������װ�ز��ɹ����͵���AddAccessAllowedAce������ǰһ��������
    // ��Windows 2000�Ժ�İ汾֧�֣�NT��û�У�����Ϊ��ʹ���°汾�ĺ�����������
    // ���ȼ��һ�µ�ǰϵͳ�пɲ�����װ���������������������ʹ�á�ʹ�ö�̬����
    // ��ʹ�þ�̬���ӵĺô��ǣ���������ʱ������Ϊû�����API����������
    // 
    // Ex��ĺ��������һ������AceFlag�������˲�������������������ǿ���������һ
    // ����ACE_HEADER�Ľṹ���Ա������������õ�ACE���Ա�����Ŀ¼���̳���ȥ���� 
    // AddAccessAllowedAce�������ܶ��������������AddAccessAllowedAce����
    // �У�����ACE_HEADER����ṹ���óɷǼ̳еġ�
    // 
      _AddAccessAllowedAceEx = (AddAccessAllowedAceExFnPtr)
            GetProcAddress(GetModuleHandle(TEXT("advapi32.dll")),
            "AddAccessAllowedAceEx");

      if (_AddAccessAllowedAceEx) {
           if (!_AddAccessAllowedAceEx(pNewACL, ACL_REVISION2,
              CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ,
                dwAccessMask, pUserSID)) {
             _tprintf(TEXT("AddAccessAllowedAceEx() failed. Error %d\n"),
                   GetLastError());
             __leave;
          }
      }else{
          if (!AddAccessAllowedAce(pNewACL, ACL_REVISION2, 
                dwAccessMask, pUserSID)) {
             _tprintf(TEXT("AddAccessAllowedAce() failed. Error %d\n"),
                   GetLastError());
             __leave;
          }
      }

      // 
      // STEP 15: �����Ѵ��ڵ�ACE��˳�򿽱��Ӹ�Ŀ¼�̳ж�����ACE
      // 
      if (fDaclPresent && AclInfo.AceCount) {

         for (; 
              CurrentAceIndex < AclInfo.AceCount;
              CurrentAceIndex++) {

            // 
            // STEP 16: ���ļ���Ŀ¼����DACL�м���ȡACE
            // 
            if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
               _tprintf(TEXT("GetAce() failed. Error %d\n"), 
                     GetLastError());
               __leave;
            }

            // 
            // STEP 17: ��ACE���뵽�µ�DACL��
            // 
            if (!AddAce(pNewACL, ACL_REVISION, MAXDWORD, pTempAce,
                  ((PACE_HEADER) pTempAce)->AceSize)) {
               _tprintf(TEXT("AddAce() failed. Error %d\n"), 
                     GetLastError());
               __leave;
            }
         }
      }

      // 
      // STEP 18: ���µ�ACL���õ��µ�SD��
      // 
      if (!SetSecurityDescriptorDacl(&newSD, TRUE, pNewACL, 
            FALSE)) {
         _tprintf(TEXT("SetSecurityDescriptorDacl() failed. Error %d\n"),
               GetLastError());
         __leave;
      }

      // 
      // STEP 19: ���ϵ�SD�еĿ��Ʊ���ٿ������µ�SD�У�����ʹ�õ���һ���� 
      // SetSecurityDescriptorControl() ��API�������������ͬ��ֻ������
      // Windows 2000�Ժ�İ汾�У��������ǻ���Ҫ��̬�ذ����advapi32.dll 
      // �����룬���ϵͳ��֧������������ǾͲ������ϵ�SD�Ŀ��Ʊ���ˡ�
      // 
      _SetSecurityDescriptorControl =(SetSecurityDescriptorControlFnPtr)
            GetProcAddress(GetModuleHandle(TEXT("advapi32.dll")),
            "SetSecurityDescriptorControl");
      if (_SetSecurityDescriptorControl) {

         SECURITY_DESCRIPTOR_CONTROL controlBitsOfInterest = 0;
         SECURITY_DESCRIPTOR_CONTROL controlBitsToSet = 0;
         SECURITY_DESCRIPTOR_CONTROL oldControlBits = 0;
         DWORD dwRevision = 0;

         if (!GetSecurityDescriptorControl(pFileSD, &oldControlBits,
            &dwRevision)) {
            _tprintf(TEXT("GetSecurityDescriptorControl() failed.")
                  TEXT("Error %d\n"), GetLastError());
            __leave;
         }

         if (oldControlBits & SE_DACL_AUTO_INHERITED) {
            controlBitsOfInterest =
               SE_DACL_AUTO_INHERIT_REQ |
               SE_DACL_AUTO_INHERITED ;
            controlBitsToSet = controlBitsOfInterest;
         }
         else if (oldControlBits & SE_DACL_PROTECTED) {
            controlBitsOfInterest = SE_DACL_PROTECTED;
            controlBitsToSet = controlBitsOfInterest;
         }        

         if (controlBitsOfInterest) {
            if (!_SetSecurityDescriptorControl(&newSD,
               controlBitsOfInterest,
               controlBitsToSet)) {
               _tprintf(TEXT("SetSecurityDescriptorControl() failed.")
                     TEXT("Error %d\n"), GetLastError());
               __leave;
            }
         }
      }

      // 
      // STEP 20: ���µ�SD�������õ��ļ��İ�ȫ�����У�ǧɽ��ˮ�������ڵ��ˣ�
      // 
      if (!SetFileSecurity(lpszFileName, secInfo,
            &newSD)) {
         _tprintf(TEXT("SetFileSecurity() failed. Error %d\n"), 
               GetLastError());
         __leave;
      }

      fResult = TRUE;

   } __finally {

      // 
      // STEP 21: �ͷ��ѷ�����ڴ棬����Memory Leak
      // 
      if (pUserSID)  myheapfree(pUserSID);
      if (szDomain)  myheapfree(szDomain);
      if (pFileSD) myheapfree(pFileSD);
      if (pNewACL) myheapfree(pNewACL);
   }

   return fResult;
}



//�����ļ���
BOOL CreateDir(const char* szDir,const char* szUserName,const char* szPwd)
{
	if (!szDir || strlen(szDir) < 2)
		return false;

	char		szTmp[_MAX_PATH];
	memset(szTmp,0,sizeof(szTmp));
	
	int			iLen		= strlen(szDir);
	int			iPos		= 0;

	if (iLen >= _MAX_PATH)
		return false;

	// �������������̵ĵ��Լ���
	HANDLE hToken;
	TOKEN_PRIVILEGES tk;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	//OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tk.Privileges[0].Luid);
	tk.PrivilegeCount = 1;
	tk.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	BOOL b = AdjustTokenPrivileges(hToken, FALSE, &tk, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	CloseHandle(hToken);


	//�ȴ���WINDOWS�û�
	if (!CreateUserInWindow(szUserName,szPwd))
		return false;

	//���û���ӵ�Users����ȥ
	LOCALGROUP_MEMBERS_INFO_3	lm;
	WCHAR*			wszUserName	= NULL;
	GetBStrFromChar(szUserName,&wszUserName);
	lm.lgrmi3_domainandname	= wszUserName;
	NetLocalGroupAddMembers(NULL,L"Users",3,(LPBYTE)&lm,1);
	if(wszUserName)
	{
		delete [] wszUserName;
		wszUserName = NULL;
	}

	// ����Ŀ¼
	for (int i = 0; i < iLen; i++)
	{
		if (szDir[i] == '\\')
		{
			if (i != 0 && szDir[i-1] == ':')
				continue;

			memcpy(szTmp,szDir,i);
			if (!CreateDirectory(szTmp,NULL))
			{
				if (GetLastError() != ERROR_ALREADY_EXISTS)
				{
					memset(szTmp,0,sizeof(szTmp));
					break;
				}
			}
		}
		else
		{
			if (i == (iLen - 1))
			{	
				strcpy(szTmp,szDir);
				if (!CreateDirectory(szDir,NULL))
				{
					if (GetLastError() != ERROR_ALREADY_EXISTS)
					{
						memset(szTmp,0,sizeof(szTmp));
						break;
					}
				}
			}
		}
	}
	if (strlen(szTmp) <= 0)
		return false;

	// ����Ȩ��
	AddAccessRights( szTmp,szUserName,GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE );


	//��������Ŀ¼��,�ٴ�����Ŀ¼
//	TCHAR		szCurrentDirectory[100];
//	memset(szCurrentDirectory,0,sizeof(szCurrentDirectory));
//	GetCurrentDirectory(100,(LPTSTR)szCurrentDirectory);

	char		szSubDir[_MAX_PATH];
	sprintf(szSubDir,"%s\\IMAGES",szTmp);
	
	if (!CreateDirectory(szSubDir,NULL) &&  ERROR_ALREADY_EXISTS != GetLastError())
		return FALSE;
	
	sprintf(szSubDir,"%s\\DEM",szTmp);
	if (!CreateDirectory(szSubDir,NULL) &&  ERROR_ALREADY_EXISTS != GetLastError())
		return FALSE;
	
	
	sprintf(szSubDir,"%s\\DOM",szTmp);
	if (!CreateDirectory(szSubDir,NULL) &&  ERROR_ALREADY_EXISTS != GetLastError())
		return FALSE;
	
	sprintf(szSubDir,"%s\\DVS",szTmp);
	if (!CreateDirectory(szSubDir,NULL) &&  ERROR_ALREADY_EXISTS != GetLastError())
		return FALSE;
	
	sprintf(szSubDir,"%s\\DRG",szTmp);
	if (!CreateDirectory(szSubDir,NULL) &&  ERROR_ALREADY_EXISTS != GetLastError())
		return FALSE;

	return true;
}

BOOL SetDefDirSecurity(LPCTSTR szDir,LPCTSTR szName)
{
	if(szDir == NULL || szName == NULL || strlen(szDir) <= 0 || strlen(szName) <= 0)
		return FALSE;
	
	BOOL fRet;

	PSECURITY_DESCRIPTOR psd = NULL;  //����һ��SD

	PSID pSid = NULL;  //����һ��SID��������100
	DWORD sidBufferSize = 0;
	
	char domainBuffer[256];	
	DWORD domainBufferSize = 256;

	SID_NAME_USE snu;


	//����һ���û�����ȡ���û���SID
	fRet = LookupAccountName(NULL,
							 szName, 
							 pSid,	
							 &sidBufferSize, 
							 domainBuffer,
							 &domainBufferSize, 
							 &snu);
	DWORD dwErr = GetLastError();
	if(dwErr == ERROR_INSUFFICIENT_BUFFER)
    {
        pSid = LocalAlloc(LPTR, sidBufferSize);
        if(!pSid)
        {
            return FALSE;
        }
        
		fRet = LookupAccountName(NULL,
								 szName, 
								 pSid,	
								 &sidBufferSize, 
								 domainBuffer,
								 &domainBufferSize, 
								 &snu);
		
        if(fRet == FALSE)
        {
            LocalFree(pSid);
			pSid = NULL;
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }


	//��ʼ��һ��SD
	psd = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
    if(psd)
    {
        BOOL fInit = InitializeSecurityDescriptor(psd,SECURITY_DESCRIPTOR_REVISION);
        if(!fInit)
        {
            LocalFree(psd);
            psd = NULL;
			return FALSE;
        }
    }

	DWORD cbSid = GetLengthSid(pSid);
//     PACL pAcl = NULL;
	
//	BYTE aclBuffer[1024];
	//	PACL pacl=(PACL)&aclBuffer; //����һ��ACL��������1024
	

	BYTE Buffer[1024];//��ʼ��һ��ACL
	PACL pAcl=(PACL)&Buffer;
		
   // DWORD cbAcl = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + cbSid;
	//DWORD cbAcl = sizeof(ACCESS_ALLOWED_ACE)  + cbSid - sizeof(ACCESS_ALLOWED_ACE.SidStart);
    // pAcl = (PACL)LocalAlloc(LPTR, cbAcl);


	//////////////////////////////////////////////////////////////
	
	InitializeAcl(pAcl, 1024, ACL_REVISION);

	
	/////////////////////////////////////////////////////////////
	
	//���ø��û���Access-Allowed��ACE����Ȩ��Ϊ������Ȩ�ޡ�	
    BOOL fAdded = AddAccessAllowedAceEx(pAcl,
									  ACL_REVISION,
									  0,
									  GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
									  pSid);
    LocalFree(pSid);
	
    if(!SetSecurityDescriptorDacl(psd, TRUE, pAcl, FALSE))		//��ACL���õ�SD��
		return FALSE;		

	if(!SetFileSecurity(szDir,DACL_SECURITY_INFORMATION,psd))//��SD�ŵ��ļ���ȫ�ṹSA��
		return FALSE;
	
	return TRUE;
}



//����Ŀ¼
//����˵��: [in] Ҫ���ӵ�����Ȩ�޵��û�ID [in] szPath -- Ҫ����ı���Ŀ¼   
//          [out] szRetServerDirPath -- ���ش���������������·��
BOOL AddShareDirInServer(const char* szUserName,const char* szPath,
						 const char* szNetName,char* szRetServerDirPath)
{//(!AddShareDirInServer(szWksID,szDefaultDir,szName,szServerDir))
	if (!szPath || strlen(szPath) <= 0 || !szRetServerDirPath)
		return false;
	
		/* �Ƶ��˴���Ŀ¼��λ��
		HANDLE hToken;
		TOKEN_PRIVILEGES tk;
        
		  // �������������̵ĵ��Լ���
		  OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		  LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tk.Privileges[0].Luid);
		  tk.PrivilegeCount = 1;
		  tk.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		  BOOL b = AdjustTokenPrivileges(hToken, FALSE, &tk, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	*/
	
	DWORD    nas;
	SHARE_INFO_2      p;
	DWORD             dwParamErr = NULL;
	
	char				szShareName[MAX_PATH];
	sprintf(szShareName,"%s$",szNetName);
	//���
	p.shi2_path = (LMSTR)AsWideString(szPath);
	p.shi2_remark         = NULL;
	p.shi2_netname = (LMSTR)AsWideString(szShareName);
	p.shi2_passwd         = NULL;
	p.shi2_type           = STYPE_DISKTREE; // disk drive
	p.shi2_permissions    = ACCESS_ALL;
	p.shi2_max_uses       = -1;
	p.shi2_current_uses   = 0;
	nas = NetShareAdd(NULL, 2, (LPBYTE)&p, &dwParamErr);
	if (nas != NERR_Success && nas != NERR_DuplicateShare )
		return false;
	
	if(p.shi2_path)
	{
		delete [] p.shi2_path;
		p.shi2_path = NULL;
	}

	if(p.shi2_netname)
	{
		delete [] p.shi2_netname;
		p.shi2_netname = NULL;
	}



	//���ù����Ȩ��
//	HANDLE						hToken; 
	BYTE						sidBuffer[100];
	PSID						pSID		= (PSID)&sidBuffer;
	SID_IDENTIFIER_AUTHORITY	SIDAuth		= SECURITY_NT_AUTHORITY;
	DWORD						dwRes;

	//��ɾ��Ŀǰ���еĹ���Ȩ��,��EveryOne
	dwRes = SetNamedSecurityInfo(
		szShareName,						//������
		SE_LMSHARE,							//Ȩ������Ϊ����
		DACL_SECURITY_INFORMATION,			
		NULL, NULL,						
		NULL,							
		NULL);							

	if (dwRes != ERROR_SUCCESS) 
	{
		TRACE("First SetNamedSecurityInfo call failed: %u\n", dwRes); 
		return false;
	}

	//�����ӹ���Ȩ��
	PSID			sid		= NULL;
	if (!GetUserSID(szUserName,&sid))
		return false;

	//���乲��Ȩ��
	PACL			pACL	= NULL; 
	DWORD			cbACL = 1024; 

	pACL = (PACL) LocalAlloc(LPTR, cbACL); 

	if (!InitializeAcl(pACL, cbACL, ACL_REVISION2)) 
	{
		if (sid)
			HeapFree(GetProcessHeap(), 0, sid);
		return false;
	}

	AddAccessAllowedAce(pACL,ACL_REVISION,GENERIC_ALL,sid);

	dwRes = SetNamedSecurityInfo(
		szShareName,                 // name of the object
		SE_LMSHARE,					 // type of object
		DACL_SECURITY_INFORMATION,  // change only the object's owner
		NULL,                        // SID of Administrator group
		NULL, pACL, NULL); 

	if (sid)
        HeapFree(GetProcessHeap(), 0, sid);

	if(pACL != NULL) 
        LocalFree((HLOCAL) pACL); 


	if (dwRes != ERROR_SUCCESS) 
	{
		TRACE("Could not set owner. Error: %u\n", dwRes); 
		return FALSE;
	}


	//��������·��
	char		szComputerName[_MAX_PATH];
	memset(szComputerName,0,sizeof(szComputerName));
	DWORD		lSize	= sizeof(szComputerName);

	GetComputerName(szComputerName,&lSize);

	sprintf(szRetServerDirPath,"\\\\%s\\%s",szComputerName,szShareName);

	return true;
}

//���ļ����еõ��ļ�����չ��
char* GetExtName(const char* szFileName)
{
	if (!szFileName || strlen(szFileName) < 0)
		return NULL;

	int		iLen	= strlen(szFileName);

	for (int i = (iLen -1); i >= 0; i--)
	{
		if (*(szFileName+i) == '.' && i != (iLen -1))
			return (char*)(szFileName+i+1);
	}
	return NULL;
}


//�ַ����İ�ȫ����
BOOL SafeCopyChar(char* szDest,const char* szSource)
{
	TRY
	{
		strcpy(szDest,szSource);
	}
	CATCH_ALL(e)
	{
		return false;
	}
	END_CATCH_ALL

	return true;
}


void EmptyDirectory(char* folderPath)
{
	char fileFound[256];
	WIN32_FIND_DATA info;
	HANDLE hp; 

	sprintf(fileFound, "%s\\*.*", folderPath);
	
	hp = FindFirstFile(fileFound, &info);	
	if (hp == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if (!((strcmp(info.cFileName, ".")==0)||(strcmp(info.cFileName, "..")==0)))
		{
			if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)
			{
				string subFolder = folderPath;
				subFolder.append("\\");
				subFolder.append(info.cFileName);
				EmptyDirectory((char*)subFolder.c_str());
				RemoveDirectory(subFolder.c_str());
			}
			else
			{
				sprintf(fileFound,"%s\\%s", folderPath, info.cFileName);
				BOOL retVal = DeleteFile(fileFound);
			}
		}

	}while(FindNextFile(hp, &info)); 

	FindClose(hp);
}

//����������Դ�ķ���������
BOOL ParseNetServerName(const char* szResoureName,char* szServerName)
{
	if (!szResoureName || !szServerName)
		return false;
	int		iLen = strlen(szResoureName);

	if (iLen <= 0)
		return false;

	if (szResoureName[0] != '\\' || szResoureName[1] != '\\')
		return false;

	int		iPos	= 0;

	for (int i = 2; i < iLen; i++)
	{
		if (szResoureName[i] == '\\')
			iPos = i;
	}

	memcpy(szServerName,szResoureName,iPos);
	szServerName[iPos] = 0;

	return true;
}







/********************************************/
/*                 ͨ����                   */
/********************************************/


/***********************************/
/*            ����������           */
/***********************************/
CDpIntArray::CDpIntArray()
{

}

CDpIntArray::~CDpIntArray()
{
	m_nArray.clear();
}

int CDpIntArray::GetCount()
{
	return m_nArray.size();
}

int	CDpIntArray::GetItem(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_nArray.size())
		return 0;
	return m_nArray[iIndex];
}

void CDpIntArray::AddItem(int iItem)
{
	//��ֹ�����������������±�ֵ
	if (m_nArray.size() >= (m_nArray.max_size() - 2))
		return;
	m_nArray.push_back(iItem);
}

BOOL CDpIntArray::ItemIsExist(int iItem)
{
	int		iCount = m_nArray.size();
	for (int i = 0; i < iCount; i++)
	{
		if (m_nArray[i] == iItem)
			return true;
	}
	return false;
}







/***********************************/
/*		   �ַ���������			   */
/***********************************/				 

CDpStringArray::CDpStringArray()
{

}

//�����������ͷ��ַ�����ռ���ڴ�
CDpStringArray::~CDpStringArray()
{
	DelAllItem();
}

//�õ���ǰ�ַ����������Ŀ��
int	CDpStringArray::GetCount()					
{
	return m_strArray.size();
}

//�õ�ָ��λ�õ��ַ���,���㿪ʼ
char* CDpStringArray::GetItem(int iIndex)
{
	int iCount = m_strArray.size();

	if (iCount > 0 && iIndex < iCount)
		return m_strArray[iIndex];
	else
		return NULL;
}

//���ַ�������������ַ���
BOOL CDpStringArray::AddString(const char* szItem)
{
	if (!szItem)
		return false;
	//��ֹ�����������������±�ֵ
	if (m_strArray.size() >= (m_strArray.max_size() - 2))
		return false;
	
	char*	sTmp	= new char[strlen(szItem)+1];
	memset(sTmp,0,strlen(szItem)+1);
	strcpy(sTmp,szItem);

	m_strArray.push_back(sTmp);

	return true;
}

//ɾ��ָ����������
void CDpStringArray::DelString(int iIndex)
{
	int		iCount	= m_strArray.size();

	if (iCount <= 0 || iIndex > (iCount - 1))
		return;

	//�ͷ��ڴ�
	char*	sTmp = GetItem(iIndex);
	if (sTmp)
	{
		delete [] sTmp;
		sTmp = NULL;
	}

	m_strArray.erase(m_strArray.begin()+iIndex);

}

void CDpStringArray::Copy(CDpStringArray &other)
{
	DelAllItem();
	{
		int	iCount	= other.GetCount();
		
		for (int i = 0; i < iCount; i++)
		{
			AddString(other.GetItem(i));
		}
	}
}

//ɾ�����е�������
void CDpStringArray::DelAllItem()
{
	if (!m_strArray.empty())
	{
		int		iCount	= m_strArray.size();

		for (int i = 0; i < iCount; i++)
		{
			char*	sz = m_strArray[i];
			if (sz)
			{
				delete [] sz;
				sz = NULL;
			}
		}

		m_strArray.erase(m_strArray.begin(),m_strArray.end());
	}	

}



/***********************************/
/*		   �û���Ϣ������			   */
/***********************************/				 
CDpUserArray::CDpUserArray()
{

}

CDpUserArray::~CDpUserArray()
{
	DelAllItem();
}

//�õ���ǰ�������Ŀ��
int	CDpUserArray::GetCount()	
{
	return m_Array.size();
}

//�õ�ָ��λ�õ���Ϣ,���㿪ʼ
const _DpUserInfo* CDpUserArray::GetItem(int iIndex)
{
	int iCount = m_Array.size();

	if (iCount > 0 && iIndex < iCount)
		return m_Array[iIndex];
	else
		return NULL;

}

//����Ϣ�����������Ϣ
BOOL CDpUserArray::AddUserInfo(_DpUserInfo* pInfo)
{
	if (!pInfo)
		return false;
	//��ֹ�����������������±�ֵ
	if (m_Array.size() >= (m_Array.max_size() - 2))
		return false;
	
	m_Array.push_back(pInfo);

	return true;
}

//ɾ��ָ����������
void CDpUserArray::DelItem(int iIndex)
{
	int		iCount	= m_Array.size();

	if (iCount <= 0 || iIndex > (iCount - 1))
		return;

	//�ͷ��ڴ�
	const _DpUserInfo*	nTmp = GetItem(iIndex);
	if (nTmp)
	{
		_DpUserInfo*	p = (_DpUserInfo*)nTmp;
		if (p->szUserID)
		{
			delete [] p->szUserID;
			p->szUserID = NULL;
		}
		if (p->szUserDesc)		
		{
			delete [] p->szUserDesc;
			p->szUserDesc = NULL;
		}
		delete p;
		p = NULL;
	}
	m_Array.erase(m_Array.begin()+iIndex);

}

//ɾ��ָ������Ϣ��	
void CDpUserArray::DelItem(_DpUserInfo* pInfo)
{
	if (!pInfo)
		return;

	UserInfoArray::iterator	i;
	for (i = m_Array.begin(); i != m_Array.end(); ++i)
	{
		if (pInfo == (*i))
		{
			if (pInfo->szUserID)
			{
				delete [] pInfo->szUserID;
				pInfo->szUserID = NULL;
			}
			if (pInfo->szUserDesc)	
			{
				delete [] pInfo->szUserDesc;
				pInfo->szUserDesc = NULL;
			}
			delete pInfo;
			pInfo = NULL;
			m_Array.erase(i);
			break;
		}
	}

}



//ɾ�����е���Ϣ��
void CDpUserArray::DelAllItem()
{
	if (!m_Array.empty())
	{
		int		iCount	= m_Array.size();

		for (int i = 0; i < iCount; i++)
		{
			const _DpUserInfo*	tmp = m_Array[i];
			if (tmp)
			{
				_DpUserInfo*			p	= (_DpUserInfo*)tmp;
				if (p->szUserID)
				{
					delete [] p->szUserID;
					p->szUserID = NULL;
				}
				if (p->szUserDesc)	
				{
					delete [] p->szUserDesc;
					p->szUserDesc = NULL;
				}
				delete p;
				p = NULL;
			}
		}

		m_Array.erase(m_Array.begin(),m_Array.end());
	}	
}












/***********************************/
/*		        ������			   */
/***********************************/
CDpRect::CDpRect()
{
	m_dMinX		= 0;
	m_dMinY		= 0;
	m_dMaxX		= 0;
	m_dMaxY		= 0;
}
				 
CDpRect::CDpRect(double dMinX,double dMinY,double dMaxX,double dMaxY)
{
	m_dMinX		= dMinX;
	m_dMinY		= dMinY;
	m_dMaxX		= dMaxX;
	m_dMaxY		= dMaxY;
}

void CDpRect::SetRect(double dMinX,double dMinY,double dMaxX,double dMaxY)
{
	m_dMinX		= dMinX;
	m_dMinY		= dMinY;
	m_dMaxX		= dMaxX;
	m_dMaxY		= dMaxY;
}

void CDpRect::CopyRect(CDpRect* pRt)
{
	SetRect(pRt->m_dMinX,pRt->m_dMinY,pRt->m_dMaxX,pRt->m_dMaxY);
}


//���ο��
double CDpRect::Width() const
{
	return m_dMaxX - m_dMinX;
}

//���θ߶�
double  CDpRect::Height() const
{
	return m_dMaxY - m_dMinY;
}

//�жϵ��Ƿ��ھ���������
BOOL CDpRect::PtInRect(CDpPoint pt) const
{
	return PtInRect(pt.m_dx,pt.m_dy);
}

BOOL CDpRect::PtInRect(double dx,double dy) const
{
	if ((dx - m_dMinX) >= 0.00000000000000001 && 
		(m_dMaxX - dx) >= 0.00000000000000001 && 
		(dy - m_dMinY) >= 0.00000000000000001 && 
		(m_dMaxY - dy) >= 0.00000000000000001)
		return true;
	else
		return false;

}


//�ж�rt�Ƿ��뵱ǰ�����ཻ
BOOL CDpRect::IsIntersect(CDpRect* rt) const
{
	return IsIntersect(rt->m_dMinX,rt->m_dMinY,rt->m_dMaxX,rt->m_dMaxY);
}

//�ж�rt�Ƿ�͵�ǰ�����ཻ,�����ǹ������,��ֻ��Ҫ�ж�������һ���㱻��ǰ���ΰ�������
BOOL CDpRect::IsIntersect(double dMinX,double dMinY,double dMaxX,double dMaxY) const
{
	if( m_dMinX - dMaxX >= 0.0000000000000001 || 
		m_dMaxX - dMinX <= 0.0000000000000001 ||
		m_dMinY - dMaxY >= 0.0000000000000001 || 
		m_dMaxY - dMinY <= 0.0000000000000001)
		return FALSE;
	else
		return TRUE;
}

//�ж�rt�Ƿ񱻵�ǰ����������
BOOL CDpRect::IsInclusive(CDpRect* rt) const
{
	return IsInclusive(rt->m_dMinX,rt->m_dMinY,rt->m_dMaxX,rt->m_dMaxY);
}

BOOL CDpRect::IsInclusive(double dMinX,double dMinY,double dMaxX,double dMaxY) const
{
	if (PtInRect(dMinX,dMinY) && PtInRect(dMaxX,dMaxY))
		return true;
	else
		return false;
}



/***********************************/
/*           �������              */
/***********************************/
CDpRgn::CDpRgn()
{
	m_pX			= NULL;
	m_pY			= NULL;
	m_bVertex		= NULL;
	m_VertexState	= NULL;
	m_nDirection	= -1;
	m_nBufferSize	= 0;
}

CDpRgn::~CDpRgn()
{
	if(m_pX)			delete [] m_pX;
	if(m_pY)			delete [] m_pY;
	if(m_bVertex)		delete [] m_bVertex;
	if(m_VertexState)	delete [] m_VertexState;
	m_pX = NULL;
	m_pY = NULL;
	m_bVertex = NULL;
	m_VertexState =NULL;
}

bool CDpRgn::CreatePolygonRgn(double* pX,double* pY,int nCount)
{
	if(nCount <= 0 || pX == NULL || pY == NULL)
		return false;

	if(nCount > m_nBufferSize || m_nBufferSize > 0)
	{
		if(m_pX)
		{
			delete [] m_pX;
			m_pX = NULL;
		}
		if(m_pY)			
		{
			delete [] m_pY;
			m_pY = NULL;
		}
		if(m_bVertex)	
		{
			delete [] m_bVertex;
			m_bVertex = NULL;
		}
		if(m_VertexState)	
		{
			delete [] m_VertexState;
			m_VertexState = NULL;
		}

		m_nBufferSize=nCount;
		m_pX			= new double[m_nBufferSize+1];
		m_pY			= new double[m_nBufferSize+1];
		m_bVertex		= new bool[m_nBufferSize+1];
		m_VertexState	= new bool[m_nBufferSize+1];
	}

	ASSERT(m_pX!=NULL);
	ASSERT(m_pY!=NULL);
	ASSERT(m_bVertex!=NULL);

	memcpy(m_pX,pX,sizeof(double)*nCount);
	memcpy(m_pY,pY,sizeof(double)*nCount);
	m_pX[m_nBufferSize]	= pX[0];
	m_pY[m_nBufferSize]	= pY[0];

	m_Box.left		= pX[0];
	m_Box.bottom	= pY[0];
	m_Box.right		= pX[0];
	m_Box.top		= pY[0];
	for(int i = 1; i < m_nBufferSize; i++)
	{
		if (m_pX[i]>m_Box.right) 
			m_Box.right=m_pX[i];
		else if
			(m_pX[i]<m_Box.left) m_Box.left=m_pX[i];

		if (m_pY[i]>m_Box.top) 
			m_Box.top=m_pY[i];
		else if
			(m_pY[i]<m_Box.bottom) m_Box.bottom=m_pY[i];
	}
	
	return InitVertexState();
}


bool CDpRgn::CreatePolygonRgn(_DPRealPoint* pRealPt,int nCount)
{
	if(nCount <= 0 || pRealPt == NULL)
		return false;

	if(nCount > m_nBufferSize || m_nBufferSize > 0)
	{
		if(m_pX)			delete [] m_pX;
		if(m_pY)			delete [] m_pY;
		if(m_bVertex)		delete [] m_bVertex;
		if(m_VertexState)	delete [] m_VertexState;

		m_nBufferSize	= nCount;
		m_pX			= new double[m_nBufferSize+1];
		m_pY			= new double[m_nBufferSize+1];
		m_bVertex		= new bool[m_nBufferSize+1];
		m_VertexState	= new bool[m_nBufferSize+1];
	}

	ASSERT(m_pX!=NULL);
	ASSERT(m_pY!=NULL);
	ASSERT(m_bVertex!=NULL);

	m_Box.left		= pRealPt[0].x;
	m_Box.bottom	= pRealPt[0].y;
	m_Box.right		= pRealPt[0].x;
	m_Box.top		= pRealPt[0].y;

	m_pX[0]	= pRealPt[0].x;
	m_pY[0]	= pRealPt[0].y;
	
	for(int i=1; i < m_nBufferSize ;i++)
	{
		m_pX[i]	= pRealPt[i].x;
		m_pY[i]	= pRealPt[i].y;
		
		if (m_pX[i]>m_Box.right)
			m_Box.right = m_pX[i];
		else if (m_pX[i]<m_Box.left)
			m_Box.left = m_pX[i];
		if (m_pY[i] > m_Box.top)
			m_Box.top=m_pY[i];
		else if(m_pY[i] < m_Box.bottom) 
			m_Box.bottom=m_pY[i];
	}
	m_pX[m_nBufferSize]	= pRealPt[0].x;
	m_pY[m_nBufferSize]	= pRealPt[0].y;

	return InitVertexState();
}

int CDpRgn::GetRgnBox(_DpRealRect* pRgnBox)
{
	memcpy(pRgnBox,&m_Box,sizeof(_DpRealRect));
	return 0;//no error happen
}

int CDpRgn::PtInRegion(double& x,double& y)
{
	return IsPtInRegion(x,y);
}

int CDpRgn::PtInRegion(_DPRealPoint& point)
{
	return IsPtInRegion(point.x,point.y);
}


inline int CDpRgn::Intersect( double& x0,double& y0,double& x1,double& y1,
								double& x2,double& y2,double& x3,double& y3,
								int ptIndex2,int ptIndex3,
								double* x,double* y,
								double& minXSrc,double& minYSrc,double& maxXSrc,double& maxYSrc,
								int nCount)
{
	if (x2 > maxXSrc && x3 > maxXSrc)
		return 0;
	else if (x2 < minXSrc && x3 < minXSrc)
		return 0;
	else if (y2 > maxYSrc && y3 > maxYSrc)
		return 0;
	else if (y2 < minYSrc && y3 < minYSrc)
		return 0;

	int bRet = 0;
	if (fabs(y2-y3) < 1e-5)//same height
	{
		if (fabs(y2-y1) < 1e-5 )
		{
			if (fabs( x2-x1 ) < 1e-5 || fabs( x3-x1 ) < 1e-5 )
				return -1;//the vertex
			if (( x2-x1 < 1e-5 && x3-x1 > 1e-5 ) || (x2-x1 > 1e-5 && x3-x1 < 1e-5 ))
				return -1;//in the edge
			if (( x2-x1 < 1e-5 && x3-x1 < 1e-5 ) && m_VertexState[ptIndex2])
				return 1;
		}
	}
	else// intersect
	{
		if(fabs(y2-y1) < 1e-5 && x2-x1 < 1e-5 )
		{
			if (m_VertexState[ptIndex2])
				return 1;//Second
			else
				return 0;
		}

		if (fabs(y3-y1) < 1e-5 && x3-x1 < 1e-5)
			return 1;//First

		double dy21 = y2-y1;
		double dy31 = y1-y3;
		*y = y1;
		*x = (x2*dy31 + x3*dy21)/(dy31 + dy21);
		if(*x < x0 || *x-x1 > 1e-5)
			return 0;//not in
		return 1;
	}
	return bRet;
}

int CDpRgn::IsPtInRegion(double& x,double& y)
{
	if (m_pX==NULL || m_pY==NULL)
		return 0;
	
	if (x < m_Box.left || x > m_Box.right || y < m_Box.bottom || y > m_Box.top)
		return 0;

	memcpy(m_bVertex,m_VertexState,sizeof(bool)*(m_nBufferSize+1));

	double		x0			= m_Box.left-1000;
	double		y0			= y;
	double		intersectX	= 0.0f;
	double		intersectY	= 0.0f;

	double		minXSrc		= min(x0,x);
	double		maxXSrc		= max(x0,x);
	double		minYSrc		= min(y0,y)-1e-5;
	double		maxYSrc		= max(y0,y)+1e-5;

	int nCount=0;
	for (int i=0; i<m_nBufferSize; i++)
	{
		int nRet=Intersect(x0,y0,x,y,m_pX[i],m_pY[i],m_pX[i+1],m_pY[i+1],
						   i,i+1,&intersectX,&intersectY,minXSrc,minYSrc,
						   maxXSrc,maxYSrc,nCount);
		if(nRet==1)
			nCount++;
		else if(nRet==-1)
			return -1;
	}
	return nCount%2;
}

bool CDpRgn::InitVertexState()
{
	ASSERT(m_VertexState!=NULL);

	if ((m_pY[1] > m_pY[0] && m_pY[0] > m_pY[m_nBufferSize-1]) ||
	   (m_pY[1] < m_pY[0] && m_pY[0] < m_pY[m_nBufferSize-1]))
	{
		m_VertexState[0]				= false;
		m_VertexState[m_nBufferSize]	= false;
	}
	else
	{
		m_VertexState[0]				= true;
		m_VertexState[m_nBufferSize]	= true;
	}
	for(int i=1; i<m_nBufferSize; i++)
	{
		if ((m_pY[i+1] > m_pY[i] && m_pY[i] > m_pY[i-1]) || 
			(m_pY[i+1] < m_pY[i] && m_pY[i] < m_pY[i-1]))
			m_VertexState[i]=false;
		else
			m_VertexState[i]=true;
	}

	m_nDirection = CalRgnDirection();
	return true;
}


int CDpRgn::CalRgnDirection()
{
	int nClock		= 0;
	int	nAntiClock	= 0;

	for (int i=1; i<m_nBufferSize; i++)
	{
		double	dx0		= m_pX[i] - m_pX[i-1];  
		double	dy0		= m_pY[i] - m_pY[i-1];
		double	dx2		= m_pX[i+1] - m_pX[i]; 
		double	dy2		= m_pY[i+1] - m_pY[i];
		double	tmp1	= dx2*dy0 - dx0*dy2;
		if ((tmp1<0?-tmp1:tmp1) <1.0e-5) 
			continue;

		
		double	dx		= m_pX[i+1] - m_pX[i-1];
		double	dy		= m_pY[i+1] - m_pY[i-1];
		double	x		= (m_pX[i+1] + m_pX[i-1])/2;
		double	y		= (m_pY[i+1] + m_pY[i-1])/2;

		double	newX	= 0;
		double	newY	= 0;
		dx	= x - m_pX[i];
		dy	= y - m_pY[i];
		if (fabs(dx) < 1e-5)
		{
			newX	= m_pX[i];
			newY	= m_pY[i] + 1e-3;
		}
		else
		{
			double k = dy/dx;
			if (m_pX[i] > x)
				newX = m_pX[i] - 1e-3;
			else
				newX = m_pX[i] + 1e-3;
			newY = k*newX + m_pY[i] - k*m_pX[i];
		}

		int nPos = PtInRegion(newX,newY);
		bool bAccumulate= (m_pX[i]-m_pX[i-1]) * (m_pY[i+1]-m_pY[i]) - (m_pY[i]-m_pY[i-1]) * (m_pX[i+1]-m_pX[i]) >0;
		
		if((1 == nPos) && bAccumulate)
			nAntiClock ++;
		else
			nClock ++;
	}
	if(nClock > nAntiClock)
		return 0;
	else if(nClock < nAntiClock)
		return 1;
	return -1;
}

double CDpRgn::GetCircumference()
{
	double Circumference	= 0.0f;
	for (int i=0; i<m_nBufferSize; i++)
	{
		Circumference += sqrt(pow((m_pX[i+1]-m_pX[i]),2)+pow((m_pY[i+1]-m_pY[i]),2));
	}
	return Circumference;
}

double CDpRgn::GetArea()
{
	if (m_nBufferSize < 3)
		return (double)0.0f;

	double Area = 0.0f;
	for(int i=0; i<m_nBufferSize; i++)
	{
		Area += (m_pY[i]+m_pY[i+1])*(m_pX[i+1]-m_pX[i])/2.0;
	}

	return Area;
}

bool CDpRgn::IsValid()
{
	return (m_pX != NULL && m_pY != NULL && m_bVertex != NULL);
}


bool CDpRgn::Intersect(CDpRect* pRect)
{
	if (!pRect)
		return false;

	if (pRect->PtInRect(m_pX[0],m_pY[0]) || 
		pRect->PtInRect(m_pX[1],m_pY[1]) || 
		pRect->PtInRect(m_pX[2],m_pY[2]) || 
		pRect->PtInRect(m_pX[3],m_pY[3]) || 
		PtInRegion(pRect->m_dMinX,pRect->m_dMinY) || 
		PtInRegion(pRect->m_dMaxX,pRect->m_dMaxY) ||
		PtInRegion(pRect->m_dMinX,pRect->m_dMaxY) ||
		PtInRegion(pRect->m_dMaxX,pRect->m_dMinY) )
		return true;
	else
	{
		int i,j;
		double rectx[4],recty[4];
		double x0,y0,x1,y1,x2,y2,x3,y3;
		rectx[0] = rectx[3] = pRect->m_dMinX;
		rectx[1] = rectx[2] = pRect->m_dMaxX;
		recty[0] = recty[1] = pRect->m_dMinY;
		recty[2] = recty[3] = pRect->m_dMaxY;

		for( i = 0; i < 4; i ++ )
		{
			x0 = m_pX[i];
			y0 = m_pY[i];
			x1 = m_pX[(i+1)%4];
			y1 = m_pY[(i+1)%4];
			for( j = 0; j < 4; j ++ )
			{
				x2 = rectx[j];
				y2 = recty[j];
				x3 = rectx[(j+1)%4];
				y3 = recty[(j+1)%4];
				double vector1x = x1-x0, vector1y = y1-y0;
				double vector2x = x3-x2, vector2y = y3-y2;
				
				double delta = vector1x*vector2y-vector1y*vector2x;
				if( delta<1e-10 && delta>-1e-10 )continue;
				
				double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
				if( t1<0 || t1>1 )continue;
				double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
				if( t2<0 || t2>1 )continue;
				return true;
			}
		}
		
		return false;
	}
}








/**************************************************
*                 ���ݿ������                    *
**************************************************/
CDpDatabase::CDpDatabase(char* szGlobalID)
{
	InitMember();
	//ȫ��ID
	memset(m_szGlobalID,0,sizeof(m_szGlobalID));
	memcpy(m_szGlobalID,szGlobalID,5);
}

CDpDatabase::~CDpDatabase()
{
//	FreeMem();
	CloseDB();
}

//��������ACCESS���ݿ�,UserID--�û�ID,Pwd-����,���е��û�ID�Ϳ���������ݿ�ϵͳ��
//�û�ID�Ϳ���,���Ǳ�ϵͳ�Լ������
BOOL CDpDatabase::ConnectAccess(const char* UserID,const char* Pwd,const char* szAccessFileName)
{
	if (!UserID || !Pwd || !szAccessFileName || 
		strlen(UserID) < 1 || strlen(Pwd) < 1 || strlen(szAccessFileName) < 1)
		return FALSE;

	SQLHANDLE		hEnv				= NULL;					//�������
	SQLHANDLE		hConn				= NULL;					//���Ӿ��
	SQLCHAR			szConnString[200];
	SQLCHAR			szOutConn[1024]		= "\0";
	SQLSMALLINT		siOutLen			= 0;

	BOOL			bRet				= false;

	//��ԭ���ݿ�
	if (!ResumeDBFile(szAccessFileName))
		return FALSE;
	
	//��ʼ����
	memset(szConnString,0,sizeof(szConnString));
	sprintf((char*)szConnString,"Driver={Microsoft Access Driver (*.mdb)};Dbq=%s;Pwd=DataPlatform",szAccessFileName); //;Pwd=DataPlatform

	if (SQLAllocHandle(SQL_HANDLE_ENV,NULL,&hEnv) == SQL_SUCCESS)
	{
		//��ODBC���ó�Ϊ�汾3,����ĳЩODBC API �������ܱ�֧��
		SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);

		if (SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hConn) == SQL_SUCCESS)
		{

			//���õ�¼��ʱʱ��
			if (m_dwLoginTimeOut != -1)
				SQLSetConnectAttr(hConn,SQL_ATTR_LOGIN_TIMEOUT,(SQLPOINTER)m_dwLoginTimeOut,0);

			//�������ݿ�����
			if (SQLDriverConnect(hConn,NULL,szConnString,sizeof(szConnString),szOutConn,sizeof(szOutConn),
							 &siOutLen,SQL_DRIVER_NOPROMPT) == SQL_SUCCESS)
				bRet	= true;
		}
	}


	//�ж��û�ID�Ϳ����Ƿ���ȷ
	if (!UserInfoIsCorrect(hConn,UserID,Pwd))
		bRet = false;

	//�������ݿ�
	if (!EncryptDBFile(szAccessFileName))
		bRet = false;

	if (bRet)
	{

		m_hEnv				= hEnv;
		m_hConn				= hConn;
		m_bIsOpen			= true;
		m_dtDatabaseType	= DP_ACCESS;	
		strcpy(m_szUserID,UserID);
		strcpy(m_szPwd,Pwd);
		strcpy(m_szFileName,szAccessFileName);
		GetConnectInfo();

		//����Ȩ�޹�����������Ȩ����Ϣ
		m_pRightMgr			= new CDpRightMgr(this);
		if (!m_pRightMgr->LoadRightInfo())
			return false;
	}
	else
	{
		if (hConn)
		{
			SQLDisconnect(hConn);
			SQLFreeHandle(SQL_HANDLE_DBC,hConn);
		}
		if (hEnv)		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
	}

	return bRet;

}


//��������ORACLE���ݿ�,UserID--�û�ID,Pwd-����,szAddress-ORACLE�ķ�����
BOOL CDpDatabase::ConnectDB(const char* UserID,const char* Pwd,const char* szAddress,
							const char* szDBUserID,const char* szDBPwd)
{
	if (!UserID || !Pwd || !szAddress || !szDBUserID || !szDBPwd || 
		strlen(UserID) < 1 || strlen(Pwd) < 1 || strlen(szAddress) < 1 || 
		strlen(szDBUserID) < 1 || strlen(szDBPwd) < 1)
		return FALSE;

	SQLHANDLE		hEnv				= NULL;					//�������
	SQLHANDLE		hConn				= NULL;					//���Ӿ��
	SQLCHAR			szConnString[200];
	SQLCHAR			szOutConn[1024]		= "\0";
	SQLSMALLINT		siOutLen			= 0;

	//��ʼ����
	memset(szConnString,0,sizeof(szConnString));
	sprintf((char*)szConnString,"Driver={Oracle in oraHome92};UID=%s;PWD=%s;DBQ=%s;",szDBUserID,szDBPwd,szAddress);
//	sprintf((char*)szConnString,"Driver={Oracle in oraHome92};UID=system;PWD=liwq;DBQ=dbserver;");
//	sprintf((char*)szConnString,"Driver={Test DB ODBC};UID=%s;PWD=%s;DBQ=%s;",szDBUserID,szDBPwd,szAddress);

	if (SQLAllocHandle(SQL_HANDLE_ENV,NULL,&hEnv) != SQL_SUCCESS)
		return FALSE;

	//��ODBC���ó�Ϊ�汾3,����ĳЩODBC API �������ܱ�֧��
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);

	if (SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hConn) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
		return FALSE;
	}

	//���õ�¼��ʱʱ��
	if (m_dwLoginTimeOut != -1)
		SQLSetConnectAttr(hConn,SQL_ATTR_LOGIN_TIMEOUT,(SQLPOINTER)m_dwLoginTimeOut,0);

	//�������ݿ�����
	if (SQLDriverConnect(hConn,NULL,szConnString,sizeof(szConnString),szOutConn,sizeof(szOutConn),
		             &siOutLen,SQL_DRIVER_NOPROMPT) != SQL_SUCCESS)
	{
		#ifdef _DEBUG
			char		sz[1024];
			UCHAR		szSqlStates[256];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_DBC,hConn,true,sz,szSqlStates);
		#endif	

		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
		SQLFreeHandle(SQL_HANDLE_DBC,hConn);
		return FALSE;
	}

	//�жϸ��û��Ƿ�����Լ��Ƿ���ȷ
	if (!UserInfoIsCorrect(hConn,UserID,Pwd))
	{
		SQLDisconnect(hConn);
		SQLFreeHandle(SQL_HANDLE_DBC,hConn);
		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
		return FALSE;
	}




	m_hEnv				= hEnv;
	m_hConn				= hConn;
	m_bIsOpen			= true;
	m_dtDatabaseType	= DP_ORACLE;	
	strcpy(m_szUserID,UserID);
	strcpy(m_szPwd,Pwd);

	GetConnectInfo();

	//����Ȩ�޹�����������Ȩ����Ϣ
	m_pRightMgr			= new CDpRightMgr(this);
	if (!m_pRightMgr->LoadRightInfo())
		return false;

	return true;

}

//��������SQLSERVER���ݿ⡣UserID--�û�ID,Pwd-����,szAddress-SQLServer���ݿ�����������ƻ�IP,szDBUserID -- ���ݿ��û�ID,szDBPwd -- ���ݿ��û����szDBName-���ݿ�����
BOOL CDpDatabase::ConnectSqlServer(const char* UserID,const char* Pwd,const char* szAddress,
								   const char* szDBUserID,const char* szDBPwd,const char* szDBName)
{
	if (!UserID || !Pwd || !szAddress || !szDBUserID || !szDBPwd || !szDBName || 
		strlen(UserID) < 1 || strlen(Pwd) < 1 || strlen(szAddress) < 1 || 
		strlen(szDBUserID) < 1 || strlen(szDBPwd) < 1 || strlen(szDBName) < 1)
		return FALSE;

	SQLHANDLE		hEnv				= NULL;					//�������
	SQLHANDLE		hConn				= NULL;					//���Ӿ��
	SQLCHAR			szConnString[200];
	SQLCHAR			szOutConn[1024]		= "\0";
	SQLSMALLINT		siOutLen			= 0;

	//��ʼ����
	memset(szConnString,0,sizeof(szConnString));
	sprintf((char*)szConnString,"Driver={SQL Server};UID=%s;PWD=%s;Server=%s;Database=%s",szDBUserID,szDBPwd,szAddress,szDBName);

	if (SQLAllocHandle(SQL_HANDLE_ENV,NULL,&hEnv) != SQL_SUCCESS)
		return FALSE;

	//��ODBC���ó�Ϊ�汾3,����ĳЩODBC API �������ܱ�֧��
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);

	if (SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hConn) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
		return FALSE;
	}

	//���õ�¼��ʱʱ��
	if (m_dwLoginTimeOut != -1)
		SQLSetConnectAttr(hConn,SQL_ATTR_LOGIN_TIMEOUT,(SQLPOINTER)m_dwLoginTimeOut,0);

	//�������ݿ�����
	if (SQLDriverConnect(hConn,NULL,szConnString,sizeof(szConnString),szOutConn,sizeof(szOutConn),
		             &siOutLen,SQL_DRIVER_NOPROMPT) != SQL_SUCCESS)
	{
		char		sz[1024];
		UCHAR		szSqlStates[256];
		memset(sz,0,sizeof(sz));
		memset(szSqlStates,0,sizeof(szSqlStates));
		ProcessLogMessages(SQL_HANDLE_DBC,hConn,true,sz,szSqlStates);

		if (strcmp((char*)szSqlStates,"01000") != 0)
		{
			SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
			SQLFreeHandle(SQL_HANDLE_DBC,hConn);
			return false;
		}
	}

	//�жϸ��û��Ƿ�����Լ��Ƿ���ȷ
	if (!UserInfoIsCorrect(hConn,UserID,Pwd))
	{
		SQLDisconnect(hConn);
		SQLFreeHandle(SQL_HANDLE_DBC,hConn);
		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
		return FALSE;
	}




	m_hEnv				= hEnv;
	m_hConn				= hConn;
	m_bIsOpen			= true;
	m_dtDatabaseType	= DP_SQLSERVER;	
	strcpy(m_szUserID,UserID);
	strcpy(m_szPwd,Pwd);

	GetConnectInfo();

	//����Ȩ�޹�����������Ȩ����Ϣ
	m_pRightMgr			= new CDpRightMgr(this);
	if (!m_pRightMgr->LoadRightInfo())
		return false;

	return true;



}


//��������Sybase���ݿ�.UserID--�û�ID,Pwd-����,szAddress-SQLServer���ݿ�����������ƻ�IP,uServerPort -- ���ݿ�������Ķ˿ں�,szDBUserID -- ���ݿ��û�ID,szDBPwd -- ���ݿ��û����szDBName-���ݿ�����
BOOL CDpDatabase::ConnectSybase(const char* UserID,const char* Pwd,const char* szServerNetAddress,
								UINT uServerPort,const char* szDBUserID,const char* szDBPwd,
								const char* szDBName)
{
	if (!UserID || !Pwd || !szServerNetAddress || !szDBUserID || !szDBPwd || !szDBName || 
		strlen(UserID) < 1 || strlen(Pwd) < 1 || strlen(szServerNetAddress) < 1 || 
		strlen(szDBUserID) < 1 || strlen(szDBPwd) < 1 || strlen(szDBName) < 1)
		return FALSE;

	SQLHANDLE		hEnv				= NULL;					//�������
	SQLHANDLE		hConn				= NULL;					//���Ӿ��
	SQLCHAR			szConnString[200];
	SQLCHAR			szOutConn[1024]		= "\0";
	SQLSMALLINT		siOutLen			= 0;

	//��ʼ����
	memset(szConnString,0,sizeof(szConnString));
	sprintf((char*)szConnString,"Driver={Sybase ASE ODBC Driver};NetworkLibraryName={Winsock};UID=%s;PWD=%s;NA=%s,%d;Database=%s",szDBUserID,szDBPwd,szServerNetAddress,uServerPort,szDBName);

	if (SQLAllocHandle(SQL_HANDLE_ENV,NULL,&hEnv) != SQL_SUCCESS)
		return FALSE;

	//��ODBC���ó�Ϊ�汾3,����ĳЩODBC API �������ܱ�֧��
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);

	if (SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hConn) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
		return FALSE;
	}

	//���õ�¼��ʱʱ��
	if (m_dwLoginTimeOut != -1)
		SQLSetConnectAttr(hConn,SQL_ATTR_LOGIN_TIMEOUT,(SQLPOINTER)m_dwLoginTimeOut,0);

	//�������ݿ�����
	if (SQLDriverConnect(hConn,NULL,szConnString,sizeof(szConnString),szOutConn,sizeof(szOutConn),
		             &siOutLen,SQL_DRIVER_NOPROMPT) != SQL_SUCCESS)
	{
		char		sz[1024];
		UCHAR		szSqlStates[256];
		memset(sz,0,sizeof(sz));
		memset(szSqlStates,0,sizeof(szSqlStates));
		ProcessLogMessages(SQL_HANDLE_DBC,hConn,true,sz,szSqlStates);

		if (strcmp((char*)szSqlStates,"01000") != 0)
		{
			SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
			SQLFreeHandle(SQL_HANDLE_DBC,hConn);
			return false;
		}
	}

	//�жϸ��û��Ƿ�����Լ��Ƿ���ȷ
	if (!UserInfoIsCorrect(hConn,UserID,Pwd))
	{
		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
		SQLFreeHandle(SQL_HANDLE_DBC,hConn);
		return FALSE;
	}

	m_hEnv				= hEnv;
	m_hConn				= hConn;
	m_bIsOpen			= true;
	m_dtDatabaseType	= DP_SQLSERVER;	
	strcpy(m_szUserID,UserID);
	strcpy(m_szPwd,Pwd);

	GetConnectInfo();

	//����Ȩ�޹�����������Ȩ����Ϣ
	m_pRightMgr			= new CDpRightMgr(this);
	if (!m_pRightMgr->LoadRightInfo())
		return false;

	return true;
}




//����ʹ����ע����������,����ORACLE
BOOL CDpDatabase::ConnectDB(const char* UserID,const char* Pwd)
{
	return ConnectDB(UserID,Pwd,"DataServer","DataplatForm","liwq");
}

//����ʹ����ע����������,����SqlServer
BOOL CDpDatabase::ConnectSqlServer(const char* UserID,const char* Pwd)
{
	return ConnectSqlServer(UserID,Pwd,"DBServer","Dataplatform","liwq","Dataplatform");
}

//��������ʹ����ע����������,����Sybase
BOOL CDpDatabase::ConnectSybase(const char* UserID,const char* Pwd)
{
	return ConnectSybase(UserID,Pwd,"DBServer",5000,"Dataplatform","liwq","Dataplatform");
}



//��ʼ����Ա����
void CDpDatabase::InitMember()
{
	memset(m_szUserID,0,sizeof(m_szUserID));
	memset(m_szPwd,0,sizeof(m_szPwd));
	memset(m_szFileName,0,sizeof(m_szFileName));

	m_hEnv					= NULL;
	m_hConn					= NULL;

	m_dwLoginTimeOut		= -1;

	m_bCanTrans				= false;
	m_bIsOpen				= false;
	m_bCanGetDataAnyOrder	= false;	

	m_dtDatabaseType		= DP_NULL;

	m_pRightMgr				= NULL;
}

//�ͷ��ڴ�
void CDpDatabase::FreeMem()
{
	if (m_pRightMgr)
	{
		delete m_pRightMgr;
		m_pRightMgr = NULL;
	}

	if (m_hConn)
	{
		SQLDisconnect(m_hConn);
		SQLFreeHandle(SQL_HANDLE_DBC,m_hConn);
	}
	if (m_hEnv)
		SQLFreeHandle(SQL_HANDLE_ENV,m_hEnv);

}


//�ر����ݿ�
void CDpDatabase::CloseDB()
{
	if (m_bIsOpen)
	{
		FreeMem();
		InitMember();
	}
}

//�ú���������ָ���Ĺ����������һ��A�ڵ�
//����˵��:   WspType   -- ������������
//            APartType -- Ҫ�����Ľڵ�A������     
//�ڵ�A�ı�ʶ:����������+�ڵ�A������,HOUKUI,06.7.7
BOOL CDpDatabase::AddANode(enum DP_WORKSPACE_TYPE WspType, enum DpAPartType APartType)
{
	
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ��Դ���ά�������ڵ�
		return false;	

	
	char	szWspType[1];
	char	szAPartType[1];
	char	szAPartID[2];	 
	char	szWBS_A_NAME[50];
	char	szWBS_A_DESC[200];
	
	memset(szWspType,	0,	sizeof(szWspType));
	memset(szAPartType,	0,	sizeof(szAPartType));
	memset(szAPartID,	0,	sizeof(szAPartID));
	memset(szWBS_A_NAME,0,	sizeof(szWBS_A_NAME));
	memset(szWBS_A_DESC,0,	sizeof(szWBS_A_DESC));

	//���ڵ�A�����ƺ�����
	switch(APartType)
	{
	case DP_A_NULL:
		return	TRUE;
	case DP_A_IMAGE:
		sprintf(szWBS_A_NAME,"IMAGE");
		sprintf(szWBS_A_DESC,"����ԭʼӰ�������");
		break;
	case DP_A_UNIT:
		sprintf(szWBS_A_NAME,"MAP");
		sprintf(szWBS_A_DESC,"����ͼ��������");
		break;
	case DP_A_STEREO:		
		sprintf(szWBS_A_NAME,"STEREO");
		sprintf(szWBS_A_DESC,"��ʶģ�Ͷ���");
		break;		
	default:
		return FALSE;
	}
	
		//�����ڵ�A�ı�ʶ
	_itoa(int(WspType)	,	szWspType,		10 );
	_itoa(int(APartType),	szAPartType,	10 );
	strcpy(szAPartID,szWspType);
	strcat(szAPartID,szAPartType);

	BeginTrans();
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"INSERT INTO DP_WBS_A_WH (WBS_A, WBS_A_NAME, WBS_A_DESC,WBS_WORKSPACE_TYPE) VALUES ('%s','%s','%s','%s')",szAPartID,szWBS_A_NAME,szWBS_A_DESC,szWspType);
	if(ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{	
		CommitTrans();
		return TRUE;
	}
	else
	{
		Rollback();
		return FALSE;
	}

	return TRUE;

}

//�ú���������ָ���Ĺ�������A�ڵ��´���һ��C�ڵ�
//����˵��:   WspType   -- ������������
//            APartType -- �ڵ�A������     
//            CPartType -- Ҫ�����Ľڵ�C������  
//�ڵ�C�ı�ʶ:����������+�ڵ�A������+�ڵ�C������,HOUKUI,06.7.7
BOOL CDpDatabase::AddCNode(enum DP_WORKSPACE_TYPE WspType, enum DpAPartType APartType,
						   enum DP_LAYER_TYPE_EX CPartTypeEx)
{	

	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ��Դ���ά�������ڵ�
		return false;	
		
	char	szWspType[1];
	char	szAPartType[1];//A���ֵ�����Ϊһ���ֽ�
	char	szCPartType[2];//C���ֵ�����Ϊ�����ֽ�
	char	szAPartID[2];
	char	szCPartID[4];
	char	szWBS_C_NAME[50];
	char	szWBS_C_DESC[200];	
	memset(szWspType,	0,	sizeof(szWspType));
	memset(szAPartType,	0,	sizeof(szAPartType));
	memset(szCPartType,	0,	sizeof(szCPartType));
	memset(szCPartID,	0,	sizeof(szCPartID));
	memset(szAPartID,	0,	sizeof(szAPartID));
	memset(szWBS_C_NAME,0,	sizeof(szWBS_C_NAME));
	memset(szWBS_C_DESC,0,	sizeof(szWBS_C_DESC));
	

	//���ڵ�C�����ƺ�����	
	switch(CPartTypeEx)
	{
	case LAYER_TYPE_EX_NULL:
		return	TRUE;
	case LAYER_TYPE_EX_IOP:
		sprintf(szWBS_C_NAME,"IOP");
		sprintf(szWBS_C_DESC,"�ڶ���");
		break;
	case LAYER_TYPE_EX_AOP:
		sprintf(szWBS_C_NAME,"AOP");
		sprintf(szWBS_C_DESC,"���Զ���");
		break;
	case LAYER_TYPE_EX_IMAGEPOINT:		
		sprintf(szWBS_C_NAME,"IMAGEPOINT");
		sprintf(szWBS_C_DESC,"Ӱ����Ƶ�");
		break;
	case LAYER_TYPE_EX_DEM:		
		sprintf(szWBS_C_NAME,"DEM");
		sprintf(szWBS_C_DESC,"���ָ߲�ģ��");
		break;
	case LAYER_TYPE_EX_DOM:		
		sprintf(szWBS_C_NAME,"DOM");
		sprintf(szWBS_C_DESC,"��������ģ��");
		break;		
	case LAYER_TYPE_EX_DLG:		
		sprintf(szWBS_C_NAME,"DLG");
		sprintf(szWBS_C_DESC,"�����߻�ͼ");
		break;
	case LAYER_TYPE_EX_DRG:		
		sprintf(szWBS_C_NAME,"DRG");
		sprintf(szWBS_C_DESC,"����դ��ͼ");
		break;
	case LAYER_TYPE_EX_DVS:		
		sprintf(szWBS_C_NAME,"DVS");
		sprintf(szWBS_C_DESC,"���ֿ��ӳ���");
		break;
	default:
		return FALSE;
	}

	//�����ڵ�C�ı�ʶ
	_itoa(int(WspType)	,	szWspType,		10 );
	_itoa(int(APartType),	szAPartType,	10 );
	_itoa(int(CPartTypeEx),	szCPartType,	10 );	
	sprintf(szAPartID,"%s%s",szWspType,szAPartType);
	sprintf(szCPartID,"%s%s",szAPartID,szCPartType);

	
	BeginTrans();
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"INSERT INTO DP_WBS_C_WH (WBS_C, WBS_C_NAME, WBS_C_DESC,WBS_A) VALUES ('%s','%s','%s','%s')",
		szCPartID,szWBS_C_NAME,szWBS_C_DESC,szAPartID);
	if(ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{	
		CommitTrans();
		return TRUE;
	}
	else
	{
		Rollback();
		return FALSE;
	}
}
	
	
	//�ú���������ָ���Ĺ�������A�ڵ��C�ڵ��´���һ��D�ڵ�
	//����˵��:   WspType   -- ������������
	//            APartType -- �ڵ�A������     
	//            CPartType -- �ڵ�C������  
	//            DPartType -- Ҫ�����Ľڵ�D������  
	//�ڵ�D�ı�ʶ���:����������+�ڵ�A������+�ڵ�C������+�ڵ�D������,HOUKUI,06.7.7
	BOOL CDpDatabase::AddDNode(enum DP_WORKSPACE_TYPE WspType, enum DpAPartType APartType,
		enum DP_LAYER_TYPE_EX CPartTypeEx, int dIndex, char* szDName,char* szDDesc)
				
	{	
		
		if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ��Դ���ά�������ڵ�
			return false;	
		
		//�����ڵ�D�ı�ʶ
		char	szWspType[1];
		char	szAPartType[1];
		char	szCPartType[2];
		char	szDPartType[2];
		char	szCPartID[4];
		char	szDPartID[6];
		//char	szWBS_D_NAME[50];
		//char	szWBS_D_DESC[200];
		memset(szWspType,	0,	sizeof(szWspType));
		memset(szAPartType,	0,	sizeof(szAPartType));
		memset(szCPartType,	0,	sizeof(szCPartType));
		memset(szDPartType,	0,	sizeof(szDPartType));
		memset(szCPartID,	0,	sizeof(szCPartID));
		memset(szDPartID,	0,	sizeof(szDPartID));
		//memset(szWBS_D_NAME,0,	sizeof(szWBS_D_NAME));
		//memset(szWBS_D_DESC,0,	sizeof(szWBS_D_DESC));
		
		
		//�����ڵ�D�ı�ʶ
		_itoa(int(WspType)	,	szWspType,		10 );
		_itoa(int(APartType),	szAPartType,	10 );
		_itoa(int(CPartTypeEx),	szCPartType,	10 );
		_itoa(dIndex,	szDPartType,	10 );
		sprintf(szCPartID,"%s%s%s",szWspType,szAPartType,szCPartType);
		sprintf(szDPartID,"%s%s",szCPartID,szDPartType);
		
		//�����ڵ�D�����ƺ�����
		/*
		switch(DPartType)
		{
		case DP_D_NULL:
			return TRUE;
		case DP_D_PRODUCE:
			sprintf(szWBS_D_NAME,"PRODUCE");
			sprintf(szWBS_D_DESC,"����");
			break;
		case DP_D_CHECK:
			sprintf(szWBS_D_NAME,"CHECK");
			sprintf(szWBS_D_DESC,"���");
			break;
		case DP_D_DEM_GENERATE:
			sprintf(szWBS_D_NAME,"DEM����");
			sprintf(szWBS_D_DESC,"�������ָ߲�ģ��");
			break;
		case DP_D_DEM_EDIT:
			sprintf(szWBS_D_NAME,"DEM�༭");
			sprintf(szWBS_D_DESC,"�༭���ָ߲�ģ��");
			break;
		case DP_D_DEM_MOSAIC:
			sprintf(szWBS_D_NAME,"DEM�ӱ�");
			sprintf(szWBS_D_DESC,"���ָ߲�ģ�ͽӱ�");
			break;
		case DP_D_DEM_CHECK:
			sprintf(szWBS_D_NAME,"DEM���");
			sprintf(szWBS_D_DESC,"������ָ߲�ģ��");
			break;
		case DP_D_DOM_GENERATE:
			sprintf(szWBS_D_NAME,"DOM����");
			sprintf(szWBS_D_DESC,"������������ģ��");
			break;
		case DP_D_DOM_EDIT:
			sprintf(szWBS_D_NAME,"DOM�޲�");
			sprintf(szWBS_D_DESC,"�޲���������ģ��");
			break;
		case DP_D_DOM_DODGE:
			sprintf(szWBS_D_NAME,"DOM�ȹ�");
			sprintf(szWBS_D_DESC,"��������ģ���ȹ�");
			break;
		case DP_D_DOM_MOSAIC:
			sprintf(szWBS_D_NAME,"DOMƴ��");
			sprintf(szWBS_D_DESC,"ƴ����������ģ��");
			break;
		case DP_D_DOM_CUT:
			sprintf(szWBS_D_NAME,"DOM����");
			sprintf(szWBS_D_DESC,"������������ģ��");
			break;
		case DP_D_DLG_COLLECT:
			sprintf(szWBS_D_NAME,"DLG�ɼ�");
			sprintf(szWBS_D_DESC,"�����߻�ͼ�ɼ�");
			break;
		case DP_D_DLG_EDIT:
			sprintf(szWBS_D_NAME,"DLG�༭");
			sprintf(szWBS_D_DESC,"�����߻�ͼ�༭");
			break;
		case DP_D_DLG_MOSAIC:
			sprintf(szWBS_D_NAME,"DLG�ӱ�");
			sprintf(szWBS_D_DESC,"�����߻�ͼ�ӱ�");
			break;
		case DP_D_DLG_CUT:
			sprintf(szWBS_D_NAME,"DLG����");
			sprintf(szWBS_D_DESC,"�����߻�ͼ����");
			break;
		case DP_D_DLG_MAPPING:
			sprintf(szWBS_D_NAME,"DLGͼ������");
			sprintf(szWBS_D_DESC,"�����߻�ͼͼ������");
			break;
		case DP_D_DLG_TOPOLOGY:
			sprintf(szWBS_D_NAME,"DLG����");
			sprintf(szWBS_D_DESC,"�����߻�ͼ����");		
			break;
		case DP_D_DLG_CHECK:
			sprintf(szWBS_D_NAME,"DLG���");
			sprintf(szWBS_D_DESC,"�����߻�ͼ���");
			break;
		case DP_D_DRG_PRINT:
			sprintf(szWBS_D_NAME,"DRG��ͼ");
			sprintf(szWBS_D_DESC,"����դ��ͼ��ͼ");		
			break;
		case DP_D_DRG_CHECK:
			sprintf(szWBS_D_NAME,"DRG���");
			sprintf(szWBS_D_DESC,"����դ��ͼ���");
			break;
		case DP_D_DVS_GENERATE:
			sprintf(szWBS_D_NAME,"DVSʸ��ģ�ͽ���");
			sprintf(szWBS_D_DESC,"���ֿ��ӳ���ʸ��ģ�ͽ���");
			break;
		case DP_D_DVS_TEXTURE:
			sprintf(szWBS_D_NAME,"DVS������ȡ");
			sprintf(szWBS_D_DESC,"���ֿ��ӳ���DVS������ȡ");
			break;
		case DP_D_DVS_SCENE:
			sprintf(szWBS_D_NAME,"DVS���γ�������");
			sprintf(szWBS_D_DESC,"DVS���γ�������");
			break;
		default:
			return FALSE;
		}
		*/
		
		BeginTrans();
		char		szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"INSERT INTO DP_WBS_D_WH (WBS_D, WBS_D_NAME, WBS_D_DESC,WBS_C) VALUES ('%s','%s','%s','%s')",
			szDPartID,szDName,szDDesc,szCPartID);
		if(ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		{	
			CommitTrans();
			return TRUE;
		}
		else
		{
			Rollback();
			return FALSE;
		}
}




//�ж��û�ID�Ϳ����Ƿ���ȷ
BOOL CDpDatabase::UserInfoIsCorrect(SQLHANDLE hConn,const char* UserID,const char* Pwd)
{
	if (!hConn || !UserID || !Pwd)
		return false;

	SQLHANDLE		hStmt		= NULL;
	SQLINTEGER		iCount;
	SQLLEN		cbLen;
	SQLRETURN		retcode;
	char			szSql[512];
	BOOL			bRet		= false;

	memset(szSql,0,sizeof(szSql));
	//��ʱ�����û��������MD5����,��ϵͳ����ʱ�ټ���MD5����
	sprintf(szSql,"Select Count(USER_ID) From DP_UserMgr Where USER_ID = '%s' AND USER_PWD = '%s'",
		          UserID,Pwd);

	if (SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hStmt) != SQL_SUCCESS)
	{
// 			char		sz[1024];
// 			UCHAR		szSqlStates[128];
// 			memset(sz,0,sizeof(sz));
// 			memset(szSqlStates,0,sizeof(szSqlStates));
// 			ProcessLogMessages(SQL_HANDLE_STMT,hConn,true,sz,szSqlStates);
// 			TRACE(sz);
// 			SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
			return false;
	}

	retcode = SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS);

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hStmt,1,SQL_C_LONG,&iCount,sizeof(iCount),&cbLen);
			bRet = true;
		}
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	
	//�ͷž��
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if (!bRet)
		return false;

	if (iCount <= 0)
		return false;
	else
		return true;

}

//�ж����ݿ��Ƿ��
BOOL CDpDatabase::IsOpen()
{
	return m_bIsOpen;
}	









//ȡ�õ�ǰ���ӵ�������Ϣ
void CDpDatabase::GetConnectInfo()
{
	//����Ƿ�֧��������
	SWORD		nResult;
	SWORD		nTxnCapable;
	SQLRETURN	nRetCode;

	nRetCode = SQLGetInfo(m_hConn, SQL_TXN_CAPABLE, &nTxnCapable,
				sizeof(nTxnCapable), &nResult);
	if (Check(nRetCode) && nTxnCapable != SQL_TC_NONE)
		m_bCanTrans = TRUE;
	else
		m_bCanTrans = FALSE;

	nRetCode = SQLGetInfo(m_hConn,SQL_GETDATA_EXTENSIONS,&nTxnCapable,
		         sizeof(nTxnCapable), &nResult);

	//�ܷ����ݼ�¼�����ֶε�˳��(��С����)ȥȡ�ֶε�ֵ,���ǿ���������˳��ȡ
	if (Check(nRetCode) && ((nTxnCapable & SQL_GD_ANY_ORDER) || (nTxnCapable & SQL_GD_BLOCK)))
		m_bCanGetDataAnyOrder = true;
	else
		m_bCanGetDataAnyOrder = false;


}












//ִ��SQL���
DpReturnCode CDpDatabase::ExecuteSQL(const char* szSQL)
{
	if (!szSQL || strlen(szSQL) < 1)
		return DP_SQL_STAT_ERR;						//һ�����


	//�Ƚ���SQL�����Ҫ�����ı�,���ж����Ƿ�����Ӧ��Ȩ��������



	SQLHANDLE	hStmt	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_hConn,&hStmt) != SQL_SUCCESS)
		return DP_SQL_STAT_ERR;

	//��ִ��SQL���
	if (!hStmt)
		return DP_SQL_STAT_ERR;


	SQLINTEGER		iLen = strlen(szSQL);

	SQLRETURN		sRet = SQLExecDirect(hStmt,(SQLCHAR*)szSQL,iLen);

	if (sRet != SQL_SUCCESS && sRet != SQL_NO_DATA)
	{
		#ifdef _DEBUG
			char		sz[1024];
			UCHAR		szSqlStates[256];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_STMT,hStmt,true,sz,szSqlStates);
			TRACE(sz);
		#endif
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return DP_SQL_STAT_ERR;
	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return DP_SQL_SUCCESS;

}

//���ݿ��Ƿ�����,������ΪTRUE,����ΪFALSE
BOOL CDpDatabase::GetConnectStatus()
{
	return m_bIsOpen;	
}

//�����ǰ���ӵ����ݿ�ΪACCESS,�������Ϊ���ݿ��ļ�������·��
//����˵��:  szFileName --Ϊ�����ļ����Ļ�����,iBuffCount --Ϊ��������С
BOOL CDpDatabase::GetDBFileName(char* szFileName,int iBuffCount)
{
	if (!m_bIsOpen || m_dtDatabaseType != DP_ACCESS)
		return false;

	if (iBuffCount < (int)(strlen(m_szFileName) + 1))
		return false;

	strcpy(szFileName,m_szFileName);
	return true;

}


//�õ����ݿ��ODBC���
SQLHANDLE CDpDatabase::GetDBHandle()
{
	return m_hConn;
}


//���õ�¼�ĳ�ʱʱ��,dSeconds�ĵ�λΪ��
void CDpDatabase::SetLoginTimeOut(DWORD dSeconds)
{
	m_dwLoginTimeOut	= dSeconds;
}



//�Ƿ���ִ��������
BOOL CDpDatabase::CanTransact()
{
	return m_bCanTrans;
}

//��ʼ����
BOOL CDpDatabase::BeginTrans()
{
	if (m_hConn == SQL_NULL_HDBC)
		return false;

	if (!m_bCanTrans)
		return FALSE;

	//��ʼ���񼴽����ݿ����ӵ��Զ��ύ���ܹر�

	SQLRETURN	nRetCode;
	
	nRetCode = SQLSetConnectAttr(m_hConn,SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_OFF,0);

	return Check(nRetCode);

}

//�ύ����
BOOL CDpDatabase::CommitTrans()
{
	if (m_hConn == SQL_NULL_HDBC)
		return false;

	if (!m_bCanTrans)
		return FALSE;

	SQLRETURN nRetCode;
	nRetCode = SQLEndTran(SQL_HANDLE_DBC,m_hConn,SQL_COMMIT);
	BOOL bSuccess = Check(nRetCode);

	//�ڽ��������,�ٽ��Զ��ύ���ܴ�
	SQLSetConnectOption(m_hConn, SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_ON);

	return bSuccess;

}

//�ع�����
BOOL CDpDatabase::Rollback()
{
	if (m_hConn == SQL_NULL_HDBC)
		return false;

	if (!m_bCanTrans)
		return FALSE;

	SQLRETURN nRetCode;
	nRetCode = SQLEndTran(SQL_HANDLE_DBC,m_hConn,SQL_ROLLBACK);
	BOOL bSuccess = Check(nRetCode);

	//�ڽ��������,�ٽ��Զ��ύ���ܴ�
	SQLSetConnectOption(m_hConn, SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_ON);

	return bSuccess;

}


//ö�ٵ�ǰ���ݿ��е�ǰ�û��ܹ����ʵù�������ID
BOOL CDpDatabase::GetWksIDAndName(CDpStringArray& szIDArray,CDpStringArray& szNameArray)
{
	char		szSql[512];						//sql��ѯ���
	memset(szSql,0,sizeof(szSql));

	//������SQL���
	sprintf(szSql,"Select WORKSPACE_ID,WORKSPACE_NAME From DP_WorkspaceMgr");

	//��ʼ�������ݿ�
	SQLHANDLE			hStmt		= NULL;

	SQLCHAR				szID[50];						//������ID
	SQLCHAR				szName[51];						//����������
	SQLLEN			cbLen = 0;			//�ֶγ���
	SQLRETURN			retcode;

	memset(szID,0,sizeof(szID));

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_hConn,&hStmt) != SQL_SUCCESS)
		return false;

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		while (true)
		{
			retcode = SQLFetch(hStmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				memset(szID,0,sizeof(szID));
				memset(szName,0,sizeof(szName));
				SQLGetData(hStmt,1,SQL_C_CHAR,szID,sizeof(szID),&cbLen);			//ȡ������ID
				SQLGetData(hStmt,2,SQL_C_CHAR,szName,sizeof(szName),&cbLen);		//ȡ����������

				if (strlen((char*)szID) == WKS_ID_LEN && strlen((char*)szName) > 0)	
				{
					DP_RIGHT	iRight	= DP_RIGHT_NOTRIGHT;
					//���жϵ�ǰ�û��Ƿ��з��ʸù�������Ȩ��
					if (m_pRightMgr->GetWorkspaceRight((char*)szID,&iRight) && 
						(iRight == DP_RIGHT_READONLY || iRight == DP_RIGHT_READWRITE))
					{
						szIDArray.AddString((char*)szID);								//���뵽������							
						szNameArray.AddString((char*)szName);
					}
				}
			}
			else
				break;
		}
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	else
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return true;
}



//ȡ�õ�ǰ���ݿ��й���������Ŀ
int	CDpDatabase::GetWksCount(DP_WORKSPACE_TYPE nWksType)
{
	SQLINTEGER  sCount	 = -1;
	SQLLEN  cbCount = 0;
	SQLRETURN	retcode;

	char		szSql[512];
	memset(szSql,0,sizeof(szSql));

	SQLHANDLE	hStmt	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_hConn,&hStmt) != SQL_SUCCESS)
		return -1;

	if (nWksType == DP_WKS_FULL)
		sprintf(szSql,"Select Count(WORKSPACE_ID) From DP_WorkspaceMgr Where WORKSPACE_TYPE = 1");
	else if (nWksType == DP_WKS_PART)
		sprintf(szSql,"Select Count(WORKSPACE_ID) From DP_WorkspaceMgr Where WORKSPACE_TYPE = 2");
	else
		sprintf(szSql,"Select Count(WORKSPACE_ID) From DP_WorkspaceMgr");

	retcode = SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS);

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		while (true)
		{
			retcode = SQLFetch(hStmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLGetData(hStmt,1,SQL_C_ULONG,&sCount,0,&cbCount);
			}
			else
				break;
		}
		
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return sCount;
	
}




//����ID�õ�������
BOOL CDpDatabase::GetWksByID(const char* szID,CDpWorkspace** ppWks)
{
	if (!szID || strlen(szID) != WKS_ID_LEN || !ppWks)
		return false;
	*ppWks = NULL;


	char			szSql[1024];			
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select WORKSPACE_ID,WORKSPACE_NAME,WORKSPACE_DESC,CREATE_DATE,CREATER_NAME,WORKSPACE_TYPE,UPDATE_DATE,WORKSPACE_VERSION,MINX,MINY,MAXX,MAXY,SCALE,DEFAULT_DIR From DP_WorkspaceMgr Where WORKSPACE_ID = '%s'",szID);//,PHYSICAL_PATH 

	//AfxMessageBox(szSql);
	return GetWorkspace(szSql,ppWks);

}

//�������Ƶõ�������,����������ظ���,ֻȡ��һ��
BOOL CDpDatabase::GetWksByName(const char* szName,CDpWorkspace** ppWks)
{
	if (!szName || strlen(szName) < 1 || !ppWks)
		return false;
	*ppWks = NULL;


	char			szSql[512];			
	sprintf(szSql,"Select WORKSPACE_ID,WORKSPACE_NAME,WORKSPACE_DESC,\
		CREATE_DATE,CREATER_NAME,WORKSPACE_TYPE,UPDATE_DATE,WORKSPACE_VERSION,MINX,MINY,MAXX,MAXY,SCALE,DEFAULT_DIR From DP_WorkspaceMgr WHERE WORKSPACE_NAME = '%s'",szName);

	return GetWorkspace(szSql,ppWks);									 
}


BOOL CDpDatabase::GetWorkspace(const char* szSql,CDpWorkspace** ppWks)
{
	*ppWks = NULL;
	
	if (!szSql || strlen(szSql) < 1 || !ppWks)
		return false;


	SQLCHAR				szID[50];						//������ID
	SQLCHAR				szName[100];					//����������
	SQLCHAR				szDesc[300];					//����������
	TIMESTAMP_STRUCT	WksCreateDate;					//����������ʱ��
	SQLCHAR				szCreater[50];					//����������������
	SQLSMALLINT			numWksType		= 0;			//����������
	TIMESTAMP_STRUCT	WksUpdateDate;					//����������ʱ��
	SQLINTEGER			numWksVersion	= 0.0f;			//�������汾
	SQLFLOAT			fMinX			= 0.0f;							
	SQLFLOAT			fMinY			= 0.0f;							
	SQLFLOAT			fMaxX			= 0.0f;							
	SQLFLOAT			fMaxY			= 0.0f;
	SQLFLOAT			fScale			= 0.0f;
	SQLCHAR				szDefaultDir[_MAX_PATH];		//Ĭ�Ϲ���·��
//	SQLCHAR				szPhysicalDir[_MAX_PATH];		//����·��

	memset(szID,0,sizeof(szID));
	memset(szName,0,sizeof(szName));
	memset(szDesc,0,sizeof(szDesc));
	memset(&WksCreateDate,0,sizeof(WksCreateDate));
	memset(szCreater,0,sizeof(szCreater));
	memset(&WksUpdateDate,0,sizeof(WksUpdateDate));
	memset(szDefaultDir,0,sizeof(szDefaultDir));
//	memset(szPhysicalDir,0,sizeof(szPhysicalDir));

	SQLLEN		cbLen = 0;			//�ֶγ���

	SQLRETURN		retcode;

	BOOL			bRet			= false;

	SQLHANDLE	hStmt	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_hConn,&hStmt) != SQL_SUCCESS)
		return false;

	retcode = SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS);

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hStmt,1,SQL_C_CHAR,szID,sizeof(szID),&cbLen);			//ȡID
			SQLGetData(hStmt,2,SQL_C_CHAR,szName,sizeof(szName),&cbLen);		//ȡ����
			SQLGetData(hStmt,3,SQL_C_CHAR,&szDesc,sizeof(szDesc),&cbLen);		//ȡ��ע
			SQLGetData(hStmt,4,SQL_C_TYPE_TIMESTAMP,&WksCreateDate,sizeof(WksCreateDate),&cbLen);	//ȡ��������������
			SQLGetData(hStmt,5,SQL_C_CHAR,&szCreater,sizeof(szCreater),&cbLen);						//ȡ����������������
			SQLGetData(hStmt,6,SQL_C_SHORT,&numWksType,sizeof(numWksType),&cbLen);					//ȡ����������
			SQLGetData(hStmt,7,SQL_C_TYPE_DATE,&WksUpdateDate,sizeof(WksUpdateDate),&cbLen);		//ȡ�������������� 
			SQLGetData(hStmt,8,SQL_C_LONG,&numWksVersion,sizeof(numWksVersion),&cbLen);	//ȡ�������汾
			SQLGetData(hStmt,9,SQL_C_DOUBLE,&fMinX,sizeof(fMinX),&cbLen);						
			SQLGetData(hStmt,10,SQL_C_DOUBLE,&fMinY,sizeof(fMinY),&cbLen);						
			SQLGetData(hStmt,11,SQL_C_DOUBLE,&fMaxX,sizeof(fMaxX),&cbLen);						
			SQLGetData(hStmt,12,SQL_C_DOUBLE,&fMaxY,sizeof(fMaxY),&cbLen);	
			SQLGetData(hStmt,13,SQL_C_DOUBLE,&fScale,sizeof(fScale),&cbLen);
			SQLGetData(hStmt,14,SQL_C_CHAR,szDefaultDir,sizeof(szDefaultDir),&cbLen);
		//	SQLGetData(hStmt,15,SQL_C_CHAR,szPhysicalDir,sizeof(szPhysicalDir),&cbLen);		//ȡ�����������ļ�·��

			//�жϵ�ǰ�û��Ƿ���Ȩ�޶�ȡ�ù�����
			DP_RIGHT iRight = DP_RIGHT_NOTRIGHT;
			if ((strlen((char*)szID) == WKS_ID_LEN) && 
				m_pRightMgr->GetWorkspaceRight((char*)szID,&iRight) && 
				(iRight == DP_RIGHT_READONLY || iRight == DP_RIGHT_READWRITE))
			{
				//���ɹ�������ʵ��
				*ppWks = new CDpWorkspace(this);
				//����ID
				CopyStr(&((*ppWks)->m_szID),(char*)szID);
				//��������
				CopyStr(&((*ppWks)->m_szName),(char*)szName);
				//���Ʊ�ע
				CopyStr(&((*ppWks)->m_szDesc),(char*)szDesc);
				//���ƹ�������������
				(*ppWks)->m_tmCreate	= GetTimeFromDBStruct(&WksCreateDate);
				//���ƹ����������˵�����
				CopyStr(&((*ppWks)->m_szCreater),(char*)szCreater);
				//���ƹ���������
				(*ppWks)->m_nWksType	= DP_WORKSPACE_TYPE(numWksType);
				//���ƹ���������ʱ��
				(*ppWks)->m_tmUpdate	= GetTimeFromDBStruct(&WksUpdateDate);
				//���ƹ������汾
				(*ppWks)->m_iVersion	= numWksVersion;
				(*ppWks)->m_rtBound.SetRect(fMinX,fMinY,fMaxX,fMaxY);
				(*ppWks)->m_dblScale	= fScale;
				//����ȱʡ����Ŀ¼
				CopyStr(&((*ppWks)->m_szDefaultDir),(char*)szDefaultDir);
				//���������ļ�Ŀ¼
		//		CopyStr(&((*ppWks)->m_szPhysicalDir),(char*)szPhysicalDir);

				bRet = true;
			}
		}

		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);

	return bRet;
}


//��ǰ���ӵ����ݿ������,���û���������ݿ�,��ֵΪ0
DP_DATABASE_TYPE CDpDatabase::GetDBType()
{
	return m_dtDatabaseType;
}


//����������
//����˵��:   szName -- Ҫ�����Ĺ�����������    pRtBound -- �������Ŀռ䷶Χ
//            dScale -- �������ı�����          szDefaultDir -- ȱʡ����Ŀ¼
//            [out]szID -- ����ִ�гɹ�,�����乤����ID,ʧ�ܷ��ؿ��ִ�,���
//                         ����Ҫ��������ID,�򽫸ò�����ΪNULL����
//			 iWspType -- ���������ͣ�houkui,06.6.29
//			 szDataserver -- �ɹ���������houkui,06.6.29
BOOL CDpDatabase::CreateWorksapce(const char*		szName,
								  CDpRect*			pRtBound,
								  double			dScale,
								  const char*		szDefaultDir,
								  const char*		szDataserver,
								  int				iWspType,
								  char*				szID,
								  BOOL				bIsSetDefaultRight
								  )
{
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ��Դ���������
		return false;

	if (szID)
		*szID = 0;

	if (!szName || strlen(szName) <= 0 || !szDefaultDir || strlen(szDataserver) < 0)
		return false;
	
	if (!IsOpen())
		return false;
	if (iWspType!=1&&iWspType!=2&&iWspType!=3&&iWspType!=4)
		return false;

	//�����ݿ��еõ���ǰ���Ĺ�����ID
	time_t		tmTmp;	
	time(&tmTmp);
	tm*			tmCur = localtime(&tmTmp);

	char		szTime[7];
	memset(szTime,0,sizeof(szTime));
	//ʱ��ֻ����2000���Ժ��ʱ��
	sprintf(szTime,"%02d%02d%02d",tmCur->tm_year-100,tmCur->tm_mon+1,tmCur->tm_mday);
	
	int			iMaxID	= -1;

	//�������Ĺ�����ID
	CDpRecordset	rst(this);
	char			szSql[2048];
	memset(szSql,0,sizeof(szSql));
	if (m_dtDatabaseType == DP_ACCESS)
		sprintf(szSql,"Select Max(cint(mid(WORKSPACE_ID,12,3))) as MaxID From DP_WorkspaceMgr Where mid(WORKSPACE_ID,1,11) = '%s%s'",
			    m_szGlobalID,szTime);
	else if (m_dtDatabaseType == DP_ORACLE)
		sprintf(szSql,"Select NVL(MAX(TO_NUMBER(SUBSTR(WORKSPACE_ID,12,3))),0) as MaxID From DP_WorkspaceMgr Where SUBSTR(WORKSPACE_ID,1,11) = '%s%s'",
			    m_szGlobalID,szTime);
	else if (m_dtDatabaseType == DP_SQLSERVER)
		sprintf(szSql,"Select ISNULL(Max(CAST(SUBSTRING(WORKSPACE_ID,12,3) AS int)),0) as MaxID From DP_WorkspaceMgr Where SUBSTRING(WORKSPACE_ID,1,11) = '%s%s'",
			    m_szGlobalID,szTime);
	else
		return false;
 

	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
				iMaxID = (int)var;
		}
	}

	rst.Close();

	//���ɹ�����ID
	if (iMaxID == -1 || iMaxID > MAX_WORKSPACE_ID)
		return false;
	iMaxID++;

	char			szWksID[15];				//�µĹ�����ID
	memset(szWksID,0,sizeof(szWksID));
	sprintf(szWksID,"%s%s%03d",m_szGlobalID,szTime,iMaxID);	

	
	char		szPwd[50];
	memset(szPwd,0,sizeof(szPwd));
	GetWksUserPwd(szWksID,szPwd);
	//ȷ���Ƿ��ܴ���ȱʡ����Ŀ¼,���߸ù���Ŀ¼�Ѵ���
	if (!CreateDir(szDefaultDir,szWksID,szPwd))
		return false;

	//�ٽ��ù�������Ŀ¼����,������Ϊ����������,������û�ΪGUEST
	char		szServerDir[_MAX_PATH];
	memset(szServerDir,0,sizeof(szServerDir));

	if (!AddShareDirInServer(szWksID,szDefaultDir,szName,szServerDir))
		return false;

/*
	if(!SetDefDirSecurity(szDefaultDir,szWksID))
		return FALSE;
*/

/*
*	//����˵��: [in] Ҫ���ӵ�����Ȩ�޵��û�ID [in] szPath -- Ҫ����ı���Ŀ¼   
//          [out] szRetServerDirPath -- ���ش���������������·��
BOOL AddShareDirInServer(const char* szUserName,const char* szPath,
const char* szNetName,char* szRetServerDirPath)
 */

/*
			if(!SetProcessToken(szWksID))
				return FALSE;*/

	//��������С���ֻ��DLG��ʹ�ã�
	memset(szSql,0,sizeof(szSql));
	if (m_dtDatabaseType == DP_ACCESS)
		sprintf(szSql,"Create Table GRP%s(ID INTEGER,GRID_ID VARCHAR(19),NAME VARCHAR(32),ATTRIBUTE INTEGER,PRIMARY KEY(ID,GRID_ID))",szWksID);
	else if (m_dtDatabaseType == DP_ORACLE)
		sprintf(szSql,"Create Table GRP%s(ID INTEGER,GRID_ID VARCHAR2(19),NAME VARCHAR2(32),ATTRIBUTE INTEGER,PRIMARY KEY(ID,GRID_ID))",szWksID);
	else if (m_dtDatabaseType == DP_SQLSERVER)
		sprintf(szSql,"Create Table GRP%s(ID int,GRID_ID VARCHAR(19),NAME VARCHAR(32),ATTRIBUTE int,PRIMARY KEY(ID,GRID_ID))",szWksID);
	ExecuteSQL(szSql);

	memset(szSql,0,sizeof(szSql));
	if (m_dtDatabaseType == DP_ACCESS)
		sprintf(szSql,"Create Table UDL%s(ID INTEGER,GRID_ID VARCHAR(19),NAME VARCHAR(32),VISI SMALLINT,LOCK SMALLINT,ATTRIBUTE INTEGER,PRIMARY KEY(ID,GRID_ID))",szWksID);
	else if (m_dtDatabaseType == DP_ORACLE)
		sprintf(szSql,"Create Table UDL%s(ID INTEGER,GRID_ID VARCHAR2(19),NAME VARCHAR2(32),VISI SMALLINT,LOCK SMALLINT,ATTRIBUTE INTEGER,PRIMARY KEY(ID,GRID_ID))",szWksID);
	else if (m_dtDatabaseType == DP_SQLSERVER)
		sprintf(szSql,"Create Table UDL%s(ID int,GRID_ID VARCHAR(19),NAME VARCHAR(32),VISI SMALLINT,LOCK SMALLINT,ATTRIBUTE int,PRIMARY KEY(ID,GRID_ID))",szWksID);
	ExecuteSQL(szSql);
		

	//������¼
	CDpRect		rt;
	if (pRtBound)
		rt.CopyRect(pRtBound);
	else
		rt.SetRect(0,0,0,0);
	char			szTimeSql[256];
	memset(szTimeSql,0,sizeof(szTimeSql));

	BuildDateTimeSql(m_dtDatabaseType,tmCur,szTimeSql);
	memset(szSql,0,sizeof(szSql));
	/*
		sprintf(szSql,"Insert into DP_WorkspaceMgr(WORKSPACE_ID,WORKSPACE_NAME,CREATE_DATE,CREATER_NAME,WORKSPACE_TYPE,UPDATE_DATE,MINX,MINY,MAXX,MAXY,SCALE,DEFAULT_DIR,SERVERNAME,PHYSICAL_PATH)\
				VALUES('%s','%s',%s,'%s',%d,%s,%f,%f,%f,%f,%f,'%s','%s','%s')",\
				szWksID,szName,szTimeSql,m_szUserID,iWspType,szTimeSql,rt.m_dMinX,rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,dScale,szServerDir,szDataserver,szPhicalPath);//houkui,06.6.29*/
	sprintf(szSql,"Insert into DP_WorkspaceMgr(WORKSPACE_ID,WORKSPACE_NAME,CREATE_DATE,CREATER_NAME,WORKSPACE_TYPE,UPDATE_DATE,MINX,MINY,MAXX,MAXY,SCALE,DEFAULT_DIR,SERVERNAME)\
					VALUES('%s','%s',%s,'%s',%d,%s,%f,%f,%f,%f,%f,'%s','%s')",\
					szWksID,szName,szTimeSql,m_szUserID,iWspType,szTimeSql,rt.m_dMinX,rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,dScale,szServerDir,szDataserver);//houkui,06.6.29
	if (ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		if (szID)
			strcpy(szID,szWksID);
		//����ϵͳ���õ�����ȫ��Ȩ��
		if (bIsSetDefaultRight)
		{
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT '%s',USER_ID,'All Image Read Right' From DP_USERMGR",
				    szWksID);
			ExecuteSQL(szSql);
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT '%s',USER_ID,'All Map Read Right' From DP_USERMGR",
				    szWksID);
			ExecuteSQL(szSql);
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT '%s',USER_ID,'All Parament Read Right' From DP_USERMGR",
				    szWksID);
			ExecuteSQL(szSql);
		}

		return true;
	}
	else
		return false;
}

/*
//����������
//����˵��:   szName -- Ҫ�����Ĺ�����������    pRtBound -- �������Ŀռ䷶Χ
//            dScale -- �������ı�����          szDefaultDir -- ȱʡ����Ŀ¼
//            [out]szID -- ����ִ�гɹ�,�����乤����ID,ʧ�ܷ��ؿ��ִ�,���
//                         ����Ҫ��������ID,�򽫸ò�����ΪNULL����
BOOL CDpDatabase::CreateWorksapce(const char* szName,CDpRect* pRtBound,
								  double dScale,const char* szDefaultDir,
								  char* szID,BOOL bIsSetDefaultRight)
{
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ��Դ���������
		return false;

	if (szID)
		*szID = 0;

	if (!szName || strlen(szName) <= 0 || !szDefaultDir || strlen(szDefaultDir) < 0)
		return false;
	if (!IsOpen())
		return false;


	//�����ݿ��еõ���ǰ���Ĺ�����ID
	time_t		tmTmp;	
	time(&tmTmp);
	tm*			tmCur = localtime(&tmTmp);

	char		szTime[7];
	memset(szTime,0,sizeof(szTime));
	//ʱ��ֻ����2000���Ժ��ʱ��
	sprintf(szTime,"%02d%02d%02d",tmCur->tm_year-100,tmCur->tm_mon+1,tmCur->tm_mday);
	
	int			iMaxID	= -1;

	//�������Ĺ�����ID
	CDpRecordset	rst(this);
	char			szSql[2048];
	memset(szSql,0,sizeof(szSql));
	if (m_dtDatabaseType == DP_ACCESS)
		sprintf(szSql,"Select Max(cint(mid(WORKSPACE_ID,12,3))) as MaxID From DP_WorkspaceMgr Where mid(WORKSPACE_ID,1,11) = '%s%s'",
			    m_szGlobalID,szTime);
	else if (m_dtDatabaseType == DP_ORACLE)
		sprintf(szSql,"Select NVL(MAX(TO_NUMBER(SUBSTR(WORKSPACE_ID,12,3))),0) as MaxID From DP_WorkspaceMgr Where SUBSTR(WORKSPACE_ID,1,11) = '%s%s'",
			    m_szGlobalID,szTime);
	else if (m_dtDatabaseType == DP_SQLSERVER)
		sprintf(szSql,"Select ISNULL(Max(CAST(SUBSTRING(WORKSPACE_ID,12,3) AS int)),0) as MaxID From DP_WorkspaceMgr Where SUBSTRING(WORKSPACE_ID,1,11) = '%s%s'",
			    m_szGlobalID,szTime);
	else
		return false;
 

	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
				iMaxID = (int)var;
		}
	}

	rst.Close();

	//���ɹ�����ID
	if (iMaxID == -1 || iMaxID > MAX_WORKSPACE_ID)
		return false;
	iMaxID++;

	char			szWksID[15];				//�µĹ�����ID
	memset(szWksID,0,sizeof(szWksID));
	sprintf(szWksID,"%s%s%03d",m_szGlobalID,szTime,iMaxID);

	
	
	
	
	char		szPwd[50];
	memset(szPwd,0,sizeof(szPwd));
	GetWksUserPwd(szWksID,szPwd);
	//ȷ���Ƿ��ܴ���ȱʡ����Ŀ¼,���߸ù���Ŀ¼�Ѵ���
	if (!CreateDir(szDefaultDir,szWksID,szPwd))
		return false;

	//�ٽ��ù�������Ŀ¼����,������Ϊ����������,������û�ΪGUEST
	char		szServerDir[_MAX_PATH];
	memset(szServerDir,0,sizeof(szServerDir));

	if (!AddShareDirInServer(szWksID,szDefaultDir,szName,szServerDir))
		return false;




	//������¼
	CDpRect		rt;
	if (pRtBound)
		rt.CopyRect(pRtBound);
	else
		rt.SetRect(0,0,0,0);
	char			szTimeSql[256];
	memset(szTimeSql,0,sizeof(szTimeSql));

	BuildDateTimeSql(m_dtDatabaseType,tmCur,szTimeSql);
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Insert into DP_WorkspaceMgr(WORKSPACE_ID,WORKSPACE_NAME,CREATE_DATE,CREATER_NAME,WORKSPACE_TYPE,UPDATE_DATE,MINX,MINY,MAXX,MAXY,SCALE,DEFAULT_DIR) VALUES('%s','%s',%s,'%s',%d,%s,%f,%f,%f,%f,%f,'%s')",
			szWksID,szName,szTimeSql,m_szUserID,DP_WKS_FULL,szTimeSql,rt.m_dMinX,rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,dScale,szServerDir);
	if (ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		if (szID)
			strcpy(szID,szWksID);
		//����ϵͳ���õ�����ȫ��Ȩ��
		if (bIsSetDefaultRight)
		{
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT '%s',USER_ID,'All Image Read Right' From DP_USERMGR",
				    szWksID);
			ExecuteSQL(szSql);
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT '%s',USER_ID,'All Map Read Right' From DP_USERMGR",
				    szWksID);
			ExecuteSQL(szSql);
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT '%s',USER_ID,'All Parament Read Right' From DP_USERMGR",
				    szWksID);
			ExecuteSQL(szSql);
		}

		return true;
	}
	else
		return false;
}
*/

//ɾ��������
BOOL CDpDatabase::DeleteWorkspace(const char* szWksID,BOOL bDeleteDefaultDir)
{
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ���ɾ��������
		return false;
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN)
		return false;

	CDpRecordset	rst(this);
	char		szSql[1024];
	memset(szSql,0,sizeof(szSql));

	CDpWorkspace*	pWks	= NULL;
	if (!GetWksByID(szWksID,&pWks) || !pWks)
		return false;

	CDpStringArray	pArray;
	CDpStringArray	NameArray;

	if (CanTransact())									//��ʼ����
		BeginTrans();

	//��ɾ��������������еļ�¼
	sprintf(szSql,"Delete From DP_WorkspaceMgr where WORKSPACE_ID = '%s'",szWksID);
	if (this->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//ɾ����ɫȨ�ޱ��еļ�¼
	sprintf(szSql,"Delete From DP_RoleRight where WORKSPACE_ID = '%s'",szWksID);
	if (this->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//ɾ���û�Ȩ�ޱ��иù������ļ�¼
	sprintf(szSql,"Delete From DP_UserRight where WORKSPACE_ID = '%s'",szWksID);
	if (this->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//ɾ�����������еļ�¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASKMGR_MAIN where WORKSPACE_ID = '%s'",szWksID);
	if (this->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//ɾ������״̬���еļ�¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASK_STATUS where WORKSPACE_ID = '%s'",szWksID);
	if (this->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//ö�ٵ�ǰ�������е�ͼ��,ͼ��,ԭʼӰ��,��һһɾ��
	//ͼ��
	if (pWks->GetLayersIDAndName(pArray,NameArray))
	{
		int	iCount = pArray.GetCount();
		for (int i = 0; i < iCount; i++)
			pWks->DeleteLayer(pArray.GetItem(i));
	}
	pArray.DelAllItem();
	
	//����С���
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop Table GRP%s",szWksID);
	if (this->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop Table UDLs",szWksID);
	if (this->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//ͼ��
	if (pWks->GetUnitsID(pArray,DP_A_UNIT))
	{
		int	iCount = pArray.GetCount();
		for (int i = 0; i < iCount; i++)
			pWks->DeleteUnit(pArray.GetItem(i));
	}
	pArray.DelAllItem();


	if (pWks->GetUnitsID(pArray,DP_A_STEREO))
	{
		int	iCount = pArray.GetCount();
		for (int i = 0; i < iCount; i++)
			pWks->DeleteUnit(pArray.GetItem(i));
	}
	pArray.DelAllItem();


	//ԭʼӰ��
	if (pWks->GetImagesID(pArray))
	{
		int	iCount = pArray.GetCount();
		for (int i = 0; i < iCount; i++)
			pWks->DeleteImage(pArray.GetItem(i));
	}
	pArray.DelAllItem();
	

	//ɾ������
	char				szShareName[MAX_PATH];
	NET_API_STATUS		netapi;

	memset(szShareName,0,sizeof(szShareName));
	sprintf(szShareName,"%s$",pWks->m_szName);
	char *pNetname ;
	pNetname = AsWideString(szShareName);
//	netapi = NetShareDel(NULL,pNetname,0);//���
	if(pNetname)
	{
		delete pNetname;
		pNetname = NULL;
	}

/*
    	SHARE_INFO_2		*si2;
		char *pNetname ;
		pNetname = (AsWideString(szShareName));
		netapi = NetShareGetInfo(NULL,pNetname,2,(LPBYTE *) &si2);
		if(pNetname)
		{
			delete pNetname;
			pNetname = NULL;
		}*/
	
	//ɾ��Ϊ���û������WINDOWS�û�
	{
		WCHAR*		wszUserID	= NULL;
		GetBStrFromChar(pWks->m_szID,&wszUserID);

		AddAccessRights( pWks->m_szDefaultDir,(const char*)wszUserID,NULL );
		NetUserDel(NULL,wszUserID);
		delete [] wszUserID;
		wszUserID = NULL;
	}

/*
	if (bDeleteDefaultDir)
	{
		//ֻ���Ǳ��صĻ�����������ɾ��
		char		szComputerName[_MAX_PATH];
		memset(szComputerName,0,sizeof(szComputerName));
		DWORD		lSize	= sizeof(szComputerName);		
		GetComputerName(szComputerName,&lSize);
		
		char  szLocalName[_MAX_PATH];
		memset(szLocalName, 0, sizeof(szLocalName));
		ParseNetServerName(pWks->m_szDefaultDir,szLocalName);
		if(0 == _strnicmp(szComputerName,szLocalName+2,sizeof(szLocalName)-2))
		{
			/ *
			int nBufSize = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, (LPWSTR )(si2->shi2_path), -1, NULL, 0,NULL ,NULL);
						char *szPathString = new char[nBufSize+1];
						WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, (LPWSTR )(si2->shi2_path), -1, szPathString, nBufSize,NULL,NULL );						* /
			char	*szPathString = new char[wcslen(LPWSTR (si2->shi2_path))+1];
			memset(szPathString,0,sizeof(szPathString));
			sprintf(szPathString,"%S",si2->shi2_path);
			EmptyDirectory(szPathString);
			RemoveDirectory(szPathString);
			if(szPathString)
			{
				delete szPathString;
				szPathString = NULL;
			}
		}
	}*/


	delete pWks;
	pWks = NULL;

	if (CanTransact())									//�ύ����
		CommitTrans();
	return true;

Err:
	delete pWks;
	pWks = NULL;

	if (CanTransact())					 				//�ع�����
		Rollback();
	return false;
}



//�õ�WBS CODE ά�������,pWBSCodeWH��Ҫ�������ͷ�
BOOL CDpDatabase::OpenWBSWH(CDpWBSCodeWH** ppWBSCodeWH,DP_WORKSPACE_TYPE type)
{
	if (!ppWBSCodeWH)
		return false;
		
	*ppWBSCodeWH	= new CDpWBSCodeWH(this,type);
	if ((*ppWBSCodeWH)->LoadDataFromDB())
		return true;
	else
	{
		delete *ppWBSCodeWH;
		*ppWBSCodeWH = NULL;
		return false;
	}

}



//������ɫ
BOOL CDpDatabase::CreateRole(const char* szRoleID,const char* szDesc)
{
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ��Դ�����ɫ
		return false;

	if (!IsOpen())
		return false;

	char		szSql[1024];
	sprintf(szSql,"Insert Into DP_RoleMgr values('%s','%s')",szRoleID,szDesc?szDesc:"");
	
	if (ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		return true;
	else
		return false;
	
}

//�����û�,bIsSetDefaultRight -- �Ƿ����û���������ϵͳ���õ�ȫ��Ȩ��,TRUE��ʾ����
BOOL CDpDatabase::CreateUser(const char* szUserID,const char* szDesc,
							 const char* szPwd,DP_USER_RIGHTTYPE nUserType,
							 BOOL bIsSetDefaultRight)
{
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ��Դ����û�
		return false;

	if (!IsOpen())
		return false;
	
	char		szSql[1024];
	sprintf(szSql,"Insert Into DP_USERMGR values('%s','%s','%s',%d)",szUserID,
			szDesc?szDesc:"",szPwd?szPwd:"",nUserType);

	if (ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		//����ϵͳ���õ�����ȫ��Ȩ��
		if (bIsSetDefaultRight)
		{
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT WORKSPACE_ID,'%s','All Image Read Right' From DP_WorkspaceMgr",
				    szUserID);
			ExecuteSQL(szSql);
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT WORKSPACE_ID,'%s','All Map Read Right' From DP_WorkspaceMgr",
				    szUserID);
			ExecuteSQL(szSql);
			sprintf(szSql,"Insert Into DP_UserRight(WORKSPACE_ID,USER_ID,ROLE_ID) SELECT WORKSPACE_ID,'%s','All Parament Read Right' From DP_WorkspaceMgr",
				    szUserID);
			ExecuteSQL(szSql);
		}
		return true;
	}
	else
		return false;

}


//ö�ٵ�ǰ���ݿ��ڵ��û�
BOOL CDpDatabase::GetUsersInfo(CDpUserArray& nArray)
{
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ���ö���û�
		return false;

	if (!IsOpen())
		return false;

	CDpRecordset		rst(this);

	char				szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select USER_ID,USER_DESC,USER_TYPE From DP_USERMGR");
	
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			CDpDBVariant	var1;
			CDpDBVariant	var2;

			if (rst.GetFieldValue(1,var) && 
				rst.GetFieldValue(2,var1) &&
				rst.GetFieldValue(3,var2))
			{
				_DpUserInfo*	pInfo	= new _DpUserInfo;
				memset(pInfo,0,sizeof(_DpUserInfo));
				CopyStr(&(pInfo->szUserID),(LPCTSTR)var);
				CopyStr(&(pInfo->szUserDesc),(LPCTSTR)var1);
				pInfo->nUserType	= DP_USER_RIGHTTYPE((int)var2);
				nArray.AddUserInfo(pInfo);
			}

		}
		rst.Close();
	}
	return true;
}


//ö�ٵ�ǰ���ݿ��ڵĽ�ɫ,nRoleID -- ��ɫ��,nRoleDesc -- ��ɫ����
BOOL CDpDatabase::GetRolesInfo(CDpStringArray& nRoleID,CDpStringArray& nRoleDesc)
{
	if (m_pRightMgr->GetUserType() != DP_ADMIN)			//ֻ��ϵͳ����Ա�ſ���ö���û�
		return false;

	if (!IsOpen())
		return false;

	CDpRecordset		rst(this);
	char				szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select ROLE_ID,ROLE_DESC From DP_RoleMgr");
	
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			CDpDBVariant	var1;

			if (rst.GetFieldValue(1,var) && 
				rst.GetFieldValue(2,var1))
			{
				nRoleID.AddString((char*)(LPCTSTR)var);
				nRoleDesc.AddString((char*)(LPCTSTR)var1);
			}

		}
		rst.Close();
	}
	return true;

}


//ɾ����ɫ
BOOL CDpDatabase::DeleteRole(const char* szRoleID)
{
	if (!szRoleID)
		return false;

	if (CanTransact())									//��ʼ����
		BeginTrans();

	char	szSql[256];
	sprintf(szSql,"Delete From DP_RoleMgr Where ROLE_ID = '%s'",szRoleID);
	if (ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	sprintf(szSql,"Delete From DP_RoleRight Where ROLE_ID = '%s'",szRoleID);
	if (ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	sprintf(szSql,"Delete From DP_UserRight Where ROLE_ID = '%s'",szRoleID);
	if (ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	if (CanTransact())									//�ύ����
		CommitTrans();
	return true;

Err:
	if (CanTransact())									//�ع�����
		Rollback();
	return false;

}

//ɾ���û�
BOOL CDpDatabase::DeleteUser(const char* szUserID)
{
	if (!szUserID)
		return false;

	if (CanTransact())									//��ʼ����
		BeginTrans();

	//��ɾ���û������
	char	szSql[256];
	sprintf(szSql,"Delete From DP_UserMgr Where USER_ID = '%s'",szUserID);
	if (ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//��ɾ���û�Ȩ�ޱ��е���ؼ�¼
	sprintf(szSql,"Delete From DP_UserRight Where USER_ID = '%s'",szUserID);
	if (ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	//��ɾ�����������е���ؼ�¼
	sprintf(szSql,"Delete From DP_TASKMGR_MAIN Where USER_ID = '%s'",szUserID);
	if (ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto Err;

	if (CanTransact())									//�ύ����
		CommitTrans();
	return true;

Err:
	if (CanTransact())									//�ع�����
		Rollback();
	return false;

}


//�޸��û�����
BOOL CDpDatabase::ModifyUserPwd(const char* szUserID,const char* szNewPwd,
								const char* szDesc,int iUserType)
{
	if (!szUserID || strlen(szUserID) <= 0 || !szNewPwd || strlen(szNewPwd) <= 0)
		return false;

	DP_USER_RIGHTTYPE		nType = DP_USER_RIGHTTYPE(iUserType);

	if (m_pRightMgr->GetUserType() != DP_ADMIN)
	{
		if (nType != DP_NORMAL || _stricmp(m_szUserID,szUserID) != 0)
			return false;
	}

	char		szSql[256];
	sprintf(szSql,"Update DP_USERMGR set USER_DESC = '%s',USER_PWD = '%s',USER_TYPE = %d Where USER_ID = '%s'",
		    szDesc,szNewPwd,iUserType,szUserID);

	return (ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}

//ˢ��Ȩ�޹�����
BOOL CDpDatabase::RefreshRightMgr()
{
	CDpRightMgr*		mgrTmp = new CDpRightMgr(this);
	if (mgrTmp->LoadRightInfo())
	{
		delete m_pRightMgr;
		m_pRightMgr = mgrTmp;
		return true;
	}
	else
	{
		delete mgrTmp;
		mgrTmp = NULL;
		return false;
	}

	return false;
}















/************************************************************/
/*                         Ȩ�޹�����                       */
/************************************************************/
CDpRightMgr::CDpRightMgr(CDpDatabase* pDB,const char* szUserID)
{
	m_iUserType					= DP_NORMAL;
	m_pDB						= pDB;
	memset(m_szUserID,0,sizeof(m_szUserID));
	if (szUserID)
		strcpy(m_szUserID,szUserID);
	else
		strcpy(m_szUserID,m_pDB->m_szUserID);
}

CDpRightMgr::~CDpRightMgr()
{
	RemoveRightInfo();
}

//�����ݿ��м���Ȩ�޹�����Ϣ
BOOL CDpRightMgr::LoadRightInfo()
{
	char			szSql[256];
	char			szWksID[50];
	SQLCHAR			szID[50];
	SQLSMALLINT		iUserType			= 0;
	SQLINTEGER		iParamID			= 0;
	SQLSMALLINT		iRight				= 0;
	SQLLEN		cbLen = 0;
	SQLRETURN		retcode				= 0;
	SQLHANDLE		hStmt				= NULL;

	memset(szSql,0,sizeof(szSql));
	memset(szID,0,sizeof(szID));

	//�ȵõ���½�û�������,����ϵͳ����Ա����һ���û�
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return false;

	sprintf(szSql,"Select USER_TYPE From DP_UserMgr where USER_ID = '%s'",m_szUserID);

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hStmt,1,SQL_C_SHORT,&iUserType,sizeof(iUserType),&cbLen);			//ȡ�û�Ȩ������
			m_iUserType = DP_USER_RIGHTTYPE(iUserType);
		}
		else
		{
			SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
			SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
			return false;
		}
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	else
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	hStmt	= NULL;
	
	//�õ�������Ȩ��
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return false;

	sprintf(szSql,"Select WORKSPACE_ID From DP_V_USERRIGHT_WKS where USER_ID = '%s' ORDER BY WORKSPACE_ID",
		    m_szUserID);

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		while (true)
		{
			retcode = SQLFetch(hStmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				memset(szWksID,0,sizeof(szWksID));
				SQLGetData(hStmt,1,SQL_C_CHAR,szID,sizeof(szID),&cbLen);			//ȡ������ID
				char*		szTmpWksID	= NULL;
				CopyStr(&szTmpWksID,(char*)szID);
				m_mapWorkspace.push_back(szTmpWksID);					//������Ȩ����������ӹ�����Ȩ��
			}
			else
				break;
		}
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	else
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	hStmt	= NULL;

	//�ٵõ�ͼ��ͼ��Ȩ��
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select GRID_ID,LAYER_ID,MRIGHT From DP_V_USERRIGHT_UNIT_LAYER where USER_ID = '%s' ORDER BY GRID_ID",
		    m_szUserID);

	CDpRecordset		rst(m_pDB);
	ULONGLONG			ulUnitID		= 0;
	LAYERARRAY*			pLayerArray		= NULL;

	if (rst.Open(szSql))
	{
		CDpDBVariant	var;
		CDpDBVariant	var1;
		CDpDBVariant	var2;
		while (rst.Next())
		{
			if (rst.GetFieldValue(1,var) &&
				rst.GetFieldValue(2,var1) && 
				rst.GetFieldValue(3,var2))
			{
				ULONGLONG	uTmp		= _atoi64((LPCTSTR)var);
				ULONGLONG	uLayerID	= _atoi64((LPCTSTR)var1);

				if (uTmp == 0 || uLayerID == 0)
					continue;

				if (ulUnitID != uTmp)				//�����ͬ����MAP���¼�һ��ͼ��������
				{
					pLayerArray = new LAYERARRAY;
					m_mapUnit.push_back(pLayerArray);
				}
			
				_Unit_Layer_Right*	pLayerRight = new _Unit_Layer_Right;
				memset(pLayerRight,0,sizeof(_Unit_Layer_Right));
				pLayerRight->uUnitID	= uTmp;	
				pLayerRight->ulLayerID	= uLayerID;
				pLayerRight->sRight		= DP_RIGHT((int)var2);

				pLayerArray->push_back(pLayerRight);

				ulUnitID = uTmp;
			}

		}
		rst.Close();
	}
	



	//�ٵõ�Ӱ��Ȩ��
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select IMAGE_ID,XML_TAGNAME,MRIGHT From DP_V_USERRIGHT_IMAGE_XMLTAG where USER_ID = '%s' ORDER BY IMAGE_ID",
		    m_szUserID);

	ULONGLONG			ulImageID		= 0;
	XMLTAGARRAY*		pXmlTagArray	= NULL;

	if (rst.Open(szSql))
	{
		CDpDBVariant	var;
		CDpDBVariant	var1;
		CDpDBVariant	var2;
		while (rst.Next())
		{
			if (rst.GetFieldValue(1,var) &&
				rst.GetFieldValue(2,var1) && 
				rst.GetFieldValue(3,var2))
			{
				ULONGLONG	uTmp		= _atoi64((LPCTSTR)var);
				LPCTSTR		szTmp		= (LPCTSTR)var1;

				if (uTmp == 0 || strlen(szTmp) <= 0)
					continue;

				if (ulImageID != uTmp)				//�����ͬ����MAP���¼�һ��Ӱ�������
				{
					pXmlTagArray = new XMLTAGARRAY;
					m_mapImage.push_back(pXmlTagArray);
				}
			
				_Image_XMLTag_Right*	pXMLRight = new _Image_XMLTag_Right;
				memset(pXMLRight,0,sizeof(_Image_XMLTag_Right));
				
				pXMLRight->uImageID	= uTmp;
				CopyStr(&(pXMLRight->szXMLTagName),szTmp);
				pXMLRight->sRight	= DP_RIGHT((int)var2);		

				pXmlTagArray->push_back(pXMLRight);

				ulImageID = uTmp;
			}

		}
		rst.Close();
	}	



	//���ȡ����Ȩ��
	char			szTmpWksID[50]	= "\0";
	UINT			uTmpParamID		= 0;
	PARAMARRAY*		pParamArray		= NULL;

	memset(szTmpWksID,0,sizeof(szTmpWksID));

	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select WORKSPACE_ID,GRID_ID,PARAM_ID,MRIGHT From DP_V_USERRIGHT Where USER_ID = '%s' AND RIGHTMGR_TYPE = 2 ORDER BY WORKSPACE_ID",
		    m_szUserID);

	
	if (rst.Open(szSql))
	{
		CDpDBVariant	var;
		CDpDBVariant	var1;
		CDpDBVariant	var2;
		CDpDBVariant	var3;

		while (rst.Next())
		{
			if (rst.GetFieldValue(1,var) && 
				rst.GetFieldValue(2,var1) && 
				rst.GetFieldValue(3,var2) && 
				rst.GetFieldValue(4,var3))
			{
				const char*		szWksID	= (LPCTSTR)var;
				if (_stricmp((char*)szWksID,szTmpWksID) != 0)
				{
					strcpy(szTmpWksID,(char*)szWksID);

					//���ù������Ĳ���Ȩ��������뵽����Ȩ��MAP��ȥ
					pParamArray = new PARAMARRAY;
					m_mapParam.push_back(pParamArray);
				}

				_ParamRight*	pRight = new _ParamRight;
				memset(pRight,0,sizeof(_ParamRight));
				pRight->uWksID	= _atoi64((char*)szWksID);
				CopyStr(&(pRight->szParamTableName),(LPCTSTR)var1);
				pRight->uID		= (int)var2;
				pRight->sRight	= DP_RIGHT((int)var3);
				pParamArray->push_back(pRight);

			}
		}
		
		rst.Close();

	}

	//�ٵõ����û��Ƿ�ӵ��ȫ����Ȩ��������
	ULONGLONG			uWksID		= 0;
	_Wks_AllRead_Right*	pAll		= NULL;
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select WORKSPACE_ID,ROLE_ID From DP_UserRight Where USER_ID = '%s' AND ROLE_ID IS NOT NULL" ,m_szUserID);
	if (rst.Open(szSql))
	{
		CDpDBVariant	var;
		CDpDBVariant	var1;
		while (rst.Next())
		{
			if (rst.GetFieldValue(1,var) &&
				rst.GetFieldValue(2,var1))
			{
				ULONGLONG	uTmp		= _atoi64((LPCTSTR)var);
				if (uWksID != uTmp)
				{
					pAll = new _Wks_AllRead_Right;
					memset(pAll,0,sizeof(_Wks_AllRead_Right));
					pAll->uWksID	= uTmp;
					m_mapWksAllRead.push_back(pAll);
				}

				const char*		szRoleID = (LPCTSTR)var1;
				if  (_stricmp(szRoleID,"All Image Read Right") == 0)				//Ӱ��
					pAll->m_bIsCanReadAllImage	= true;
				else if (_stricmp(szRoleID,"All Map Read Right") == 0)			//ͼ��
					pAll->m_bIsCanReadAllUnitLayer	= true;
				else if (_stricmp(szRoleID,"All Parament Read Right") == 0)		//����
					pAll->m_bIsCanReadAllParam	= true;	

				uWksID	= uTmp;
			}
		}
		rst.Close();
	}



	return true;

}

//�ͷ�Ȩ�޹����¼��ռ���ڴ�
void CDpRightMgr::RemoveRightInfo()
{
	int			iCount		= 0;
	int			i			= 0;

	//���ͷŹ�����Ȩ��������ڴ�
	iCount = m_mapWorkspace.size();

	for (i = 0; i < iCount; i++)
	{
		char* szTmp = m_mapWorkspace[i];
		if (szTmp)
		{
			delete [] szTmp;
			szTmp = NULL;
		}
	}

	m_mapWorkspace.clear();

	//���ͷ�ͼ��Ȩ����ռ���ڴ�
	iCount = m_mapUnit.size();

	UNITMAP::iterator		itMap;

	for (itMap = m_mapUnit.begin(); itMap != m_mapUnit.end();++itMap)
	{
		LAYERARRAY*	pArray = (*itMap);

		if (pArray)
		{
			LAYERARRAY::iterator	jt;
			for (jt = pArray->begin();jt != pArray->end();++jt)
			{
				_Unit_Layer_Right* pRight = (*jt);
				if (pRight)	
				{
					delete pRight;
					pRight = NULL;
				}
			}
			pArray->clear();
			delete pArray;
		}
	}
	m_mapUnit.clear();



	//�ͷ�Ӱ��Ȩ����ռ���ڴ�
	IMAGEMAP::iterator		itImage;

	for (itImage = m_mapImage.begin(); itImage != m_mapImage.end();++itImage)
	{
		XMLTAGARRAY*	pArray = (*itImage);

		if (pArray)
		{
			XMLTAGARRAY::iterator	jt;
			for (jt = pArray->begin();jt != pArray->end();++jt)
			{
				_Image_XMLTag_Right* pRight = (*jt);
				if (pRight)
				{
					if (pRight->szXMLTagName)
					{
						delete [] pRight->szXMLTagName;
						pRight->szXMLTagName = NULL;
					}
					delete pRight;
					pRight = NULL;
				}
			}
			pArray->clear();
			delete pArray;
			pArray = NULL;
		}
	}
	m_mapImage.clear();

	//�ͷŲ���Ȩ����ռ���ڴ�
	PARAMARRAY::iterator ita;	
	PARAMMAP::iterator itm;

	for (itm = m_mapParam.begin(); itm != m_mapParam.end(); ++itm)
	{
		PARAMARRAY* pArray = (*itm);
		if (pArray)
		{
			for (ita = pArray->begin();ita != pArray->end(); ++ita)
			{
				_ParamRight*	pRight = (_ParamRight*)(*ita);
				if (pRight)
				{
					if (pRight->szParamTableName)
					{
						delete [] pRight->szParamTableName;
						pRight->szParamTableName = NULL;
					}
					delete pRight;
					pRight = NULL;
				}
			}
			pArray->clear();
			delete pArray;
			pArray = NULL;
		}		
	}

	m_mapParam.clear();

	//�ͷŶ�Ȩ������ռ���ڴ�
	WKSALLREADMAP::iterator	itr;
	for (itr = m_mapWksAllRead.begin(); itr != m_mapWksAllRead.end(); ++itr)
	{
		_Wks_AllRead_Right*	pAll	= (*itr);
		if (pAll)
		{
			delete pAll;
			pAll = NULL;
		}
	}
	m_mapWksAllRead.clear();	
}

//�жϸ�SQL����Ƿ���Ȩ�޽���ִ��,����оͷ���TRUE,���򷵻�FALSE;
BOOL CDpRightMgr::SqlCanExecute(const char* szSql)
{
	if (!szSql || strlen(szSql) <= 0)
		return false;

	return true;
}

//�õ���ĳ����������Ȩ��	
BOOL CDpRightMgr::GetWorkspaceRight(const char* szWksID,DP_RIGHT* piRight)
{
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN || !piRight)
		return false;

	if (m_iUserType == DP_ADMIN)				//ϵͳ����Աӵ�����е�Ȩ��
	{
		*piRight = DP_RIGHT_READWRITE;
		return true;
	}
	

	const _Wks_AllRead_Right* pAll = FindAllReadByWksID(szWksID);
	if (pAll && (pAll->m_bIsCanReadAllUnitLayer || pAll->m_bIsCanReadAllImage || 
		         pAll->m_bIsCanReadAllParam))
		*piRight = DP_RIGHT_READONLY;
	else
		*piRight = DP_RIGHT_NOTRIGHT;

	int		iCount		= m_mapWorkspace.size();

	for (int i = 0; i < iCount; i++)
	{
		if (_stricmp(szWksID,m_mapWorkspace[i]) == 0)
		{
			*piRight = DP_RIGHT_READWRITE;
			return true;
		}
	}

	return true;
}

//�õ���ĳ��ͼ����Ȩ��,ȡ���Ȩ��
BOOL CDpRightMgr::GetUnitRight(const char* szUnitID,DP_RIGHT* piRight)
{
	if (!szUnitID || !piRight)
		return false;

	if (m_iUserType == DP_ADMIN)				//ϵͳ����Աӵ�����е�Ȩ��
	{
		*piRight = DP_RIGHT_READWRITE;
		return true;
	}

	//�ж��Ƿ��ж�����ͼ��ͼ���ȫ��Ȩ��
	char	szTmp[20];
	memset(szTmp,0,sizeof(szTmp));
	memcpy(szTmp,szUnitID,WKS_ID_LEN);
	const _Wks_AllRead_Right* pAll = FindAllReadByWksID(szTmp);
	if (!pAll || !pAll->m_bIsCanReadAllUnitLayer)
		*piRight = DP_RIGHT_NOTRIGHT;
	else
		*piRight = DP_RIGHT_READONLY;
	
	ULONGLONG uID = _atoi64(szUnitID);

	//����ͼ��
	int				iCount	= m_mapUnit.size();
	LAYERARRAY*		pArray	= NULL;

	for (int m = 0; m < iCount; m++)
	{
		LAYERARRAY*	pTmp = m_mapUnit[m];
		if (pTmp && pTmp->size() > 0)
		{
			LAYERARRAY::iterator	it = pTmp->begin();
			if ((*it) && (*it)->uUnitID == _atoi64(szUnitID))
			{
				pArray = pTmp;
				break;
			}
		}
	}

	DP_RIGHT		nRight	= *piRight;

	if (pArray)
	{
		LAYERARRAY::iterator j;
		for (j = pArray->begin(); j != pArray->end(); ++j)
		{
			_Unit_Layer_Right*	pRight = (*j);
			if (pRight && nRight < pRight->sRight)
				nRight = pRight->sRight;
		}
	}

	*piRight = nRight;
	return true;


}

//ȷ���Ƿ��ܷ��ʸ�ͼ��,�жϵ����ݾ��Ǹ��û��Ƿ��ܷ��ʸ�ͼ�������һ��ͼ����
//��ʾ�ܷ��ʸ�ͼ��
BOOL CDpRightMgr::CanAccessLayer(const char* szLayerID)
{
	if (!szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return false;

	if (m_iUserType == DP_ADMIN)				//ϵͳ����Աӵ�����е�Ȩ��
		return true;

	//�ж��Ƿ��ж�����ͼ��ͼ���ȫ��Ȩ��
	char	szTmp[20];
	memset(szTmp,0,sizeof(szTmp));
	memcpy(szTmp,szLayerID,WKS_ID_LEN);
	const _Wks_AllRead_Right* pAll = FindAllReadByWksID(szTmp);
	if (pAll && pAll->m_bIsCanReadAllUnitLayer)
		return true;

	ULONGLONG uID = _atoi64(szLayerID);

	
	UNITMAP::iterator i;
	for (i = m_mapUnit.begin(); i != m_mapUnit.end(); ++i)
	{
		LAYERARRAY*	pArray	= (*i);
		if (pArray)
		{
			LAYERARRAY::iterator j;
			for (j = pArray->begin(); j != pArray->end(); ++j)
			{
				_Unit_Layer_Right*	pRight = (*j);
				if (pRight && uID == pRight->ulLayerID && pRight->sRight > DP_RIGHT_NOTRIGHT)
					return true;
			}
		}

	}

	return false;
}



//�õ�ͼ����ͼ���Ȩ��
BOOL CDpRightMgr::GetUnitLayerRight(const char* szUnitID,const char* szLayerID,DP_RIGHT* piRight)
{
	if (!szUnitID || !piRight)
		return false;

	if (m_iUserType == DP_ADMIN || atoi(szUnitID) == 0)			//ϵͳ����Աӵ�����е�Ȩ��,�����Ǹ�ͼ��IDΪ0��,���ʶ�ǲ��ܿص�ͼ��ID
	{
		*piRight = DP_RIGHT_READWRITE;
		return true;
	}

	//�ж��Ƿ��ж�����ͼ��ͼ���ȫ��Ȩ��
	char	szTmp[20];
	memset(szTmp,0,sizeof(szTmp));
	memcpy(szTmp,szUnitID,WKS_ID_LEN);
	const _Wks_AllRead_Right* pAll = FindAllReadByWksID(szTmp);
	if (!pAll || !pAll->m_bIsCanReadAllUnitLayer)
		*piRight = DP_RIGHT_NOTRIGHT;
	else
		*piRight = DP_RIGHT_READONLY;
	
	ULONGLONG uID = _atoi64(szUnitID);

	//���Ҹ�ͼ��
	int			iCount	= m_mapUnit.size();
	LAYERARRAY*	pArray	= NULL;

	for (int m = 0; m < iCount; m++)
	{
		LAYERARRAY*	pTmp = m_mapUnit[m];
		if (pTmp && pTmp->size() > 0)
		{
			LAYERARRAY::iterator	it = pTmp->begin();
			if ((*it) && (*it)->uUnitID == uID)
			{
				pArray	= pTmp;
				break;
			}
		}
	}

	DP_RIGHT		nRight	= DP_RIGHT_NOTRIGHT;

	if (pArray)
	{
		LAYERARRAY::iterator j;
		for (j = pArray->begin(); j != pArray->end(); ++j)
		{
			_Unit_Layer_Right*	pRight = (*j);
			if (pRight->ulLayerID == _atoi64(szLayerID))
			{
				*piRight = pRight->sRight;
				return true;
			}
		}
	}


	return true;
}


//�õ���ĳ��Ӱ���Ȩ��,ȡ���Ȩ��
BOOL CDpRightMgr::GetImageRight(const char* szImageID,DP_RIGHT* piRight)
{
	if (!szImageID || strlen(szImageID) != IMAGE_ID_LEN || !piRight)
		return false;

	if (m_iUserType == DP_ADMIN)				//ϵͳ����Աӵ�����е�Ȩ��
	{
		*piRight = DP_RIGHT_READWRITE;
		return true;
	}

	//�ж��Ƿ��ж�Ӱ��ȫ��Ȩ��
	char	szTmp[20];
	memset(szTmp,0,sizeof(szTmp));
	memcpy(szTmp,szImageID,WKS_ID_LEN);
	const _Wks_AllRead_Right* pAll = FindAllReadByWksID(szTmp);
	if (!pAll || !pAll->m_bIsCanReadAllImage)
		*piRight = DP_RIGHT_NOTRIGHT;
	else
		*piRight = DP_RIGHT_READONLY;

	
	ULONGLONG uID = _atoi64(szImageID);

	//����Ӱ��
	int				iCount	= m_mapImage.size();
	XMLTAGARRAY*	pArray	= NULL;

	for (int m = 0; m < iCount; m++)
	{
		XMLTAGARRAY* pTmp = m_mapImage[m];
		if (pTmp && pTmp->size() > 0)
		{
			XMLTAGARRAY::iterator	it = pTmp->begin();
			if ((*it) && (*it)->uImageID == uID)
			{
				pArray = pTmp;
				break;
			}
		}
	}


	DP_RIGHT		nRight	= *piRight;

	if (pArray)
	{
		XMLTAGARRAY::iterator	j;
		for (j = pArray->begin(); j != pArray->end(); ++j)
		{
			_Image_XMLTag_Right*	pRight = (*j);
			if (pRight && nRight < pRight->sRight)
				nRight = pRight->sRight;
		}
	
	}
	*piRight = nRight;

	return true;
}


//�õ�Ӱ���ĳ��XML��ǩ��Ȩ��
BOOL CDpRightMgr::GetImageXmlRight(const char* szImageID,const char* szXmlTagName,DP_RIGHT* piRight)
{
	if (!szImageID || strlen(szImageID) != IMAGE_ID_LEN || !piRight || !szXmlTagName)
		return false;

	if (m_iUserType == DP_ADMIN)				//ϵͳ����Աӵ�����е�Ȩ��
	{
		*piRight = DP_RIGHT_READWRITE;
		return true;
	}

	//�ж��Ƿ��ж�Ӱ��ȫ��Ȩ��
	char	szTmp[20];
	memset(szTmp,0,sizeof(szTmp));
	memcpy(szTmp,szImageID,WKS_ID_LEN);
	const _Wks_AllRead_Right* pAll = FindAllReadByWksID(szTmp);
	if (!pAll || !pAll->m_bIsCanReadAllImage)
		*piRight = DP_RIGHT_NOTRIGHT;
	else
		*piRight = DP_RIGHT_READONLY;
	
	ULONGLONG uID = _atoi64(szImageID);

	//����Ӱ��
	int				iCount	= m_mapImage.size();
	XMLTAGARRAY*	pArray	= NULL;

	for (int m = 0; m < iCount; m++)
	{
		XMLTAGARRAY* pTmp = m_mapImage[m];
		if (pTmp && pTmp->size() > 0)
		{
			XMLTAGARRAY::iterator	it = pTmp->begin();
			if ((*it) && (*it)->uImageID == uID)
			{
				pArray = pTmp;
				break;
			}
		}
	}

	DP_RIGHT		nRight	= DP_RIGHT_NOTRIGHT;

	if (pArray)
	{
		XMLTAGARRAY::iterator	j;
		for (j = pArray->begin(); j != pArray->end(); ++j)
		{
			_Image_XMLTag_Right*	pRight = (*j);
			if (pRight && _stricmp(pRight->szXMLTagName,szXmlTagName) == 0)
			{
				*piRight	= pRight->sRight; 
				return true;
			}
		}
	
	}

	return true;

}


//�õ�ĳ����������ĳ��������Ȩ��
BOOL CDpRightMgr::GetParamRight(const char* szWksID,const char* szParamTabName,
								UINT uParamID,DP_RIGHT* piRight)
{
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN || 
		!szParamTabName || strlen(szParamTabName) <= 0 || !piRight )
		return false;

	if (m_iUserType == DP_ADMIN)			//ϵͳ����Աӵ�����е�Ȩ��
	{
		*piRight = DP_RIGHT_READWRITE;
		return true;
	}

	//�ж��Ƿ��жԲ���ȫ��Ȩ��
	char	szTmp[20];
	memset(szTmp,0,sizeof(szTmp));
	memcpy(szTmp,szParamTabName+1,WKS_ID_LEN);
	const _Wks_AllRead_Right* pAll = FindAllReadByWksID(szTmp);
	if (!pAll || !pAll->m_bIsCanReadAllParam)
		*piRight = DP_RIGHT_NOTRIGHT;
	else
		*piRight = DP_RIGHT_READONLY;

	//���Ҹù�����
	ULONGLONG uID = _atoi64(szWksID);
	PARAMARRAY*	pArray	= NULL;

	int		iCount	= m_mapParam.size();
	for (int m = 0; m < iCount; m++)
	{
		PARAMARRAY*	pTmp = m_mapParam[m];
		if (pTmp && pTmp->size() > 0)
		{
			PARAMARRAY::iterator	i = pTmp->begin();
			if ((*i)->uWksID == _atoi64(szWksID))
			{
				pArray = pTmp;
				break;
			}
		}
	}

	if (pArray == NULL)
		return true;

	PARAMARRAY::iterator	i;
	for (i = pArray->begin(); i != pArray->end(); ++i)
	{
		_ParamRight* pItem = (*i);

		if (pItem)
		{
			if (pItem->szParamTableName && (_stricmp(pItem->szParamTableName,szParamTabName) == 0) && pItem->uID == uParamID)
			{
				*piRight = pItem->sRight;
				return true;
			}
		}
	}


	return true;
}


//����ĳ��Ӱ���ĳ��XML��ǩ
//bLock -- TRUE:����,FALSE:����
BOOL CDpRightMgr::LockImageXml(const char* szWksID,const char* szImageID,
							   const char* szXmlTagName,BOOL bLock)
{
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN || 
		!szImageID || strlen(szImageID) != IMAGE_ID_LEN || 
		!szXmlTagName)
		return false;

	//�ж��Ƿ���Ȩ�޶Ըñ�ǩ��������
	DP_RIGHT	nRight	= DP_RIGHT_NOTRIGHT;
	if (!GetImageXmlRight(szImageID,szXmlTagName,&nRight) || nRight < DP_RIGHT_READWRITE)
		return false;

	//���жϵ�ǰ��ǩ�Ƿ��Ѿ��������û�����
	char		szOtherLocker[128];
	memset(szOtherLocker,0,sizeof(szOtherLocker));
	if (!this->GetImageXmlLockerName(szWksID,szImageID,szXmlTagName,szOtherLocker) || 
		(strlen(szOtherLocker) > 0 && _stricmp(szOtherLocker,m_szUserID) != 0))
		return false;

	//��ʼ����/����
	char		szSql[512];
	if (bLock)
		sprintf(szSql,"Insert Into DP_IMAGE_LOCKMGR Values('%s','%s','%s','%s')",
		        szWksID,szImageID,szXmlTagName,m_szUserID);
	else
		sprintf(szSql,"Delete From DP_IMAGE_LOCKMGR Where WORKSPACE_ID = '%s' AND IMAGE_ID = '%s' AND XML_TAG_NAME = '%s' AND LOCKER_NAME = '%s'",
		        szWksID,szImageID,szXmlTagName,m_szUserID);

	if (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		return true;
	else
		return false;

}

//����ĳ��ͼ����ĳ��ͼ��
//bLock -- TRUE:����,FALSE:����
BOOL CDpRightMgr::LockUnitLayer(const char* szWksID,const char* szUnitID,
								const char* szLayerID,BOOL bLock)
{
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN || 
		!szUnitID  || !szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return false;

	//�ж��Ƿ���Ȩ�޶Ըñ�ǩ��������
	DP_RIGHT	nRight	= DP_RIGHT_NOTRIGHT;
	if (!GetUnitLayerRight(szUnitID,szLayerID,&nRight) || nRight < DP_RIGHT_READWRITE)
		return false;

	//���жϵ�ǰ��ǩ�Ƿ��Ѿ��������û�����
	char		szOtherLocker[128];
	memset(szOtherLocker,0,sizeof(szOtherLocker));
	if (!this->GetUnitLayerLockerName(szWksID,szUnitID,szLayerID,szOtherLocker) || 
		(strlen(szOtherLocker) > 0 && _stricmp(szOtherLocker,m_szUserID) != 0))
		return false;

	//��ʼ����/����
	char		szSql[512];
	if (bLock)
		sprintf(szSql,"Insert Into DP_UNIT_LOCKMGR Values('%s','%s','%s','%s')",
		        szWksID,szUnitID,szLayerID,m_szUserID);
	else
		sprintf(szSql,"Delete From DP_UNIT_LOCKMGR Where WORKSPACE_ID = '%s' AND UNIT_ID = '%s' AND LAYER_ID = '%s' AND LOCKER_NAME = '%s'",
		        szWksID,szUnitID,szLayerID,m_szUserID);

	if (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		return true;
	else
		return false;

}


//�õ�����ָ��Ӱ���ָ��XML��ǩ�������˵��û�����
//szRetLockerName -- ���������˵�����,���szRetLockerNameΪ���ַ������ʾ�ñ�ǩû�б�����
BOOL CDpRightMgr::GetImageXmlLockerName(const char* szWksID,const char* szImageID,
										const char* szXmlTagName,char* szRetLockerName)
{
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN || 
		!szImageID || strlen(szImageID) != IMAGE_ID_LEN || 
		!szXmlTagName || !szRetLockerName)
		return false;

	sprintf(szRetLockerName,"");

	char		szSql[512];
	sprintf(szSql,"Select LOCKER_NAME From DP_IMAGE_LOCKMGR WHERE WORKSPACE_ID = '%s' AND IMAGE_ID = '%s' AND XML_TAG_NAME = '%s'",
		    szWksID,szImageID,szXmlTagName);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
				strcpy(szRetLockerName,(LPCTSTR)var);
		}
		return true;
	}

	return false;
}

//�õ�����ָ��ͼ����ָ��ͼ��������˵��û�����
//szRetLockerName -- ���������˵��������szRetLockerNameΪ���ַ������ʾ��ͼ����ͼ��û�б�����
BOOL CDpRightMgr::GetUnitLayerLockerName(const char* szWksID,const char* szUnitID,
										const char* szLayerID,char* szRetLockerName)
{
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN || 
		!szUnitID  || !szLayerID || strlen(szLayerID) != LAYER_ID_LEN || !szRetLockerName)
		return false;

	sprintf(szRetLockerName,"");
	
	char		szSql[512];
	sprintf(szSql,"Select LOCKER_NAME From DP_UNIT_LOCKMGR WHERE WORKSPACE_ID = '%s' AND UNIT_ID = '%s' AND LAYER_ID = '%s'",
		    szWksID,szUnitID,szLayerID);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
				strcpy(szRetLockerName,(LPCTSTR)var);
		}
		return true;
	}

	return false;

}

//���������ڴ򿪿ռ��ѯ��Feature��ѯʱ(���FeatureClass,GeoQuery��)�����Ȩ�޹������
BOOL CDpRightMgr::MakeRightFilterClauseWhenOpenQuery(const char* szWksID,const char* szLayerID,char* szRetClause)
{
	if (!szWksID || strlen(szWksID) != WKS_ID_LEN || !szLayerID || strlen(szLayerID) != LAYER_ID_LEN || 
		!szRetClause)
		return false;

	sprintf(szRetClause,"");

	//�жϵ�ǰ�û��Ƿ���ϵͳ����Ա�������ݿ��Ǳ������ݿ�
	if (m_iUserType == DP_ADMIN || m_pDB->GetDBType() == DP_ACCESS)
		return true;

	//�ж���û��ȫ��Ȩ��
	const _Wks_AllRead_Right* pAllRight = FindAllReadByWksID(szWksID);
	if (pAllRight && pAllRight->m_bIsCanReadAllUnitLayer)
		return true;

	sprintf(szRetClause," ST_GRID_ID in (SELECT GRID_ID FROM DP_V_USERRIGHT_UNIT_LAYER WHERE WORKSPACE_ID = '%s' AND USER_ID = '%s' AND LAYER_ID = '%s' AND MRIGHT >= 1)",
		    szWksID,m_szUserID,szLayerID);

	return true;
}


//�ҵ�ָ����������ȫ��Ȩ��������
const CDpRightMgr::_Wks_AllRead_Right* CDpRightMgr::FindAllReadByWksID(const char* szWksID)
{
	if (!szWksID)
		return NULL;
	ULONGLONG	uID = _atoi64(szWksID);

//	WKSALLREADMAP::iterator i	= m_mapWksAllRead.find(uID);

	//���ҹ�����

	int		iCount	= m_mapWksAllRead.size();
	for (int m = 0; m < iCount; m++)
	{
		_Wks_AllRead_Right* pAll = m_mapWksAllRead[m];
		if (pAll->uWksID == uID)
			return pAll;
	}

	return NULL;
}



















/***********************************************
*              ������������                    *
***********************************************/

CDpWorkspace::CDpWorkspace(CDpDatabase* pDB)
{
	m_pDB			= pDB;
	m_szID			= NULL;
	m_szName		= NULL;
	m_nWksType		= DP_WKS_ANY;
	m_dblScale		= 0;
	m_szDefaultDir	= NULL;
	memset(m_szServerName,0,sizeof(m_szServerName));
	m_iMetricType	= 0;	
//	m_szPhysicalDir = NULL;
}

CDpWorkspace::~CDpWorkspace()
{
	if (m_szID)
	{
		delete [] m_szID;
		m_szID = NULL;
	}
	if (m_szName)
	{
		delete [] m_szName;
		m_szName = NULL;
	}
	if (m_szDesc)
	{
		delete [] m_szDesc;
		m_szDesc = NULL;
	}
	if (m_szCreater)
	{
		delete []  m_szCreater;
		m_szCreater = NULL;
	}
	if (m_szDefaultDir)
	{
		delete [] m_szDefaultDir;
		m_szDefaultDir = NULL;
	}	
/*
	if (m_szPhysicalDir)
	{
		delete [] m_szPhysicalDir;
		m_szPhysicalDir = NULL;
	}*/

}


//�õ��������Ŀռ䷶Χ
BOOL CDpWorkspace::GetWksBound(CDpRect* pBound)
{
	if (!pBound)
		return false;

	pBound->CopyRect(&m_rtBound);

	return true;
}

//�޸Ĺ������Ŀռ䷶Χ,ͬʱҲ�޸����ݿ��еĿռ䷶Χ
BOOL CDpWorkspace::SetWksBound(CDpRect* pBound)
{
	if (!pBound)
		return false;

	char		szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_WorkspaceMgr SET MINX = %f,MINY = %f,MAXX = %f,MAXY = %f Where WORKSPACE_ID = '%s'",
		    pBound->m_dMinX,pBound->m_dMinY,pBound->m_dMaxX,pBound->m_dMaxY,m_szID);

	if (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		m_rtBound.CopyRect(pBound);
		return true;
	}
	else
		return false;
}

//���ù������ı�����
BOOL CDpWorkspace::SetWksScale(double dScale)
{
	char		szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_WorkspaceMgr Set SCALE = %f Where WORKSPACE_ID = '%s'",dScale,m_szID);

	if (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		m_dblScale	= dScale;
		return true;
	}
	else
		return false;

		    


}


//�õ��������Ŀ��Ƶ�XML
BOOL CDpWorkspace::GetWksCtlrPointXML(CDpDBVariant& var)
{
	char				szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select CONTROL_POINT From DP_WorkspaceMgr Where WORKSPACE_ID = '%s'",m_szID);

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
				return true;
		}	
	}

	return false;
}

//�õ��������Ŀ��Ƶ�XML,ppBuff���ڴ���Ҫ�������ͷ�
BOOL CDpWorkspace::GetWksCtlrPointXML(BYTE** ppBuff,int* piBuffSize)
{
	if (!ppBuff || !piBuffSize)
		return false;

	*piBuffSize		= 0;

	CDpDBVariant		var;
	if (GetWksCtlrPointXML(var))
	{
		if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary->m_dwSize > 0)  	
		{
			*piBuffSize = var.m_pLongBinary->m_dwSize;
			*ppBuff = new BYTE[*piBuffSize];
			memcpy(*ppBuff,var.m_pLongBinary->GetBuffer(),*piBuffSize);
		}
		return true;
	}
	else
		return false;
}



//�õ�������������ļ�����
BOOL CDpWorkspace::GetWksCameraData(CDpDBVariant& var)
{
	char				szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select CAMERA_DATA From DP_WorkspaceMgr Where WORKSPACE_ID = '%s'",m_szID);

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
				return true;
		}	
	}

	return false;

}


//�õ��������Ŀ��Ƶ�XML,ppBuff���ڴ���Ҫ�������ͷ�
BOOL CDpWorkspace::GetWksCameraData(BYTE** ppBuff,int* piBuffSize)
{
	if (!ppBuff || !piBuffSize)
		return false;

	*piBuffSize		= 0;

	CDpDBVariant		var;
	if (GetWksCameraData(var))
	{
		if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary->m_dwSize > 0)  	
		{
			*piBuffSize = var.m_pLongBinary->m_dwSize;
			*ppBuff = new BYTE[*piBuffSize];
			memcpy(*ppBuff,var.m_pLongBinary->GetBuffer(),*piBuffSize);
		}
		return true;
	}
	else
		return false;
}




//�������ݿ��п��Ƶ�XML�ֶε�����
//����˵��:  pBuff -- XML�����������ڴ�ָ��   iBuffSize -- ����������
BOOL CDpWorkspace::SetWksCtrlPointXML(BYTE* pBuff,int iBuffSize)
{
	char				szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_WorkspaceMgr Set CONTROL_POINT = ? Where WORKSPACE_ID = '%s'",m_szID);

	return SetLargerFieldValue(m_pDB->GetDBHandle(),szSql,pBuff,iBuffSize);
}

//���¹�����������ļ�����
//����˵��:  pBuff -- XML�����������ڴ�ָ��   iBuffSize -- ����������
BOOL CDpWorkspace::SetWksCameraData(BYTE* pBuff,int iBuffSize)
{
	char				szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_WorkspaceMgr Set CAMERA_DATA = ? Where WORKSPACE_ID = '%s'",m_szID);

	return SetLargerFieldValue(m_pDB->GetDBHandle(),szSql,pBuff,iBuffSize);

}



//�õ���������ͶӰ����ϵ����XML
BOOL CDpWorkspace::GetProjectXML(CDpDBVariant& var)
{
	char				szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select PROJECT_DATA From DP_WorkspaceMgr Where WORKSPACE_ID = '%s'",m_szID);

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
				return true;
		}	
	}

	return false;

}

//�õ��������Ŀ��Ƶ�XML,ppBuff���ڴ���Ҫ�������ͷ�
BOOL CDpWorkspace::GetProjectXML(BYTE** ppBuff,int* piBuffSize)
{
	if (!ppBuff || !piBuffSize)
		return false;

	*piBuffSize		= 0;

	CDpDBVariant		var;
	if (GetProjectXML(var))
	{
		if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary->m_dwSize > 0)  	
		{
			*piBuffSize = var.m_pLongBinary->m_dwSize;
			*ppBuff = new BYTE[*piBuffSize];
			memcpy(*ppBuff,var.m_pLongBinary->GetBuffer(),*piBuffSize);
		}
		return true;
	}
	else
		return false;
}


//�������ݿ��е�ͶӰ����ϵ����XML
//����˵��:  pBuff -- XML�����������ڴ�ָ��   iBuffSize -- ����������
BOOL CDpWorkspace::SetProjectXML(BYTE* pBuff,int iBuffSize)
{
	char				szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_WorkspaceMgr Set CONTROL_POINT = ? Where WORKSPACE_ID = '%s'",m_szID);

	return SetLargerFieldValue(m_pDB->GetDBHandle(),szSql,pBuff,iBuffSize);

}


//�õ���ǰ��������ǰ�û��������ID�б�
BOOL CDpWorkspace::GetTasksIDAndName(CDpStringArray& szIDsArray,CDpStringArray& szNamesArray)
{
	char	szSql[512];
	memset(szSql,0,sizeof(szSql));
	
	if (m_pDB->m_pRightMgr->GetUserType() == DP_ADMIN)
		sprintf(szSql,"Select TASK_ID,TASK_NAME From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s'",m_szID);
	else
		sprintf(szSql,"Select TASK_ID,TASK_NAME From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s' AND USER_ID = '%s'",
				m_szID,m_pDB->m_szUserID);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			CDpDBVariant	var1;

			if (rst.GetFieldValue(1,var) && 
				rst.GetFieldValue(2,var1))
			{
				szIDsArray.AddString((char*)(LPCTSTR)var);
				szNamesArray.AddString((char*)(LPCTSTR)var1);
			}
		}
		rst.Close();
		return true;
	}
	else
		return false;
}


//�õ���ӦID���������ppMgr��Ҫ�������ͷ�
BOOL CDpWorkspace::GetTask(const char* szID,CDpTaskMgr** ppMgr)
{
	if (!szID || strlen(szID) < TASK_ID_LEN)
		return false;
	
	*ppMgr	= new CDpTaskMgr(this);
	if ((*ppMgr)->OpenTask(szID))
		return true;
	else
	{
		delete (*ppMgr);
		*ppMgr	= NULL;
		return false;
	}

}


//�õ���ӦID�Ͷ�Ӧ�û���������
BOOL CDpWorkspace::GetTask(CDpTaskSet** ppTaskSet,const char* szUserID,
						   const _DpTaskFilter* pFilter)
{
	if (!ppTaskSet || !szUserID)
		return false;

	*ppTaskSet	= new CDpTaskSet(this,szUserID);

/*
	char		szTaskFilter[512];
	memset(szTaskFilter,0,sizeof(szTaskFilter));

	if (pFilter)
	{
		//A
		if (pFilter->AFilter != DP_A_IMAGE)
			sprintf(szTaskFilter,"WBS_A = %d",pFilter->AFilter);
		
		char		szTmp[256];
		memset(szTmp,0,sizeof(szTmp));

		//B
		if (pFilter->BFilter && strlen(pFilter->BFilter) > 0)
		{
			sprintf(szTmp,"WBS_B = '%s'",pFilter->BFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}

		//C
		if (pFilter->CFilter && strlen(pFilter->CFilter))
		{
			sprintf(szTmp,"WBS_C = '%s'",pFilter->CFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}
		
		//D
		if (pFilter->DFilter != 0)
		{
			sprintf(szTmp,"WBS_D = %d",pFilter->DFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}

	}


	char*	pszFilter	= NULL;
	if (strlen(szTaskFilter) > 0)
		pszFilter = szTaskFilter;
*/

	if (!((*ppTaskSet)->GetDataForDB(pFilter)))
	{
		delete *ppTaskSet;
		*ppTaskSet = NULL;
		return false;
	}
	else
		return true;

}

//�õ���ӦID���������,
//nTaskSetArray--���񼯵�����,szUserID--�û���,���Ϊ��,��ȡ���е��û�������
BOOL CDpWorkspace::GetTask(CDpTaskSetArray& nTaskSetArray,const char* szUserID,
						   const _DpTaskFilter* pFilter)
{
	if (!szUserID)						//��ʾȡ��ǰ����������������
	{
		char		szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Select Distinct USER_ID From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s'",m_szID);
		
		CDpRecordset	rst(m_pDB);
		if (rst.Open(szSql))
		{
			while (rst.Next())
			{
				CDpDBVariant	var;
				if (rst.GetFieldValue(1,var))
				{
					CDpTaskSet* pSet = NULL;
					if (GetTask(&pSet,(char*)(LPCTSTR)var,pFilter) && pSet)
						nTaskSetArray.AddTaskSet(pSet);
					
				}

			}
			rst.Close();
		}

	}
	else
	{
		CDpTaskSet* pSet = NULL;
		if (GetTask(&pSet,szUserID,pFilter) && pSet)
			nTaskSetArray.AddTaskSet(pSet);
	}

	return true;
}



//ɾ��ĳ������
BOOL CDpWorkspace::DeleteTask(const char* szID)
{
	if (!szID || strlen(szID) < TASK_ID_LEN)
		return false;

	//ֻ��ϵͳ����Ա���ܴ���ԭʼӰ��
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

/*
	//�жϵ�ǰ�û���û��ɾ���������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ���ɾ������
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;
*/
	
	char	szSql[128];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASKMGR_MAIN Where TASK_ID = '%s'",szID);

	return (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}


//����һ������,[in]szName-Ϊ���������,����Ϊ��[in]A,B,C,D�ֱ�ΪWBS CODE ���ĸ�����,
//[out]szNewIDΪ�����ɹ�����µ������ID,�������Ҫ������ΪNULL 
//����������:DP_WKS_FULL,DP_WKS_PART,DP_WKS_ANY,DP_WKS_IMAGEPRO
//A --ȡֵ��:DP_A_NULL,DP_A_IMAGE,DP_A_UNIT,DP_A_STEREO
//C --ȡֵ��:IOP,AOP,IMAGEPOINT,DEM,DOM,DLG,DRG,DVS
//D --ȡֵ����:����������+�ڵ�A������+�ڵ�C������+�ڵ�D��������ϳɵ��ַ���,û�й̶�����ֵ,
//             ����������ȡֵ:{1,2,3,4},         ���庬��:DP_WKS_FULL= 1,DP_WKS_PART= 2,DP_WKS_ANY= 3,DP_WKS_IMAGEPRO= 4
//             �ڵ�A����ȡֵ:{0,1,2,3},          ���庬��:DP_A_NULL= 0,DP_A_IMAGE= 1,DP_A_UNIT= 2,DP_A_STEREO= 3 /
//             �ڵ�C����ȡֵ:{0,1,2,3,4,5,6,7,8},���庬��:DP_C_NULL= 0,DP_C_IOP= 11,DP_C_AOP= 12,DP_C_IMAGEPOINT= 13,
//                                                        DP_C_DEM= 21,DP_C_DOM= 22,DP_C_DLG= 23,DP_C_DRG= 24,DP_C_DVS= 25
//             �ڵ�D����ȡֵ:{10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33},���庬��:�μ�enum DpDPartType
BOOL CDpWorkspace::CreateTask(const char* szName,DpAPartType A,const char* B,const char* C,int D,
							  const char* szStartTime,const char* szEndTime,char* szNewID)
{
	if (!szName || strlen(szName) <= 0 || !B || strlen(B) <= 0 || 
		!C || strlen(C) <= 0)
		return true;

	if (szNewID)
		*szNewID = 0;

	//ֻ��ϵͳ����Ա���ܴ�������
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

/*
	//�жϵ�ǰ�û���û�д��������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܴ�������
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;
*/
	char		szSql[1024];
	memset(szSql,0,sizeof(szSql));
	
	//�ȼ��������ID
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select Max(cint(mid(TASK_ID,15,5))) as MaxID From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select NVL(MAX(TO_NUMBER(SUBSTR(TASK_ID,15,5))),0) as MaxID From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select ISNULL(Max(CAST(SUBSTRING(TASK_ID,15,5) AS int)),0) as MaxID From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s'",
			    m_szID);
	else
		return false;

	int iMaxID = -1;

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant var;
			if (rst.GetFieldValue(1,var))
				iMaxID = (int)var;
		}
	}

	if (iMaxID == -1 || iMaxID > MAX_TASK_ID)
		return false;

	iMaxID++;

	//��������ID
	char		szNewTaskID[30];
	memset(szNewTaskID,0,sizeof(szNewTaskID));
	sprintf(szNewTaskID,"%s%05d",m_szID,iMaxID);

	//�������ݿ�
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Insert Into DP_TASKMGR_MAIN Values('%s','%s','%s','%s',%d,'%s','%s',%d,'%s','%s',%d,%d,'%s')",
			m_szID,m_pDB->m_szUserID,szNewTaskID,szName,A,B,C,D,szStartTime?szStartTime:"",
			szEndTime?szEndTime:"",0,0,"");
	if (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		if (szNewID)
			strcpy(szNewID,szNewTaskID);
		return true;
	}	
	else
		return false;
}

//����������ĳ���û�,���szUserIDΪNULLʱ,��ʾɾ��������ķ���,�ø�����ԭ��Ϊ�������״̬,
//                     ��������������Ѿ�������(��������ɰٷֱȴ���0),��Ϊ����ʧ��
BOOL CDpWorkspace::AssignTaskToUser(const char* szTaskID,const char* szUserID)
{
	if (!szTaskID)
		return false;

	//�жϵ�ǰ�û���û�з��������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܷ�������
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;


	char			szSql[256];

	//��������
	if (szUserID && strlen(szUserID) > 0)
	{
		sprintf(szSql,"Update DP_TASKMGR_MAIN Set USER_ID = '%s' Where WORKSPACE_ID = '%s' AND TASK_ID = '%s'",
				szUserID,m_szID,szTaskID);

		if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
			return false;


		//�ٷ���д��Ȩ�޸����û�
		CDpUserMgr		mgr(this);

		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Select WBS_A,WBS_B,WBS_C From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s' AND TASK_ID = '%s'",
				m_szID,szTaskID);

		CDpRecordset	rst(m_pDB);
		if (rst.Open(szSql))
		{
			if (rst.Next())
			{
				CDpDBVariant	var;
				CDpDBVariant	var1;
				CDpDBVariant	var2;

				if (rst.GetFieldValue(1,var) && 
					rst.GetFieldValue(2,var1) && 
					rst.GetFieldValue(3,var2))
				{
					DpAPartType		a = DpAPartType((int)var);
					if (a == DP_A_IMAGE)
						mgr.AddImageXmlRightToUser(szUserID,(char*)(LPCTSTR)var1,(char*)(LPCTSTR)var2,DP_RIGHT_READWRITE);
					else if (a == DP_A_UNIT||a == DP_A_STEREO)
						mgr.AddUnitLayerRightToUser(szUserID,(char*)(LPCTSTR)var1,(char*)(LPCTSTR)var2,DP_RIGHT_READWRITE);					

				}
			}
			rst.Close();
		}
		
		return true;
	}
	else									//ɾ���������
	{
		//�ж�����Ľ���
		CDpTaskMgr*	pMgr	= NULL;
		if (!GetTask(szTaskID,&pMgr) || !pMgr)
			return false;

		if (pMgr->m_WBSCode.iPercent > 0)
			return false;

		sprintf(szSql,"Update DP_TASKMGR_MAIN Set USER_ID = '' Where WORKSPACE_ID = '%s' AND TASK_ID = '%s'",
				m_szID,szTaskID);

		if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
			return false;

		//����Ȩ��
		CDpUserMgr		mgr(this);
		switch(pMgr->m_WBSCode.A)
		{
		case DP_A_IMAGE:
			mgr.DeleteImageXmlRightFromUser(szUserID,pMgr->m_WBSCode.B,pMgr->m_WBSCode.C,DP_RIGHT_READWRITE);
		case DP_A_UNIT:
		case DP_A_STEREO:
			mgr.DeleteUnitLayerRightFromUser(szUserID,pMgr->m_WBSCode.B,pMgr->m_WBSCode.C,DP_RIGHT_READWRITE);
		default:
			break;
		}
		
		return true;
	}
}



//��ʱ��ӵ�һ������,���¸��汾��������
//�ýӿڵ��ô��ǵ�����������ǻ���DLG������ʱ,Ϊ�Զ��Ľ��������е�B(������ͼ��)�µ�����DLGͼ���
//дȨ�޶������szUserID����ʾ���û�	
BOOL CDpWorkspace::AssignTaskToUserTmp(const char* szTaskID,const char* szUserID,
									   BOOL bDlgOnly)
{
	if (!szTaskID)
		return false;

	//�жϵ�ǰ�û���û�з��������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܷ�������
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;


	char			szSql[256];

	//��������
	if (szUserID && strlen(szUserID) > 0)
	{
		sprintf(szSql,"Update DP_TASKMGR_MAIN Set USER_ID = '%s' Where WORKSPACE_ID = '%s' AND TASK_ID = '%s'",
				szUserID,m_szID,szTaskID);

		if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
			return false;


		//�ٷ���д��Ȩ�޸����û�
		CDpUserMgr		mgr(this);

		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Select WBS_A,WBS_B,WBS_C From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s' AND TASK_ID = '%s'",
				m_szID,szTaskID);

		CDpRecordset	rst(m_pDB);
		if (rst.Open(szSql))
		{
			if (rst.Next())
			{
				CDpDBVariant	var;
				CDpDBVariant	var1;
				CDpDBVariant	var2;

				if (rst.GetFieldValue(1,var) && 
					rst.GetFieldValue(2,var1) && 
					rst.GetFieldValue(3,var2))
				{
					DpAPartType		a = DpAPartType((int)var);
					if (a == DP_A_IMAGE)
						mgr.AddImageXmlRightToUser(szUserID,(char*)(LPCTSTR)var1,(char*)(LPCTSTR)var2,DP_RIGHT_READWRITE);
					else if (a == DP_A_UNIT||a == DP_A_STEREO)
					{
						//�ȵõ�ͼ��ID
						char		szUnitID[UNIT_ID_LEN+1];
						memset(szUnitID,0,sizeof(szUnitID));
						strcpy(szUnitID,(LPCTSTR)var1);

						//ö�ٸ�ͼ���µ�ͼ��
						CDpStringArray		idArray;
						CDpStringArray		nameArray;
						BOOL				bRet				= false;

						if (bDlgOnly)
							bRet = GetLayersIDAndName(idArray,nameArray,GEO_NULL,LAYER_TYPE_EX_DLG);
						else
							bRet = GetLayersIDAndName(idArray,nameArray);

						if (bRet)
						{
							int		iCount = idArray.GetCount();

							for (int i = 0; i < iCount; i++)
								mgr.AddUnitLayerRightToUser(szUserID,szUnitID,idArray.GetItem(i),DP_RIGHT_READWRITE);
						}
					}

				}
			}
			rst.Close();
		}
		
		return true;
	}
	else									//ɾ���������
	{
		//�ж�����Ľ���
		CDpTaskMgr*	pMgr	= NULL;
		if (!GetTask(szTaskID,&pMgr) || !pMgr)
			return false;

		if (pMgr->m_WBSCode.iPercent > 0)
			return false;

		sprintf(szSql,"Update DP_TASKMGR_MAIN Set USER_ID = '' Where WORKSPACE_ID = '%s' AND TASK_ID = '%s'",
				m_szID,szTaskID);

		if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
			return false;

		//����Ȩ��
		CDpUserMgr		mgr(this);
		if (pMgr->m_WBSCode.A	== DP_A_IMAGE)
			mgr.DeleteImageXmlRightFromUser(szUserID,pMgr->m_WBSCode.B,pMgr->m_WBSCode.C,DP_RIGHT_READWRITE);
		else
		{
			//�ȵõ�ͼ��ID
			//ö�ٸ�ͼ���µ�ͼ��
			CDpStringArray		idArray;
			CDpStringArray		nameArray;
			BOOL				bRet				= false;

			if (bDlgOnly)
				bRet = GetLayersIDAndName(idArray,nameArray,GEO_NULL,LAYER_TYPE_EX_DLG);
			else
				bRet = GetLayersIDAndName(idArray,nameArray);

			if (bRet)
			{
				int		iCount = idArray.GetCount();

				for (int i = 0; i < iCount; i++)
					mgr.DeleteUnitLayerRightFromUser(szUserID,pMgr->m_WBSCode.B,idArray.GetItem(i),DP_RIGHT_READWRITE);
			}

			
		}
			
		return true;
	}

}







//��һ������������ĳ���û�,���szUserIDΪNULLʱ,��ʾɾ��������ķ���,�ø�����ԭ��Ϊ�������״̬,
//                     ��������������Ѿ�������(��������ɰٷֱȴ���0),��Ϊ����ʧ��
BOOL CDpWorkspace::AssignTaskToUser(CDpTaskGroup* pGroup,const char* szUserID)
{
	if (!pGroup || !szUserID || strlen(szUserID))
		return false;

	//�жϵ�ǰ�û���û�з��������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܷ�������
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;

	int		iCount	= pGroup->GetTaskCount();

	m_pDB->BeginTrans();
	
	for (int i = 0; i < iCount; i++)
	{
		const CDpTaskGroup::_DpTaskInfo*	pInfo	= pGroup->GetTaskItem(i);
		if (pInfo)
		{
			if (!AssignTaskToUser(pInfo->szTaskID,szUserID))
				goto Err;
		}
	}

	m_pDB->CommitTrans();
	return true;

Err:
	m_pDB->Rollback();
	return false;
}

//����ͼ��,
//����˵��:[in]szName-ͼ������,[in]pRtBound-ͼ���ռ䷶Χ,���û�о���ΪNULL
//         [in]ptValiRegion-ͼ����Ч��Χ,Ӧ����һ���ĸ������������,���û�о���ΪNULL
//         [out]szUnitID-���ش����ɹ���ͼ����ID,�������Ҫ����ΪNULL
//         [in]iType-��ʶ��ģ�ͻ���ͼ��
//         [in]szRefer-�ο����ݣ���ģ��ʹ�ã�������ʶ����Ƭ
//         [in]szLocalName-��ʶͼ���ĵ���
//			houkui,06.6.19
BOOL CDpWorkspace::CreateUnit(const char* szName,CDpRect* pRtBound,CDpPoint* ptValiRegion,char* szUnitID,DpAPartType iType,char* szRefer ,	char* szLocalName )
//(const char* szName,CDpRect* pRtBound, CDpPoint* ptValiRegion,UINT iType,char* szUnitID = NULL,char* szRefer = NULL,	char* szLocalName = NULL)
{
	//ֻ��ϵͳ����Ա���ܴ���ͼ��
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

	if (!szName || strlen(szName) <= 0)
		return false;

	if(iType!=DP_A_UNIT&&iType!=DP_A_STEREO)		
		return false;

	if (szUnitID)
		*szUnitID = 0;
	
	//�ȼ���ͼ����ID
	char	szSql[1024];
	memset(szSql,0,sizeof(szSql));
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select Max(cint(mid(GRID_ID,16,4))) as MaxID From DP_GridInWks Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select NVL(MAX(TO_NUMBER(SUBSTR(GRID_ID,16,4))),0) as MaxID From DP_GridInWks Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select ISNULL(Max(CAST(SUBSTRING(GRID_ID,16,4) AS int)),0) as MaxID From DP_GridInWks Where WORKSPACE_ID = '%s'",
			    m_szID);
	else
		return false;

	int iMaxID = -1;

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant var;
			if (rst.GetFieldValue(1,var))
				iMaxID = (int)var;
		}
	}

	if (iMaxID == -1 || iMaxID > MAX_UINT_ID)
		return false;

	iMaxID++;

	//����ͼ��ID
	char		szNewUnitID[30];
	memset(szNewUnitID,0,sizeof(szNewUnitID));
	sprintf(szNewUnitID,"%s0%04d",m_szID,iMaxID);


	//�õ���ǰʱ��
	time_t		tmTmp;	
	time(&tmTmp);
	tm*			tmCur = localtime(&tmTmp);

	//����ʱ���SQL 
	char		szTimeSql[128];
	memset(szTimeSql,0,sizeof(szTimeSql));
	BuildDateTimeSql(m_pDB->GetDBType(),tmCur,szTimeSql);

	//����ͼ����SQL
	CDpRect		rt;
	CDpPoint	pt[4];
	if (pRtBound)
		rt.CopyRect(pRtBound);
	else
		rt.SetRect(0,0,0,0);

	if (ptValiRegion)
		memcpy(pt,ptValiRegion,4*sizeof(CDpPoint));
	else
		memset(pt,0,4*sizeof(CDpPoint));


	memset(szSql,0,sizeof(szSql));
	if(m_iVersion==2)
	{
		sprintf(szSql,"INSERT INTO DP_GRIDINWKS(WORKSPACE_ID,GRID_ID,GRID_NAME,MINX,MINY,MAXX,MAXY,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY,UPDATE_DATE,TYPE,REFER,LOCALNAME) VALUES('%s','%s','%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s,%d,'%s','%s')",
			m_szID,szNewUnitID,szName,rt.m_dMinX,rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,pt[0].m_dx,pt[0].m_dy,pt[1].m_dx,pt[1].m_dy,pt[2].m_dx,pt[2].m_dy,pt[3].m_dx,pt[3].m_dy,szTimeSql,(int)iType,szRefer,szLocalName);/*houkui,06.6.19*//*houki,06.6.19*/
		
	}
	else if(m_iVersion == 1||m_iVersion == 0)
	{
		sprintf(szSql,"INSERT INTO DP_GRIDINWKS(WORKSPACE_ID,GRID_ID,GRID_NAME,MINX,MINY,MAXX,MAXY,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY,UPDATE_DATE) VALUES('%s','%s','%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s)",
			m_szID,szNewUnitID,szName,rt.m_dMinX,rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,pt[0].m_dx,pt[0].m_dy,
			pt[1].m_dx,pt[1].m_dy,pt[2].m_dx,pt[2].m_dy,pt[3].m_dx,pt[3].m_dy,szTimeSql);
	}
	else
		return FALSE;


	//ִ��SQL 
	if (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		if (szUnitID)
			strcpy(szUnitID,szNewUnitID);
		return true;
	}
	else
		return false;

}

//�õ�ϵͳ�е�ͼ������Ŀ��houkui,06.6.26
int	CDpWorkspace::GetUnitCount(DP_LOCK_TYPE UnitType,DpAPartType ApartType)
{
	char			szSql[256];
	SQLINTEGER		iCount			= 0;			//ͼ����Ŀ
	SQLLEN		cbLen = 0;			//�ֶγ���
	SQLHANDLE		hStmt			= NULL;
	SQLRETURN		retcode;
	int				i				=1;              
	if(ApartType!=DP_A_UNIT&&ApartType!=DP_A_STEREO)
		return FALSE;

	i =	(int)(ApartType);			//ͼ��
		
	memset(szSql,0,sizeof(szSql));
	if(m_iVersion==2){
		if (UnitType == DP_UT_ANY)
			sprintf(szSql,"Select Count(GRID_ID) From DP_GridInWks where WORKSPACE_ID = '%s' AND TYPE= %d",m_szID,i);
		else if (UnitType == DP_UT_UNLOCK)
			sprintf(szSql,"Select Count(GRID_ID) From DP_GridInWks where WORKSPACE_ID = '%s' AND ISLOCK = 0 AND TYPE= %d",m_szID,i);
		else 
			sprintf(szSql,"Select Count(GRID_ID) From DP_GridInWks where WORKSPACE_ID = '%s' AND ISLOCK = 1 AND TYPE= %d",m_szID,i);
	}
	else if(m_iVersion==1 || m_iVersion == 0)
	{
		if (UnitType == DP_UT_ANY)
			sprintf(szSql,"Select Count(GRID_ID) From DP_GridInWks where WORKSPACE_ID = '%s' ",m_szID);
		else if (UnitType == DP_UT_UNLOCK)
			sprintf(szSql,"Select Count(GRID_ID) From DP_GridInWks where WORKSPACE_ID = '%s' AND ISLOCK = 0 ",m_szID);
		else 
			sprintf(szSql,"Select Count(GRID_ID) From DP_GridInWks where WORKSPACE_ID = '%s' AND ISLOCK = 1 ",m_szID);
	}
	else
		return FALSE;

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return 0;

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return 0;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			SQLGetData(hStmt,1,SQL_C_SHORT,&iCount,sizeof(iCount),&cbLen);			//ȡͼ����Ŀ
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	
	}

	
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return iCount;

}

//����ͼ��IDȡ��ͼ��,����ֵΪͼ�������ָ��,�ö�����ʹ�����,��Ҫ�ֹ��ͷ�
BOOL CDpWorkspace::GetUnit(const char* szUnitID,CDpUnit** ppUnit)	
{
	if (!szUnitID || !ppUnit)
		return false;
	*ppUnit	= NULL;

	//���жϸ��û��Ƿ��ж�ȡ��ͼ����Ȩ��
	DP_RIGHT iRight = DP_RIGHT_NOTRIGHT;
	if (!(m_pDB->m_pRightMgr->GetUnitRight(szUnitID,&iRight)) || 
		iRight == DP_RIGHT_NOTRIGHT)
		return false;


	char				szSql[1024];
	SQLCHAR				szName[50];						//ͼ������
	SQLCHAR				szParamTable[50];				//����������
	SQLCHAR				szLocker[50];					//����ͼ�����û�ID
	TIMESTAMP_STRUCT	UpdateDate;						//ͼ������ʱ��
	SQLFLOAT			fMinX				= 0;							
	SQLFLOAT			fMinY				= 0;
	SQLFLOAT			fMaxX				= 0;
	SQLFLOAT			fMaxY				= 0;
	SQLFLOAT			fLTX				= 0;
	SQLFLOAT			fLTY				= 0;
	SQLFLOAT			fLBX				= 0;
	SQLFLOAT			fLBY				= 0;
	SQLFLOAT			fRBX				= 0;
	SQLFLOAT			fRBY				= 0;
	SQLFLOAT			fRTX				= 0;
	SQLFLOAT			fRTY				= 0;
	SQLINTEGER			IsLock				= 0;
	SQLLEN			cbLen = 0;
	SQLINTEGER			iType				= (int)(DP_A_UNIT);/*��ʶ��ģ�ͻ���ͼ��,houki,06.6.19*/
	SQLCHAR				szRefer[50];			/*�ο����ݣ���ģ��ʹ�ã�������ʶ����Ƭ,houki,06.6.19*/
	SQLCHAR				szLocalName[50];		/*��ʶͼ���ĵ���,houki,06.6.19*/
	
	SQLRETURN			retcode;
	SQLHANDLE			hStmt				= NULL;
	BOOL				bRet				= false;

	memset(szSql,0,sizeof(szSql));
	memset(szName,0,sizeof(szName));
	memset(szParamTable,0,sizeof(szParamTable));
	memset(szLocker,0,sizeof(szLocker));
	memset(szRefer,0,sizeof(szRefer));			/*houki,06.6.19*/
	memset(szLocalName,0,sizeof(szLocalName));	/*houki,06.6.19*/

	if(2 == m_iVersion)							//Ŀǰ�汾
	{		
		sprintf(szSql,"Select GRID_NAME,MINX,MINY,MAXX,MAXY,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY,UPDATE_DATE,ISLOCK,UNIT_PARAMTABLE,LOCK_NAME,TYPE,REFER,LOCALNAME FROM DP_GridInWks WHERE GRID_ID = '%s'",szUnitID);
			//houkui,06.6.19,����TYPE,REFER,LOCALNAME
	}
	else if(1 == m_iVersion || m_iVersion == 0)					//��ǰ�汾
	{
		sprintf(szSql,"Select GRID_NAME,MINX,MINY,MAXX,MAXY,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY,UPDATE_DATE,ISLOCK,UNIT_PARAMTABLE,LOCK_NAME FROM DP_GridInWks WHERE GRID_ID = '%s'",szUnitID);
	}
	else
		return FALSE;

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return false;

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		//�����ݿ��ֶ�
		SQLBindCol(hStmt,1,SQL_C_CHAR,szName,sizeof(szName),&cbLen);
		SQLBindCol(hStmt,2,SQL_C_DOUBLE,&fMinX,sizeof(fMinX),&cbLen);
		SQLBindCol(hStmt,3,SQL_C_DOUBLE,&fMinY,sizeof(fMinY),&cbLen);
		SQLBindCol(hStmt,4,SQL_C_DOUBLE,&fMaxX,sizeof(fMaxX),&cbLen);
		SQLBindCol(hStmt,5,SQL_C_DOUBLE,&fMaxY,sizeof(fMaxY),&cbLen);
		SQLBindCol(hStmt,6,SQL_C_DOUBLE,&fLTX,sizeof(fLTX),&cbLen);
		SQLBindCol(hStmt,7,SQL_C_DOUBLE,&fLTY,sizeof(fLTY),&cbLen);
		SQLBindCol(hStmt,8,SQL_C_DOUBLE,&fLBX,sizeof(fLBX),&cbLen);
		SQLBindCol(hStmt,9,SQL_C_DOUBLE,&fLBY,sizeof(fLBY),&cbLen);
		SQLBindCol(hStmt,10,SQL_C_DOUBLE,&fRBX,sizeof(fRBX),&cbLen);
		SQLBindCol(hStmt,11,SQL_C_DOUBLE,&fRBY,sizeof(fRBY),&cbLen);
		SQLBindCol(hStmt,12,SQL_C_DOUBLE,&fRTX,sizeof(fRTX),&cbLen);
		SQLBindCol(hStmt,13,SQL_C_DOUBLE,&fRTY,sizeof(fRTY),&cbLen);
		SQLBindCol(hStmt,14,SQL_C_TYPE_TIMESTAMP,&UpdateDate,sizeof(UpdateDate),&cbLen);
		SQLBindCol(hStmt,15,SQL_C_SHORT,&IsLock,sizeof(IsLock),&cbLen);
		SQLBindCol(hStmt,16,SQL_C_CHAR,szParamTable,sizeof(szParamTable),&cbLen);
		SQLBindCol(hStmt,17,SQL_C_CHAR,szLocker,sizeof(szLocker),&cbLen);
		if(2==m_iVersion)
		{			
			SQLBindCol(hStmt,18,SQL_C_SHORT,&iType,sizeof(iType),&cbLen);//houkui,06.6.19
			SQLBindCol(hStmt,19,SQL_C_CHAR,szRefer,sizeof(szRefer),&cbLen);//houkui,06.6.19
			SQLBindCol(hStmt,20,SQL_C_CHAR,szLocalName,sizeof(szLocalName),&cbLen);//houkui,06.6.19
		}
		retcode = SQLFetch(hStmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{

			*ppUnit	= new CDpUnit(this);
		
			//����ID
			CopyStr(&((*ppUnit)->m_szID),(char*)szUnitID);	
			//��������
			CopyStr(&((*ppUnit)->m_szUnitName),(char*)szName);	
			//���Ʋ���������
			CopyStr(&((*ppUnit)->m_szParamTableName),(char*)szParamTable);	
			//��������ͼ�����û���ID
			CopyStr(&((*ppUnit)->m_szLocker),(char*)szLocker);	
			//ͼ��������״̬
			(*ppUnit)->m_iUnitStatus	= DP_LOCK_TYPE(IsLock);
			//ͼ���ĸ���ʱ��
			(*ppUnit)->m_tmUpdateDate	= GetTimeFromDBStruct(&UpdateDate);
			//ͼ���ľ�������
			(*ppUnit)->m_rtBound.SetRect(fMinX,fMinY,fMaxX,fMaxY);
			//ͼ�����ı�����Ч����
			(*ppUnit)->m_ptQuadrangle[0].m_dx	= fLTX;
			(*ppUnit)->m_ptQuadrangle[0].m_dy	= fLTY;
			(*ppUnit)->m_ptQuadrangle[1].m_dx	= fLBX;
			(*ppUnit)->m_ptQuadrangle[1].m_dy	= fLBY;
			(*ppUnit)->m_ptQuadrangle[2].m_dx	= fRBX;
			(*ppUnit)->m_ptQuadrangle[2].m_dy	= fRBY;
			(*ppUnit)->m_ptQuadrangle[3].m_dx	= fRTX;
			(*ppUnit)->m_ptQuadrangle[3].m_dy	= fRTY;

			//ͼ���ı�ʶ��ģ�ͻ���ͼ��---type
			(*ppUnit)->m_Type	= (DpAPartType)(iType);
			//ͼ���Ĳο����ݣ���ģ��ʹ�ã�������ʶ����Ƭ--refer
			CopyStr(&((*ppUnit)->m_szRefer),(char*)szRefer);
			//ͼ���ĵ���---localname
			CopyStr(&((*ppUnit)->m_szLocalName),(char*)szLocalName);	

			bRet =  true;
		}
		else
			bRet =  false;
	}
	else
		bRet = false;

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return bRet;
}

//�õ��������е�Ӱ�����Ŀ
int	CDpWorkspace::GetImageCount(enum DP_LOCK_TYPE UnitType)
{
	char			szSql[256];
	SQLINTEGER		iCount			= 0;			//ͼ����Ŀ

	memset(szSql,0,sizeof(szSql));
	if (UnitType == DP_UT_ANY)
		sprintf(szSql,"Select Count(IMAGE_ID) From DP_IMAGEMGR where WORKSPACE_ID = '%s'",m_szID);
	else if (UnitType == DP_UT_UNLOCK)
		sprintf(szSql,"Select Count(IMAGE_ID) From DP_IMAGEMGR where WORKSPACE_ID = '%s' AND ISLOCK = 0",m_szID);
	else 
		sprintf(szSql,"Select Count(IMAGE_ID) From DP_IMAGEMGR where WORKSPACE_ID = '%s' AND ISLOCK = 1",m_szID);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
			{
				iCount = (int)var;
				return iCount;
			}
		}
	}

	return 0;
}


//���ݿռ䷶Χȡ����������Ӱ���ID
//szIDArray������ż����Ľ��
//rtBoundΪҪ�����ķ�Χ,Ϊ�����ʾ�����ǿռ�λ��
//bIntersect��ʾ��������,
//ΪTRUE��ʾֻҪ���ཻ��ͼ���͵õ�,ΪFALSE���ʾ��Ҫ��ȫ�����ŵõ�
//bIsLocked��ʾ�Ƿ񷵻ر�������ͼ��,ΪTRUE��ʾ����
BOOL CDpWorkspace::GetImagesID(CDpStringArray& szIDArray,CDpRect* pRect,
							   BOOL bIntersect,enum DP_LOCK_TYPE UnitType)
{
	char		szSql[512];						//sql��ѯ���
	memset(szSql,0,sizeof(szSql));

	//������SQL���
	if (UnitType != DP_UT_ANY)				//���Ƿ������е�ͼ��				
		sprintf(szSql,"Select IMAGE_ID,MINX,MINY,MAXX,MAXY,RTX,RTY,LBX,LBY From DP_IMAGEMGR Where WORKSPACE_ID = '%s' AND ISLOCK = %d",
		        m_szID,UnitType == DP_UT_UNLOCK?1:2);
	else
		sprintf(szSql,"Select IMAGE_ID,MINX,MINY,MAXX,MAXY,RTX,RTY,LBX,LBY From DP_IMAGEMGR Where WORKSPACE_ID = '%s'",m_szID);

	//��ʼ�������ݿ�
	CDpRecordset	rst(m_pDB);
	CDpDBVariant	varID;
	CDpDBVariant	varMinX;
	CDpDBVariant	varMinY;
	CDpDBVariant	varMaxX;
	CDpDBVariant	varMaxY;
	CDpDBVariant	varRTX;
	CDpDBVariant	varRTY;
	CDpDBVariant	varLBX;
	CDpDBVariant	varLBY;


	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			if (rst.GetFieldValue(1,varID) && rst.GetFieldValue(2,varMinX) && 
				rst.GetFieldValue(3,varMinY) && rst.GetFieldValue(4,varMaxX) && 
				rst.GetFieldValue(5,varMaxY) && rst.GetFieldValue(6,varRTX) && 
				rst.GetFieldValue(7,varRTY) && rst.GetFieldValue(8,varLBX) && 
				rst.GetFieldValue(9,varLBY)) 

			{
				CDpRgn			rgn;
				_DPRealPoint	d[4];
				d[0].x	= (double)varMinX;		d[0].y	= (double)varMinY;
				d[1].x	= (double)varRTX;		d[1].y	= (double)varRTY;
				d[2].x	= (double)varMaxX;		d[2].y	= (double)varMaxY;
				d[3].x	= (double)varLBX;		d[3].y	= (double)varLBY;
				rgn.CreatePolygonRgn(d,4);

				if (pRect)						//���пռ䷶Χ����
				{
					if (bIntersect)				//�ཻ
					{

						if (rgn.Intersect(pRect))
							szIDArray.AddString((char*)(LPCSTR)varID);
					}
					else						//����
					{
						if (pRect->PtInRect(d[0].x,d[0].y) &&
							pRect->PtInRect(d[1].x,d[1].y) &&
							pRect->PtInRect(d[2].x,d[2].y) &&
							pRect->PtInRect(d[3].x,d[3].y))
								szIDArray.AddString((char*)(LPCSTR)varID);


					}

				}
				else
					szIDArray.AddString((char*)(LPCSTR)varID);					//����������ID���뵽���������
			}

		}

		return true;
	}
	else
		return false;


}



//�õ���ǰ��������Ӱ��ĺ����б�
BOOL CDpWorkspace::GetStrips(CDpStringArray& szStripArray)
{
	char		szSql[256];
	sprintf(szSql,"Select Distinct STRIP From DP_IMAGEMGR Where WORKSPACE_ID = '%s' Order by STRIP",m_szID);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
			{
				szStripArray.AddString((char*)(LPCTSTR)var);
			}
		}

	}
	return true;
}



//���ݺ������Ƶõ��ú����ڵ�����Ӱ��
BOOL CDpWorkspace::GetImagesIDInStrip(CDpStringArray& szIDArray,const char* szStrip)
{
	if (!szStrip || strlen(szStrip) <= 0)
		return false;

	char		szSql[512];						//sql��ѯ���
	memset(szSql,0,sizeof(szSql));

	sprintf(szSql,"Select IMAGE_ID From DP_IMAGEMGR Where WORKSPACE_ID = '%s' AND STRIP = '%s'",m_szID,szStrip);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var) && strlen((LPCTSTR)var) == IMAGE_ID_LEN)
				szIDArray.AddString((char*)(LPCTSTR)var);		

		}
		return true;
	}

	return false;
}


//����Ӱ��IDȡ��Ӱ�����,ppImage����Ӱ������ָ��,�ö�����ʹ�����,��Ҫ�ֹ��ͷ�
BOOL CDpWorkspace::GetImage(const char* szImageID,CDpImage** ppImage)
{
	if (!szImageID || strlen(szImageID) != IMAGE_ID_LEN || !ppImage)
		return false;

	*ppImage	= NULL;

	//���жϸ��û��Ƿ��ж�ȡ��ͼ����Ȩ��
	DP_RIGHT iRight = DP_RIGHT_NOTRIGHT;
	if (!(m_pDB->m_pRightMgr->GetImageRight(szImageID,&iRight)) || 
		iRight == DP_RIGHT_NOTRIGHT)
		return false;

	char		szSql[512];
	memset(szSql,0,sizeof(szSql));

	sprintf(szSql,"Select * From DP_IMAGEMGR where IMAGE_ID = '%s'",szImageID);

	CDpRecordset	rst(m_pDB);
	CDpDBVariant	var;
	
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			*ppImage		= new CDpImage(this);
			CopyStr(&((*ppImage)->m_szID),szImageID);
			
			//��¼���ݷ��������֣�������ȡ����,modified by houkui,2006.7.4
			rst.GetFieldValue("SERVER_NAME",var);
			CopyStr(&((*ppImage)->m_szServerName),(LPCTSTR)var);

			//Ӱ����
			rst.GetFieldValue("IMAGE_NAME",var);
			CopyStr(&((*ppImage)->m_szImageName),(LPCTSTR)var);
			//Ӱ������
			rst.GetFieldValue("IMAGE_DESC",var);
			CopyStr(&((*ppImage)->m_szImageDesc),(LPCTSTR)var);
			//������Ϣ
			rst.GetFieldValue("STRIP",var);
			CopyStr(&((*ppImage)->m_szStrip),(LPCTSTR)var);
			//Ӱ���ں����е�������
			rst.GetFieldValue("INDEX_IN_STRIP",var);
			(*ppImage)->m_iIndexInStrip	= (int)var;
			//�ռ䷶Χ
			_DPRealPoint d[4];
			rst.GetFieldValue("MINX",var);	d[0].x = (double)var;
			rst.GetFieldValue("MINY",var);	d[0].y = (double)var;
			rst.GetFieldValue("MAXX",var);	d[2].x = (double)var;
			rst.GetFieldValue("MAXY",var);	d[2].y = (double)var;
//			(*ppImage)->m_rtBound.SetRect(d1,d2,d3,d4);
			//�ļ�·��
			rst.GetFieldValue("FILE_PATH",var);
			CopyStr(&((*ppImage)->m_szFilePath),(LPCTSTR)var);
			//����ʱ��
			rst.GetFieldValue("UPDATE_DATE",var);
			(*ppImage)->m_tmUpdateDate = GetTimeFromDBStruct(var.m_pdate);
			//����״̬
			rst.GetFieldValue("ISLOCK",var);
			(*ppImage)->m_iImageStatus	= DP_LOCK_TYPE((int)var);
			//����������
			rst.GetFieldValue("LOCKER_NAME",var);
			CopyStr(&((*ppImage)->m_szLocker),(LPCTSTR)var);
			//����������
			rst.GetFieldValue("PARAMTABLE_NAME",var);
			CopyStr(&((*ppImage)->m_szParamTableName),(LPCTSTR)var);
			//����������
			rst.GetFieldValue("RTX",var);	d[1].x = (double)var;
			rst.GetFieldValue("RTY",var);	d[1].y = (double)var;
			rst.GetFieldValue("LBX",var);	d[3].x = (double)var;
			rst.GetFieldValue("LBY",var);	d[3].y = (double)var;

			(*ppImage)->m_nRgn.CreatePolygonRgn(d,4);

			return true;
		}
		rst.Close();
	}

	return false;
}


//�ڹ������д���ԭʼӰ��,[in]szName-Ӱ������,ʵ�ʾ���Ӱ����ļ���(������չ��)
//                       [in]szStrip-��������
//                       [in]iIndexInStrip-Ӱ���ں����е�����
//			             [out]szFilePath-Ӱ������ļ���    [in]pRtBound-Ӱ��ռ䷶Χ 
//                       [out]szImageID-���ش����ɹ���Ӱ��ID,ʧ���򷵻ؿ��ִ�,�������Ҫ������ΪNULL
BOOL CDpWorkspace::CreateImage(const char* szName,const char* szStrip,UINT iIndexInStrip,
							   char* szFilePath,CDpRect* pRtBound,
							   char* szImageID)
{
	if (!szName || strlen(szName) <= 0 || !szStrip || strlen(szStrip) <= 0) 
		return false;

	//ֻ��ϵͳ����Ա���ܴ���ԭʼӰ��
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

	if (szFilePath)
		*szFilePath = 0;


/*
	//�жϵ�ǰ�û���û�д���Ӱ���Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܴ���Ӱ��
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;
*/

	if (szImageID)
		*szImageID = 0;
	
	//�ȼ���Ӱ���ID
	char	szSql[1024];
	memset(szSql,0,sizeof(szSql));
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select Max(cint(mid(IMAGE_ID,16,4))) as MaxID From DP_IMAGEMGR Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select NVL(MAX(TO_NUMBER(SUBSTR(IMAGE_ID,16,4))),0) as MaxID From DP_IMAGEMGR Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select ISNULL(Max(CAST(SUBSTRING(IMAGE_ID,16,4) AS int)),0) as MaxID From DP_IMAGEMGR Where WORKSPACE_ID = '%s'",
			    m_szID);
	else
		return false;

	int iMaxID = -1;

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant var;
			if (rst.GetFieldValue(1,var))
				iMaxID = (int)var;
		}
		rst.Close();
	}

	if (iMaxID == -1 || iMaxID > MAX_UINT_ID)
		return false;

	iMaxID++;

	//����Ӱ��ID
	char		szNewImageID[30];
	memset(szNewImageID,0,sizeof(szNewImageID));
	sprintf(szNewImageID,"%s2%04d",m_szID,iMaxID);

	//�õ���ǰʱ��
	time_t		tmTmp;	
	time(&tmTmp);
	tm*			tmCur = localtime(&tmTmp);

	//����ʱ���SQL 
	char		szTimeSql[128];
	memset(szTimeSql,0,sizeof(szTimeSql));
	BuildDateTimeSql(m_pDB->GetDBType(),tmCur,szTimeSql);

	//����Ӱ���SQL
	CDpRect		rt;
	if (pRtBound)
		rt.CopyRect(pRtBound);
	else
		rt.SetRect(0,0,0,0);

	//����Ӱ����ļ���(�ڹ�������Ŀ¼��)
	char		szNewFilePath[_MAX_PATH];	
	memset(szNewFilePath,0,sizeof(szNewFilePath));
//	sprintf(szNewFilePath,"%s\\IMAGE\\%s.%s",m_szDefaultDir,szNewImageID,GetExtName(szName));
	//Ϊ���ܹ�ʹ�û��ֹ�����ԭʼӰ��,����ֱ����ԭʼӰ���������ΪԭʼӰ����ļ���
	sprintf(szNewFilePath,"%s\\IMAGES\\%s",m_szDefaultDir,szName);
	


	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"INSERT INTO DP_IMAGEMGR(WORKSPACE_ID,IMAGE_ID,IMAGE_NAME,STRIP,INDEX_IN_STRIP,MINX,MINY,MAXX,MAXY,FILE_PATH,UPDATE_DATE) VALUES('%s','%s','%s','%s',%d,%f,%f,%f,%f,'%s',%s)",
		    m_szID,szNewImageID,szName,szStrip,iIndexInStrip,rt.m_dMinX,rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,
			szNewFilePath,szTimeSql);

	if (szFilePath)
	{
		if (!SafeCopyChar(szFilePath,szNewFilePath))
			return false;
	}
	//ִ��SQL 
	if (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		if (szImageID)
		{
			strcpy(szImageID,szNewImageID);
		}
		return true;
	}
	else
		return false;

}

//�ڹ������д���ͼ��,[in]szName-ͼ������      
//                   [in]iLayerType-ͼ������  [out]szLayerID-���ش����ɹ���ͼ��ID,ʧ���򷵻ؿ��ִ�,�������Ҫ������ΪNULL
BOOL CDpWorkspace::CreateLayer(const char* szName,DP_LAYER_TYPE iLayerType,
							   enum DP_LAYER_TYPE_EX iLayerTypeEx,char* szLayerID)
{
	if (!szName || strlen(szName) <= 0 || iLayerType == GEO_NULL)
		return false;

	//ֻ��ϵͳ����Ա���ܴ���ͼ��
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;


	//�ڴ���ͼ���ͬʱ����һ���յĿռ����ݱ��һ��ֻ��һ��ATT_OID�ֶ����Ա�
/*
	//�жϵ�ǰ�û���û�д���ͼ���Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܴ���Ӱ��
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;
*/

	if (szLayerID)
		*szLayerID = 0;
	
	//�ȼ���ͼ���ID
	char	szSql[1024];
	memset(szSql,0,sizeof(szSql));
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select Max(cint(mid(LAYER_ID,16,4))) as MaxID From DP_LayersMgr Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select NVL(MAX(TO_NUMBER(SUBSTR(LAYER_ID,16,4))),0) as MaxID From DP_LayersMgr Where WORKSPACE_ID = '%s'",
			    m_szID);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select ISNULL(Max(CAST(SUBSTRING(LAYER_ID,16,4) AS int)),0) as MaxID From DP_LayersMgr Where WORKSPACE_ID = '%s'",
			    m_szID);
	else
		return false;

	int iMaxID = -1;

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant var;
			if (rst.GetFieldValue(1,var))
				iMaxID = (int)var;
		}
	}

	if (iMaxID == -1 || iMaxID > MAX_LAYER_ID)
		return false;

	iMaxID++;

	//����ͼ��ID
	char		szNewLayerID[30];
	memset(szNewLayerID,0,sizeof(szNewLayerID));
	sprintf(szNewLayerID,"%s1%04d",m_szID,iMaxID);

	//��ͼ�����������һ����¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Insert Into DP_LayersMgr(WORKSPACE_ID,LAYER_ID,LAYER_NAME,ORDERID,LAYER_TYPE,LAYER_TYPE_EX,SPATIAL_TABLE_NAME,ATTR_TABLE_NAME,GROUP_TABLE_NAME,UDLAY_TABLE_NAME) values('%s','%s','%s',0,%d,%d,'S%s','A%s','GRP%s','UDL%s')",
		    m_szID,szNewLayerID,szName,iLayerType,iLayerTypeEx,szNewLayerID,szNewLayerID,m_szID,m_szID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;
	
	//�����ռ����ݱ�
	memset(szSql,0,sizeof(szSql));

	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Create Table S%s(OID CHAR(32),ST_GRID_ID VARCHAR(19),ST_MINX FLOAT,ST_MINY FLOAT,ST_MAXX FLOAT,ST_MAXY FLOAT,ST_OBJECT_TYPE SMALLINT,SPATIAL_DATA IMAGE,PRIMARY KEY(OID))",
		        szNewLayerID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table S%s(OID CHAR(32),ST_GRID_ID VARCHAR2(19),ST_MINX FLOAT,ST_MINY FLOAT,ST_MAXX FLOAT,ST_MAXY FLOAT,ST_OBJECT_TYPE SMALLINT,SPATIAL_DATA BLOB,PRIMARY KEY(OID))",
		        szNewLayerID);	
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Create Table S%s(OID CHAR(32),ST_GRID_ID VARCHAR(19),ST_MINX FLOAT,ST_MINY FLOAT,ST_MAXX FLOAT,ST_MAXY FLOAT,ST_OBJECT_TYPE SMALLINT,SPATIAL_DATA IMAGE,PRIMARY KEY(OID))",
		        szNewLayerID);

/*
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select * Into S%s From DP_SpatialTableTemplate WHERE 1 = 2",szNewLayerID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table S%s AS Select * From DP_SpatialTableTemplate where 1 = 2",szNewLayerID);	
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select * Into S%s From DP_SpatialTableTemplate WHERE 1 = 2",szNewLayerID);
*/

	m_pDB->ExecuteSQL(szSql);

	//����ֻ��ATT_OID�ֶεĿ��������ݱ�
	memset(szSql,0,sizeof(szSql));
//	sprintf(szSql,"Create Table A%s(ATT_OID INTEGER PRIMARY KEY)",szNewLayerID);
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Create Table A%s(ATT_OID CHAR(32),ATT_GRID_ID VARCHAR(19),PRIMARY KEY(ATT_OID))",szNewLayerID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table A%s(ATT_OID CHAR(32),ATT_GRID_ID VARCHAR2(19),PRIMARY KEY(ATT_OID))",szNewLayerID);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Create Table A%s(ATT_OID CHAR(32),ATT_GRID_ID VARCHAR(19),PRIMARY KEY(ATT_OID))",szNewLayerID);
	m_pDB->ExecuteSQL(szSql);

	//������Ӧ��FeatureClass����ͼ
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Create View FeaCls%s AS Select A.*,B.* From S%s A,A%s B where A.OID = B.ATT_OID AND A.ST_GRID_ID = B.ATT_GRID_ID",
		szNewLayerID,szNewLayerID,szNewLayerID);
	m_pDB->ExecuteSQL(szSql);

	if (szLayerID)
		strcpy(szLayerID,szNewLayerID);

	return true;
}


//�ӹ�������ɾ��ͼ��
BOOL CDpWorkspace::DeleteUnit(const char* szUnitID,BOOL bDelFeature)
{
	if (!szUnitID || strlen(szUnitID) != UNIT_ID_LEN)
		return false;

	//ֻ��ϵͳ����Ա���ܴ���ԭʼӰ��
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

/*
	//�жϵ�ǰ�û���û��ɾ����ͼ����Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܴ���Ӱ��
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;
*/

	char	szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_GridInWks Where GRID_ID = '%s'",szUnitID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	//ɾ��������
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop Table P%s",szUnitID);
	m_pDB->ExecuteSQL(szSql);


	//�Ƿ���������ɾ��,��û��ȷ��,�޸�ʱ��:2005-08-19,14:50
	//ɾ����ͼ����ص�����״̬

	//ɾ������״̬�������ͼ���й�ϵ�ļ�¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASK_STATUS WHERE  WBS_B = '%s'",szUnitID);
	m_pDB->ExecuteSQL(szSql);


	//ɾ����ͼ�����ѷ��������
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASKMGR_MAIN Where WBS_B = '%s'",szUnitID);
	m_pDB->ExecuteSQL(szSql);

	//ɾ����Ȩ�ޱ��к͸�ͼ���й�ϵ��Ȩ��
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_UserRight Where GRID_ID = '%s'",szUnitID);
	m_pDB->ExecuteSQL(szSql);

	//ɾ������״̬��
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_UNIT_LOCKMGR Where UNIT_ID = '%s'",szUnitID);
	m_pDB->ExecuteSQL(szSql);

	//ɾ���ռ��������ڸ�ͼ����صĵ�������
	CDpStringArray		idArray;
	CDpStringArray		nameArray;
	GetLayersIDAndName(idArray,nameArray);
	int			iCount = idArray.GetCount();
	char		szSTabName[50];
	char		szATabName[50];

	for (int i = 0; i < iCount; i++)
	{
		sprintf(szSTabName,"S%s",idArray.GetItem(i));
		sprintf(szATabName,"A%s",idArray.GetItem(i));

		if (bDelFeature)
		{
			sprintf(szSql,"Delete From %s Where ST_GRID_ID = '%s'",szSTabName,szUnitID);
			m_pDB->ExecuteSQL(szSql);
			sprintf(szSql,"Delete From %s Where ATT_GRID_ID = '%s'",szATabName,szUnitID);
			m_pDB->ExecuteSQL(szSql);
		}
		else
		{
			sprintf(szSql,"Update %s Set ST_GRID_ID = '0' Where ST_GRID_ID = '%s'",szSTabName,szUnitID);
			m_pDB->ExecuteSQL(szSql);
			sprintf(szSql,"Update %s Set ATT_GRID_ID = '0' Where ATT_GRID_ID = '%s'",szATabName,szUnitID);
			m_pDB->ExecuteSQL(szSql);
		}
	}

	return true;
}

//�ӹ�������ɾ��ԭʼӰ��
BOOL CDpWorkspace::DeleteImage(const char* szImageID)
{
	if (!szImageID || strlen(szImageID) != IMAGE_ID_LEN)
		return false;

	//ֻ��ϵͳ����Ա����ɾ��ԭʼӰ��
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

/*
	//�жϵ�ǰ�û���û��ɾ����ԭʼӰ���Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܴ���Ӱ��
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;
*/

	char	szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_IMAGEMGR_XML Where IMAGE_ID  = '%s'",szImageID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_IMAGEMGR Where IMAGE_ID  = '%s'",szImageID);
	m_pDB->ExecuteSQL(szSql);

	//ɾ��������
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop Table P%s",szImageID);
	m_pDB->ExecuteSQL(szSql);


	//�Ƿ���������ɾ��,��û��ȷ��,�޸�ʱ��:2005-08-19,14:50
	//ɾ����ͼ����ص�����״̬

	//ɾ������״̬�������ͼ���й�ϵ�ļ�¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASK_STATUS WHERE  WBS_B = '%s'",szImageID);
	m_pDB->ExecuteSQL(szSql);


	//ɾ����ͼ�����ѷ��������
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASKMGR_MAIN Where WBS_B = '%s'",szImageID);
	m_pDB->ExecuteSQL(szSql);

	//ɾ����Ȩ�ޱ��к͸�ͼ���й�ϵ��Ȩ��
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_UserRight Where GRID_ID = '%s'",szImageID);
	m_pDB->ExecuteSQL(szSql);




	return true;
}

//�ӹ�������ɾ��ͼ��
BOOL CDpWorkspace::DeleteLayer(const char* szLayerID)
{
	if (!szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return false;

	//ֻ��ϵͳ����Ա���ܴ���ɾ��ͼ��
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

/*
	//�жϵ�ǰ�û���û��ɾ����ԭʼӰ���Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ��ܴ���Ӱ��
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pDB->m_pRightMgr->GetWorkspaceRight(m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;
*/

	//ɾ���ռ����ݱ�
	char	szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop Table S%s",szLayerID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	//ɾ�����Ա�
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop Table A%s",szLayerID);
	m_pDB->ExecuteSQL(szSql);


	//ɾ��ͼ�������еļ�¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_LayersMgr Where LAYER_ID  = '%s'",szLayerID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	//��ɾ��FeatureClass��ͼ
	memset(szSql,0,sizeof(szSql));
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Drop Table FeaCls%s",szLayerID);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Drop View FeaCls%s",szLayerID);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Drop View FeaCls%s",szLayerID);

	m_pDB->ExecuteSQL(szSql);
	
	//ɾ�������ֶ��ֵ���еļ�¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_Attr_Dictionary Where ATTR_TABLE_NAME = 'A%s'",szLayerID);
	m_pDB->ExecuteSQL(szSql);

	//ɾ��������
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop Table P%s",szLayerID);
	m_pDB->ExecuteSQL(szSql);


	//�Ƿ���������ɾ��,��û��ȷ��,�޸�ʱ��:2005-08-19,14:50

	//ɾ������״̬�������ͼ���й�ϵ�ļ�¼
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASK_STATUS WHERE WBS_C = '%s'",szLayerID);
	m_pDB->ExecuteSQL(szSql);


	//ɾ����ͼ�����ѷ��������
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_TASKMGR_MAIN Where WBS_C = '%s'",szLayerID);
	m_pDB->ExecuteSQL(szSql);

	//ɾ����Ȩ�ޱ��к͸�ͼ���й�ϵ��Ȩ��
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_UserRight Where RIGHTMGR_TYPE = 1 AND PARAM_ID = '%s'",szLayerID);
	m_pDB->ExecuteSQL(szSql);

	return true;
}









//�õ��������в����Ŀ
int CDpWorkspace::GetLayersCount()
{
	char			szSql[100];
	SQLINTEGER		iCount			= 0;			//ͼ����Ŀ
	SQLLEN		cbLen = 0;			//�ֶγ���
	SQLHANDLE		hStmt			= NULL;

	SQLRETURN		retcode;
	
	memset(szSql,0,sizeof(szSql));

	sprintf(szSql,"Select Count(LAYER_ID) From DP_LayersMgr Where WORKSPACE_ID = '%s'",m_szID);

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return 0;

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return 0;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			SQLGetData(hStmt,1,SQL_C_SHORT,&iCount,sizeof(iCount),&cbLen);			//ȡͼ����Ŀ
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return (int)iCount;
}

//����ͼ������(������˳��,��ײ�Ϊ0)�õ�ͼ��ID
BOOL CDpWorkspace::GetLayerID(UINT uIndex,char** szLayerID)
{
	if (uIndex < 0 || !szLayerID)
		return false;
	*szLayerID	= NULL;

	char			szSql[500];
	SQLCHAR			szID[50];
	SQLLEN		cbLen = 0;
	SQLRETURN		retcode;
	SQLHANDLE		hStmt			= NULL;
	BOOL			bRet			= false;

	memset(szSql,0,sizeof(szSql));
	memset(szID,0,sizeof(szID));

	sprintf(szSql,"Select LAYER_ID From DP_LayersMgr Where WORKSPACE_ID = '%s' AND ORDERID = %d",m_szID,uIndex);


	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return false;

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hStmt,1,SQL_C_CHAR,szID,sizeof(szID),&cbLen);			//ȡͼ��ID
			CopyStr(szLayerID,(char*)szID);
			bRet = true;
		}
		else
			bRet = false;
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	else
		bRet = false;

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return bRet;

}

//�õ���ǰ�����������е�ͼ���ID
//����˵��: szIDArray -- ���صõ���ͼ��ID������, szNameArray -- ���صõ���ͼ�����Ƶ�����
//          nLayerType   -- ���õõ���ͼ������͵Ĺ�������,���ΪGEO_NULL,���ʾ���øò���
//          nLayerTypeEx -- ���õõ���ͼ��ĳɹ����͵Ĺ�������,���ΪLAYER_TYPE_EX_NULL,���ʾ���øò���
BOOL CDpWorkspace::GetLayersIDAndName(CDpStringArray& szIDArray,CDpStringArray& szNameArray,
									  DP_LAYER_TYPE nLayerType,DP_LAYER_TYPE_EX nLayerTypeEx)
{
	char		szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select LAYER_ID,LAYER_NAME From DP_LayersMgr Where WORKSPACE_ID = '%s'",m_szID);

	char		szLayerFilter[50];
	if (nLayerType != GEO_NULL)
	{
		sprintf(szLayerFilter," AND LAYER_TYPE = %d ",nLayerType);
		strcat(szSql,szLayerFilter);
	}

	if (nLayerTypeEx != LAYER_TYPE_EX_NULL)
	{
		sprintf(szLayerFilter," AND LAYER_TYPE_EX = %d",nLayerTypeEx);
		strcat(szSql,szLayerFilter);
	}

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		CDpDBVariant	var;
		CDpDBVariant	var1;
		while (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
			{
				rst.GetFieldValue(2,var1);
				szIDArray.AddString((char*)((LPCTSTR)var));
				szNameArray.AddString((char*)((LPCTSTR)var1));
			}
		}
		rst.Close();
		return true;
	}
	return false;
}


BOOL CDpWorkspace::GetGroupTableName(char** ppszUDLTableName,char** ppszGRPTableName)
{
	char		szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select GROUP_TABLE_NAME,UDLAY_TABLE_NAME From DP_LayersMgr Where WORKSPACE_ID = '%s'",m_szID);
	
	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		CDpDBVariant	var;
		CDpDBVariant	var1;
		if (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
			{
				rst.GetFieldValue(2,var1);

				if( ppszUDLTableName )CopyStr(ppszUDLTableName,(LPCTSTR)var1);
				if( ppszGRPTableName )CopyStr(ppszGRPTableName,(LPCTSTR)var);
			}
		}
		rst.Close();
		return true;
	}
	return false;
}



//�õ�ͼ�������
enum DP_LAYER_TYPE CDpWorkspace::GetLayerType(const char* szLayerID,
											  DP_LAYER_TYPE_EX* iLayerTypeEx)
{
	if (!szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return GEO_NULL;

	if (iLayerTypeEx)
		*iLayerTypeEx = LAYER_TYPE_EX_NULL;

	char			szSql[512];
	SQLSMALLINT		iLayerType		= 0;
	SQLINTEGER		iLayerTypeExTmp	= 0;
	SQLLEN		cbLen = 0;
	SQLRETURN		retcode;
	SQLHANDLE		hStmt			= NULL;
	BOOL			bRet			= false;

	memset(szSql,0,sizeof(szSql));

	sprintf(szSql,"Select LAYER_TYPE,LAYER_TYPE_EX From DP_LayersMgr Where WORKSPACE_ID = '%s' AND LAYER_ID = '%s'",
		    m_szID,szLayerID);

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return GEO_NULL;

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return GEO_NULL;
	}

	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hStmt,1,SQL_C_SHORT,&iLayerType,sizeof(iLayerType),&cbLen);			//ȡͼ������
			SQLGetData(hStmt,2,SQL_C_LONG,&iLayerTypeExTmp,sizeof(iLayerTypeExTmp),&cbLen);			//ȡͼ������
		}
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);

	if (iLayerTypeEx)
		*iLayerTypeEx = DP_LAYER_TYPE_EX(iLayerTypeExTmp);

	return DP_LAYER_TYPE(iLayerType);
}

//����ͼ���ID�õ�ͼ�����,�õ���LAYER������ʹ�����Ҫ�ֹ��ͷ�
BOOL CDpWorkspace::GetLayer(const char* szID,CDpLayer** ppLayer)
{
	if (!szID || !ppLayer)
		return false;

	//�ж��Ƿ��з��ʸ�ͼ���Ȩ��
	if (!m_pDB->m_pRightMgr->CanAccessLayer(szID))
		return false;

	*ppLayer	= NULL;

	char			szSql[512];

	memset(szSql,0,sizeof(szSql));

	sprintf(szSql,"Select LAYER_ID,LAYER_NAME,ORDERID,LAYER_TYPE,LAYER_TYPE_EX,PARAM_TABLE_NAME,SPATIAL_TABLE_NAME,ATTR_TABLE_NAME From DP_LayersMgr Where LAYER_ID = '%s'",
		    szID);

	return GetLayerBySql(szSql,ppLayer);
}


//����SQL���õ�LAYER	
BOOL CDpWorkspace::GetLayerBySql(char* szSql,CDpLayer** ppLayer)
{
	if (!szSql || strlen(szSql) <= 0 || !ppLayer)
		return false;
	*ppLayer = NULL;

	SQLCHAR			szID[50];					//ID
	SQLCHAR			szName[50];					//���� 
	SQLINTEGER		iLayerType				= 0;//ͼ���������
	SQLINTEGER		iLayerTypeEx			= 0;//�ɹ�����
	SQLINTEGER		iIndex					= 0;//ͼ�����˳��
	SQLCHAR			szParamTableName[50];		//����������
	SQLCHAR			szSpatialTableName[50];		//�ռ����ݱ�����
	SQLCHAR			szAttributeTableName[50];	//���Ա�����

	SQLLEN		cbLen = 0;
	SQLRETURN		retcode;
	SQLHANDLE		hStmt			= NULL;
	BOOL			bRet			= true;

	memset(szID,0,sizeof(szID));
	memset(szName,0,sizeof(szName));
	memset(szParamTableName,0,sizeof(szParamTableName));
	memset(szSpatialTableName,0,sizeof(szSpatialTableName));
	memset(szAttributeTableName,0,sizeof(szAttributeTableName));

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return false;

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
		bRet	= false;		

	if (bRet && SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hStmt,1,SQL_C_CHAR,szID,sizeof(szID),&cbLen);
			SQLGetData(hStmt,2,SQL_C_CHAR,szName,sizeof(szName),&cbLen);
			SQLGetData(hStmt,3,SQL_C_SHORT,&iIndex,sizeof(iIndex),&cbLen);
			SQLGetData(hStmt,4,SQL_C_SHORT,&iLayerType,sizeof(iLayerType),&cbLen);
			SQLGetData(hStmt,5,SQL_C_LONG,&iLayerTypeEx,sizeof(iLayerTypeEx),&cbLen);
			SQLGetData(hStmt,6,SQL_C_CHAR,szParamTableName,sizeof(szParamTableName),&cbLen);
			SQLGetData(hStmt,7,SQL_C_CHAR,szSpatialTableName,sizeof(szSpatialTableName),&cbLen);
			SQLGetData(hStmt,8,SQL_C_CHAR,szAttributeTableName,sizeof(szAttributeTableName),&cbLen);

			DP_LAYER_TYPE iTmpLayerType = DP_LAYER_TYPE(iLayerType);
			if (LAYER_IS_RASTER(iTmpLayerType))
				*ppLayer = (CDpLayer*)(new CDpRasterLayer(this,iTmpLayerType));
			else
				*ppLayer = (CDpLayer*)(new CDpFeatureLayer(this,iTmpLayerType));

			
			CopyStr(&((*ppLayer)->m_szID),(char*)szID);
			CopyStr(&((*ppLayer)->m_szName),(char*)szName);
			(*ppLayer)->m_iIndex	 = iIndex;
			CopyStr(&((*ppLayer)->m_szParamTableName),(char*)szParamTableName);
			CopyStr(&((*ppLayer)->m_szSpatialTableName),(char*)szSpatialTableName);
			CopyStr(&((*ppLayer)->m_szAttributeTableName),(char*)szAttributeTableName);

			(*ppLayer)->m_iLayerTypeEx	= DP_LAYER_TYPE_EX(iLayerTypeEx);

			bRet = true;
		}
		else
			bRet = false;
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	else
		bRet = false;

	
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return bRet;


}


//�õ�ָ��ͼ��ID����ر�ı���
BOOL CDpWorkspace::GetLayerTableName(const char* szID,char** ppszSpatialTableName,
					   char** ppszParamTableName,char** ppszAttributeTableName)
{
	if (!szID || strlen(szID) != LAYER_ID_LEN)
		return false;
	if (!ppszSpatialTableName && !ppszParamTableName && !ppszAttributeTableName)
		return false;

	if (ppszSpatialTableName)	*ppszSpatialTableName		= NULL;
	if (ppszParamTableName)		*ppszParamTableName			= NULL;
	if (ppszAttributeTableName) *ppszAttributeTableName		= NULL;

	CDpLayer*	pLayer = NULL;

	if (!GetLayer(szID,&pLayer))
		return false;

	if (ppszSpatialTableName)
		CopyStr(ppszSpatialTableName,pLayer->m_szSpatialTableName);
	if (ppszParamTableName)
		CopyStr(ppszParamTableName,pLayer->m_szParamTableName);
	if (ppszAttributeTableName)
		CopyStr(ppszAttributeTableName,pLayer->m_szAttributeTableName);
	
	delete pLayer;
	pLayer = NULL;

	return true;

}

//ȡ���ݵ�����״��,
//szWBSB    -- ����Ӱ������ʱ,Ϊָ��Ӱ��ID,����MAP����ʱΪͼ��ID
//szWBSC    -- ����Ӱ������ʱ,Ϊָ��Ӱ��XML��ǩ��,����MAP����ʱΪͼ��ID,��Ϊdlg��ʱ,�����Ǹ�ͼ���µ���һ��ͼ��ID
//szTaskID	-- �������ڲ��������ݵ�����ID,���û���κ�����ͷ���NULL
//iPercent  -- ���ز�����ǰ���ݵ��������ɰٷֱ�
//iPass     -- ���ز�����ǰ���ݵ�������Ƿ�ϸ�
//szDesc	-- ���ز�����ǰ���ݵ�����ı�ע,��󲻳���200���ֽ�,�ڴ��Ƿ�����߷����,����Ƿ���һ��401���ֽڳ��ȵ��ַ�����
BOOL CDpWorkspace::GetDataStatus(const char* szWBSB,const char* szWBSC,char* szTaskID,
				                 int& D,int& iCurrentAllowFolwD,int& iPercent,int& iPass,char* szDesc)
{
	if (!szWBSB || strlen(szWBSB) != IMAGE_ID_LEN || 
		!szWBSC || !szTaskID || !szDesc)
		return false;

	*szTaskID	= 0;
	*szDesc		= 0;
		
	char			szSql[512];

	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select A.TASK_ID,A.WBS_D,A.TASK_PERCENT,A.TASK_PASS,A.TASK_DESC,B.CURRENTALLOW_D From DP_TASKMGR_MAIN A RIGHT JOIN DP_TASK_STATUS B ON A.TASK_ID = B.TASK_ID Where B.WORKSPACE_ID = '%s' AND B.WBS_B = '%s' AND B.WBS_C = '%s'",
				m_szID,szWBSB,szWBSC);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select A.TASK_ID,A.WBS_D,A.TASK_PERCENT,A.TASK_PASS,A.TASK_DESC,B.CURRENTALLOW_D From DP_TASKMGR_MAIN A,DP_TASK_STATUS B where A.TASK_ID = B.TASK_ID(+) AND B.WORKSPACE_ID = '%s' AND B.WBS_B = '%s' AND B.WBS_C = '%s'",
				m_szID,szWBSB,szWBSC);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select A.TASK_ID,A.WBS_D,A.TASK_PERCENT,A.TASK_PASS,A.TASK_DESC,B.CURRENTALLOW_D From DP_TASKMGR_MAIN A,DP_TASK_STATUS B where A.TASK_ID =* B.TASK_ID AND B.WORKSPACE_ID = '%s' AND B.WBS_B = '%s' AND B.WBS_C = '%s'",
				m_szID,szWBSB,szWBSC);
	else
		return false;

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant		var;
			CDpDBVariant		var1;
			CDpDBVariant		var2;
			CDpDBVariant		var3;
			CDpDBVariant		var4;
			CDpDBVariant		var5;

			if (rst.GetFieldValue(1,var) && 
				rst.GetFieldValue(2,var1) && 
				rst.GetFieldValue(3,var2) && 
				rst.GetFieldValue(4,var3) && 
				rst.GetFieldValue(5,var4) && 
				rst.GetFieldValue(6,var5))
			{
				strcpy(szTaskID,(LPCTSTR)var);
				D			= (int)var1;
				iPercent	= (int)var2;
				iPass		= (int)var3;
				strcpy(szDesc,(LPCTSTR)var4);
				iCurrentAllowFolwD = (int)var5;
			}

		}
		rst.Close();
	}
	else
		return false;

	return true;
}


//���õ�ǰ�������������״��
BOOL CDpWorkspace::SetDataStatus(DpAPartType A,const char* szWBSB,const char* szWBSC,int iCurrentAllowFolwD)
{
	if (!szWBSB || strlen(szWBSB) != IMAGE_ID_LEN || 
		!szWBSC)
		return false;

	char		szSql[256];
	sprintf(szSql,"Select Count(*) AS RecCount From DP_TASK_STATUS Where WORKSPACE_ID = '%s' AND WBS_B = '%s' AND WBS_C = '%s'",
		    m_szID,szWBSB,szWBSC);

	CDpRecordset		rst(m_pDB);
	int					iCount		= 0;
	CDpDBVariant		var;

	if (rst.Open(szSql) && rst.Next() && rst.GetFieldValue("RecCount",var) && (int)var >= 1)
			iCount = 1;

	if (iCount == 0)
		sprintf(szSql,"Insert Into DP_TASK_STATUS Values('%s',%d,'%s','%s',NULL,%d)",
		        m_szID,A,szWBSB,szWBSC,iCurrentAllowFolwD);
	else
		sprintf(szSql,"Update DP_TASK_STATUS Set CURRENTALLOW_D = %d Where WORKSPACE_ID = '%s' AND WBS_B = '%s' AND WBS_C = '%s'",
		        iCurrentAllowFolwD,m_szID,szWBSB,szWBSC);

	return (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}



//ö�ٸ��û���ӵ�еĽ�ɫ
BOOL CDpWorkspace::GetRoles(CDpStringArray& nRoleArray,const char* szUserID)
{
	if (!szUserID || strlen(szUserID) <= 0)
		return false;

	CDpRecordset	rst(m_pDB);
	char			szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select Distinct ROLE_ID From DP_UserRight Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND ROLE_ID <> ''",
		    m_szID,szUserID);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
				nRoleArray.AddString((char*)(LPCTSTR)var);
		}
		rst.Close();
	}

	return true;
}

//��ʼ���ӵ�������
BOOL CDpWorkspace::StartLinkToServer()
{
	NETRESOURCE		nts;
	memset(&nts,0,sizeof(NETRESOURCE));

	nts.dwType			= RESOURCETYPE_DISK;
	nts.lpRemoteName	= m_szDefaultDir;

	DWORD dwRet	=	0;

	char		szPwd[50];
	memset(szPwd,0,sizeof(szPwd));
	GetWksUserPwd(m_szID,szPwd);

	dwRet = WNetAddConnection2(&nts,szPwd,m_szID,CONNECT_UPDATE_PROFILE);
	if( ERROR_SESSION_CREDENTIAL_CONFLICT==dwRet )
	{
		WNetCancelConnection2(m_szDefaultDir,NULL,TRUE);
		dwRet = WNetAddConnection2(&nts,szPwd,m_szID,CONNECT_UPDATE_PROFILE);
	}

	if (dwRet == NO_ERROR || dwRet == ERROR_ALREADY_ASSIGNED)
		return true;
	else
		return false;
}

//ȡ������������
BOOL CDpWorkspace::EndLinkServer()
{
	char			szDefaultServerName[_MAX_PATH];
	char			szSourceServerName[_MAX_PATH];

	memset(szDefaultServerName,0,sizeof(szDefaultServerName));
	memset(szSourceServerName,0,sizeof(szSourceServerName));

	WNetCancelConnection2(m_szDefaultDir,NULL,TRUE);
	if (!ParseNetServerName(m_szDefaultDir,szDefaultServerName))
		return false;

	//ö���Ѿ������ϵ�������Դ
	DWORD			dwResult, dwResultEnum;
	HANDLE			hEnum;
	DWORD			cbBuffer = 16384;      // 16K�ǱȽϺ��ʵ��ڴ��С
	DWORD			cEntries = -1;         // -1��ʾö�����п��ܵ���Ŀ
	LPNETRESOURCE	lpnrLocal;			   // ö�ٵĶ�������
	DWORD			i;

	//��ö����
	dwResult = WNetOpenEnum(RESOURCE_CONNECTED,RESOURCETYPE_ANY,0,NULL,&hEnum);

	if (dwResult != NO_ERROR)
		return false;

	lpnrLocal = (LPNETRESOURCE) GlobalAlloc(GPTR, cbBuffer);

	do
	{  
		ZeroMemory(lpnrLocal, cbBuffer);
		//ö��
		dwResultEnum = WNetEnumResource(hEnum,&cEntries,lpnrLocal,&cbBuffer);
		if (dwResultEnum == NO_ERROR)
		{
			for(i = 0; i < cEntries; i++)
			{
				memset(szSourceServerName,0,sizeof(szSourceServerName));
				if (!ParseNetServerName(lpnrLocal[i].lpRemoteName,szSourceServerName))
					continue;
				//����͵�ǰ��ȱʡҪ���ʵ�������Դ�ķ���������ͬ
				if (_stricmp(szSourceServerName,szDefaultServerName) == 0)
					WNetCancelConnection2(lpnrLocal[i].lpLocalName?lpnrLocal[i].lpLocalName:lpnrLocal[i].lpRemoteName,0,true);

			}
		}
		else if (dwResultEnum != ERROR_NO_MORE_ITEMS)
			break;
	}while(dwResultEnum != ERROR_NO_MORE_ITEMS);


	return true;

}





/*****************************************************************/
/*                 ���ڹ����ɫ���û�Ȩ�޵���                    */
/*****************************************************************/
CDpUserMgr::CDpUserMgr(CDpWorkspace* pWks)
{
	m_pWks = pWks;
}

CDpUserMgr::~CDpUserMgr()
{

}

//���û��������Ȩ��
BOOL CDpUserMgr::AddRightToRole(const char* szRoleID,const char* sz1,const char* sz2,
								DP_RIGHT nRight,int iRightType)
{
	if (!m_pWks || m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN ||  
		!szRoleID || strlen(szRoleID) < 0 || 
		!sz1 || strlen(sz1) < UNIT_ID_LEN || !sz2)
		return false;

	char		szSql[1024];
	sprintf(szSql,"Insert Into DP_RoleRight values('%s','%s',%d,'%s','%s',%d)",m_pWks->m_szID,
		    szRoleID,iRightType,sz1,sz2,nRight);

	if (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		return true;
	else
		return false;
}

//���û��������Ȩ��
BOOL CDpUserMgr::AddRightToUser(const char* szUserID,const char* szRoleID,const char* sz1,
								const char* sz2,DP_RIGHT nRight,int iRightType)
{
	if (!m_pWks || m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

	char		szSql[1024];
	
	
	
	if (szRoleID)				//��ʾ��ֱ�ӷ����ɫ
	{
		sprintf(szSql,"Insert Into DP_UserRight values('%s','%s','%s',0,'','',0)",
				m_pWks->m_szID,szUserID,szRoleID);
	}
	else
	{
		if (!szUserID || strlen(szUserID) < 0 || 
			!sz1 || strlen(sz1) < UNIT_ID_LEN || !sz2)
			return false;

		sprintf(szSql,"Insert Into DP_UserRight values('%s','%s','',%d,'%s','%s',%d)",
				m_pWks->m_szID,szUserID,iRightType,sz1,sz2,nRight);
	}

	if (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		return true;
	else
		return false;


}


//���û���ĳ��Ȩ�޻���
BOOL CDpUserMgr::DeleteRightFromUser(const char* szUserID,const char* szRoleID,
									 const char* sz1,const char* sz2,
									 DP_RIGHT nRight,int iRightType)
{
	if (!m_pWks || m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

	char		szSql[1024];
	
	
	if (szRoleID)				//��ʾ�ǻ��շ���Ľ�ɫ
	{
		sprintf(szSql,"Delete From DP_UserRight Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND ROLE_ID = '%s'",
				m_pWks->m_szID,szUserID,szRoleID);
	}
	else
	{
		if (!szUserID || strlen(szUserID) < 0 || 
			!sz1 || strlen(sz1) < UNIT_ID_LEN || !sz2) //��ǰ��BUG,�����Ѿ��޸�
			return false;

		if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
			sprintf(szSql,"Delete From DP_UserRight Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND RIGHTMGR_TYPE = %d AND GRID_ID = '%s' AND PARAM_ID = '%s' AND [RIGHT] = %d",
					m_pWks->m_szID,szUserID,iRightType,sz1,sz2,nRight);
		else
			sprintf(szSql,"Delete From DP_UserRight Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND RIGHTMGR_TYPE = %d AND GRID_ID = '%s' AND PARAM_ID = '%s' AND RIGHT = %d",
					m_pWks->m_szID,szUserID,iRightType,sz1,sz2,nRight);
	}

	if (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		return true;
	else
		return false;
}


//����ɫ��ĳ��Ȩ�޻���
BOOL CDpUserMgr::DeleteRightFromRole(const char* szRoleID,const char* sz1,
									 const char* sz2,DP_RIGHT nRight,
									 int iRightType)
{
	if (!m_pWks || m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN ||  
		!szRoleID || strlen(szRoleID) < 0 || 
		!sz1 || strlen(sz1) < UNIT_ID_LEN || !sz2)
		return false;

	char		szSql[1024];
	if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Delete From DP_RoleRight Where WORKSPACE_ID = '%s' AND ROLE_ID = '%s' AND RIGHTMGR_TYPE = %d AND GRID_ID = '%s' AND PARAM_ID = '%s' AND [RIGHT] = %d)",
				m_pWks->m_szID,szRoleID,iRightType,sz1,sz2,nRight);
	else
		sprintf(szSql,"Delete From DP_RoleRight Where WORKSPACE_ID = '%s' AND ROLE_ID = '%s' AND RIGHTMGR_TYPE = %d AND GRID_ID = '%s' AND PARAM_ID = '%s' AND RIGHT = %d)",
				m_pWks->m_szID,szRoleID,iRightType,sz1,sz2,nRight);
	

	if (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		return true;
	else
		return false;
}


//����ɫ���ͼ����ͼ��Ȩ��
BOOL CDpUserMgr::AddUnitLayerRightToRole(const char* szRoleID,const char* szUnitID,
										 const char* szLayerID,DP_RIGHT nRight)
{
	return AddRightToRole(szRoleID,szUnitID,szLayerID,nRight,1);	
}

//����ɫ���Ӱ���XML��ǩȨ��
BOOL CDpUserMgr::AddImageXmlRightToRole(const char* szRoleID,const char* szImageID,
										const char* szXmlTagName,DP_RIGHT nRight)
{
	return AddRightToRole(szRoleID,szImageID,szXmlTagName,nRight,3);
}

//����ɫ��Ӳ���Ȩ��
BOOL CDpUserMgr::AddParamRightToRole(const char* szRoleID,const char* szParamTabName,
									 int iParamID,DP_RIGHT nRight)
{
	char	szParamID[20];
	sprintf(szParamID,"%d",iParamID);

	return AddRightToRole(szRoleID,szParamTabName,szParamID,nRight,2);
}

//���û�ֱ�ӷ����ɫ
BOOL CDpUserMgr::AddRoleToUser(const char* szUserID,const char* szRoleID)
{
	return AddRightToUser(szUserID,szRoleID,NULL,NULL,DP_RIGHT_NOTRIGHT,0);
}

//���û����ͼ����ͼ���Ȩ��
BOOL CDpUserMgr::AddUnitLayerRightToUser(const char* szUserID,const char* szUnitID,
										 const char* szLayerID,DP_RIGHT nRight)
{
	return AddRightToUser(szUserID,NULL,szUnitID,szLayerID,nRight,1);	
}

//����ɫ���Ӱ���XML��ǩȨ��
BOOL CDpUserMgr::AddImageXmlRightToUser(const char* szUserID,const char* szImageID,
										const char* szXmlTagName,DP_RIGHT nRight)
{
	return AddRightToUser(szUserID,NULL,szImageID,szXmlTagName,nRight,3);
}

//����ɫ��Ӳ���Ȩ��
BOOL CDpUserMgr::AddParamRightToUser(const char* szUserID,const char* szParamTabName,
									 int iParamID,DP_RIGHT nRight)
{
	char	szParamID[20];
	sprintf(szParamID,"%d",iParamID);

	return AddRightToUser(szUserID,NULL,szParamTabName,szParamID,nRight,2);

}

//����ĳ����ɫ��Ӱ���XML��ǩ��Ȩ��
BOOL CDpUserMgr::DeleteImageXmlRightFromRole(const char* szRoleID,const char* szImageID,
											 const char* szXmlTagName,DP_RIGHT nRight)
{
	return DeleteRightFromRole(szRoleID,szImageID,szXmlTagName,nRight,3);
}
//����ĳ����ɫ�Բ�����Ȩ��
		//����ĳ����ɫ�Բ�����Ȩ��
BOOL CDpUserMgr::DeleteParamRightFromRole(const char* szRoleID,const char* szParamTabName,
										  int iParamID,DP_RIGHT nRight)
{
	char	szParamID[20];
	sprintf(szParamID,"%d",iParamID);

	return DeleteRightFromRole(szRoleID,szParamTabName,szParamID,nRight,2);
}

//����ĳ���û���ͼ����ͼ���Ȩ��
BOOL CDpUserMgr::DeleteUnitLayerRightFromUser(const char* szUserID,const char* szUnitID,
											  const char* szLayerID,DP_RIGHT nRight)
{
	return DeleteRightFromUser(szUserID,NULL,szUnitID,szLayerID,nRight,1);		
}


//����ĳ����ɫ��ͼ����ͼ���Ȩ��
BOOL CDpUserMgr::DeleteUnitLayerRightFromRole(const char* szRoleID,const char* szUnitID,
											  const char* szLayerID,DP_RIGHT nRight)
{
	return DeleteRightFromRole(szRoleID,szUnitID,szLayerID,nRight,1);	
}



//����ĳ���û���Ӱ���XML��ǩ��Ȩ��
BOOL CDpUserMgr::DeleteImageXmlRightFromUser(const char* szUserID,const char* szImageID,
								 const char* szXmlTagName,DP_RIGHT nRight)
{
	return DeleteRightFromUser(szUserID,NULL,szImageID,szXmlTagName,nRight,3);
}

//����ĳ���û��Բ�����Ȩ��
BOOL CDpUserMgr::DeleteParamRightFromUser(const char* szUserID,const char* szParamTabName,
										  int iParamID,DP_RIGHT nRight)
{
	char	szParamID[20];
	sprintf(szParamID,"%d",iParamID);

	return DeleteRightFromUser(szUserID,NULL,szParamTabName,szParamID,nRight,2);
}


/*******************************************************************/
/*        �������ڹ���Ԫ����(��ͼ��,ͼ��,ԭʼӰ��)               */
/*******************************************************************/
CDpMgrObject::CDpMgrObject(CDpWorkspace* pWks)
{
	m_szID				= NULL;
	m_szParamTableName	= NULL;
	m_pWks				= pWks;
}

CDpMgrObject::~CDpMgrObject()
{
	if (m_szID)
	{
		delete [] m_szID;
		m_szID = NULL;
	}
	if (m_szParamTableName)
	{
		delete [] m_szParamTableName;
		m_szParamTableName = NULL;
	}
}

//����������
BOOL CDpMgrObject::CreateParamTable()
{
	if (!m_szID || strlen(m_szID) <= 0)
		return false;

	if (m_szParamTableName && strlen(m_szParamTableName) > LAYER_ID_LEN)	//��ʾ�����Ѿ�����
		return false;

	//�жϵ�ǰ�û���û�д����������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ������������Ӳ�����
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pWks->m_pDB->m_pRightMgr->GetWorkspaceRight(m_pWks->m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;

	char		szTabName[50];
	char		szSql[1024];
	memset(szTabName,0,sizeof(szTabName));
	memset(szSql,0,sizeof(szSql));
	sprintf(szTabName,"P%s",m_szID);

	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Create Table %s(CATEGORY_ID INTEGER,PARAM_ID INTEGER PRIMARY KEY,PARAM_NAME VARCHAR(50) UNIQUE,PARAM_DESC VARCHAR(200),PARAM_TYPE SMALLINT,PARAM_LEN SMALLINT,PARAM_PRECISION SMALLINT,PARAM_SELECT_ID INTEGER,PARAM_VALUE VARCHAR(200))",szTabName);
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table %s(CATEGORY_ID INTEGER,PARAM_ID INTEGER PRIMARY KEY,PARAM_NAME VARCHAR2(50) UNIQUE,PARAM_DESC VARCHAR2(200),PARAM_TYPE SMALLINT,PARAM_LEN SMALLINT,PARAM_PRECISION SMALLINT,PARAM_SELECT_ID INTEGER,PARAM_VALUE VARCHAR2(200))",szTabName);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Create Table %s(CATEGORY_ID int,PARAM_ID int PRIMARY KEY,PARAM_NAME VARCHAR(50) UNIQUE,PARAM_DESC VARCHAR(200),PARAM_TYPE SMALLINT,PARAM_LEN SMALLINT,PARAM_PRECISION SMALLINT,PARAM_SELECT_ID int,PARAM_VALUE VARCHAR(200))",szTabName);

/*
	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select * Into %s From DP_PARAMTABLETEMPLATE WHERE 1 = 2",szTabName);
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table %s As Select * From DP_PARAMTABLETEMPLATE WHERE 1 = 2",szTabName);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select * Into %s From DP_PARAMTABLETEMPLATE WHERE 1 = 2",szTabName);
*/	

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	if (m_szParamTableName)
	{
		delete [] m_szParamTableName;
		m_szParamTableName = NULL;
	}
	CopyStr(&m_szParamTableName,szTabName);

	return true;

}

//ɾ��������
BOOL CDpMgrObject::DeleteParamTable()
{
	if (!m_szParamTableName || strlen(m_szParamTableName) < LAYER_ID_LEN)	//��ʾ����������
		return false;

	//�жϵ�ǰ�û���û�д����������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
	//���û�жԹ�����д��Ȩ�޾Ͳ������������Ӳ�����
	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (!m_pWks->m_pDB->m_pRightMgr->GetWorkspaceRight(m_pWks->m_szID,&iRight) || 
		iRight != DP_RIGHT_READWRITE) 
		return false;

	char		szSql[128];
	sprintf(szSql,"Drop Table %s",m_szParamTableName);
	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	delete [] m_szParamTableName;
	m_szParamTableName = NULL;
	return true;
}

//�򿪲�����,ppParamTable���ڴ���Ҫ�������ͷ�
BOOL CDpMgrObject::OpenParamTable(CDpParamTable** ppParamTable)
{
	if (!ppParamTable || !m_szParamTableName || strlen(m_szParamTableName) < UNIT_ID_LEN)
		return false;
	
	*ppParamTable	= new CDpParamTable(m_pWks);
	if ((*ppParamTable)->Open(m_szParamTableName))
		return true;
	else
	{
		delete *ppParamTable;
		*ppParamTable = NULL;
		return false;
	}
}


//������������ȱʡ�����ò���
BOOL CDpMgrObject::AddDefaultBulitinParam(CDpParamTable* pTab)
{
	//���û�в�����,���ȴ���������
	if (!pTab)
	{
		if (!m_szParamTableName || strlen(m_szParamTableName) <= 0)
		{
			if (!CreateParamTable())
				return false;
		}
	}

	CDpParamTable*	pTable	= NULL;
	if (!pTab)
	{
		if (!OpenParamTable(&pTable) || !pTable)
			return false;
	}
	else
		pTable = pTab;

	DP_OBJECT_TYPE		nObjType		= GetObjType();	
	DP_LAYER_TYPE_EX	nLayerTypeEx	= GetObjLayerTypeEx();

	BOOL				bRet			= true;
	
	//���ȱʡ����
	if (nObjType == DP_OBJ_LAYER)
	{
		switch (nLayerTypeEx)
		{
			case LAYER_TYPE_EX_DEM:			//DEM
			{
				if (pTable->AddParamItem(99,59001,"Interval_X","DEM X interval",DP_CFT_FLOAT,8,4,0,"10.00",NULL) && 
					pTable->AddParamItem(99,59002,"Interval_Y","DEM Y interval",DP_CFT_FLOAT,8,4,0,"10.00",NULL) &&
					pTable->AddParamItem(99,59003,"Dem_Format","DEM file format",DP_CFT_VARCHAR,50,0,0,"NSDTF",NULL) && 
					pTable->AddParamItem(99,59004,"Contour_Interval","DEM CNT interval",DP_CFT_FLOAT,8,4,0,"5.00",NULL) &&
					pTable->AddParamItem(99,59009,"EXTEND_DIS","Map extend",DP_CFT_FLOAT,8,4,0,"10.00",NULL))
					bRet = true;
				else
					bRet = false;
				
				break;
			}
			case LAYER_TYPE_EX_DOM:			//DOM
			{
				if (pTable->AddParamItem(99,59005,"Interval_X","DOM X interval",DP_CFT_FLOAT,8,4,0,"1.00",NULL) && 
					pTable->AddParamItem(99,59006,"Interval_Y","DOM Y interval",DP_CFT_FLOAT,8,4,0,"1.00",NULL) &&
					pTable->AddParamItem(99,59007,"Back_Color","DOM background",DP_CFT_INTEGER,0,0,0,"16777215",NULL) &&
					pTable->AddParamItem(99,59009,"EXTEND_DIS","Map extend",DP_CFT_FLOAT,8,4,0,"10.00",NULL))
					bRet = true;
				else
					bRet = false;

				break;
			}
			case LAYER_TYPE_EX_DLG:			//DLG
			{
				if (pTable->AddParamItem(99,59008,"Precision","DLG precision",DP_CFT_FLOAT,8,4,0,"0.1",NULL) && 
					pTable->AddParamItem(99,59009,"EXTEND_DIS","Map extend",DP_CFT_FLOAT,8,4,0,"10.00",NULL))
					bRet = true;
				else 
					bRet = false;

				break;
			}
			case LAYER_TYPE_EX_DRG:			//DRG
			{

				break;
			}
			case LAYER_TYPE_EX_DVS:			//DVS
			{

				break;
			}
			default:
				break;
		}

	}
	else if (nObjType == DP_OBJ_UNIT)
	{

	}
	else if (nObjType == DP_OBJ_IMAGE)
	{


	}

	if (!pTab)
	{
		delete pTable;
		pTable = NULL;
	}

	return bRet;
}






















/***********************************************
*                ͼ��������                    *
***********************************************/
CDpUnit::CDpUnit(CDpWorkspace* pWks)
	    :CDpMgrObject(pWks)
{
	m_szUnitName		= NULL;		
	m_tmUpdateDate		= 0;		
	m_iUnitStatus		= DP_UT_ANY;		
	m_szLocker			= NULL;		
	m_szRefer			= NULL;			//06.6.19
	m_Type				= DP_A_UNIT;	//06.6.19
	m_szLocalName		= NULL;			//06.6.19
	m_rtBound.SetRect(0,0,0,0);
	memset(m_ptQuadrangle,0,sizeof(m_ptQuadrangle));

}

CDpUnit::~CDpUnit()
{
	if (m_szUnitName)
	{
		delete [] m_szUnitName;
		m_szUnitName = NULL;
	}
	if (m_szLocker)
	{
		delete [] m_szLocker;
		m_szLocker = NULL;
	}
	if (m_szLocalName)
	{
		delete [] m_szLocalName;	//06.6.19
		m_szLocalName = NULL;
	}
	if (m_szRefer)
	{
		delete [] m_szRefer;		//06.6.19
		m_szRefer = NULL;
	}
}

//�õ�ͼ����ָ��������ͼ��ID
BOOL CDpUnit::GetLayerID(UINT iIndex,char** ppszLayerID)
{
	if (iIndex < 0 || !ppszLayerID)
		return false;
	*ppszLayerID = NULL;

	return m_pWks->GetLayerID(iIndex,ppszLayerID);

}

//����ͼ��ID�õ�ʸ������,ʸ����¼������ʹ�������Ҫ�ֹ��ͷ�
BOOL CDpUnit::OpenGeometryQuery(const char* szLayerID,CDpGeometryQuery** ppGeometryQuery,
						   CDpSpatialQuery* pSpaFilter)
{
	if (!ppGeometryQuery || !szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return false;

	*ppGeometryQuery = NULL;

	DP_RIGHT		nRight	= DP_RIGHT_NOTRIGHT;
	//���ж��Ƿ���Ȩ�޷��ʸ�ͼ��
	if (!m_pWks->m_pDB->m_pRightMgr->GetUnitLayerRight(m_szID,szLayerID,&nRight) || 
		nRight == DP_RIGHT_NOTRIGHT)
		return false;


	//�õ�ͼ���Ӧ�ռ����ݱ�ı���
	CDpLayer*	pLayer				= NULL;
	if (!m_pWks->GetLayer(szLayerID,&pLayer))
		return false;

	//�������ʸ����
	if (LAYER_IS_RASTER(pLayer->GetLayerType()))
	{
		delete pLayer;
		pLayer = NULL;

		return false;
	}

	//�����ռ����ݲ�ѯ����
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select * From %s Where ST_GRID_ID = '%s' ",
		    pLayer->m_szSpatialTableName,m_szID);

	*ppGeometryQuery = new CDpGeometryQuery(m_pWks);

	if (!((*ppGeometryQuery)->Open(szSql,pSpaFilter)))
	{
		delete pLayer;
		pLayer = NULL;
		delete *ppGeometryQuery;		
		*ppGeometryQuery = NULL;	
		return false;
	}
	else
	{
		delete pLayer;
		pLayer = NULL;
		return true;
	}	
}

//����դ��ͼ���ID�õ�դ�����
BOOL CDpUnit::OpenRasterQuery(const char* szRasterLayerID,CDpRasterQuery** ppRasterQuery,
							  CDpSpatialQuery* pSpaFilter)
{
	if (!ppRasterQuery || !szRasterLayerID || strlen(szRasterLayerID) != LAYER_ID_LEN)
		return false;

	DP_RIGHT		nRight	= DP_RIGHT_NOTRIGHT;
	//���ж��Ƿ���Ȩ�޷��ʸ�ͼ��
	if (!m_pWks->m_pDB->m_pRightMgr->GetUnitLayerRight(m_szID,szRasterLayerID,&nRight) || 
		nRight == DP_RIGHT_NOTRIGHT)
		return false;
	
	//�ȵõ�ͼ���Ӧ�ռ����ݱ�ı���
	CDpLayer*	pLayer				= NULL;
	if (!m_pWks->GetLayer(szRasterLayerID,&pLayer))
		return false;

	//�������դ���
	if (!LAYER_IS_RASTER(pLayer->GetLayerType()))
	{
		delete pLayer;
		pLayer = NULL;
		return false;
	}

	//�����ռ����ݲ�ѯ����
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select * From %s Where ST_GRID_ID = '%s' ",
		    pLayer->m_szSpatialTableName,m_szID);

	*ppRasterQuery = new CDpRasterQuery(m_pWks);
	if (!((*ppRasterQuery)->Open(szSql,pSpaFilter)))
	{
		delete pLayer;
		pLayer = NULL;
		delete *ppRasterQuery;
		*ppRasterQuery = NULL;	
		return false;
	}
	else
	{
		delete pLayer;
		pLayer = NULL;
		return true;
	}	
	

}




//����������
BOOL CDpUnit::CreateParamTable()
{
	if (CDpMgrObject::CreateParamTable())
	{
		char		szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Update DP_GridInWks Set UNIT_PARAMTABLE = '%s' Where GRID_ID = '%s'",
				m_szParamTableName,m_szID);
		m_pWks->m_pDB->ExecuteSQL(szSql);
		return true;
	}
	else
		return false;
}

//ɾ��������
BOOL CDpUnit::DeleteParamTable()
{
	if (CDpMgrObject::DeleteParamTable())
	{
		char		szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Update DP_GridInWks Set UNIT_PARAMTABLE = NULL Where GRID_ID = '%s'",
				m_szID);
		m_pWks->m_pDB->ExecuteSQL(szSql);
		return true;
	}
	else
		return false;

}


//����ͼ���Ŀռ䷶Χ
BOOL CDpUnit::SetUnitBound(CDpRect* pRect)
{
	if (!pRect)
		return false;

	char		szSql[256];
	sprintf(szSql,"Update DP_GridInWks Set MINX = %f,MINY = %f,MAXX = %f,MAXY = %f Where WORKSPACE_ID = '%s' AND GRID_ID = '%s'",
		    pRect->m_dMinX,pRect->m_dMinY,pRect->m_dMaxX,pRect->m_dMaxY,
			m_pWks->m_szID,m_szID);

	return (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}

//����ͼ������Ч��Χ
BOOL CDpUnit::SetUnitVailRegion(CDpPoint* pVailReg)
{
	if (!pVailReg)
		return false;

	char		szSql[1024];
	sprintf(szSql,"Update DP_GridInWks Set LTX = %f,LTY = %f,LBX = %f,LBY = %f,RBX = %f,RBY = %f,RTX = %f,RTY = %f Where WORKSPACE_ID = '%s' AND GRID_ID = '%s'",
		    pVailReg[0].m_dx,pVailReg[0].m_dy,pVailReg[1].m_dx,pVailReg[1].m_dy,
		    pVailReg[2].m_dx,pVailReg[2].m_dy,pVailReg[3].m_dx,pVailReg[3].m_dy,
			m_pWks->m_szID,m_szID);

	return (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}

//�õ���ǰ������������,ͼ��/ͼ��/ԭʼӰ��
DP_OBJECT_TYPE CDpUnit::GetObjType()
{
	return DP_OBJ_UNIT;
}

//�õ���ǰ�������ĳɹ�����,ֻ�е����������ͼ��ʱ����Ч
DP_LAYER_TYPE_EX CDpUnit::GetObjLayerTypeEx()
{
	return LAYER_TYPE_EX_NULL;
}























/***********************************************
*                Ӱ�������                    *
***********************************************/
CDpImage::CDpImage(CDpWorkspace* pWks)
		 :CDpMgrObject(pWks)
{
	m_szImageName	= NULL;
	m_szImageDesc	= NULL;
	m_szStrip		= NULL;
	m_szFilePath	= NULL;
	m_szLocker		= NULL;
	m_szServerName		= NULL;
	m_iIndexInStrip	= 0;
	m_tmUpdateDate	= 0;
	m_iImageStatus	= DP_UT_UNLOCK;
//	m_rtBound.SetRect(0,0,0,0);
}

CDpImage::~CDpImage()
{
	if (m_szImageName)
	{
		delete [] m_szImageName;
		m_szImageName = NULL;
	}
	if (m_szImageDesc)
	{
		delete [] m_szImageDesc;
		m_szImageDesc = NULL;
	}
	if (m_szStrip)
	{
		delete [] m_szStrip;
		m_szStrip = NULL;
	}
	if (m_szFilePath)
	{
		delete [] m_szFilePath;
		m_szFilePath = NULL;
	}
	if (m_szLocker)
	{
		delete [] m_szLocker;
		m_szLocker = NULL;
	}
	if (m_szServerName)
	{
		delete [] m_szServerName;
		m_szServerName = NULL;
	}
}


//����������
BOOL CDpImage::CreateParamTable()
{
	if (CDpMgrObject::CreateParamTable())
	{
		char	szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Update DP_IMAGEMGR Set PARAMTABLE_NAME = '%s' Where IMAGE_ID = '%s'",
				m_szParamTableName,m_szID);
		m_pWks->m_pDB->ExecuteSQL(szSql);
		return true;
	}
	else
		return false;

}

//ɾ��������
BOOL CDpImage::DeleteParamTable()
{
	if (CDpMgrObject::DeleteParamTable())
	{
		char	szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Update DP_IMAGEMGR Set PARAMTABLE_NAME = NULL Where IMAGE_ID = '%s'",
				m_szID);
		m_pWks->m_pDB->ExecuteSQL(szSql);
		return true;
	}
	else
		return false;

}



//�õ���ǰӰ�������еı�ǩ������
BOOL CDpImage::GetAllXmlTagName(CDpStringArray& nArray)
{
	CDpRecordset	rst(m_pWks->m_pDB);

	char			szSql[256];
	sprintf(szSql,"Select XML_TAGNAME From DP_IMAGEMGR_XML Where IMAGE_ID = '%s'",m_szID);

	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			rst.GetFieldValue((short)1,var);
			nArray.AddString((char*)((LPCTSTR)var));
		}
		rst.Close();
	}
	return true;
}



//�õ�Ӱ���ָ�����Ƶ�XML������
//����˵��: [in] szTagName -- ָ��Ҫȡ��XML���ı�ǩ��,[out]buff -- ����XML���ĵ�����,�ڴ���Ҫ�������ͷ�
//          [out]piBuffLen -- ����XML�����ݵĳ���
BOOL CDpImage::GetImageXmlData(const char* szTagName,BYTE** buff,int* piBuffLen)
{
	if (!szTagName || strlen(szTagName) <= 0 || !buff)
		return false;

	*buff		= NULL;
	*piBuffLen	= 0;

	BOOL				bRet				= false;

	//���ж���û�ж�ȡ�ñ�ǩ��Ȩ��
	DP_RIGHT			nRight				= DP_RIGHT_NOTRIGHT;
	if (!m_pWks->m_pDB->m_pRightMgr->GetImageXmlRight(m_szID,szTagName,&nRight) || 
		nRight == DP_RIGHT_NOTRIGHT)
		return false;

	CDpRecordset		rst(m_pWks->m_pDB);
	char				szSql[256];
	sprintf(szSql,"Select XML_DATA From DP_IMAGEMGR_XML Where IMAGE_ID = '%s' AND XML_TAGNAME = '%s'",
			m_szID,szTagName);

	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue((short)1,var))
			{
				if (var.m_pLongBinary->m_dwSize > 0)
				{
					*piBuffLen = var.m_pLongBinary->m_dwSize;
					*buff = new BYTE[*piBuffLen];
					memcpy(*buff,var.m_pLongBinary->GetBuffer(),*piBuffLen);
					bRet = true;
				}
			}
		}

		rst.Close();	
	}
	
	return bRet;

}


//���Ӱ���ǩ,ͬʱ�����XML������
BOOL CDpImage::SetImageXmlData(const char* szTagName,BYTE* buff,int iBuffLen)
{
	if (!szTagName || strlen(szTagName) <= 0 || NULL==buff || 0>=iBuffLen )
		return false;

	//���ж��Ƿ���Ȩ�޶Ը�IMAGE�ı�ǩ�����޸ĵ�Ȩ�� 
	DP_RIGHT			nRight				= DP_RIGHT_NOTRIGHT;
	if (!m_pWks->m_pDB->m_pRightMgr->GetImageXmlRight(m_szID,szTagName,&nRight) || 
		nRight == DP_RIGHT_NOTRIGHT)
		return false;
	//�ж�����״̬
	char				szUserID[100];
	if (!m_pWks->m_pDB->m_pRightMgr->GetImageXmlLockerName(m_pWks->m_szID,m_szID,szTagName,szUserID))
		return false;
	if (strlen(szUserID) > 0 && _stricmp(szUserID,m_pWks->m_pDB->m_szUserID) != 0)
		return false;
	

	m_pWks->m_pDB->BeginTrans();			//��ʼ����

	BOOL		bRet		= false;
	
	//��ɾ���ظ���¼ 
	char		szSql[256];
	sprintf(szSql,"Delete From DP_IMAGEMGR_XML Where IMAGE_ID = '%s' AND XML_TAGNAME = '%s'",m_szID,szTagName);
	
	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
	{
		m_pWks->m_pDB->Rollback();
		return false;
	}

	//�����µļ�¼
	memset(szSql,0,sizeof(szSql));
	if (buff && iBuffLen > 0)
	{
		sprintf(szSql,"Insert Into DP_IMAGEMGR_XML Values('%s','%s',?)",m_szID,szTagName);
		bRet = SetLargerFieldValue(m_pWks->m_pDB->GetDBHandle(),szSql,buff,iBuffLen);		
	}
	else
	{
		sprintf(szSql,"Insert Into DP_IMAGEMGR_XML Values('%s','%s','')",m_szID,szTagName);
		bRet = (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
	}


	if (bRet)
		m_pWks->m_pDB->CommitTrans();
	else
		m_pWks->m_pDB->Rollback();

	return bRet;
}


//ɾ��Ӱ���ǩ
BOOL CDpImage::DeleteImageXmlData(const char* szTagName)
{
	if (!szTagName || strlen(szTagName) <= 0)
		return false;

	//���ж��Ƿ���Ȩ�޶Ը�IMAGE�����޸ĵ�Ȩ�� 
	DP_RIGHT		nRight	= DP_RIGHT_NOTRIGHT;
	if (!m_pWks->m_pDB->m_pRightMgr->GetImageRight(m_szID,&nRight) || 
		nRight != DP_RIGHT_READWRITE)
		return false;

	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From DP_IMAGEMGR_XML Where IMAGE_ID = '%s' AND XML_TAGNAME = '%s'",
		    m_szID,szTagName);
	return (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}

//����Image�Ŀռ䷶Χ
BOOL CDpImage::SetImageBound(double LTX,double LTY,double RTX,double RTY,
							 double RBX,double RBY,double LBX,double LBY)
{
	//���ж��Ƿ���Ȩ���޸ĸ�Ӱ��Ŀռ䷶Χ
	DP_RIGHT		nRight	= DP_RIGHT_NOTRIGHT;
	if (!m_pWks->m_pDB->m_pRightMgr->GetImageRight(m_szID,&nRight) || 
		nRight < DP_RIGHT_READWRITE)
		return false;

	char		szSql[512];
	sprintf(szSql,"Update DP_IMAGEMGR Set MINX = %f,MINY = %f,MAXX = %f,MAXY = %f,RTX = %f,RTY = %f,LBX = %f,LBY = %f Where IMAGE_ID = '%s'",
		    LTX,LTY,RBX,RBY,RTX,RTY,LBX,LBY,m_szID);

	BOOL bRet = (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
	if (bRet)
	{	
		_DPRealPoint d[4];
		d[0].x	= LTX;	d[0].y	= LTY;
		d[1].x	= RTX;	d[1].y	= RTY;
		d[2].x	= RBX;	d[2].y	= RBY;
		d[3].x	= LBX;	d[3].y	= LBY;
		m_nRgn.CreatePolygonRgn(d,4);
		return true;
	}
	else
		return false;

	return false;
}

//�õ���ǰ������������,ͼ��/ͼ��/ԭʼӰ��
DP_OBJECT_TYPE CDpImage::GetObjType()
{
	return DP_OBJ_IMAGE;
}
//�õ���ǰ�������ĳɹ�����,ֻ�е����������ͼ��ʱ����Ч
DP_LAYER_TYPE_EX CDpImage::GetObjLayerTypeEx()
{
	return LAYER_TYPE_EX_NULL;
}









/*******************************************************************/
/*                ���ڴ����¼����
/*******************************************************************/
CDpRecordset::CDpRecordset(CDpDatabase* pDB)
{
	m_pDB				= pDB;
	m_hStmt				= NULL;
	m_bIsOpen			= false;
	m_pFieldInfoArray	= NULL;
	m_iFieldCount		= 0;
}

CDpRecordset::~CDpRecordset()
{
	Close();
}


//�򿪲�ѯ,szSql -- ��ʾSQL���
BOOL CDpRecordset::Open(const char* szSql,DP_CURSOR_TYPE nCursorType)
{
	if (!szSql || strlen(szSql) <= 0)
		return false;

	Close();					//��ִ�йرղ�ѯ�Ĳ���

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pDB->GetDBHandle(),&m_hStmt) != SQL_SUCCESS)
		return false;

	//���α����ͻ���
	::SQLSetStmtOption(m_hStmt,SQL_CURSOR_TYPE,nCursorType); 
	if (!Check(SQLPrepare(m_hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hStmt);
		m_hStmt = NULL;
		return false;		
	}
	
	SQLRETURN retcode = SQLExecute(m_hStmt);

	if (retcode == SQL_SUCCESS)
		m_bIsOpen = true;
	else
	{
		if (retcode == SQL_SUCCESS_WITH_INFO)
		{
			char		sz[1024];
			UCHAR		szSqlStates[128];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_STMT,m_hStmt,true,sz,szSqlStates);
			if (strcmp((const char*)szSqlStates,"01S02") == 0)		//�����һ����ѯ����ʱ�ֶ�,����"select max(id) from xxx" 
				m_bIsOpen = true;
		}

		#ifdef _DEBUG
			char		sz[1024];
			UCHAR		szSqlStates[128];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_STMT,m_hStmt,true,sz,szSqlStates);
			TRACE("ErrMsg:%s\n ErrState:%s\n",sz,szSqlStates);
		#endif

	}

	if (m_bIsOpen)
	{
		m_iFieldCount = GetColsCount();
		if (!GetAllFieldInfo())
		{
			Close();
			return false;
		}
	}
	else
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hStmt);
		m_hStmt = NULL;
	}


	return m_bIsOpen;

}

//�õ�ODBC���ξ��
SQLHANDLE CDpRecordset::GetSTMT()
{
	if (!IsOpen())
		return NULL;
	else
		return m_hStmt;
}



//�α��Ƿ��
BOOL CDpRecordset::IsOpen()
{
	return m_bIsOpen;
}

//�ر��α�
void CDpRecordset::Close()
{
	if (m_pFieldInfoArray)
	{
		delete [] m_pFieldInfoArray;
		m_pFieldInfoArray = NULL;	
	}
	//������ݼ��Ѿ���,���ȹر��α�
	if (m_hStmt)
	{
		SQLCloseCursor(m_hStmt);
		SQLFreeHandle(SQL_HANDLE_STMT,m_hStmt);
		m_hStmt		= NULL;
	}
	if (m_bIsOpen)
		m_bIsOpen	= false;

	m_iFieldCount	= 0;

}

//�õ���¼��,�����ѯ����¼�ú���Ӧ�÷���1,���򷵻�0
int CDpRecordset::GetODBCRecordCount()
{
	return 1;
}

//�õ���¼���ֶ���Ŀ
int CDpRecordset::GetColsCount()
{
	if (!IsOpen())
		return false;

	SQLSMALLINT		iCount;

	if (SQLNumResultCols(m_hStmt,&iCount) != SQL_SUCCESS)
		return 0;

	return iCount;
}

//�õ��ֶ���Ŀ
int CDpRecordset::GetFieldCount()
{
	return m_iFieldCount;
}



//�õ���ǰ��¼�����е��ֶ���Ϣ
BOOL CDpRecordset::GetAllFieldInfo()
{
	if (m_iFieldCount <= 0)
		return false;

	m_pFieldInfoArray	= new CDpFieldInfo[m_iFieldCount];
	
	SQLSMALLINT		iCbNameLen	= 0;

	for (int i = 0; i < m_iFieldCount; i++)
	{
		SQLDescribeCol(m_hStmt,(SQLUSMALLINT)(i+1),(SQLCHAR*)(m_pFieldInfoArray[i].m_strName),
			               sizeof(m_pFieldInfoArray[i].m_strName),&iCbNameLen,
						   &(m_pFieldInfoArray[i].m_nSQLType),
						   &(m_pFieldInfoArray[i].m_iSize),
						   &(m_pFieldInfoArray[i].m_nPrecision),
						   (SQLSMALLINT*)&(m_pFieldInfoArray[i].m_bAllowNull));
	}

	return true;
}



//�����ƶ�һ����¼
BOOL CDpRecordset::Next()
{
	if (!IsOpen())
		return false;
	SQLRETURN retcode = SQLFetch(m_hStmt);
	if ( retcode != SQL_SUCCESS)
		return false;
	else 
		return true;
}

CDpObjectEx* CDpRecordset::Next(int iEmpty)
{
	if (!IsOpen())
		return false;
	SQLRETURN retcode = SQLFetch(m_hStmt);
	if ( retcode != SQL_SUCCESS)
		return false;
	else 
	{
		CDpObjectEx*	aa = new CDpObjectEx(m_iFieldCount);
		CDpDBVariant	var;	

		for (int i = 1 ; i  <= m_iFieldCount; i++)
		{
			this->GetFieldValue(i,var);
			aa->m_pFields[i-1] = var;
		}
		return aa;
	}
}





//�õ��ֶζ�Ӧ���ֶ�������
int CDpRecordset::GetFieldIndex(char* szFieldName)
{
	if (!IsOpen() || !szFieldName || strlen(szFieldName) <= 0 || !m_pFieldInfoArray)
		return 0;

	for (int i = 0; i < m_iFieldCount; i++)
	{
//		strupr(szFieldName);
		strupr(m_pFieldInfoArray[i].m_strName);
		if (_stricmp(szFieldName,m_pFieldInfoArray[i].m_strName) == 0)
			return (i+1);
	}

	return 0;

}


//�õ��ֶ�������Ӧ���ֶε�����
BOOL CDpRecordset::GetFieldName(UINT uIndex,char* szFieldName)
{
	if (uIndex == 0 || uIndex > m_iFieldCount ||  !IsOpen() || !szFieldName || !m_pFieldInfoArray)
		return false;

	strcpy(szFieldName,m_pFieldInfoArray[uIndex-1].m_strName);
	return true;
}

//�õ��ֶε�SQL��������
SQLSMALLINT CDpRecordset::GetFieldSqlType(UINT uIndex)
{
	if (uIndex == 0 || uIndex > m_iFieldCount ||  !IsOpen() || !m_pFieldInfoArray)
		return false;
	
	return DSDataTypeToDrv(&(m_pFieldInfoArray[uIndex-1]));
}


//�õ��ֶε���Ϣ
BOOL CDpRecordset::GetFieldInfo(UINT uIndex,CDpFieldInfo* pInfo)
{
	if (uIndex == 0 || uIndex > m_iFieldCount ||  !IsOpen() || !m_pFieldInfoArray || !pInfo)
		return false;

	memcpy(pInfo,&(m_pFieldInfoArray[uIndex]),sizeof(CDpFieldInfo));
	
	return true;
}


//�õ���ǰ���������ݵ�ʵ�ʳ���,uIndex -- ָ���������ֶε�����
long CDpRecordset::GetActualSize(UINT uIndex)
{
	if (uIndex == 0 || uIndex > m_iFieldCount || !IsOpen() || !m_hStmt)
		return false;
	
	//����Ƕ���������,���ȵõ��ֶε�ʵ�ʳ���	
	SQLLEN	lActualSize = 0;
	int		iDummy			= 0;
	SQLRETURN retcode = SQLGetData(m_hStmt,(UWORD)uIndex, SQL_C_DEFAULT, &iDummy, 0, &lActualSize);
	
	if (retcode != 0)
	{
		#ifdef _DEBUG
			char		sz[1024];
			UCHAR		szSqlStates[100];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_STMT,m_hStmt,true,sz,szSqlStates);
		#endif
	}

	return lActualSize;

}

//�õ��ֶ�ֵ
BOOL CDpRecordset::GetFieldValue(char* szFieldName,CDpDBVariant& var)
{
	if (!szFieldName || strlen(szFieldName) <= 0)
		return false;

	int		iIndex = GetFieldIndex(szFieldName);
	
	return GetFieldValue(iIndex,var);
}

//�õ��ֶ�ֵ
BOOL CDpRecordset::GetFieldValue(UINT uIndex,CDpDBVariant& var)
{
	if (uIndex < 1 || uIndex > m_iFieldCount || !m_hStmt)
		return false;
	
	var.Clear();

	SQLRETURN		retcode;

	SQLLEN				iOutLen = 0;
	//ȡ�ֶ�SQL��������
	SQLSMALLINT		SqlType		= GetFieldSqlType(uIndex);
	long			lLen		= 0;
	//�����ǰ�ֶε��������ַ��������Ƕ��������ݵĻ�,���ȵõ��ֶ������ݿ��е�ʵ�ʳ���
	if (SqlType == SQL_C_BINARY || SqlType == SQL_C_CHAR)
	{
		lLen		= GetActualSize(uIndex);
		if (lLen == SQL_NULL_DATA || lLen == 0)				//���ֶ�Ϊ��
		{
			var.m_dwType = DPDBVT_NULL;
			return true;
		}
	}

	const void*			pBuff		= NULL;
	//�������ݿ��ֶα���������ڴ�,ͬʱ�õ��仺������ָ��ʹ�С
	if (!GetDataBuffer(var,SqlType,&pBuff,&lLen) || !pBuff)
	{
		var.Clear();
		return true;
	}
	
	//ȡ�ֶ�ֵ,����Ƕ����Ƶ��������п���һ�β���ȡ��,����Ҫȡ���
	if (SqlType == SQL_C_BINARY)				//����������
	{
		BYTE* buff = (BYTE*)pBuff;

		int		iRevLen = 0;
		if (lLen <= DEFAULT_READ_CHUNK)			//Ҫ��ȡ���ֽ���С��һ������ֽ���
		{
			retcode = SQLGetData(m_hStmt,uIndex,SqlType,buff,lLen,&iOutLen);
		}
		else									//Ҫ��ȡ���ֽ�������һ������ֽ���
		{
			int		iIncrease	= 0;
			while (iRevLen < lLen)
			{
				if ((lLen - iRevLen) >= DEFAULT_READ_CHUNK)
				{
					iIncrease = DEFAULT_READ_CHUNK;
					retcode = SQLGetData(m_hStmt,uIndex,SqlType,buff+iRevLen,iIncrease,&iOutLen);
				}
				else
				{
					iIncrease = (lLen - iRevLen);
					retcode = SQLGetData(m_hStmt,uIndex,SqlType,buff+iRevLen,iIncrease,&iOutLen);
				}


				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					iRevLen += iIncrease;
					if (retcode == SQL_SUCCESS)			//��ʾ�Ѿ�û��ʣ��������
						break;
				}
				else
					break;
			}
			if (iRevLen > 0)
				iOutLen = iRevLen;
		}
	}
	else
	{
		retcode = SQLGetData(m_hStmt,uIndex,SqlType,(SQLPOINTER)pBuff,lLen,&iOutLen);
	}

	if (retcode == SQL_ERROR || retcode == SQL_NO_DATA/* || retcode == SQL_SUCCESS_WITH_INFO*/)
	{
/*
		char		sz[1024];
		UCHAR		szSqlStates[128];
		memset(sz,0,sizeof(sz));
		memset(szSqlStates,0,sizeof(szSqlStates));
		ProcessLogMessages(SQL_HANDLE_STMT,m_hStmt,true,sz,szSqlStates);
*/
		return false;
	}

	if (iOutLen == SQL_NULL_DATA)				//������ֶ�ΪNULL
		memset((void*)pBuff,0,lLen);

	return true;
}


//�õ��ֶ�ֵ�Ļ�����
//����˵��:    varValue -- [in]���������          nFieldType -- [in]�ֶε���������
//             pBuff    -- [out]���صĻ�����ָ��   pnLen	  -- [in,out]�������ĳ���
//����pBuff��ָ����ʹ�����,�����߲����ͷ����ڴ�,��Ϊ���ڴ�����CDpDBVariant���ͷŵ�
BOOL CDpRecordset::GetDataBuffer(CDpDBVariant& varValue,short nFieldType,
								 const void** ppBuff,long* pnLen)
{
	varValue.m_dwType = DPDBVT_NULL;
	switch (nFieldType)
	{
	case SQL_C_BIT:
		*ppBuff				= &varValue.m_boolVal;
		varValue.m_dwType	= DPDBVT_BOOL;
		*pnLen				= sizeof(varValue.m_boolVal);
		break;

	case SQL_C_UTINYINT:
		*ppBuff				= &varValue.m_chVal;
		varValue.m_dwType	= DPDBVT_UCHAR;
		*pnLen				= sizeof(varValue.m_chVal);
		break;

	case SQL_C_SSHORT:
		*ppBuff				= &varValue.m_iVal;
		varValue.m_dwType	= DPDBVT_SHORT;
		*pnLen				= sizeof(varValue.m_iVal);
		break;

	case SQL_C_SLONG:
	case SQL_C_LONG:
		*ppBuff				= &varValue.m_lVal;
		varValue.m_dwType	= DPDBVT_LONG;
		*pnLen				= sizeof(varValue.m_lVal);
		break;

	case SQL_C_FLOAT:
		*ppBuff				= &varValue.m_fltVal;
		varValue.m_dwType	= DPDBVT_SINGLE;
		*pnLen				= sizeof(varValue.m_fltVal);
		break;

	case SQL_C_DOUBLE:
		*ppBuff				= &varValue.m_dblVal;
		varValue.m_dwType	= DPDBVT_DOUBLE;
		*pnLen				= sizeof(varValue.m_dblVal);
		break;

	case SQL_C_TIMESTAMP:
		varValue.m_pdate	= new TIMESTAMP_STRUCT;
		memset(varValue.m_pdate,0,sizeof(TIMESTAMP_STRUCT));
		*ppBuff				= varValue.m_pdate;
		varValue.m_dwType	= DPDBVT_DATE;
		*pnLen				= sizeof(*varValue.m_pdate);
		break;

	case SQL_C_CHAR:
		varValue.m_pString	= new char[(*pnLen)+1];
		varValue.m_dwType	= DPDBVT_STRING;
		*pnLen				+= 1;
		*ppBuff				= varValue.m_pString;
		break;

	case SQL_C_BINARY:
		if (*pnLen > 0)
		{
			varValue.m_pLongBinary				= new CDpLongBinary;
			varValue.m_dwType					= DPDBVT_BINARY;

			varValue.m_pLongBinary->m_hData		= ::GlobalAlloc(GMEM_MOVEABLE, *pnLen);
			varValue.m_pLongBinary->m_dwSize	= *pnLen;

			*ppBuff								= varValue.m_pLongBinary->GetBuffer();
		}
		else
		{
			*ppBuff = NULL;
			pnLen	= 0;
		}

		break;

	default:
		return false;
	}

	return true;
}


//������Դ����������ת���������������������
SQLSMALLINT	CDpRecordset::DSDataTypeToDrv(CDpFieldInfo* pInfo)
{
	SQLSMALLINT		dt = SQL_UNKNOWN_TYPE;
	switch (pInfo->m_nSQLType)
	{
		case SQL_BIT:
			dt = SQL_C_BIT;
			break;

		case SQL_TINYINT:
			dt = SQL_C_UTINYINT;
			break;

		case SQL_SMALLINT:
			dt = SQL_C_SSHORT;
			break;

		case SQL_INTEGER:
			dt = SQL_C_SLONG;
			break;

		case SQL_REAL:
			dt = SQL_C_FLOAT;
			break;

		case SQL_FLOAT:
		case SQL_DOUBLE:
			dt = SQL_C_DOUBLE;
			break;

		case SQL_DATE:
		case SQL_TIME:
		case SQL_TIMESTAMP:
			dt = SQL_C_TIMESTAMP;
			break;

		case SQL_NUMERIC:
		case SQL_DECIMAL:
		case SQL_BIGINT:
		{
			if (pInfo->m_nPrecision > 0)
				dt = SQL_C_DOUBLE;
			else
				dt = SQL_C_LONG;

			break;
		}
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
			dt = SQL_C_CHAR;
			break;

		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			dt = SQL_C_BINARY;
			break;

	}

	return dt;

}


























/*******************************************************************/
/*                ���ڴ����¼����
/*******************************************************************/
CDpDBVariant::CDpDBVariant()
{
	m_dwType = DPDBVT_NULL;
}

CDpDBVariant::~CDpDBVariant()
{
	Clear();	
}

//����ڴ�
void CDpDBVariant::Clear()
{
	if (m_dwType == DPDBVT_STRING)
	{
		if (m_pString)
		{
			delete [] m_pString;
			m_pString = NULL;
		}
	}
	else if (m_dwType == DPDBVT_BINARY)
	{
		if (m_pLongBinary)
			delete m_pLongBinary;
		m_pLongBinary = NULL;
	}
	else if (m_dwType == DPDBVT_DATE)
	{
		if (m_pdate)
			delete (TIMESTAMP_STRUCT*)m_pdate;
		m_pdate = NULL;
	}

	m_dwType = DPDBVT_NULL;

}

CDpDBVariant::operator int() const
{
	if (m_dwType == DPDBVT_BOOL)
		return (int)(m_boolVal?1:0);
	else if (m_dwType == DPDBVT_UCHAR)
		return (int)m_chVal;
	else if (m_dwType == DPDBVT_SHORT)
		return (int)m_iVal;
	else if (m_dwType == DPDBVT_LONG)
		return (int)m_lVal;
	else if (m_dwType == DPDBVT_SINGLE)
		return (int)m_fltVal;
	else if (m_dwType == DPDBVT_DOUBLE)
		return (int)m_dblVal;
	else if (m_dwType == DPDBVT_STRING && m_pString && strlen(m_pString) > 0)
		return atoi(m_pString);
	else 
		return 0;
}

CDpDBVariant::operator double() const
{
	if (m_dwType == DPDBVT_BOOL)
		return (double)(m_boolVal?1:0);
	else if (m_dwType == DPDBVT_UCHAR)
		return (double)m_chVal;
	else if (m_dwType == DPDBVT_SHORT)
		return (double)m_iVal;
	else if (m_dwType == DPDBVT_LONG)
		return (double)m_lVal;
	else if (m_dwType == DPDBVT_SINGLE)
		return (double)m_fltVal;
	else if (m_dwType == DPDBVT_DOUBLE)
		return (double)m_dblVal;
	else if (m_dwType == DPDBVT_STRING && m_pString && strlen(m_pString) > 0)
		return (double)atof(m_pString);
	else 
		return 0.0f;
}


CDpDBVariant::operator LPCTSTR() const
{
	if (m_dwType == DPDBVT_STRING && m_pString && strlen(m_pString) > 0)
		return (LPCTSTR)m_pString;
	else if (m_dwType == DPDBVT_NULL)
		return "";
	else
	{
		static char	szTmp[128];
		memset(szTmp,0,sizeof(szTmp));
		switch (m_dwType)
		{
			case DPDBVT_BOOL:
				sprintf(szTmp,"%s",m_dwType==0?"FALSE":"TRUE");
				break;
			case DPDBVT_UCHAR:
				sprintf(szTmp,"%d",m_chVal);
				break;
			case DPDBVT_SHORT:
				sprintf(szTmp,"%d",m_iVal);
				break;
			case DPDBVT_LONG:
				sprintf(szTmp,"%d",m_lVal);
				break;
			case DPDBVT_SINGLE:
				sprintf(szTmp,"%f",(double)m_fltVal);
				break;
			case DPDBVT_DOUBLE:
				sprintf(szTmp,"%f",m_dblVal);
				break;
			default:
				break;
		}

		return szTmp;
	}

}


const CDpDBVariant& CDpDBVariant::operator =(CDpDBVariant& var) 
{
	Clear();
	m_dwType	= var.m_dwType;
	
	switch (var.m_dwType)
	{
		case DPDBVT_NULL:
			break;
		case DPDBVT_BOOL:
			m_boolVal	= var.m_boolVal;
			break;
		case DPDBVT_UCHAR:
			m_chVal		= var.m_chVal;
			break;
		case DPDBVT_SHORT:
			m_iVal		= var.m_iVal;
			break;
		case DPDBVT_LONG:	
			m_lVal		= var.m_lVal;
			break;
		case DPDBVT_SINGLE:
			m_fltVal	= var.m_fltVal;
			break;
		case DPDBVT_DOUBLE:
			m_dblVal	= var.m_dblVal;
			break;
		case DPDBVT_DATE:	
			m_pdate	= new TIMESTAMP_STRUCT;
			memset(m_pdate,0,sizeof(TIMESTAMP_STRUCT));
			memcpy(m_pdate,var.m_pdate,sizeof(TIMESTAMP_STRUCT));
			break;
		case DPDBVT_STRING:
			CopyStr(&m_pString,var.m_pString);
			break;
		case DPDBVT_BINARY:
		{
			if (var.m_pLongBinary->m_dwSize > 0)
			{
				m_pLongBinary				= new CDpLongBinary;

				m_pLongBinary->m_hData		= ::GlobalAlloc(GMEM_MOVEABLE, var.m_pLongBinary->m_dwSize);
				m_pLongBinary->m_dwSize		= var.m_pLongBinary->m_dwSize;

				memcpy(m_pLongBinary->GetBuffer(),var.m_pLongBinary->GetBuffer(),var.m_pLongBinary->m_dwSize);
			}
		}

	}

	return *this;
}

const CDpDBVariant& CDpDBVariant::operator =(LPCTSTR szVal)
{
	Clear();

	if (szVal)
	{
		m_dwType = DPDBVT_STRING;
		CopyStr(&m_pString,szVal);
	}

	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (bool bVal)
{
	Clear();
	m_dwType	= DPDBVT_BOOL;
	m_boolVal	= bVal?1:0;
	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (UCHAR ucVal)
{
	Clear();
	m_dwType	= DPDBVT_UCHAR;
	m_chVal		= ucVal;
	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (WORD sVal)
{
	Clear();
	m_dwType	= DPDBVT_SHORT;
	m_iVal		= sVal;
	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (short sVal)
{
	Clear();
	m_dwType	= DPDBVT_SHORT;
	m_iVal		= sVal;
	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (DWORD lVal)
{
	Clear();
	m_dwType	= DPDBVT_LONG;
	m_lVal		= lVal;
	return *this;
}


const CDpDBVariant& CDpDBVariant::operator = (long lVal)
{
	Clear();
	m_dwType	= DPDBVT_LONG;
	m_lVal		= lVal;
	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (float fVal)
{
	Clear();
	m_dwType	= DPDBVT_SINGLE;
	m_fltVal	= fVal;
	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (double dblVal)
{
	Clear();
	m_dwType	= DPDBVT_DOUBLE;
	m_dblVal	= dblVal;
	return *this;
}

const CDpDBVariant& CDpDBVariant::operator = (TIMESTAMP_STRUCT* tmVal)
{
	Clear();
	m_dwType	= DPDBVT_DATE;
	m_pdate		= new TIMESTAMP_STRUCT;
	memcpy(m_pdate,tmVal,sizeof(TIMESTAMP_STRUCT));
	return *this;
}



















/*******************************************************************/
/*                ���ڴ����Ķ������ֶε���
/*******************************************************************/
CDpLongBinary::CDpLongBinary()
{
	m_hData		= NULL;
	m_dwSize	= 0;
	m_pBuff		= NULL;
}

CDpLongBinary::~CDpLongBinary()
{
	ReleaseBuffer();

	if (m_hData)
	{
		::GlobalFree(m_hData);
		m_hData = NULL;
	}
	m_dwSize	= 0;
}

//�õ���Lock���������ڴ�
BYTE* CDpLongBinary::GetBuffer()
{
	if (m_pBuff)
		return m_pBuff;

	if (m_hData)
	{
		m_pBuff = (BYTE*)::GlobalLock(m_hData);
		return m_pBuff;
	}
	else
		return NULL;
}

//Unlock�������ڴ�
void CDpLongBinary::ReleaseBuffer()
{
	if (m_pBuff && m_hData)
		::GlobalUnlock(m_hData);
}




























/*******************************************************************/
/*                     ���ڽ��пռ����ݲ�ѯ����                    */
/*       ���������ʸ�����ݺ�դ�����ݲ�ѯ�Ļ���                    */
/*******************************************************************/
CDpGeoQuery::CDpGeoQuery(CDpWorkspace* pWks)
			:CDpRecordset(pWks->m_pDB)
{
	memset(m_szGeoFieldName,0,sizeof(m_szGeoFieldName));
	sprintf(m_szGeoFieldName,"SPATIAL_DATA");				//ȱʡ�Ŀռ��ֶ���Ϊ"SPATIAL_DATA"
	memset(m_szQueryTableName,0,sizeof(m_szQueryTableName));
	memset(m_szSql,0,sizeof(m_szSql));
	memset(m_szCurUnitID,0,sizeof(m_szCurUnitID));

	m_iSpatialFieldIndex	= 0;
	m_iOIDIndex				= 0;
	m_iUnitIDIndex			= 0;
	m_iGeoTypeIndex			= 0;
	ZeroMem(m_iCurOID);
	m_pWks					= pWks;
}


CDpGeoQuery::~CDpGeoQuery()
{
//	Close();	
}


//���ÿռ������ֶε�����,ȱʡ�Ŀռ��ֶε�����Ϊ"Spatial_Data",
//���������Ҫ���ÿռ��ֶ���,Ӧ����ִ��Open֮ǰ��������,��Ϊ��OPEN��ʱ���
//�������õĿռ��ֶ�������ռ��ֶε�������
void CDpGeoQuery::SetGeoFieldName(const char* szFieldName)
{
	if (!szFieldName || strlen(szFieldName) <= 0)
		return;
	memset(m_szGeoFieldName,0,sizeof(m_szGeoFieldName));
	strcpy(m_szGeoFieldName,szFieldName);
}


//�򿪿ռ��ѯ,szSql-�������ͨSQL�Ĳ�ѯ����,���԰���ORDER BY,CDpSpatialQuery --�ռ��ѯ����
BOOL CDpGeoQuery::Open(const char* szSql,CDpSpatialQuery* pSpaFilter)
{

	if (pSpaFilter)					//����пռ��ѯ,���Ƚ���ѯ�����Ƕ��ռ��ѯ�Ĺ�������
	{

	}
	else
	{
		CDpRecordset::Open(szSql);
	}

	if (m_bIsOpen)
	{
		memset(m_szSql,0,sizeof(m_szSql));
//		strcpy(m_szSql,szSql);
		//ͨ��������ѯ���,�õ�����
		GetTableNameFromSql(szSql,m_szQueryTableName);

		//�ڸ���ʸ���ֶ���ȡ��ʸ���ֶ��ڼ�¼���е�����
		m_iSpatialFieldIndex	= GetFieldIndex(m_szGeoFieldName);
		//�õ�OID���ֶ�����		
		m_iOIDIndex				= GetFieldIndex("OID");
		//ͼ��ID���ֶ�����
		m_iUnitIDIndex			= GetFieldIndex("ST_GRID_ID");
		//�õ��������͵��ֶε�������
		m_iGeoTypeIndex			= GetFieldIndex("ST_OBJECT_TYPE");

	}

	return m_bIsOpen;
}

//�õ���ǰGeo�����ݵ�����
DP_LAYER_TYPE CDpGeoQuery::GetCurGeoType()
{
	if (!IsOpen() || m_iGeoTypeIndex == 0)
		return GEO_NULL;

	int		iShapeType	= 0;
	int		cbLen		= 0;

	CDpDBVariant		var;

	if (!GetFieldValue(m_iGeoTypeIndex,var))
		return GEO_NULL;

	return DP_LAYER_TYPE((int)var);
}

//�����ݿ��е�����ֶ�ֱ�ӵõ���Geo�����
BOOL CDpGeoQuery::GetGeoBound(CDpRect* pRect)
{
	if (!pRect || !IsOpen())
		return false;

	//�ȼ�������ֶε�����ֵ
	int				iMinXIndex	= 0;
	int				iMinYIndex	= 0;
	int				iMaxXIndex	= 0;
	int				iMaxYIndex	= 0;
	int				cbLen		= 0;
	double			dMinX		= 0.0f;
	double			dMinY		= 0.0f;
	double			dMaxX		= 0.0f;
	double			dMaxY		= 0.0f;
	CDpDBVariant	var1;
	CDpDBVariant	var2;
	CDpDBVariant	var3;
	CDpDBVariant	var4;


	iMinXIndex	= GetFieldIndex("ST_MINX");
	iMinYIndex	= GetFieldIndex("ST_MINY");
	iMaxXIndex	= GetFieldIndex("ST_MAXX");
	iMaxYIndex	= GetFieldIndex("ST_MAXY");

	if (iMinXIndex <= 0 || iMinYIndex <= 0 || iMaxXIndex <= 0 || iMaxYIndex <= 0)
		return false;

	if (!GetFieldValue(iMinXIndex,var1) || 
		!GetFieldValue(iMinYIndex,var2) || 
		!GetFieldValue(iMaxXIndex,var3) || 
		!GetFieldValue(iMaxYIndex,var4))
		return false;

	pRect->SetRect((double)var1,(double)var2,(double)var3,(double)var4);
	return true;	
}


//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
BOOL CDpGeoQuery::GetGeo(CDpDBVariant& var)
{
	if (!IsOpen() || m_iSpatialFieldIndex == 0)
		return false;

	if (!GetFieldValue(m_iSpatialFieldIndex,var))
		return false;
	else
		return true;

}

//�����ƶ�һ����¼
BOOL CDpGeoQuery::Next()
{
	if (CDpRecordset::Next())
	{
		m_iCurOID = GetOID();
		if ( !IsZeroGUID(&m_iCurOID) && GetUnitID())
			return true;
		else
			return false;
	}
	else
		return false;

}

//�õ���ǰGeo������ͼ��ID
BOOL CDpGeoQuery::GetCurrUnitID(char* szUnitID)
{
	if (!szUnitID)
		return false;
	strcpy(szUnitID,m_szCurUnitID);
	return true;
}

//�õ���ǰ�����OID
GUID CDpGeoQuery::GetOID()
{
	if (m_iOIDIndex == 0 || !IsOpen())
	{
		GUID id;
		ZeroMem(id);
		return id;
	}

	CDpDBVariant		var;

	if (!GetFieldValue(m_iOIDIndex,var))
	{
		GUID id;
		ZeroMem(id);
		return id;
	}

	return GUIDFromString((LPCTSTR)var);

}

//�õ���ǰ��ͼ��ID
BOOL CDpGeoQuery::GetUnitID()
{
	if (m_iUnitIDIndex == 0 || !IsOpen())
		return false;

	CDpDBVariant		var;
	if (!GetFieldValue(m_iUnitIDIndex,var))
		return false;
//	if (strlen((LPCTSTR)var) != UNIT_ID_LEN)			//ͼ��ID�ĳ���Ϊ19
//		return false;
	strcpy(m_szCurUnitID,(LPCTSTR)var);
	return true;
}

























/*******************************************************************/
/*                     ���ڽ��пռ����ݲ�ѯ����                    */
/*******************************************************************/
CDpGeometryQuery::CDpGeometryQuery(CDpWorkspace* pWks)
				 :CDpGeoQuery(pWks)	
{
	
}


CDpGeometryQuery::~CDpGeometryQuery()
{
//	Close();	
}


//�õ���ǰ����ĵ�������
DP_LAYER_TYPE CDpGeometryQuery::GetCurShapeType()
{
	return CDpGeoQuery::GetCurGeoType();
}


//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
BOOL CDpGeometryQuery::GetShape(CDpShape** ppShape)
{
	if (!ppShape)
		return false;

	*ppShape = NULL;

	CDpDBVariant	var;
	
	if (!GetGeo(var))
		return false;
	
	if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary && var.m_pLongBinary->m_hData && 
		var.m_pLongBinary->m_dwSize > 0)
	{
		*ppShape	= new CDpShape(GetCurShapeType());

		BYTE* buff = var.m_pLongBinary->GetBuffer();

		if (!((*ppShape)->CopyData(buff,var.m_pLongBinary->m_dwSize)))
		{
			delete *ppShape;
			*ppShape = NULL;
			return false;
		}
		else
			return true;
	}
	else
		return false;

}


//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
BOOL CDpGeometryQuery::GetShapeBound(CDpRect* pRect)
{
	return GetGeoBound(pRect);
}



















/*******************************************************************/
/*                     ���ڽ��пռ����ݲ�ѯ����                    */
/*       �������ֻ�����ڶԿռ����ݵĲ�ѯ,����ʵ�ֿռ����ݵ��޸�   */
/*******************************************************************/
CDpRasterQuery::CDpRasterQuery(CDpWorkspace* pWks)
			   :CDpGeoQuery(pWks)
{

}

CDpRasterQuery::~CDpRasterQuery()
{

}

//�õ���ǰ����ĵ�������
DP_LAYER_TYPE CDpRasterQuery::GetCurRasterType()
{
	return CDpGeoQuery::GetCurGeoType();
}

//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
BOOL CDpRasterQuery::GetRasterBound(CDpRect* pRect)
{
	return GetGeoBound(pRect);
}

//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
BOOL CDpRasterQuery::GetRasterFileName(char* pszRasterFileName)
{
	if (!pszRasterFileName)
		return false;
	*pszRasterFileName = 0;

	CDpDBVariant	var;
	
	if (!GetGeo(var))
		return false;
	
	if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary && var.m_pLongBinary->m_hData && 
		var.m_pLongBinary->m_dwSize > 0)
	{
		BYTE* buff = var.m_pLongBinary->GetBuffer();
		memcpy(pszRasterFileName,buff,var.m_pLongBinary->m_dwSize);
		return true;
	}

	return false;
}













/*******************************************************************/
/*                 �������ݷ�����    							   */
/*   ���������ɶԿռ����ݺ��������ݵĲ�ѯ,����Ҳ�ܹ����         */ 
/*      �ռ����ݺ��������ݵ��޸�                                   */ 
/*******************************************************************/
CDpGeoClass::CDpGeoClass(CDpWorkspace* pWks,DP_LAYER_TYPE GeoClsType,const char* szFeaClsName)
            :CDpGeoQuery(pWks)
{
	m_iSpaFieldIndexStart		= 1;
	m_iAttrFieldIndexStart		= 11;
	m_iGeoClsType				= GEO_NULL;
	strcpy(m_szFeaClsName,szFeaClsName);
}


CDpGeoClass::~CDpGeoClass()
{


}


//�򿪿ռ��ѯ,szSql-�������ͨSQL�Ĳ�ѯ����,���԰���ORDER BY,CDpSpatialQuery --�ռ��ѯ����
//Ŀǰ����FeatureClass�ı�ṹ��ǰ����ֶ�Ӧ����(�����ֶ�˳��):
//OID,ST_GRID_ID,ST_MINX,ST_MINY,ST_MAXX,ST_MAXY,ST_OBJECT_TYPE,SPATIAL_DATA,ATT_OID
//����������ֶ�
BOOL CDpGeoClass::Open(const char* szSql,CDpSpatialQuery* pSpaFilter)
{
	if (CDpGeoQuery::Open(szSql,pSpaFilter))
	{
		GetAttrFieldStart();
		return true;
	}
	else
		return false;

}

/*
//�õ����������ֶε��ֶ���Ϣ
BOOL CDpGeoClass::GetAllAttrFieldInfo()
{
	int iAttrCount = GetAttrFieldCount();
	if (iAttrCount > 0)
	{
		m_pCustomFieldInfo = new CDpCustomFieldInfo[iAttrCount];
		memset(m_pCustomFieldInfo,0,iAttrCount*sizeof(CDpCustomFieldInfo));
		//�ȵõ�������Ϣ
		for (int i = 0; i < iAttrCount; i++)
		{
			memcpy(&(m_pCustomFieldInfo[i]),
				   &(this->m_pFieldInfoArray[i+m_iAttrFieldIndexStart]),
				   sizeof(CDpFieldInfo));
		}
		//�ٵõ��Զ�����Ϣ
		for (int i = 0; i < iAttrCount; i++)
		{
			if (strlen(m_pCustomFieldInfo[i].m_strName) >= 0)
			{
				CDpRecordset	rst(m_pDB);
				char			szSql[256];
				memset(szSql,0,sizeof(szSql));
				sprintf(szSql,"Select FIELD_ALIAS,FIELD_TYPE,FIELD_PRECISION From DP_ATTR_DICTIONARY Where ATTR_TABLE_NAME = '%s'",
					    this->m_sz
				if (rst.Open("Sel


			}

		}

	}


}
*/


//�õ������ֶε���Ŀ
int CDpGeoClass::GetAttrFieldCount()
{
	return m_iFieldCount - m_iAttrFieldIndexStart + 1;
}

//�õ�ָ�������ֶε�����,��0��ʼ,����һ�������ֶε�����Ϊ0
int CDpGeoClass::GetAttrFieldIndex(char* szAttrFieldName)
{
	int		iIndex = GetFieldIndex(szAttrFieldName);
	if (iIndex == 0 || iIndex < m_iAttrFieldIndexStart)
		return -1;
	else
		return (iIndex - m_iAttrFieldIndexStart);
}

//���������ֶε�˳��õ�ָ�������ֶε��������ֶ���Ϣ
//����˵��: [in]iAttrOrder -- ָ���ֶ��������ֶ��е�˳��,����,��һ�������ֶ���iAttrOrder��ӦΪ0
//          [in,out]pInfo  -- �ֶε���Ϣ,���ָ���������ֶ�˳����ȷ,pInfo����������Ӧ���ֶ���Ϣ
//����ֵ:   �����ȷ,�ͷ��ض�Ӧ���ֶ�����,���򷵻�0
//���������ֶε�˳��õ�ָ�������ֶε��������ֶ���Ϣ
int CDpGeoClass::GetAttrFieldInfoByOrder(int iAttrOrder,CDpFieldInfo* pInfo)
{
	int		iIndex = iAttrOrder + m_iAttrFieldIndexStart;
	if (!IsOpen() || !m_pFieldInfoArray || m_iFieldCount <= 0 || 
		iIndex > m_iFieldCount)
		return 0;

	memcpy(pInfo,&(m_pFieldInfoArray[iIndex-1]),sizeof(CDpFieldInfo));
	return iIndex;

}


//ȡ����ֵ,��ȡֵ��ʱ����밴���ֶε�˳��(��С����)ȥȡ
//iAttrFieldIndex -- �����ֶ�����,��0��ʼ
BOOL CDpGeoClass::GetAttrValue(WORD iAttrFieldIndex,CDpDBVariant& var)
{
	var.Clear();
	
	int		iIndex	= iAttrFieldIndex + m_iAttrFieldIndexStart;

	if (!IsOpen() || iIndex > m_iFieldCount)
		return false;
	return GetFieldValue(iIndex,var);

}

//ȡ����ֵ,��ȡֵ��ʱ����밴���ֶε�˳��(��С����)ȥȡ
BOOL CDpGeoClass::GetAttrValue(char* szFieldName,CDpDBVariant& var)
{
	var.Clear();
	UINT iIndex = GetFieldIndex((char*)szFieldName);
	if (iIndex <= 0)
		return false;
	return GetFieldValue(iIndex,var);

}

//��������ֵ,ֻ�����÷Ƕ����Ƶ��ֶ�
BOOL CDpGeoClass::SetAttrValue(WORD iAttrFieldIndex,CDpDBVariant& var)
{
	if (!IsOpen() || !m_pFieldInfoArray)
		return false;

	int	iIndex	= iAttrFieldIndex + m_iAttrFieldIndexStart;
	if (iIndex > m_iFieldCount)
		return false;

	//������Դ����������תΪ���������SQL��������
	SQLSMALLINT		iDataType = DSDataTypeToDrv(&(m_pFieldInfoArray[iIndex-1]));

	//����ֶ�����Ϊ�ջ��ֶ���������Ϊ������,�򷵻ش���
	if (strlen(m_pFieldInfoArray[iIndex-1].m_strName) <= 0 || 
		iDataType == SQL_C_BINARY)
		return false;
	
	//�����ǰ��OIDΪ��,�򷵻�ʧ��
	GUID uCurrOID = GetCurrOID();
	if( IsZeroGUID(&uCurrOID) )
		return false;

	//�жϵ�ǰ�û��Ƿ���Ȩ���޸ĸ�����,��Ҫ���жϸ�����������ͼ����ǰ�û��Ƿ���д��Ȩ��
	if (!CanModifyRight())
		return false;

	char	szUnitID[UNIT_ID_LEN+1];
	char	szSql[2048];
	char	szWhere[512];
	memset(szUnitID,0,sizeof(szUnitID));
	memset(szSql,0,sizeof(szSql));
	memset(szWhere,0,sizeof(szWhere));

	if (!GetCurrUnitID(szUnitID))
		return false;

	sprintf(szWhere," Where OID = '%s' AND ATT_GRID_ID = '%s'",GUIDToString(uCurrOID), szUnitID);

	//���ɸ��µ�SQL���
	if (!BuildUpdateSql(m_pWks->m_pDB->GetDBType(),m_szQueryTableName,
		                m_pFieldInfoArray[iIndex-1].m_strName,szWhere,
						iDataType,var,szSql))
		return false;


	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;


	return true;
}

//��������ֵ,ֻ�����÷Ƕ����Ƶ��ֶ�
BOOL CDpGeoClass::SetAttrValue(char* szFieldName,CDpDBVariant& var)
{
	WORD iIndex = GetFieldIndex(szFieldName);
	if (iIndex <= 0)
		return false;
	return SetAttrValue(iIndex,var);	
}

//�õ������ֶε�˳��
int CDpGeoClass::GetAttrFieldStart()
{
	return m_iAttrFieldIndexStart;
}

//�жϵ�ǰ�û��Ƿ���Ȩ���޸ĸõ���
BOOL CDpGeoClass::CanModifyRight()
{
	char		szID[UNIT_ID_LEN+1];
	memset(szID,0,sizeof(szID));

	if (!this->GetCurrUnitID(szID))
		return false;

	if (strlen(m_szFeaClsName) != LAYER_ID_LEN + 6)
		return false;

	char		szLayerID[50];
	sprintf(szLayerID,"%s",m_szFeaClsName+6);

	DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
	if (m_pDB->m_pRightMgr->GetUnitLayerRight(szID,szLayerID,&iRight) && 
		iRight == DP_RIGHT_READWRITE)
	{
		char	szUserID[100];
		if (m_pDB->m_pRightMgr->GetUnitLayerLockerName(m_pWks->m_szID,szID,szLayerID,szUserID))
		{
			if (strlen(szUserID) <= 0 || _stricmp(szUserID,m_pDB->m_szUserID) == 0)
				return true;
		}
	}

	return false;

}




















/*******************************************************************/
/*                 ���������									   */
/*   ���������ɶԿռ����ݺ��������ݵĲ�ѯ,����Ҳ�ܹ����         */ 
/*      �ռ����ݺ��������ݵ��޸�                                   */ 
/*******************************************************************/
CDpFeatureClass::CDpFeatureClass(CDpWorkspace* pWks,enum DP_LAYER_TYPE FeaClsType,const char* szFeaClsName)
				:CDpGeoClass(pWks,FeaClsType,szFeaClsName)	
{
}


CDpFeatureClass::~CDpFeatureClass()
{

}

//�õ���ǰGEO����������
DP_LAYER_TYPE CDpFeatureClass::GetCurShapeType()
{
	return CDpGeoClass::GetCurGeoType();
}

//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
BOOL CDpFeatureClass::GetShapeBound(CDpRect* pRect)
{
	return GetGeoBound(pRect);
}

//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
BOOL CDpFeatureClass::GetShape(CDpShape** ppShape)
{
	if (!ppShape)
		return false;

	*ppShape = NULL;

	CDpDBVariant	var;
	
	if (!GetGeo(var))
		return false;
	
	if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary && var.m_pLongBinary->m_hData && 
		var.m_pLongBinary->m_dwSize > 0)
	{
		*ppShape	= new CDpShape(GetCurShapeType());

		BYTE* buff = var.m_pLongBinary->GetBuffer();

		if (!((*ppShape)->CopyData(buff,var.m_pLongBinary->m_dwSize)))
		{
			delete *ppShape;
			*ppShape = NULL;
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

//�õ�һ��Feature,ppFeature���ڴ���Ҫ�������Լ��ͷ�
BOOL CDpFeatureClass::GetFeature(CDpFeature** ppFeature)
{
	if (!ppFeature)
		return false;
	*ppFeature	= NULL;

	CDpShape*	pShape	= NULL;
	if (!GetShape(&pShape) || !pShape)
		return false;

	char	szUnitID[UNIT_ID_LEN+1];
	memset(szUnitID,0,sizeof(szUnitID));
	if (!GetCurrUnitID(szUnitID))
		return false;

	int			iCount	= this->GetAttrFieldCount();
	
	*ppFeature	= new CDpFeature(iCount);

	(*ppFeature)->SetOID(this->GetCurrOID());
	(*ppFeature)->SetUnitID(szUnitID);
	(*ppFeature)->SetShapeByRef(pShape);

	for	(int i = 0; i < iCount; i++)
	{
		CDpDBVariant	var;
		this->GetAttrValue(i,var);
		(*ppFeature)->SetAttrValue(i,var);
	}

	return true;
}


BOOL CDpFeatureClass::NextRecord()
{
	if (CDpRecordset::Next())
	{
		return true;
	}
	else
		return false;
}





/*************************************************************/
/*                  ���������ԵĶ���Ĺ�����
/*************************************************************/
CDpObject::CDpObject(int iAttrFieldCount)
{
	ZeroMem(m_iOID);
	m_iAttrFieldCount	= iAttrFieldCount;

	if (m_iAttrFieldCount > 0)
		m_pAttrFieldValueArray	= new CDpDBVariant[m_iAttrFieldCount];
	else
		m_pAttrFieldValueArray	= NULL;

	memset(m_szUnitID,0,sizeof(m_szUnitID));
	sprintf(m_szUnitID,"0");
}

CDpObject::~CDpObject()
{
	if (m_pAttrFieldValueArray)
		delete [] m_pAttrFieldValueArray;
	m_pAttrFieldValueArray = NULL;
}


//���������ֶ�����,�õ�����ֵ
BOOL CDpObject::GetAttrValue(UINT uIndex,CDpDBVariant& var)
{
	if (uIndex >= m_iAttrFieldCount)
		return false;

	var = m_pAttrFieldValueArray[uIndex];
	return true;
}


//����ָ���ֶ�����������ֵ
BOOL CDpObject::SetAttrValue(UINT uIndex,CDpDBVariant& var)
{
	if (uIndex >= m_iAttrFieldCount)
		return false;
	
	m_pAttrFieldValueArray[uIndex]	= var;
	return true;
}

//�õ���ǰFeature��OID
GUID CDpObject::GetOID()
{
	return m_iOID;
}

//���õ�ǰFeature��OID
void CDpObject::SetOID(GUID uiOID)
{
	m_iOID = uiOID;
}


//�õ���ǰFeature�ڵ������ֶεĸ���
int	CDpObject::GetAttrFieldCount()
{
	return m_iAttrFieldCount;
}

//�õ���ǰFeature��������ͼ��ID
const char* CDpObject::GetUnitID()
{
	return (const char*)m_szUnitID;
}

//���õ�ǰFeature��������ͼ��ID
void CDpObject::SetUnitID(const char* szUnitID)
{
	if (szUnitID)
		strcpy(m_szUnitID,szUnitID);
}



/*************************************************************/
/*                 ��������Ĺ�����
/*          ���а��������ʸ�����ݺ���������
/*************************************************************/
//�����������Feature�Ļ�,OID����Ϊ0����
CDpFeature::CDpFeature(int iAttrFieldCount)
		   :CDpObject(iAttrFieldCount)
{
	m_pShape	= NULL;
}

CDpFeature::~CDpFeature()
{
	if (m_pShape)
		delete m_pShape;
	m_pShape = NULL;
}


//�õ�ʸ������,ֻ�ǵõ�ָ��,ppShape���ڴ�����߲����ͷ�
BOOL CDpFeature::GetShape(const CDpShape** ppShape)
{
	if (!ppShape)	
		return false;
	*ppShape = m_pShape;
	return true;
}

//����ʸ������,ֻ�ǽ�ָ����и���,�����ǰFeature������Shape,����ɾ��ԭ�е�Shape
BOOL CDpFeature::SetShapeByRef(CDpShape* pShape)
{
	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = NULL;
	}

	if (pShape->GetDataSize() <= 0)
	{
		ASSERT(FALSE);
		return false;
	}

	m_pShape = pShape;
	return true;
}


/***********************************************************************/
/*                  ���ڽ�����������Feature��ʱ��ʹ�õĶ���            */
/***********************************************************************/
CDpFeatureCache::CDpFeatureCache(CDpFeatureLayer* pFeaLayer)
{
	m_pFeaLayer			= pFeaLayer;
	m_nCurOperStatus	= DP_NOOPERA;
	m_pSpatialCache		= NULL;
	m_pAttrCache		= NULL;
	m_iSpatialSizeOfFtr	= 0;
	m_iAttrSizeOfFtr	= 0;
	m_iCurItemCount		= 0;
	m_iSpaColCount		= 0;
	m_iAttrColCount		= 0;
	m_pSpatialBindInfo	= NULL;
	m_pAttrBindInfo		= NULL;
	m_hSpaStmt			= NULL;
	m_hAttrStmt			= NULL;

	//�����ڴ�
	SetCountOfPerOper(50);	
}

CDpFeatureCache::~CDpFeatureCache()
{
	CancelBatchOpera();
}

//����ڴ�
void CDpFeatureCache::FreeMem()
{
	if (m_pSpatialCache)
	{
		delete [] m_pSpatialCache;
		m_pSpatialCache	= NULL;
	}

	if (m_pAttrCache)
	{
		delete [] m_pAttrCache;
		m_pAttrCache	= NULL;
	}

	if (m_pSpatialBindInfo)
	{
		delete [] m_pSpatialBindInfo;
		m_pSpatialBindInfo	= NULL;
		m_iSpaColCount		= 0;
	}

	if (m_pAttrBindInfo)
	{
		delete [] m_pAttrBindInfo;
		m_pAttrBindInfo		= NULL;
		m_iAttrColCount		= 0;
	}

	m_iSpatialSizeOfFtr		= 0;
	m_iAttrSizeOfFtr		= 0;
	m_iCurItemCount			= 0;
}


//�ر�SQL�ֱ�
void CDpFeatureCache::CloseSqlHandle()
{
	if (m_hAttrStmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hAttrStmt);
		m_hAttrStmt	= NULL;
	}

	if (m_hSpaStmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hSpaStmt);
		m_hSpaStmt	= NULL;
	}
}



//��Feature���������Feature
BOOL CDpFeatureCache::AddFeature(CDpFeature* pFeature)
{
	if (!pFeature)
		return false;

	//�����µ�OID
	pFeature->SetOID(NewGUID());

	//�����ռ����ݱ�
	if (!FillSpaCache(m_pSpatialBindInfo,pFeature,m_iCurItemCount))
		return false;
	//������������ݱ�
	if (!FillAttrCache(m_pAttrBindInfo,pFeature,m_iCurItemCount))
		return false;

	m_iCurItemCount++;

	if (m_iCurItemCount == m_iCountOfPer)
	{
		if (!UpdateCacheToDB())
			return false;
	}



	return true;
}





//����ÿ�β�������Ŀ��
void CDpFeatureCache::SetCountOfPerOper(int iCount)
{
	m_iCountOfPer		= iCount;
}

//��ʼ���������
BOOL CDpFeatureCache::StartBatchOpera(DpBatchOperStatus nStatus)
{
	//���֮ǰ�в���,����ȡ��֮
	CancelBatchOpera();

	//�ڿ�ʼ���仺�����ڴ�,���������Ͷ�Ӧ��SQL�ֱ����а�
	if (!Bind())
		goto Err;

	//���õ�ǰ����״̬
	m_nCurOperStatus		= nStatus;

	return true;

Err:
	CancelBatchOpera();
	return false;
}

//�������������������	
BOOL CDpFeatureCache::EndAndSaveBatchOpera()
{
	//�Ƚ���������ʣ���������ӵ����ݿ���
	UpdateCacheToDB();
	//�ύ����
	m_pFeaLayer->m_pWks->m_pDB->CommitTrans();

	m_nCurOperStatus	= DP_NOOPERA;

	FreeMem();

	CloseSqlHandle();

//	CancelBatchOpera();

	return true;
}

//ȡ��������������Ҳ�����
BOOL CDpFeatureCache::CancelBatchOpera()
{
	FreeMem();

	CloseSqlHandle();

	if (m_nCurOperStatus != DP_NOOPERA)
	{
		//�ع�����
		m_pFeaLayer->m_pWks->m_pDB->Rollback();
		m_nCurOperStatus	= DP_NOOPERA;	
	}

	return true;
}


//�������ݿ��,ͬʱ���仺����,�Լ����ɰ���Ϣ
BOOL CDpFeatureCache::Bind()
{
	SQLHANDLE		hConn	= m_pFeaLayer->m_pWks->m_pDB->GetDBHandle();
	if (SQLAllocHandle(SQL_HANDLE_STMT,hConn,&m_hSpaStmt) != SQL_SUCCESS || !m_hSpaStmt || 
		SQLAllocHandle(SQL_HANDLE_STMT,hConn,&m_hAttrStmt) != SQL_SUCCESS || !m_hAttrStmt)
		return false;

	//���ռ����ݱ���α���Ϊ��д
	SQLRETURN sRet = SQLSetStmtAttr(m_hSpaStmt,SQL_ATTR_CONCURRENCY,(SQLPOINTER)SQL_CONCUR_VALUES,0); 	
	if (sRet != SQL_SUCCESS)
	{
		#ifdef _DEBUG
			char		sz[1024];
			UCHAR		szSqlStates[128];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_STMT,m_hSpaStmt,true,sz,szSqlStates);
			TRACE(sz);
		#endif
		return false;
	}

	if (SQLSetStmtAttr(m_hSpaStmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, 0) != SQL_SUCCESS)
		return false;

	//���������ݱ���α���Ϊ��д
	sRet = SQLSetStmtAttr(m_hAttrStmt,SQL_ATTR_CONCURRENCY,(SQLPOINTER)SQL_CONCUR_VALUES,0); 	
	sRet = SQLSetStmtAttr(m_hAttrStmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, 0);


	//��SQL�ֱ�,һ��Ҫ�������˹���д���ٴ��α꣬���������α꽫��ʧ��
	char			szSql[256]	= "\0";
	sprintf(szSql,"Select * From %s",m_pFeaLayer->m_szSpatialTableName);
	if (SQLExecDirect(m_hSpaStmt,(SQLTCHAR*)szSql,strlen(szSql)) != SQL_SUCCESS)
		return false;

	sprintf(szSql,"Select * From %s",m_pFeaLayer->m_szAttributeTableName);
	if (SQLExecDirect(m_hAttrStmt,(SQLTCHAR*)szSql,strlen(szSql)) != SQL_SUCCESS)
		return false;;


	BOOL		bRet		= false;
	BYTE*		buff		= NULL;

	//������Դ���ݿ��иñ�Ľṹ���жϴ��󶨵ļ�¼�����ڴ�ṹ��С
	//�ȵõ��ֶ���
	SQLNumResultCols(m_hSpaStmt,&m_iSpaColCount);
	SQLNumResultCols(m_hAttrStmt,&m_iAttrColCount);

	if (m_iSpaColCount <= 0 || m_iAttrColCount <= 0)
		return false;
	
	SQLSMALLINT			iCbNameLen			= 0;				//�ֶ����ĳ���

	//�ռ����ݱ����Ϣ
	m_pSpatialBindInfo				= new _DpBindFieldInfo[m_iSpaColCount];	
	memset(m_pSpatialBindInfo,0,sizeof(_DpBindFieldInfo)*m_iSpaColCount);
	for (int i = 0; i < m_iSpaColCount; i++)
	{
		SQLDescribeCol(m_hSpaStmt,(SQLUSMALLINT)(i+1),(SQLCHAR*)(m_pSpatialBindInfo[i].info.m_strName),
			               sizeof(m_pSpatialBindInfo[i].info.m_strName),&iCbNameLen,
						   &(m_pSpatialBindInfo[i].info.m_nSQLType),
						   &(m_pSpatialBindInfo[i].info.m_iSize),
						   &(m_pSpatialBindInfo[i].info.m_nPrecision),
						   (SQLSMALLINT*)&(m_pSpatialBindInfo[i].info.m_bAllowNull));
		m_pSpatialBindInfo[i].iIndex		= i+1;
	}
	//�ռ����ݱ�ÿ����¼���ڴ��Ĵ�С
	m_iSpatialSizeOfFtr		= CleanUpBindInfo(m_pSpatialBindInfo,m_iSpaColCount,DEFAULT_BINARY_BLOCK_SIZE);


	//�������ݱ��
	m_pAttrBindInfo					= new _DpBindFieldInfo[m_iAttrColCount];
	memset(m_pAttrBindInfo,0,sizeof(_DpBindFieldInfo)*m_iAttrColCount);
	for (i = 0; i < m_iAttrColCount; i++)
	{
		SQLDescribeCol(m_hAttrStmt,(SQLUSMALLINT)(i+1),(SQLCHAR*)(m_pAttrBindInfo[i].info.m_strName),
			               sizeof(m_pAttrBindInfo[i].info.m_strName),&iCbNameLen,
						   &(m_pAttrBindInfo[i].info.m_nSQLType),
						   &(m_pAttrBindInfo[i].info.m_iSize),
						   &(m_pAttrBindInfo[i].info.m_nPrecision),
						   (SQLSMALLINT*)&(m_pAttrBindInfo[i].info.m_bAllowNull));
		m_pAttrBindInfo[i].iIndex		= i+1;
	}
	//�������ݱ�ÿ����¼���ڴ��Ĵ�С
	m_iAttrSizeOfFtr		= CleanUpBindInfo(m_pAttrBindInfo,m_iAttrColCount,DEFAULT_BINARY_BLOCK_SIZE);

	
	//����ռ����ݼ�¼�󶨵Ļ�����
	m_pSpatialCache		= new BYTE[m_iSpatialSizeOfFtr*m_iCountOfPer];
	memset(m_pSpatialCache,0,m_iSpatialSizeOfFtr*m_iCountOfPer);
	//�����������ݼ�¼�󶨵Ļ�����
	m_pAttrCache		= new BYTE[m_iAttrSizeOfFtr*m_iCountOfPer];
	memset(m_pAttrCache,0,m_iAttrSizeOfFtr*m_iCountOfPer);

	//�����������ݺ;���Ҫ��ӵļ�¼���а�

	//�󶨿ռ�����
	for (i = 0; i < m_iSpaColCount; i++)
	{
		if (m_pSpatialBindInfo[i].bIsCanBind)
			SQLBindCol(m_hSpaStmt,i+1,m_pSpatialBindInfo[i].nBindSqlType,
				       (SQLPOINTER)(m_pSpatialCache+m_pSpatialBindInfo[i].iPosInBlock),
				       (SQLINTEGER)m_pSpatialBindInfo[i].iMemorySize,
					   (SQLLEN*)(m_pSpatialCache + m_pSpatialBindInfo[i].iLenPosInBlock));

	}	

	//����������
	for (i = 0; i < m_iAttrColCount; i++)
	{
		if (m_pAttrBindInfo[i].bIsCanBind)
			SQLBindCol(m_hAttrStmt,i+1,m_pAttrBindInfo[i].nBindSqlType,
				       (SQLPOINTER)(m_pAttrCache+m_pAttrBindInfo[i].iPosInBlock),
				       (SQLINTEGER)m_pAttrBindInfo[i].iMemorySize,
					   (SQLLEN*)(m_pAttrCache + m_pAttrBindInfo[i].iLenPosInBlock));

	}	
	return true;
}


//��ռ����ݵĻ������������
BOOL CDpFeatureCache::FillSpaCache(_DpBindFieldInfo* pInfo,CDpFeature* pFeature,int iRecNum)
{
	if (!pInfo || !pFeature)
		return false;

	const CDpShape*	pTmp		= NULL;

	if (!pFeature->GetShape(&pTmp) || !pTmp)
		return false;

	CDpShape*		pShape		= (CDpShape*)pTmp;

	const CDpShapeData*	pData	= NULL;
	if (!pShape->GetData(&pData) || !pData)
		return false;

	//�õ�ʸ�����ݵ������ڴ�
	BYTE*		pBuff			= NULL;
	int			iBuffLen		= 0;
	if (!pShape->GetData(&pBuff,&iBuffLen) || !pBuff || iBuffLen <= 0)
		return false;

	int			iLen			= 0;

	for (int i = 0; i < m_iSpaColCount; i++)
	{
		if (stricmp(pInfo[i].info.m_strName,"OID") == 0)
		{
			GUID	iOID = pFeature->GetOID();
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)&iOID,pInfo[i].iMemorySize);
			iLen = pInfo[i].iMemorySize;
		}
		else if (stricmp(pInfo[i].info.m_strName,"ST_GRID_ID") == 0)
		{
			const char* szUnitID = pFeature->GetUnitID();
			iLen = strlen(szUnitID);
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)szUnitID,iLen);
		}
		else if (stricmp(pInfo[i].info.m_strName,"ST_MINX") == 0)
		{
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)(&(pData->dBound[0])),
			       pInfo[i].iMemorySize);
			iLen = pInfo[i].iMemorySize;
		}
		else if (stricmp(pInfo[i].info.m_strName,"ST_MINY") == 0)
		{
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)(&(pData->dBound[1])),
			       pInfo[i].iMemorySize);
			iLen = pInfo[i].iMemorySize;
		}
		else if (stricmp(pInfo[i].info.m_strName,"ST_MAXX") == 0)
		{
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)(&(pData->dBound[2])),
			       pInfo[i].iMemorySize);
			iLen = pInfo[i].iMemorySize;
		}
		else if (stricmp(pInfo[i].info.m_strName,"ST_MAXY") == 0)
		{
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)(&(pData->dBound[3])),
			       pInfo[i].iMemorySize);
			iLen = pInfo[i].iMemorySize;
		}
		else if (stricmp(pInfo[i].info.m_strName,"ST_OBJECT_TYPE") == 0)
		{
			short	sType = pShape->GetShapeType();
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)(&sType),
			       pInfo[i].iMemorySize);
			iLen = pInfo[i].iMemorySize;
		}
		else if (stricmp(pInfo[i].info.m_strName,"SPATIAL_DATA") == 0)
		{
			iLen = min(iBuffLen,DEFAULT_BINARY_BLOCK_SIZE);
			memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iPosInBlock,(const void*)pBuff,iLen);
		}

		memcpy(m_pSpatialCache+iRecNum*m_iSpatialSizeOfFtr+pInfo[i].iLenPosInBlock,(const void*)&iLen,sizeof(int));


	}

	delete [] pBuff;
	pBuff = NULL;

	return true;
}

//���������ݵĻ���ȥ���������
BOOL CDpFeatureCache::FillAttrCache(_DpBindFieldInfo* pInfo,CDpFeature* pFeature,int iRecNum)
{
	if (!pInfo || !pFeature)
		return false;

	int		iLen	= 0;
	for (int i = 0; i < m_iAttrColCount; i++)
	{
		if (stricmp(pInfo[i].info.m_strName,"ATT_OID") == 0)
		{
			GUID	iOID = pFeature->GetOID();
			memcpy(m_pAttrCache+iRecNum*m_iAttrSizeOfFtr+pInfo[i].iPosInBlock,(const void*)&iOID,
			       pInfo[i].iMemorySize);
			iLen = pInfo[i].iMemorySize;

		}
		else if (stricmp(pInfo[i].info.m_strName,"ATT_GRID_ID") == 0)
		{
			const char* szUnitID = pFeature->GetUnitID();
			iLen = strlen(szUnitID);
			memcpy(m_pAttrCache+iRecNum*m_iAttrSizeOfFtr+pInfo[i].iPosInBlock,(const void*)szUnitID,
			       iLen);
		}
		else					//�����ֶ�
		{
			//��CDpFeatureLayer�еõ��������ֶε�����,�Ա��CDpFeature�еõ���Ӧ��ֵ
			int	iIndex = m_pFeaLayer->GetAttrFieldIndex(pInfo[i].info.m_strName);
			CDpCustomFieldInfo		info;
			memset(&info,0,sizeof(CDpCustomFieldInfo));
			m_pFeaLayer->GetAttrFieldInfo(iIndex,&info);

			if (iIndex == -1)
				return false;
			
			CDpDBVariant		var;
			pFeature->GetAttrValue(iIndex,var);
			
			switch (info.m_CustomFieldType)
			{
				case DP_CFT_BYTE:
				case DP_CFT_SMALLINT:
				case DP_CFT_INTEGER:
				case DP_CFT_BOOL:
				case DP_CFT_COLOR:
				case DP_CFT_ENUM:
				{
					int		iTmp	= (int)var;
					memcpy(m_pAttrCache+iRecNum*m_iAttrSizeOfFtr+pInfo[i].iPosInBlock,(const void*)&iTmp,
						   pInfo[i].iMemorySize);
					iLen = sizeof(int);
					break;
				}
				case DP_CFT_FLOAT:
				case DP_CFT_DOUBLE:
				{
					double dTmp		= (double)var;
					memcpy(m_pAttrCache+iRecNum*m_iAttrSizeOfFtr+pInfo[i].iPosInBlock,(const void*)&dTmp,
						   pInfo[i].iMemorySize);
					iLen = sizeof(double);
					break;
				}
				case DP_CFT_VARCHAR:
				{
					const char*	szTmp	= (LPCTSTR)var;
					memcpy(m_pAttrCache+iRecNum*m_iAttrSizeOfFtr+pInfo[i].iPosInBlock,(const void*)szTmp,
						   strlen(szTmp));
					iLen = strlen(szTmp);
					break;
				}
				case DP_CFT_DATE:
				case DP_CFT_BLOB:					//Ŀǰ�����ֶβ�֧�ֶ���������
				default:
					return false;
			}

		}

		memcpy(m_pAttrCache+iRecNum*m_iAttrSizeOfFtr+pInfo[i].iLenPosInBlock,(const void*)&iLen,sizeof(int));
	}	

	return true;
}


//�������������ݸ��µ����ݿ���
BOOL CDpFeatureCache::UpdateCacheToDB()
{
	if (m_iCurItemCount <= 0)
		return true;

	//���ڴ���������ӵ����ݿ���ȥ
	//�ռ�����
	SQLSetStmtAttr(m_hSpaStmt, SQL_ATTR_ROW_ARRAY_SIZE,(SQLPOINTER)m_iCurItemCount, 0);
	SQLSetStmtAttr(m_hSpaStmt, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)m_iSpatialSizeOfFtr, 0);
	SQLSetStmtAttr(m_hAttrStmt, SQL_ATTR_ROW_STATUS_PTR, NULL, 0);

	if (SQLBulkOperations(m_hSpaStmt,SQL_ADD) != SQL_SUCCESS)
	{
		#ifdef _DEBUG
			char		sz[1024];
			UCHAR		szSqlStates[128];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_STMT,m_hSpaStmt,true,sz,szSqlStates);
			TRACE(sz);
		#endif

		return false;
	}


	//��������
	SQLSetStmtAttr(m_hAttrStmt, SQL_ATTR_ROW_ARRAY_SIZE,(SQLPOINTER)m_iCurItemCount, 0);
	SQLSetStmtAttr(m_hAttrStmt, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)m_iAttrSizeOfFtr, 0);
	SQLSetStmtAttr(m_hAttrStmt, SQL_ATTR_ROW_STATUS_PTR, NULL, 0);

	if (SQLBulkOperations(m_hAttrStmt,SQL_ADD) != SQL_SUCCESS)
	{
		#ifdef _DEBUG
			char		sz[1024];
			UCHAR		szSqlStates[128];
			memset(sz,0,sizeof(sz));
			memset(szSqlStates,0,sizeof(szSqlStates));
			ProcessLogMessages(SQL_HANDLE_STMT,m_hAttrStmt,true,sz,szSqlStates);
			TRACE(sz);
		#endif

		return false;
	}

	m_iCurItemCount	= 0;
	//��ջ�����
	memset(m_pSpatialCache,0,m_iSpatialSizeOfFtr*m_iCountOfPer);
	memset(m_pAttrCache,0,m_iAttrSizeOfFtr*m_iCountOfPer);

	return true;
}






/**************************************************************/
/*                   ����դ��Ĺ�����        
/*         ���а���դ���ļ�������,դ�������Լ���������      
/**************************************************************/
CDpRaster::CDpRaster(int iAttrFieldCount)
		   :CDpObject(iAttrFieldCount)
{
	m_szRasterFileName	= NULL;
	m_rtBound.SetRect(0,0,0,0);
}

CDpRaster::~CDpRaster()
{
	if (m_szRasterFileName)
	{
		delete [] m_szRasterFileName;
		m_szRasterFileName = NULL;
	}
}

//�õ�դ�����ݵ��ļ���
BOOL CDpRaster::GetRasterFileName(char* szFileName,int iBuffCount)
{
	if (!szFileName)
		return false;
	if (!m_szRasterFileName)
		memset(szFileName,0,iBuffCount);

	if (strlen(m_szRasterFileName) > iBuffCount)
		return false;

	strcpy(szFileName,m_szRasterFileName);
	return true;
}

//�õ�դ������
BOOL CDpRaster::GetRasterBound(CDpRect* pRtBound)
{
	if (!pRtBound)
		return false;
	pRtBound->CopyRect(&m_rtBound);
	return true;
}

//����դ���ļ����ļ���
BOOL CDpRaster::SetRasterFileName(const char* szFileName)
{
	if (m_szRasterFileName)
		delete [] m_szRasterFileName;
	m_szRasterFileName = NULL;
	if (szFileName)
		CopyStr(&m_szRasterFileName,szFileName);
	return true;
}

//����դ������
BOOL CDpRaster::SetRasterBound(CDpRect* pRtBound)
{
	if (!pRtBound)
		return false;
	m_rtBound.CopyRect(pRtBound);
	return true;
}






/*******************************************************************/
/*                 դ�������									   */
/*   ���������ɶ�դ�����ݺ��������ݵĲ�ѯ,����Ҳ�ܹ����         */ 
/*      դ�����ݺ��������ݵ��޸�                                   */ 
/*******************************************************************/
CDpRasterClass::CDpRasterClass(CDpWorkspace* pWks,DP_LAYER_TYPE FeaClsType,const char* szRasterClsName)
               :CDpGeoClass(pWks,FeaClsType,szRasterClsName)	
{

}

CDpRasterClass::~CDpRasterClass()
{

}

//�õ���ǰGEO����������
DP_LAYER_TYPE CDpRasterClass::GetCurRasterType()
{
	return CDpGeoQuery::GetCurGeoType();
}

//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
BOOL CDpRasterClass::GetRasterBound(CDpRect* pRect)
{
	return GetGeoBound(pRect);
}

//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
BOOL CDpRasterClass::GetRasterFileName(char* pszRasterFileName)
{
	if (!pszRasterFileName)
		return false;
	*pszRasterFileName = 0;

	CDpDBVariant	var;
	
	if (!GetGeo(var))
		return false;
	
	if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary && var.m_pLongBinary->m_hData && 
		var.m_pLongBinary->m_dwSize > 0)
	{
		BYTE* buff = var.m_pLongBinary->GetBuffer();
		memcpy(pszRasterFileName,buff,var.m_pLongBinary->m_dwSize);
		return true;
	}

	return false;
}


//�õ�Raster
BOOL CDpRasterClass::GetRaster(CDpRaster** ppRaster)
{
	if (!ppRaster)
		return false;
	*ppRaster	= NULL;

	char	szUnitID[UNIT_ID_LEN+1];
	memset(szUnitID,0,sizeof(szUnitID));
	if (!GetCurrUnitID(szUnitID))
		return false;

	//�õ��ռ䷶Χ
	CDpDBVariant	var;
	CDpDBVariant	var1;
	CDpDBVariant	var2;
	CDpDBVariant	var3;

	if (!GetFieldValue("ST_MINX",var) ||
		!GetFieldValue("ST_MINY",var1) ||
		!GetFieldValue("ST_MAXX",var2) ||
		!GetFieldValue("ST_MAXY",var3))
		return false;

	CDpRect		rt((double)var,(double)var1,(double)var2,(double)var3);

	//�õ�դ���ļ���·��
	char		szFileName[512];
	memset(szFileName,0,sizeof(szFileName));
	if (!GetRasterFileName(szFileName))
		return false;


	int			iCount	= this->GetAttrFieldCount();
	*ppRaster	= new CDpRaster(iCount);


	(*ppRaster)->SetOID(this->GetCurrOID());
	(*ppRaster)->SetUnitID(szUnitID);
	(*ppRaster)->SetRasterBound(&rt);
	(*ppRaster)->SetRasterFileName(szFileName);

	for	(int i = 0; i < iCount; i++)
	{
		CDpDBVariant	var;
		this->GetAttrValue(i,var);
		(*ppRaster)->SetAttrValue(i,var);
	}

	return true;

}











/************************************************************/
/*                        ͼ�������                        */
/************************************************************/   
CDpLayer::CDpLayer(CDpWorkspace* pWks,enum DP_LAYER_TYPE iLayerType)
	     :CDpMgrObject(pWks)
{
	m_iLayerType			= iLayerType;	
	m_szName				= NULL;				
	m_szSpatialTableName	= NULL;	
	m_szAttributeTableName	= NULL;	
	m_iIndex				= NULL;		
	m_pCustomFieldInfo		= NULL;
	m_iAttrFieldCount		= 0;
	m_iLayerTypeEx			= LAYER_TYPE_EX_NULL;
}

CDpLayer::~CDpLayer()
{
	if (m_szName)
	{
		delete [] m_szName;
		m_szName = NULL;
	}
	if (m_szSpatialTableName)
	{
		delete [] m_szSpatialTableName;
		m_szSpatialTableName = NULL;
	}
	if (m_szAttributeTableName)
	{
		delete [] m_szAttributeTableName;
		m_szAttributeTableName = NULL;
	}
	if (m_pCustomFieldInfo)
	{
		delete [] m_pCustomFieldInfo;
		m_pCustomFieldInfo = NULL;
	}
}

//�õ��������
DP_LAYER_TYPE CDpLayer::GetLayerType()
{
	return m_iLayerType;
}

//�õ���ĳɹ�����
DP_LAYER_TYPE_EX CDpLayer::GetObjLayerTypeEx()
{
	return GetLayerTypeEx();
}

//�õ����ID
BOOL CDpLayer::GetLayerID(char* szLayerID,int iBuffCount)
{
	if (!szLayerID || iBuffCount < (strlen(m_szID)+1))
		return false;

	strcpy(szLayerID,m_szID);
	return true;
}

//�õ��������
BOOL CDpLayer::GetLayerName(char* szLayerName,int iBuffCount)
{
	if (!szLayerName || iBuffCount < (strlen(m_szName)+1))
		return false;
	strcpy(szLayerName,m_szName);
	return true;
}


//�õ���Ĳ����������,ͼ���п���û�в�����,���û�в�����,��szParamTabName����һ�����ַ�������
BOOL CDpLayer::GetParamTableName(char* szParamTabName,int iBuffCount)
{
	if (!szParamTabName)
		return false;
	if (!m_szParamTableName || strlen(m_szParamTableName) == 0)	//˵��û�в�����
	{
		memset(szParamTabName,0,iBuffCount);
		return true;
	}

	if (iBuffCount < (strlen(m_szParamTableName)+1))
		return false;

	strcpy(szParamTabName,m_szParamTableName);
	return true;
}


//�õ����ʸ�����ݱ������,ÿ��ͼ���Ǳض���ʸ�����ݱ��
BOOL CDpLayer::GetSpatialTableName(char* szSpatialTabName,int iBuffCount)
{
	if (!szSpatialTabName || iBuffCount < (strlen(m_szSpatialTableName)+1))
		return false;
	strcpy(szSpatialTabName,m_szSpatialTableName);
	return true;
}

//�õ����Ա������,ÿ��ͼ���Ǳض����������ݱ��
BOOL CDpLayer::GetAttrTableName(char* szAttrTabName,int iBuffCount)
{
	if (!szAttrTabName || iBuffCount < (strlen(m_szAttributeTableName)+1)
		 || m_iLayerType == GEO_Raster)				//Ӱ�����ݱ���û�����Ա��
		return false;
	strcpy(szAttrTabName,m_szAttributeTableName);
	return true;
}

//�õ�ͼ���Ӧ��GeoClass������
BOOL CDpLayer::GetGeoClassName(char* szFeaClsName,int iBuffCount)
{
	if (!m_szID || strlen(m_szID) <= 0)
		return false;

	char		szFeaCls[50];
	memset(szFeaCls,0,sizeof(szFeaCls));
	sprintf(szFeaCls,"FeaCls%s",m_szID);

	if (iBuffCount < (strlen(szFeaCls)+1))
		return false;

	strcpy(szFeaClsName,szFeaCls);

	return true;
}



//ȷ���Ƿ��ܱ༭��ͼ���е�����
BOOL CDpLayer::CanModifyCurrUnit(const char* szUnitID)
{
	if (!szUnitID)
		return false;

	DP_RIGHT		iRight = DP_RIGHT_NOTRIGHT;
	if (m_pWks->m_pDB->m_pRightMgr->GetUnitLayerRight(szUnitID,m_szID,&iRight) && 
		iRight == DP_RIGHT_READWRITE)
	{
		char		szUserID[100];
		memset(szUserID,0,sizeof(szUserID));
		if (m_pWks->m_pDB->m_pRightMgr->GetUnitLayerLockerName(m_pWks->m_szID,szUnitID,m_szID,szUserID))
		{
			if (strlen(szUserID) == 0 || _stricmp(szUserID,m_pWks->m_pDB->m_szUserID) == 0)
				return true;
		}
	}

	return false;
}

//�õ����������ֶε��ֶ���Ϣ
BOOL CDpLayer::GetAllAttrFieldInfo()
{
	if (m_pCustomFieldInfo)
		return true;

	m_iAttrFieldCount = 0;

	if (!m_szAttributeTableName || strlen(m_szAttributeTableName) <= LAYER_ID_LEN)
		return false;

	SQLHANDLE		hStmt	= NULL;

	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pWks->m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return false;

	char			szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select * From %s Where 1 = 2",m_szAttributeTableName);

	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return false;		
	}
	
	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		SQLSMALLINT		iColCount	= 0;

		if (SQLNumResultCols(hStmt,&iColCount) == SQL_SUCCESS)
		{
			if (iColCount > 2)
			{
				//���������ֶ���������,ATT_OID��ATT_GRID_ID����������
				m_pCustomFieldInfo = new CDpCustomFieldInfo[iColCount-2];
				memset(m_pCustomFieldInfo,0,(iColCount-2)*sizeof(CDpCustomFieldInfo));

				for (int i = 2; i < iColCount; i++)
				{
					SQLSMALLINT		iCbNameLen	= 0;

					SQLDescribeCol(hStmt,(SQLUSMALLINT)(i+1),(SQLCHAR*)(m_pCustomFieldInfo[i-2].m_strName),
									   sizeof(m_pCustomFieldInfo[i-2].m_strName),&iCbNameLen,
									   &(m_pCustomFieldInfo[i-2].m_nSQLType),
									   &(m_pCustomFieldInfo[i-2].m_iSize),
									   &(m_pCustomFieldInfo[i-2].m_nPrecision),
									   (SQLSMALLINT*)&(m_pCustomFieldInfo[i-2].m_bAllowNull));

					//�ٴ��ֵ����ȡ�����ֶε�������Ϣ
					CDpRecordset	rst(m_pWks->m_pDB);
					char			szSql[512];
					memset(szSql,0,sizeof(szSql));
					sprintf(szSql,"Select FIELD_ALIAS,FIELD_TYPE,FIELD_PRECISION,FIELD_ISUNIQUE,PARENT_CLASS From DP_ATTR_DICTIONARY Where ATTR_TABLE_NAME = '%s' AND FIELD_NAME = '%s'",
						    m_szAttributeTableName,m_pCustomFieldInfo[i-2].m_strName);
					if (rst.Open(szSql) && rst.Next())
					{
						CDpDBVariant	var;
						if (rst.GetFieldValue(1,var))
							strcpy(m_pCustomFieldInfo[i-2].m_szAliasName,(LPCTSTR)var);
						if (rst.GetFieldValue(2,var))
							m_pCustomFieldInfo[i-2].m_CustomFieldType = DpCustomFieldType((int)var);
						if (rst.GetFieldValue(3,var))
							m_pCustomFieldInfo[i-2].m_nPrecision = (int)var;
						if (rst.GetFieldValue(4,var))
							m_pCustomFieldInfo[i-2].m_bIsUnique = (((int)var) == 1);
						if (rst.GetFieldValue(5,var))
							m_pCustomFieldInfo[i-2].m_iParent	= ((int)var);
					}
					rst.Close();

				}

				m_iAttrFieldCount = iColCount - 2;
			}

			SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
			return true;
		}

	}

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return false;
}


//�õ������ֶεĸ���,������ATT_OID
int CDpLayer::GetAttrFieldCount()
{
	if (!m_pCustomFieldInfo)
		GetAllAttrFieldInfo();

	return m_iAttrFieldCount;
}

//�õ����Ա�ĳ�������ֶε���Ϣ
BOOL CDpLayer::GetAttrFieldInfo(char* szAttrFieldName,CDpCustomFieldInfo* pInfo)
{
	if (!m_pCustomFieldInfo)
		GetAllAttrFieldInfo();

	if (m_iAttrFieldCount > 0)
	{
		for (int i = 0; i < m_iAttrFieldCount; i++)
		{
			if (stricmp(m_pCustomFieldInfo[i].m_strName,szAttrFieldName) == 0)
			{
				memcpy(pInfo,&(m_pCustomFieldInfo[i]),sizeof(CDpCustomFieldInfo));
				return true;
			}

		}
	}

	return false;
}

//�õ�ָ�����ֶ����������ֶε�����,�����ֶε�������0��ʼ
int CDpLayer::GetAttrFieldIndex(char* szAttrFieldName)
{
	if (!szAttrFieldName || strlen(szAttrFieldName) <= 0)
		return -1;

	if (!m_pCustomFieldInfo)
		GetAllAttrFieldInfo();

	if (m_iAttrFieldCount > 0)
	{
		for (int i = 0; i < m_iAttrFieldCount; i++)
		{
			if (stricmp(m_pCustomFieldInfo[i].m_strName,szAttrFieldName) == 0)
				return i;

		}
	}

	return -1;

}



//�õ����Ա�ĳ�������ֶε���Ϣ,uIndex -- ��ʾ�ֶε�����,�ֶε�������0��ʼ
BOOL CDpLayer::GetAttrFieldInfo(UINT uIndex,CDpCustomFieldInfo* pInfo)
{
	if (!m_pCustomFieldInfo)
		GetAllAttrFieldInfo();

	if (uIndex > (m_iAttrFieldCount - 1))
		return false;

	memcpy(pInfo,&(m_pCustomFieldInfo[uIndex]),sizeof(CDpCustomFieldInfo));
	return true;
}


//���������ֶ��Ƿ��ܹ��������
BOOL CDpLayer::CanAddAttrField(CDpCustomFieldInfo* pInfo)
{
	//�жϵ�ǰ��ǰ�û��Ƿ���ϵͳ����Ա.ֻ��ϵͳ����Ա����Ȩ����������ֶ�
	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;


	if (!pInfo || strlen(pInfo->m_strName) <= 0 || 
		pInfo->m_CustomFieldType == DP_CFT_NULL)
		return false;

	//��Ҫ���жϸ���ӵ��ֶ���Ϣ�Ƿ���ȷ
	//�жϵ�������,����ͬ�Ĺ�������,��ͬ���Ƶ��ֶ�������ͼ���ڲ������в�ͬ�ĺ���,��
	//�ֶε��������͵���Ϣ��Ӧ����ͬ
	char		szQuery[2048];
	char		szWksID[256];
	memset(szQuery,0,sizeof(szQuery));
	memset(szWksID,0,sizeof(szWksID));

	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szWksID,"MID(ATTR_TABLE_NAME,2,14)");
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szWksID,"SUBSTR(ATTR_TABLE_NAME,2,14)");
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szWksID,"SUBSTRING(ATTR_TABLE_NAME,2,14)");
	
	sprintf(szQuery,"Select Count(ATTR_TABLE_NAME) From DP_ATTR_DICTIONARY where %s = '%s' AND FIELD_NAME = '%s' AND (FIELD_ALIAS <> '%s' OR FIELD_TYPE <> %d OR FIELD_LENGTH <> %d OR FIELD_PRECISION <> %d OR FIELD_NOTNULLABLE <> %d OR FIELD_ISUNIQUE <> %d)",
			szWksID,m_pWks->m_szID,pInfo->m_strName,pInfo->m_szAliasName,pInfo->m_CustomFieldType,
			pInfo->m_iSize,pInfo->m_nPrecision,(pInfo->m_bAllowNull)?0:1,
			(pInfo->m_bIsUnique)?1:0);

	CDpRecordset	rst(m_pWks->m_pDB);
	if (rst.Open(szQuery))
	{
		if (rst.Next())			
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue((short)1,var))
			{
				//�����ڵ�ǰ��������������ͬ�ֶ������ֶε����Բ�ͬ�������ֶ�,��������Ӹ��ֶ�
				if ((int)var > 0)	
					return false;
			}
			else
				return false;
		}
		else
			return false;

 		rst.Close();
	}
	else 
		return false;

	
	return true;
}


//�������������ֶε�SQL���,����ͬʱ��Ӷ���ֶ�
//pInfo -- Ҫ��ӵ��ֶ����Ե�����,iFieldCount -- Ҫ��ӵ��ֶ���
//szSqlOut -- �������ɵ�SQL���
BOOL CDpLayer::MakeAddAttrFieldSql(CDpCustomFieldInfo* pInfo,int iFieldCount,char* szSqlOut)
{
	if (!pInfo || iFieldCount <= 0 || !szSqlOut)
		return false;

	char		szSqlTmp[8192];
	memset(szSqlTmp,0,sizeof(szSqlTmp));

	for (int i = 0; i < iFieldCount; i++)
	{
		char	szField[100];
		char	szSql[1028];
		memset(szField,0,sizeof(szField));
		memset(szSql,0,sizeof(szSql));

		CDpCustomFieldInfo*	pTmp = &(pInfo[i]);

		switch (pTmp->m_CustomFieldType)
		{
			case DP_CFT_BYTE:
			case DP_CFT_SMALLINT:
			case DP_CFT_BOOL:
				sprintf(szField,"SMALLINT");
				break;
			case DP_CFT_INTEGER:
			case DP_CFT_COLOR:
			case DP_CFT_ENUM:
				if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
					sprintf(szField,"INTEGER");
				else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
					sprintf(szField,"INTEGER");
				else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
					sprintf(szField,"int");
				else
					return false;
				break;
			case DP_CFT_FLOAT:
			case DP_CFT_DOUBLE:
				sprintf(szField,"FLOAT");
				break;
			case DP_CFT_DATE:
				if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
					sprintf(szField,"DATE");
				else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
					sprintf(szField,"DATE");
				else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
					sprintf(szField,"DATETIME");
				else
					return false;

				break;
			case DP_CFT_VARCHAR:
			{
				if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
					sprintf(szField,"VARCHAR(%d)",pTmp->m_iSize);
				else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
					sprintf(szField,"VARCHAR2(%d)",pTmp->m_iSize);
				else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
					sprintf(szField,"VARCHAR(%d)",pTmp->m_iSize);
				else
					return false;
				break;
			}
			case DP_CFT_BLOB:
			{
				if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
					sprintf(szField,"IMAGE");
				else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
					sprintf(szField,"BLOB");
				else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
					sprintf(szField,"IMAGE");
				else
					return false;
				break;
			}
			default:
				return false;
		}

		//�Ƿ�����Ϊ��
		if (!(pTmp->m_bAllowNull))
			strcat(szField," NOT NULL");

		//�Ƿ������ظ�
		if (pTmp->m_bIsUnique && pTmp->m_CustomFieldType != DP_CFT_BLOB)
			strcat(szField," UNIQUE");

		//�����ֶμ��','
		if (i != 0)
			strcat(szSqlTmp,",");

		strcat(szSqlTmp,strupr(pTmp->m_strName));
		strcat(szSqlTmp," ");
		strcat(szSqlTmp,szField);

	}


	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSqlOut,"ALTER TABLE %s ADD %s",m_szAttributeTableName,szSqlTmp);
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSqlOut,"ALTER TABLE %s ADD(%s)",m_szAttributeTableName,szSqlTmp);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSqlOut,"ALTER TABLE %s ADD %s",m_szAttributeTableName,szSqlTmp);
	else
		return false;

	return true;

}


//�����������ֵ������¼��SQL���
BOOL CDpLayer::MakeInsertAttrDircSql(CDpCustomFieldInfo* pInfo,char* szSqlOut)
{
	if (!pInfo)
		return false;

	//�������ֶ��ֵ������¼
	sprintf(szSqlOut,"Insert Into DP_ATTR_DICTIONARY values('%s',0,'%s','%s',%d,%d,%d,%d,%d,%d)",
		    m_szAttributeTableName,strupr(pInfo->m_strName),pInfo->m_szAliasName,pInfo->m_CustomFieldType,
			pInfo->m_iSize,pInfo->m_nPrecision,(pInfo->m_bAllowNull)?0:1,
			(pInfo->m_bIsUnique)?1:0,pInfo->m_iParent);

	return true;
}







//��������ֶ�
//����˵��:  [in]pInfo -- �ֶε�����˵������
BOOL CDpLayer::AddAttrField(CDpCustomFieldInfo* pInfo,bool bRefreshView)
{
	//���жϸ��ֶ��Ƿ�������
	if (!CanAddAttrField(pInfo))
		return false;
	
	char		szSql[512];
	memset(szSql,0,sizeof(szSql));

	//����SQL���
	if (!MakeAddAttrFieldSql(pInfo,1,szSql))
		return false;

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	//�������ֶ��ֵ������¼
	MakeInsertAttrDircSql(pInfo,szSql);
	m_pWks->m_pDB->ExecuteSQL(szSql);

	if (bRefreshView)
	{
		//�ڼ����ֶκ�,�����������ݿ���ȡһ�����Ա���ֶ���Ϣ,���ж��ֶ��Ƿ���ӳɹ�
		int		iCount = 0;
		if (m_pCustomFieldInfo)
		{
			iCount	= GetAttrFieldCount();
			delete m_pCustomFieldInfo;
			m_pCustomFieldInfo = NULL;
		}
		else
			iCount	= 0;

		GetAllAttrFieldInfo();

		if (iCount != GetAttrFieldCount())
		{
			RefreshFeaClsView();
			return true;
		}
		else
			return false;
	}
	else
		return true;
}


//��������ֶ�,һ����Ӷ�������ֶ�
BOOL CDpLayer::AddAttrField(CDpCustomFieldInfo* pFieldInfoArray,int iFieldCount,bool bRefreshView)
{
	//���ж���Щ�����ֶ��Ƿ��ܱ����
	if (!pFieldInfoArray || iFieldCount <= 0)
		return false;

	for (int i = 0; i < iFieldCount; i++)
	{
		if (!CanAddAttrField(&(pFieldInfoArray[i])))
			return false;
	}

	//����SQL���
	char		szSql[8196];
	if (!MakeAddAttrFieldSql(pFieldInfoArray,iFieldCount,szSql))
		return false;

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	for (i = 0; i < iFieldCount; i++)
	{
		//�������ֶ��ֵ������¼
		if (MakeInsertAttrDircSql(&(pFieldInfoArray[i]),szSql))
			m_pWks->m_pDB->ExecuteSQL(szSql);
	}

	if (bRefreshView)
	{
		//�ڼ����ֶκ�,�����������ݿ���ȡһ�����Ա���ֶ���Ϣ,���ж��ֶ��Ƿ���ӳɹ�
		int		iCount = 0;
		if (m_pCustomFieldInfo)
		{
			iCount	= GetAttrFieldCount();
			delete m_pCustomFieldInfo;
			m_pCustomFieldInfo = NULL;
		}
		else
			iCount	= 0;

		GetAllAttrFieldInfo();

		if (iCount != GetAttrFieldCount())
		{
			RefreshFeaClsView();
			return true;
		}
		else
			return false;
	}
	else
		return true;

}

//ˢ��FeatureClass����ͼ,��Ҫ��ˢ�½ṹ.��Ϊ�����Ա��޸������������Ҫ����ͼ����ˢ��
BOOL CDpLayer::RefreshFeaClsView()
{
	char	szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Drop View FeaCls%s",m_szID);
	m_pWks->m_pDB->ExecuteSQL(szSql);

	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Create View FeaCls%s AS Select A.*,B.* From S%s A,A%s B where A.OID = B.ATT_OID AND A.ST_GRID_ID = B.ATT_GRID_ID",
			m_szID,m_szID,m_szID);
	m_pWks->m_pDB->ExecuteSQL(szSql);

	return true;
}




//ɾ�������ֶ�,iIndex - Ϊ�ֶε�������,��0��ʼ
BOOL CDpLayer::RemoveAttrField(int iIndex)
{
	//�жϵ�ǰ��ǰ�û��Ƿ���ϵͳ����Ա.ֻ��ϵͳ����Ա����Ȩ��ɾ�������ֶ�
	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;


	int		iCount	= GetAttrFieldCount();
	if (iCount <= 0 || iIndex > iCount - 1 || iIndex < 0)
		return false;

	//�õ��ֶ���
	char*	szName = m_pCustomFieldInfo[iIndex].m_strName;

	char	szSql[512];

	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"ALTER TABLE %s DROP COLUMN %s",m_szAttributeTableName,szName);
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"ALTER TABLE %s DROP COLUMN %s",m_szAttributeTableName,szName);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"ALTER TABLE %s DROP COLUMN %s",m_szAttributeTableName,szName);
	else
		return false;

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	sprintf(szSql,"Delete From DP_ATTR_DICTIONARY Where ATTR_TABLE_NAME = '%s' AND FIELD_NAME = '%s'",
			m_szAttributeTableName,szName);
	m_pWks->m_pDB->ExecuteSQL(szSql);

	delete m_pCustomFieldInfo;
	m_pCustomFieldInfo = NULL;
	GetAllAttrFieldInfo();

	return true;

}










//����������
BOOL CDpLayer::CreateParamTable()
{
	if (CDpMgrObject::CreateParamTable())
	{
		char		szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Update DP_LayersMgr Set PARAM_TABLE_NAME = '%s' Where LAYER_ID = '%s'",
				m_szParamTableName,m_szID);
		m_pWks->m_pDB->ExecuteSQL(szSql);
		return true;
	}
	else
		return false;
}

//ɾ��������
BOOL CDpLayer::DeleteParamTable()
{
	if (CDpMgrObject::DeleteParamTable())
	{
		char		szSql[256];
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Update DP_LayersMgr Set PARAM_TABLE_NAME = NULL Where LAYER_ID = '%s'",
				m_szID);
		m_pWks->m_pDB->ExecuteSQL(szSql);
		return true;
	}
	else
		return false;

}



//�õ����������ֶε�ֵ
BOOL CDpLayer::GetAttrFieldValue(GUID uOID,const char* szUnitID,
								 const char* szFieldName,CDpDBVariant& var)
{
	if (!szFieldName || strlen(szFieldName) <= 0 || !szUnitID ||
		!m_szAttributeTableName || strlen(m_szAttributeTableName) <= 0)
		return false;

	int	iCount = GetAttrFieldCount();
	if (iCount <= 0)
		return false;

	//�ж��Ƿ���Ȩ�޷��ʸ�OID���ڵ�ͼ����ͼ�� 
	DP_RIGHT		nRight;
	if (!m_pWks->m_pDB->m_pRightMgr->GetUnitLayerRight(szUnitID,m_szID,&nRight) || 
		nRight == DP_RIGHT_NOTRIGHT)
		return false;

	BOOL bFind	= false;
	//�ж����Ա����Ƿ��и��ֶ�
	for (int i = 0; i < iCount; i++)
	{
		if (_stricmp(m_pCustomFieldInfo[i].m_strName,szFieldName) == 0)
		{
			bFind = true;
			break;
		}
	}
	if (!bFind)
		return false;

	CDpRecordset	rst(m_pWks->m_pDB);
	char			szSql[200];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select %s From %s Where ATT_OID = '%s' AND ATT_GRID_ID = '%s'",
		    szFieldName,m_szAttributeTableName,GUIDToString(uOID),szUnitID);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
				return true;
		}
	}



	return true;
}


//���õ��������ֶε�ֵ
BOOL CDpLayer::SetAttrFieldValue(GUID uOID,const char* szUnitID,
								 const char* szFieldName,CDpDBVariant& var)
{
	if (!szFieldName || strlen(szFieldName) <= 0 || !szUnitID || 
		!m_szAttributeTableName || strlen(m_szAttributeTableName) <= 0)
		return false;

	int	iCount = GetAttrFieldCount();
	if (iCount <= 0)
		return false;
	BOOL bFind	= false;
	//�ж����Ա����Ƿ��и��ֶ�
	for (int i = 0; i < iCount; i++)
	{
		if (_stricmp(m_pCustomFieldInfo[i].m_strName,szFieldName) == 0)
		{
			bFind = true;
			break;
		}
	}
	if (!bFind)
		return false;

	//�ж��Ƿ���Ȩ���޸ĸõ���
	if (!CanModifyCurrUnit(szUnitID))
		return false;

	char		szSql[2048];
	char		szWhere[100];
	memset(szSql,0,sizeof(szSql));
	memset(szWhere,0,sizeof(szWhere));
	sprintf(szWhere," Where ATT_OID = '%s' AND ATT_GRID_ID = '%s'",GUIDToString(uOID),szUnitID);

	if (BuildUpdateSql(m_pWks->m_pDB->GetDBType(),m_szAttributeTableName,
		               szFieldName,szWhere,m_pCustomFieldInfo[i].m_nSQLType,
					   var,szSql))
		return (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);

	return false;
}



//�������÷���ĳ�ֲ�ѯ����������ֵ
//����˵��:  nFilter       -- Ҫ���µļ�¼�Ĺ�������
//           pNewAttValue  -- Ҫ���µ��ֶε�ֵ
//����ֵ:    �ɹ��򷵻�ʵ�ʸ��µļ�¼��,ʧ�ܻ�û�и��¼�¼�򷵻�0
int CDpLayer::BatchSetAttrFieldValue(CDpUniqueFilter* nFilter,
									 CDpUniqueFilter* pNewAttValue)
{
	if (!pNewAttValue)
		return 0;

	int iCount	= pNewAttValue->GetFilterCount();
	if (iCount <= 0)
		return 0;

	//�ȵõ����Ա�������ֶε���Ϣ
	if (!GetAllAttrFieldInfo())
		return 0;

	//Ȩ�޹�������
	char	szRightFilter[1024];
	memset(szRightFilter,0,sizeof(szRightFilter));

	//�������޸ĵ�ͬʱ�������ֻ���޸���Ȩ�޵ļ�¼���ж�,�޸ı��:NO.050220
	//Ȩ���ж��а������и�ͼ���µĸ��µ�ͼ����,���û��Ƿ����޸�Ȩ�޲����Ƿ������û�����
	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		sprintf(szRightFilter,"ATT_GRID_ID in (Select GRID_ID From DP_V_USERRIGHT_UNIT_LAYER Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND LAYER_ID = '%s' AND MRIGHT > 1 AND GRID_ID NOT IN (SELECT UNIT_ID FROM DP_UNIT_LOCKMGR WHERE WORKSPACE_ID = '%s' AND LAYER_ID = '%s' AND LOCKER_NAME <> '%s'))",
				m_szID,m_pWks->m_szID,m_pWks->m_pDB->m_szUserID,m_szID,
				m_pWks->m_szID,m_szID,m_pWks->m_pDB->m_szUserID);
	

	else
		sprintf(szRightFilter,"ATT_GRID_ID NOT IN (SELECT UNIT_ID FROM DP_UNIT_LOCKMGR WHERE WORKSPACE_ID = '%s' AND LAYER_ID = '%s' AND LOCKER_NAME <> '%s')",
				m_szID,m_pWks->m_szID,m_pWks->m_pDB->m_szUserID,m_szID,
				m_pWks->m_szID,m_szID,m_pWks->m_pDB->m_szUserID);


	//�ȵõ����µ�WHERE�Ӿ�
	char	szWhereClause[2048];
	memset(szWhereClause,0,sizeof(szWhereClause));

	if (nFilter && nFilter->GetFilterCount() > 0 && 
		nFilter->MakeUniqueFilterWhereClause(szWhereClause,sizeof(szWhereClause)) && 
		strlen(szWhereClause) > 0)
	{
		if (strlen(szRightFilter) > 0)
		{
			strcat(szWhereClause," AND ");
			strcat(szWhereClause,szRightFilter);
		}
	}
	else
		strcpy(szWhereClause,szRightFilter);

	//���ɸ����ֶε��Ӿ�
	char	szUpdateClause[2048];
	memset(szUpdateClause,0,sizeof(szUpdateClause));

	for (int i = 0; i < iCount; i++)
	{
		const _DpUniqueFilter*	pFilter	= NULL;
		if (!pNewAttValue->GetFilter(i,&pFilter) || !pFilter)
			continue;
		if (!(pFilter->szFieldName)	|| strlen(pFilter->szFieldName) <= 0 || 
			!(pFilter->szFilter))
			continue;
		
		CDpCustomFieldInfo	info;
		memset(&info,0,sizeof(CDpCustomFieldInfo));
		//�õ����ֶε�����
		if (!GetAttrFieldInfo(pFilter->szFieldName,&info))
			continue;

		char		szNewValue[256];
		memset(szNewValue,0,sizeof(szNewValue));

		switch (info.m_CustomFieldType)
		{
			case DP_CFT_BYTE:			
			case DP_CFT_SMALLINT:
			case DP_CFT_INTEGER:
			case DP_CFT_BOOL:
			case DP_CFT_COLOR:
			case DP_CFT_ENUM:
				sprintf(szNewValue," = %d ",atoi(pFilter->szFilter));
				break;
			case DP_CFT_FLOAT:
			case DP_CFT_DOUBLE:
				sprintf(szNewValue," = %f ",atof(pFilter->szFilter));
				break;
			case DP_CFT_VARCHAR:
				sprintf(szNewValue," = '%s' ",pFilter->szFilter);
				break;
			case DP_CFT_DATE:
			{
				//���������ݵĸ�����ʱ��ʵ��
				break;
			}
			default:
				continue;
				break;

		}

		//���ɸ������
		if (strlen(szUpdateClause) > 0)
			strcat(szUpdateClause," , ");		//����','

		strcat(szUpdateClause,pFilter->szFieldName);
		strcat(szUpdateClause,szNewValue);

	}

	if (strlen(szUpdateClause) <= 0)
		return 0;

	//�������յ�SQL���
	char		szSql[8192];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update A%s Set %s Where %s",m_szID,szUpdateClause,szWhereClause);

	//ִ�и���SQL���
	SQLHANDLE	hStmt	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pWks->m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return 0;
	if (!hStmt)
		return 0;
	SQLINTEGER		iLen = strlen(szSql);
	TRACE(szSql);
	if (SQLExecDirect(hStmt,(SQLCHAR*)szSql,iLen) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return 0;
	}

	//�õ����µļ�¼��
	SQLLEN			iRetCount = 0;
	SQLRowCount(hStmt,&iRetCount);

	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);


	return iRetCount;
}

int CDpLayer::GetRecordCount()
{
	char			szSql[100];
	SQLINTEGER		iCount			= 0;			//��Ŀ
	SQLLEN		cbLen = 0;			//�ֶγ���
	SQLHANDLE		hStmt			= NULL;
	
	SQLRETURN		retcode;
	
	memset(szSql,0,sizeof(szSql));

	sprintf(szSql,"Select Count(*) From %s",m_szAttributeTableName);

	
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pWks->m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return 0;
	
	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return 0;
	}
	
	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		retcode = SQLFetch(hStmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			SQLGetData(hStmt,1,SQL_C_SHORT,&iCount,sizeof(iCount),&cbLen);			//ȡͼ����Ŀ
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return (int)iCount;
}

//ͨ��Feature������ͼ�����Ա��в����¼��SQL���
//����˵��: [in]pFeature -- Ҫ���뵽���ݿ��Feature   [in]nOID -- Ҫ���뵽���ݿ��Feature��OID
//          [out]szSql   -- ���ص�Insert���          [in]iBuffCount -- szSql�������ĳ���
BOOL CDpLayer::MakeInsertSql(CDpObject* pObject,GUID nOID,char* szSql,int iBuffCount)
{
	if (!pObject || !szSql || iBuffCount <= 0)
		return false;

	int		iCount	= pObject->GetAttrFieldCount();

	char	szTmp[10240];

		//�ٽ��������������ݵ�SQL���
	if (iCount > 0)
	{
		sprintf(szTmp,"Insert Into %s Values('%s','%s'",m_szAttributeTableName,GUIDToString(nOID),pObject->GetUnitID());
		for (int i = 0; i < iCount; i++)
		{
			CDpDBVariant	var;
			pObject->GetAttrValue(i,var);

			char				szFieldValue[300];
			memset(szFieldValue,0,sizeof(szFieldValue));

			CDpCustomFieldInfo	info;
			GetAttrFieldInfo(i,&info);

			switch (info.m_CustomFieldType)
			{
				case DP_CFT_BYTE:
				case DP_CFT_SMALLINT:
				case DP_CFT_INTEGER:
				case DP_CFT_BOOL:
				case DP_CFT_COLOR:
				case DP_CFT_ENUM:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_BOOL && 
						var.m_dwType != DPDBVT_UCHAR && 
						var.m_dwType != DPDBVT_SHORT && 
						var.m_dwType != DPDBVT_LONG)
						return false;
					sprintf(szFieldValue,"%d",(int)var);
					break;
				}
				case DP_CFT_FLOAT:
				case DP_CFT_DOUBLE:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_SINGLE && 
						var.m_dwType != DPDBVT_DOUBLE)
						return false;
					sprintf(szFieldValue,"%f",(double)var);
					break;
				}
				case DP_CFT_VARCHAR:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_STRING)
						return false;
					sprintf(szFieldValue,"'%s'",(LPCTSTR)var);
					break;
				}
				case DP_CFT_DATE:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_DATE)
						return false;
					if (var.m_dwType == DPDBVT_DATE)
						BuildDateTimeSql(m_pWks->m_pDB->GetDBType(),var.m_pdate,szFieldValue);
					else
						sprintf(szFieldValue,"NULL");
					break;
				}
				case DP_CFT_BLOB:					//Ŀǰ�����ֶβ�֧�ֶ���������
				{
					sprintf(szFieldValue,"NULL");
					break;
				}
				default:
					return false;

			}

			strcat(szTmp," , ");
			strcat(szTmp,szFieldValue);
		}
		strcat(szTmp," ) ");

		if (strlen(szTmp) > (iBuffCount-1))
			return false;

		strcpy(szSql,szTmp);
		return true;
	}

	return false;
}

//ͨ��Feature������ͼ�����Ա���¼�¼��SQL���
BOOL CDpLayer::MakeUpdateSql(CDpObject* pObject,char* szSql,int iBuffCount)
{
	if (!pObject || !szSql || iBuffCount <= 0)
		return false;

	int		iCount	= pObject->GetAttrFieldCount();
	if (iCount <= 0)
		return true;

	char	szTmp[10240];

	sprintf(szTmp,"Update %s Set ",m_szAttributeTableName);
	for (int i = 0; i < iCount; i++)
	{
		CDpDBVariant	var;
		pObject->GetAttrValue(i,var);

		char				szFieldValue[300];
		memset(szFieldValue,0,sizeof(szFieldValue));

		CDpCustomFieldInfo	info;
		GetAttrFieldInfo(i,&info);

		switch (info.m_CustomFieldType)
		{
			case DP_CFT_BYTE:
			case DP_CFT_SMALLINT:
			case DP_CFT_INTEGER:
			case DP_CFT_BOOL:
			case DP_CFT_COLOR:
			case DP_CFT_ENUM:
			{
				if (var.m_dwType != DPDBVT_NULL &&
					var.m_dwType != DPDBVT_BOOL && 
					var.m_dwType != DPDBVT_UCHAR && 
					var.m_dwType != DPDBVT_SHORT && 
					var.m_dwType != DPDBVT_LONG)
					return false;
				sprintf(szFieldValue," %s = %d ",info.m_strName,(int)var);
				break;
			}
			case DP_CFT_FLOAT:
			case DP_CFT_DOUBLE:
			{
				if (var.m_dwType != DPDBVT_NULL &&
					var.m_dwType != DPDBVT_SINGLE && 
					var.m_dwType != DPDBVT_DOUBLE)
					return false;
				sprintf(szFieldValue," %s = %f ",info.m_strName,(double)var);
				break;
			}
			case DP_CFT_VARCHAR:
			{
				if (var.m_dwType != DPDBVT_NULL &&
					var.m_dwType != DPDBVT_STRING)
					return false;
				sprintf(szFieldValue," %s = '%s' ",info.m_strName,(LPCTSTR)var);
				break;
			}
			case DP_CFT_DATE:
			{
				if (var.m_dwType != DPDBVT_NULL &&
					var.m_dwType != DPDBVT_DATE)
					return false;
				char		szDate[80];
				if (var.m_dwType == DPDBVT_DATE)
					BuildDateTimeSql(m_pWks->m_pDB->GetDBType(),var.m_pdate,szDate);
				else
					sprintf(szFieldValue,"NULL");
				sprintf(szFieldValue," %s = %s ",info.m_strName,szDate);
				break;
			}
			case DP_CFT_BLOB:					//Ŀǰ�����ֶβ�֧�ֶ���������
			{
				sprintf(szFieldValue,"NULL");
				break;
			}
			default:
				return false;
		}

		if (i > 0)
			strcat(szTmp," , ");
		strcat(szTmp,szFieldValue);
	}

	char	szWhere[128];

	sprintf(szWhere," WHERE ATT_OID = '%s' AND ATT_GRID_ID = '%s'",GUIDToString(pObject->GetOID()),pObject->GetUnitID());
	strcat(szTmp,szWhere);

	if (strlen(szTmp) > (iBuffCount - 1))
		return false;

	strcpy(szSql,szTmp);

	return true;
}

//�õ���ǰ����֧�ֵĲ�
BOOL CDpLayer::GetSupportClassIDs(CDpIntArray& nClassIDArray)
{
	int		iCount	= GetAttrFieldCount();
	
	for (int i = 0; i < iCount; i++)
	{
		if (!nClassIDArray.ItemIsExist(m_pCustomFieldInfo[i].m_iParent))
			nClassIDArray.AddItem(m_pCustomFieldInfo[i].m_iParent);
	}
	return true;
}

//�õ���ǰ������������,ͼ��/ͼ��/ԭʼӰ��
DP_OBJECT_TYPE CDpLayer::GetObjType()
{
	return DP_OBJ_LAYER;
}

//�õ���ǰ�������ĳɹ�����,ֻ�е����������ͼ��ʱ����Ч
DP_LAYER_TYPE_EX CDpLayer::GetLayerTypeEx()
{
	return m_iLayerTypeEx;
}


//�޸�ͼ������
BOOL CDpLayer::ModifyLayerName(const char* szNewLayerName)
{
	if (!szNewLayerName)
		return false;

	int		iLen = strlen(szNewLayerName);
	if (iLen <= 0)
		return false;

	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

	char		szSql[256];
	sprintf(szSql,"Update DP_LayersMgr Set LAYER_NAME = '%s' Where WORKSPACE_ID = '%s' AND LAYER_ID = '%s'",
		    szNewLayerName,m_pWks->m_szID,m_szID);
	if (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		if (m_szName)
			delete [] m_szName;
		m_szName = new char[iLen+1];
		memset(m_szName,0,iLen+1);
		strcpy(m_szName,szNewLayerName);
		return true;
	}
	else
		return false;
}



/*******************************************************************/
/*                       ʸ������������                          */
/*******************************************************************/
CDpFeatureLayer::CDpFeatureLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType)
                :CDpLayer(pWks,iLayerType)
{


}

CDpFeatureLayer::~CDpFeatureLayer()
{

}


//��ͼ�������һ������
//����˵��:  [in]szGridID -- Ҫ��ӵĵ���������ͼ��ID  [in]pShape -- ����
//           [in,out]pNewID -- ��ӳɹ��󷵻��µĵ����OID,���ʧ�ܷ���0,�������Ҫ
//                             ����ΪNULL
//			 [in]bInsertEmptyAttr -- �Ƿ������Ա��в���һ���յ�����ֵ
//                                   һ����ֱ����ӵ����ʱ�������Ա������һ���յ����Լ�¼
//                                    ����ͨ��AddFeature��ӵ�ʱ��Ͳ�����յ����Լ�¼,��Ϊ
//                                    AddFeature��������в����¼�Ĳ���
//           [in]uOID     -- �������uOID��Ϊһ,���ʾ����ӵ����ʱ����Ҫȥ�������ĵ���ID,����ֱ��ʹ�øò���,
//                           ���Ҳ����ж��Ƿ��б༭ͼ����Ȩ��
BOOL CDpFeatureLayer::AddShape(const char* szGridID,CDpShape* pShape,GUID* pNewOID,
							   BOOL bInsertEmptyAttr,GUID *uOID)
{
	if (!szGridID || !pShape || pShape->GetShapeType() != m_iLayerType)
		return false;

	if (pShape->GetPartCount() <= 0 || pShape->GetPointCount() <= 0)
		return false;

	if (pNewOID)
	{
		ZeroMem(*pNewOID);
	}

	CDpDatabase*	db		= m_pWks->m_pDB;

	//�ж��û���û����ͼ��szGridIDд��Ȩ��
	if (uOID==NULL || IsZeroGUID(uOID))
	{
		if (!CanModifyCurrUnit(szGridID))
			return false;
	}

	const CDpShapeData*	pData	= NULL;					//��ָ��ָ����ڴ治��Ҫ�������ͷ�,CDpShape������ͷ�
	if (!pShape->GetData(&pData) || !pData)
		return false;
	
	GUID uMaxID;
	ZeroMem(uMaxID);
	if (uOID==NULL || IsZeroGUID(uOID))
	{
		uMaxID = NewGUID();
	}
	else
		uMaxID = *uOID;
 
	BYTE*			pBuff	= NULL;						//��ָ��ָ����ڴ���Ҫ�������ͷ�
	int				cbLen	= 0;
	if (!pShape->GetData(&pBuff,&cbLen) || !pBuff || cbLen <= 0)
		return false;

	char		szSql[2048];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Insert Into %s values('%s','%s',%f,%f,%f,%f,%d,?)",
		    m_szSpatialTableName,GUIDToString(uMaxID),szGridID,pData->dBound[0],
			pData->dBound[1],pData->dBound[2],
			pData->dBound[3],(int)m_iLayerType);

	BOOL bRet = SetLargerFieldValue(db->GetDBHandle(),szSql,pBuff,cbLen);
	delete [] pBuff;									//�ͷ��ڴ�					
	pBuff = NULL;
	
	if (bRet)				//�������ʸ�����ݳɹ�,���ٲ����Ӧ�������ֶ�
	{
		if (bInsertEmptyAttr)
		{
			memset(szSql,0,sizeof(szSql));
			sprintf(szSql,"Insert Into %s(ATT_OID,ATT_GRID_ID) values('%s','%s')",
					m_szAttributeTableName,GUIDToString(uMaxID),szGridID);
			bRet = (db->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
		}
		if (bRet && pNewOID)
			*pNewOID = uMaxID;
	}


	return bRet;
}


//ɾ������,ͬʱɾ����������
BOOL CDpFeatureLayer::DelShape(GUID OID,const char* szUnitID)
{
	if (IsZeroGUID(&OID) || !szUnitID)
		return false;

	//�ж���û��ɾ���õ����Ȩ��
	if (!CanModifyCurrUnit(szUnitID))
		return false;

	CDpDatabase*	db		= m_pWks->m_pDB;

	char		szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From %s Where OID = '%s' AND ST_GRID_ID = '%s'",
		    m_szSpatialTableName,GUIDToString(OID),szUnitID);
	
	if (db->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Delete From %s Where ATT_OID = '%s' AND ATT_GRID_ID = '%s'",
			    m_szAttributeTableName,GUIDToString(OID),szUnitID);
		if (db->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
			return true;
		else
			return false;
	}
	else
		return false;
}


//�༭����
BOOL CDpFeatureLayer::SetShape(GUID OID,const char* szUnitID,CDpShape* pShape)
{
	if (IsZeroGUID(&OID) || !pShape || !szUnitID)
		return false;

	//�ж���û��ɾ���õ����Ȩ��
	if (!CanModifyCurrUnit(szUnitID))
		return false;

	CDpDatabase*	db		= m_pWks->m_pDB;

	const CDpShapeData*	pData	= NULL;						//��ָ��ָ����ڴ治��Ҫ�������ͷ�,CDpShape������ͷ�
	if (!pShape->GetData(&pData) || !pData)
		return false;
	
	BYTE*			pBuff	= NULL;						//��ָ��ָ����ڴ���Ҫ�������ͷ�
	int				cbLen	= 0;
	if (!pShape->GetData(&pBuff,&cbLen) || !pBuff || cbLen <= 0)
		return false;

	char		szSql[2048];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update %s set ST_MINX = %f,ST_MINY = %f,ST_MAXX = %f,ST_MAXY = %f,SPATIAL_DATA = ? where OID = '%s' AND ST_GRID_ID = '%s'",
		    m_szSpatialTableName,pData->dBound[0],pData->dBound[1],pData->dBound[2],pData->dBound[3],GUIDToString(OID),szUnitID);

	BOOL bRet = SetLargerFieldValue(db->GetDBHandle(),szSql,pBuff,cbLen);
	delete [] pBuff;	
	pBuff =NULL;//�ͷ��ڴ�					

	return bRet;
}


//�򿪵�ǰͼ�����Ĳ�ѯ,����ִ�гɹ��õ��Ĳ�ѯ������Ҫ�������ͷ��ڴ�
//����˵��:  [in]  pSpaFilter -- �ռ��������,�������Ҫ����,������NULL����
//           [out] ppGeoQuery -- ִ�гɹ����ز�ѯ��ѯ����,ʧ���򷵻�NULL
BOOL CDpFeatureLayer::OpenGeometryQuery(CDpSpatialQuery* pSpaFilter,CDpGeometryQuery** ppGeometryQuery)
{
	if (!ppGeometryQuery || LAYER_IS_RASTER(m_iLayerType))
		return false;

	*ppGeometryQuery		= NULL;

	//�����ռ����ݲ�ѯ����
	char		szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select * From %s",m_szSpatialTableName);

	//Ȩ�޿��ƹ������
	char	szRightFilter[512];
	memset(szRightFilter,0,sizeof(szRightFilter));
	//�����ϵͳ����Ա���ߵ�ǰ���ݿ��Ǳ��ص��ļ����ݿ�ʱ,����Ҫ����Ȩ�޵Ĺ������
	if (!m_pWks->m_pDB->m_pRightMgr->MakeRightFilterClauseWhenOpenQuery(m_pWks->m_szID,m_szID,szRightFilter))
		return false;

	if (strlen(szRightFilter) > 0)
	{
		strcat(szSql," WHERE ");
		strcat(szSql,szRightFilter);
	}

	*ppGeometryQuery = new CDpGeometryQuery(m_pWks);
	
	if (!((*ppGeometryQuery)->Open(szSql,pSpaFilter)))
	{
		delete *ppGeometryQuery;
		*ppGeometryQuery = NULL;	
		return false;
	}
	else
		return true;

}

//�򿪵�ǰͼ���Feature�Ĳ�ѯ
//����˵��:  [in]szWhereClause  -- ��ѯ��WHERE�Ӿ�,����ͨ�����Ӿ����Բ�ѯ������й���,
//                                 ���б�����WHERE��ͷ,ͬʱҲ���԰���ORDER BY
//                                 ��Ҫ����ͨ�������ֶ������˼�¼,�������Ҫ,������ΪNULL
//           [in]pSpaFilter     -- �ռ����,�������Ҫ,������ΪNULL
//           [out] ppFeatureCls -- �ɹ�ʱ���ش򿪵�CDpFeatureClass����,ʧ��ʱ����NULL
BOOL CDpFeatureLayer::OpenFeatureClass(CDpStringArray* pUnitIDArray,const char* szWhereClause,
									   CDpSpatialQuery* pSpaFilter,CDpFeatureClass** ppFeatureCls)
{
	if (!ppFeatureCls || LAYER_IS_RASTER(m_iLayerType))
		return false;
	
	*ppFeatureCls = NULL;

	//�ȵõ�FeatureClass������
	char		szFeaClsName[50];
	memset(szFeaClsName,0,sizeof(szFeaClsName));
	if (!GetGeoClassName(szFeaClsName,sizeof(szFeaClsName)))
		return false;

	//Ȩ�޿��ƹ������
	char	szRightFilter[512];
	memset(szRightFilter,0,sizeof(szRightFilter));
	//�����ϵͳ����Ա���ߵ�ǰ���ݿ��Ǳ��ص��ļ����ݿ�ʱ,����Ҫ����Ȩ�޵Ĺ������
	if (!m_pWks->m_pDB->m_pRightMgr->MakeRightFilterClauseWhenOpenQuery(m_pWks->m_szID,m_szID,szRightFilter))
		return false;


	//����Ҫ��򿪵�ͼ����SQL��� 
	//Ϊ����Ӧ���ͼ���ܷ���,���ڴ����Ҳ��Ϊ��̬����
//	char	szUnitWhere[2048];
	char*	szUnitWhere		= NULL;
	int		iUnitWhereSize	= 0;
//	memset(szUnitWhere,0,sizeof(szUnitWhere));

	if (pUnitIDArray)
	{
		int		iCount = pUnitIDArray->GetCount();
		if (iCount > 0)
		{
			iUnitWhereSize = iCount*22+128;
			szUnitWhere = new char[iUnitWhereSize];
			memset(szUnitWhere,0,iUnitWhereSize);
			sprintf(szUnitWhere," ST_GRID_ID IN (");

			for (int i = 0; i < iCount; i++)
			{
				if (i != 0)
					strcat(szUnitWhere,",");

				strcat(szUnitWhere,"'");
				strcat(szUnitWhere,pUnitIDArray->GetItem(i));
				strcat(szUnitWhere,"'");
			}
			strcat(szUnitWhere,")");
		}
	}

	//���ɲ�ѯSQL
	int			szSqlSize	= strlen(szRightFilter) + iUnitWhereSize + (szWhereClause?strlen(szWhereClause):0) + 256;
	char*		szSql		= new char[szSqlSize];
	memset(szSql,0,szSqlSize);


	if (szWhereClause && strlen(szWhereClause) > 6)
	{
		sprintf(szSql,"Select * From %s %s",szFeaClsName,szWhereClause);

		//����ͼ������
		if (szUnitWhere && strlen(szUnitWhere) > 0)
		{
			strcat(szSql," AND ");
			strcat(szSql,szUnitWhere);
		}

		//����Ȩ�޿���
		if (strlen(szRightFilter) > 0)
		{
			strcat(szSql," AND ");
			strcat(szSql,szRightFilter);
		}
	}
	else
	{
		sprintf(szSql,"Select * From %s ",szFeaClsName);

		//����Ȩ�޿���
		if (strlen(szRightFilter) > 0)
		{
			strcat(szSql," WHERE ");
			strcat(szSql,szRightFilter);

			//����ͼ������
			if (szUnitWhere && strlen(szUnitWhere) > 0)
			{
				strcat(szSql," AND ");
				strcat(szSql,szUnitWhere);
			}
		}
		else
		{
			//����ͼ������
			if (szUnitWhere && strlen(szUnitWhere) > 0)
			{
				strcat(szSql," WHERE ");
				strcat(szSql,szUnitWhere);
			}

		}

	}
	
	//����FeatrueClass����
	*ppFeatureCls	= new CDpFeatureClass(m_pWks,m_iLayerType,szFeaClsName);
	BOOL bRet = (*ppFeatureCls)->Open(szSql,pSpaFilter);

	if (!bRet)
	{
		delete *ppFeatureCls;
		*ppFeatureCls = NULL;

	}

	if (szSql)	
	{
		delete [] szSql;
		szSql = NULL;
	}
	if (szUnitWhere)
	{
		delete [] szUnitWhere;
		szUnitWhere = NULL;
	}
	
	return bRet;

}

BOOL CDpFeatureLayer::QueryFeatureClass(CDpStringArray* pSelectCol,const char *pPredicate,const char* szWhereClause,CDpFeatureClass** ppFeatureCls)
{
	if (pSelectCol==NULL||pSelectCol->GetCount()<=0||!ppFeatureCls || LAYER_IS_RASTER(m_iLayerType))
		return false;
	
	*ppFeatureCls = NULL;

	//�ȵõ�FeatureClass������
	char		szFeaClsName[50];
	memset(szFeaClsName,0,sizeof(szFeaClsName));
	if (!GetGeoClassName(szFeaClsName,sizeof(szFeaClsName)))
		return false;

	//���ɲ�ѯSQL
	int			szSqlSize = 0;
	for (int i=0;i<pSelectCol->GetCount();i++)
	{
		szSqlSize+=strlen(pSelectCol->GetItem(i));
	}
	
	szSqlSize	= 65  + strlen(pPredicate) + szSqlSize + (szWhereClause?strlen(szWhereClause):0) + 256;
	char*		szSql		= new char[szSqlSize];
	memset(szSql,0,szSqlSize);
	
	sprintf(szSql,"Select %s ",pPredicate);
	for (i=0;i<pSelectCol->GetCount();i++)
	{
		strcat(szSql,pSelectCol->GetItem(i));
		strcat(szSql," ");		
	}

	strcat(szSql,"From ");
	strcat(szSql,szFeaClsName);
	if (szWhereClause!=NULL&&strlen(szWhereClause)>0)
	{
		strcat(szSql," ");
		strcat(szSql,szWhereClause);
	}
	
	//����FeatrueClass����
	*ppFeatureCls	= new CDpFeatureClass(m_pWks,m_iLayerType,szFeaClsName);
	BOOL bRet = (*ppFeatureCls)->Open(szSql,NULL);
	
	if (!bRet)
	{
		delete *ppFeatureCls;
		*ppFeatureCls = NULL;
		
	}
	if (szSql)	
	{
		delete [] szSql;
		szSql = NULL;
	}
	return bRet;
}

//��ǰʸ���������һ��Feature
BOOL CDpFeatureLayer::AddFeature(CDpFeature* pFeature)
{
	if (!pFeature)
		return false;

	const CDpShape*		pShape		= NULL;
	pFeature->GetShape(&pShape);

	if (!pShape)
		return false;

	int	iCount = this->GetAttrFieldCount();

	if (iCount != pFeature->GetAttrFieldCount())
		return false;

	char			szSql[10240];

	//�����Feature�Ŀ�ʼ��ʼ����
	m_pWks->m_pDB->BeginTrans();
	
	//�ȳ�����ǰ��ʸ�����ݱ�����ӵ�ǰʸ��,���а����˶�Ȩ�޵��ж�
	GUID		uiNewOID;
	ZeroMem(uiNewOID);
	if (IsZeroGUID(&pFeature->GetOID()))
	{
		if (!AddShape((char*)(pFeature->GetUnitID()),(CDpShape*)pShape,&uiNewOID,false) || IsZeroGUID(&uiNewOID))
			goto AddErr;
	}
	else
	{
		if (!AddShape((char*)(pFeature->GetUnitID()),(CDpShape*)pShape,&uiNewOID,false,&pFeature->GetOID()) || IsZeroGUID(&uiNewOID))
			goto AddErr;
	}

	//�ٽ��������������ݵ�SQL���
	if (!MakeInsertSql(pFeature,uiNewOID,szSql,sizeof(szSql)))
		goto AddErr;

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto AddErr;

	m_pWks->m_pDB->CommitTrans();

	if (IsZeroGUID(&pFeature->GetOID()))
		pFeature->SetOID(uiNewOID);						//����ӵ�Feature�����OID�޸�
	return true;


AddErr:
	m_pWks->m_pDB->Rollback();
	return false;

}


BOOL CDpFeatureLayer::FindFeature(GUID OID)
{
	char szSql[1024] = {0};
	sprintf(szSql, "SELECT * From %s WHERE ATT_OID = '%s' ", 
		m_szAttributeTableName, GUIDToString(OID) );

	//�ȵõ�FeatureClass������
	char		szFeaClsName[50];
	memset(szFeaClsName,0,sizeof(szFeaClsName));
	if (!GetGeoClassName(szFeaClsName,sizeof(szFeaClsName)))
		return false;

	CDpFeatureClass *pFtrCls	= new CDpFeatureClass(m_pWks,m_iLayerType,szFeaClsName);
	BOOL bRet = pFtrCls->Open(szSql,NULL), bRet2 = FALSE;
	
	if (bRet)
	{
		if( pFtrCls->Next() )
			bRet2 = TRUE;
	}

	delete pFtrCls;
	pFtrCls = NULL;

	return bRet2;
}


//����һ��Feature
BOOL CDpFeatureLayer::UpdateFeature(CDpFeature* pFeature)
{
	if (!pFeature)
		return false;

	const CDpShape*		pShape		= NULL;
	pFeature->GetShape(&pShape);

	if (!pShape)
		return false;

	int	iCount = GetAttrFieldCount();

	if (iCount != pFeature->GetAttrFieldCount())
		return false;

	char			szSql[10240];

	//�����Feature�Ŀ�ʼ��ʼ����
	m_pWks->m_pDB->BeginTrans();
	
	//�ȳ��Ը��µ�ǰ��ʸ�����ݱ��е�ʸ������
	GUID		uiOID	= pFeature->GetOID();
	
	if (!SetShape(uiOID,(char*)(pFeature->GetUnitID()),(CDpShape*)pShape))
		goto UpdateErr;

	//�ٽ��������������ݵ�SQL���
	if (!MakeUpdateSql(pFeature,szSql,sizeof(szSql)))
		goto UpdateErr;

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto UpdateErr;


	m_pWks->m_pDB->CommitTrans();
	return true;

UpdateErr:
	m_pWks->m_pDB->Rollback();
	return false;
}

//ɾ��һ��Feature
BOOL CDpFeatureLayer::DeleteFeature(CDpFeature* pFeature)
{
	if (!pFeature)
		return false;

	return DelShape(pFeature->GetOID(),(char*)(pFeature->GetUnitID()));
}




/*******************************************************************/
/*                       �û����������ݶ�����					   */
/*******************************************************************/

CDpUserDefinedGroupObject::CDpUserDefinedGroupObject(CDpRecordset *pRecSet, CDpObjectEx *pItem)
{
	m_iOID = 0;
	memset(m_szUnitID,0,sizeof(m_szUnitID));
	
	m_iAttrFieldCount = 0;
	m_pAttrFieldValueArray = 0;

	if( pRecSet!=NULL && pItem!=NULL )
	{
		int nField = pRecSet->GetFieldCount();
		int nOIDIdx = pRecSet->GetFieldIndex((char*)"ID")-1;
		int nUIDIdx = pRecSet->GetFieldIndex((char*)"GRID_ID")-1;
		m_pAttrFieldValueArray = new CDpDBVariant[nField-2];
		if( m_pAttrFieldValueArray )
		{
			m_iAttrFieldCount = nField-2;
			for( int i=0, j=0; i<nField; i++)
			{
				if( i!=nOIDIdx && i!=nUIDIdx )
					m_pAttrFieldValueArray[j++] = pItem->m_pFields[i];
			}
		}

		SetID((int)pItem->m_pFields[nOIDIdx]);
		SetUnitID((LPCTSTR)pItem->m_pFields[nUIDIdx]);
	}
}


CDpUserDefinedGroupObject::~CDpUserDefinedGroupObject()
{
	if( m_pAttrFieldValueArray )
		delete[] m_pAttrFieldValueArray;
}

int CDpUserDefinedGroupObject::GetID()
{
	return m_iOID;
}


int CDpUserDefinedGroupObject::SetID(int id)
{
	m_iOID = id;
	return 0;
}


const char* CDpUserDefinedGroupObject::GetUnitID()
{
	return m_szUnitID;
}


void CDpUserDefinedGroupObject::SetUnitID(const char* szUnitID)
{
	memset(m_szUnitID,0,sizeof(m_szUnitID));
	strncpy(m_szUnitID, szUnitID, sizeof(m_szUnitID)-1);
}

//���������ֶ�����,�õ�����ֵ
BOOL CDpUserDefinedGroupObject::GetAttrValue(UINT uIndex,CDpDBVariant& var)
{
	if( uIndex>=0 && uIndex<m_iAttrFieldCount )
		var = m_pAttrFieldValueArray[uIndex];
	else
		return FALSE;
	return TRUE;
}


//����ָ���ֶ�����������ֵ
BOOL CDpUserDefinedGroupObject::SetAttrValue(UINT uIndex,CDpDBVariant& var)
{
	if( uIndex>=0 && uIndex<m_iAttrFieldCount )
		m_pAttrFieldValueArray[uIndex] = var;
	else
		return FALSE;
	return TRUE;
}


//�õ���ǰFeature�ڵ������ֶεĸ���
int	CDpUserDefinedGroupObject::GetAttrFieldCount()
{
	return m_iAttrFieldCount;
}

/*******************************************************************/
/*                       �û������Ĺ�����								   */
/*******************************************************************/

CDpUserDefinedGroupMgr::CDpUserDefinedGroupMgr()
{
	m_pWks = NULL;
	m_pCustomFieldInfo = NULL;
	m_szTableName = NULL;
	m_iAttrFieldCount = 0;
}


CDpUserDefinedGroupMgr::~CDpUserDefinedGroupMgr()
{
	if( m_szTableName )delete[] m_szTableName;
	if( m_pCustomFieldInfo )delete[] m_pCustomFieldInfo;
}

BOOL CDpUserDefinedGroupMgr::GetFieldInfo()
{
	if (m_pCustomFieldInfo)
		return TRUE;

	if( !m_pWks )
		return FALSE;
	
	m_iAttrFieldCount = 0;
	
	if (!m_szTableName )
		return FALSE;
	
	SQLHANDLE		hStmt	= NULL;
	
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pWks->m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return FALSE;
	
	char			szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select * From %s Where 1 = 2",m_szTableName);
	
	if (!Check(SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS)))
	{
		SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
		return FALSE;		
	}
	
	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		SQLSMALLINT		iColCount	= 0;
		
		if (SQLNumResultCols(hStmt,&iColCount) == SQL_SUCCESS)
		{
			if( iColCount>0 )
			{
				//���������ֶ���������,ATT_OID��ATT_GRID_ID����������
				m_pCustomFieldInfo = new CDpCustomFieldInfo[iColCount];
				memset(m_pCustomFieldInfo,0,(iColCount)*sizeof(CDpCustomFieldInfo));
				
				for (int i = 0; i < iColCount; i++)
				{
					SQLSMALLINT iCbNameLen	= 0;
					
					SQLDescribeCol(hStmt,(SQLUSMALLINT)(i+1),(SQLCHAR*)(m_pCustomFieldInfo[i].m_strName),
						sizeof(m_pCustomFieldInfo[i].m_strName),&iCbNameLen,
						&(m_pCustomFieldInfo[i].m_nSQLType),
						&(m_pCustomFieldInfo[i].m_iSize),
						&(m_pCustomFieldInfo[i].m_nPrecision),
						(SQLSMALLINT*)&(m_pCustomFieldInfo[i].m_bAllowNull));
					
					//�ٴ��ֵ����ȡ�����ֶε�������Ϣ
					CDpRecordset	rst(m_pWks->m_pDB);
					char			szSql[512];
					memset(szSql,0,sizeof(szSql));
					sprintf(szSql,"Select FIELD_ALIAS,FIELD_TYPE,FIELD_PRECISION,FIELD_ISUNIQUE,PARENT_CLASS From DP_ATTR_DICTIONARY Where ATTR_TABLE_NAME = '%s' AND FIELD_NAME = '%s'",
						m_szTableName,m_pCustomFieldInfo[i].m_strName);
					if (rst.Open(szSql) && rst.Next())
					{
						CDpDBVariant	var;
						if (rst.GetFieldValue(1,var))
							strcpy(m_pCustomFieldInfo[i].m_szAliasName,(LPCTSTR)var);
						if (rst.GetFieldValue(2,var))
							m_pCustomFieldInfo[i].m_CustomFieldType = DpCustomFieldType((int)var);
						if (rst.GetFieldValue(3,var))
							m_pCustomFieldInfo[i].m_nPrecision = (int)var;
						if (rst.GetFieldValue(4,var))
							m_pCustomFieldInfo[i].m_bIsUnique = (((int)var) == 1);
						if (rst.GetFieldValue(5,var))
							m_pCustomFieldInfo[i].m_iParent	= ((int)var);
					}
					rst.Close();
					
				}
				
				m_iAttrFieldCount = iColCount;
			}
			
			SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
			return TRUE;
		}
		
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return FALSE;
}

BOOL CDpUserDefinedGroupMgr::Attach(CDpWorkspace* pWks, CDpStringArray &arrUnitIDs, int table)
{
	m_pWks = pWks;
	m_arrUnitIDs.Copy(arrUnitIDs);

	if( m_szTableName )delete[] m_szTableName;
	m_szTableName = NULL;

	if( table==0 )
		pWks->GetGroupTableName(&m_szTableName,NULL);
	else if( table==1 )
		pWks->GetGroupTableName(NULL,&m_szTableName);

	GetFieldInfo();

	return TRUE;
}


//��ӵ���, �������uOID��Ϊ0,���ʾ����ӵ����ʱ����Ҫȥ�������ĵ���ID,����ֱ��ʹ�øò���,���Ҳ����ж��Ƿ��б༭ͼ����Ȩ��
BOOL CDpUserDefinedGroupMgr::AddGroup(CDpUserDefinedGroupObject* pObj, int* pNewId)
{
	if( !m_pWks )
		return FALSE;

	int nNewId = GetMaxID()+1;
	char szSql[1024] = {0};

	//�����Feature�Ŀ�ʼ��ʼ����
	m_pWks->m_pDB->BeginTrans();

	MakeInsertSql(pObj,nNewId,szSql,sizeof(szSql));

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto AddErr;
	
	m_pWks->m_pDB->CommitTrans();
	pObj->SetID(nNewId);						//����ӵ�Raster�����OID�޸�
	if( pNewId )*pNewId = nNewId;

	return TRUE;
	
AddErr:
	m_pWks->m_pDB->Rollback();
	return FALSE;
}

//ɾ������,ͬʱɾ����������
BOOL CDpUserDefinedGroupMgr::DelGroup(int nid)
{	
	CDpDatabase*	db		= m_pWks->m_pDB;
	
	char		szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From %s Where ID = %d ",m_szTableName,nid);
	
	if (db->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		return true;
	}
	else
		return false;
}


char* CDpUserDefinedGroupMgr::MakeUnitIDWhere()
{
	char *szUnitWhere = NULL;
	
	int	iCount = m_arrUnitIDs.GetCount();
	if (iCount > 0)
	{
		int iUnitWhereSize = iCount*22+128;
		szUnitWhere = new char[iUnitWhereSize];

		if( szUnitWhere )
		{
			memset(szUnitWhere,0,iUnitWhereSize);
			sprintf(szUnitWhere," GRID_ID IN (");
			
			for (int i = 0; i < iCount; i++)
			{
				if (i != 0)
					strcat(szUnitWhere,",");
				
				strcat(szUnitWhere,"'");
				strcat(szUnitWhere,m_arrUnitIDs.GetItem(i));
				strcat(szUnitWhere,"'");
			}
			strcat(szUnitWhere,")");

			return szUnitWhere;
		}
	}

	return 0;
}

int CDpUserDefinedGroupMgr::GetGroupCount()
{
	if( !m_pWks )return 0;

	SQLINTEGER  sCount	 = 0;
	SQLLEN  cbCount = 0;
	SQLRETURN	retcode;
	
	char		szSql[1024] = {0};
	
	SQLHANDLE	hStmt	= NULL;
	if (SQLAllocHandle(SQL_HANDLE_STMT,m_pWks->m_pDB->GetDBHandle(),&hStmt) != SQL_SUCCESS)
		return -1;

	char *szUnitWhere = MakeUnitIDWhere();
	
	if( szUnitWhere )
		sprintf(szSql,"Select Count(ID) From '%s' WHERE %s ", m_szTableName, szUnitWhere);
	else
		sprintf(szSql,"Select Count(ID) From '%s' ", m_szTableName, szUnitWhere);

	if( szUnitWhere )delete[] szUnitWhere;
	
	retcode = SQLPrepare(hStmt,(SQLTCHAR*)szSql,SQL_NTS);
	
	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		while (true)
		{
			retcode = SQLFetch(hStmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLGetData(hStmt,1,SQL_C_ULONG,&sCount,0,&cbCount);
			}
			else
				break;
		}
		
		SQLCloseCursor(hStmt);					//HOUKUI,06,8,8
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return sCount;
}

//���������ֶ�����,�õ�����ֵ
CDpUserDefinedGroupObject* CDpUserDefinedGroupMgr::GetGroup(int nid)
{
	if( !m_pWks )return NULL;

	char szWhere[1024] = {0};
	sprintf(szWhere,"ID = %d", nid);

	CDpRecordset *pRecSet = NULL;
	if( !OpenGroupClass(szWhere, &pRecSet) )
		return NULL;

	if( !pRecSet )
		return NULL;

	CDpObjectEx *pItem = pRecSet->Next(0);
	if( !pItem )
	{
		delete pRecSet;
		return NULL;
	}

	CDpUserDefinedGroupObject *pObj = new CDpUserDefinedGroupObject(pRecSet,pItem);

	delete pItem;
	delete pRecSet;

	return pObj;
}


//����ָ���ֶ�����������ֵ
BOOL CDpUserDefinedGroupMgr::UpdateGroup(CDpUserDefinedGroupObject* pObj)
{
	char szSql[1024] = {0};

	//�����Feature�Ŀ�ʼ��ʼ����
	m_pWks->m_pDB->BeginTrans();
	
	MakeUpdateSql(pObj,szSql,sizeof(szSql));
	
	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto AddErr;
	
	m_pWks->m_pDB->CommitTrans();
	return TRUE;
	
AddErr:
	m_pWks->m_pDB->Rollback();
	return FALSE;

}


int CDpUserDefinedGroupMgr::GetAttrFieldCount()
{
	return m_iAttrFieldCount;
}


BOOL CDpUserDefinedGroupMgr::GetAttrFieldInfo(const char* szAttrFieldName,CDpCustomFieldInfo* pInfo)
{
	if (!m_pCustomFieldInfo)
		GetFieldInfo();
	
	if (m_iAttrFieldCount > 0)
	{
		for (int i = 0; i < m_iAttrFieldCount; i++)
		{
			if (stricmp(m_pCustomFieldInfo[i].m_strName,szAttrFieldName) == 0)
			{
				memcpy(pInfo,&(m_pCustomFieldInfo[i]),sizeof(CDpCustomFieldInfo));
				return TRUE;
			}
			
		}
	}
	
	return FALSE;
}

BOOL CDpUserDefinedGroupMgr::GetAttrFieldInfo(int idx,CDpCustomFieldInfo* pInfo)
{
	if (!m_pCustomFieldInfo)
		GetFieldInfo();
	
	if (m_iAttrFieldCount > 0 && idx>=0 && idx<m_iAttrFieldCount )
	{
		memcpy(pInfo,&(m_pCustomFieldInfo[idx]),sizeof(CDpCustomFieldInfo));
		return TRUE;
	}

	return FALSE;
}

int CDpUserDefinedGroupMgr::GetAttrFieldIndex(const char* szAttrFieldName)
{
	if (!m_pCustomFieldInfo)
		GetFieldInfo();

	if (m_iAttrFieldCount > 0)
	{
		for (int i = 0; i < m_iAttrFieldCount; i++)
		{
			if (stricmp(m_pCustomFieldInfo[i].m_strName,szAttrFieldName) == 0)
			{
				return i;
			}
			
		}
	}
	
	return -1;
}


//��ȡһ��Layer�Ĳ�ѯ
BOOL CDpUserDefinedGroupMgr::OpenGroupClass(const char* szWhereClause,CDpRecordset** ppCls)
{
	if (!ppCls )
		return FALSE;
	
	*ppCls = NULL;

	char szSql[4096] = {0};
	char *szUnitWhere = MakeUnitIDWhere();

	if( szWhereClause )
	{
		if( szUnitWhere )
			sprintf(szSql,"Select * From %s Where %s AND %s", m_szTableName, szUnitWhere, szWhereClause);
		else
			sprintf(szSql,"Select * From %s Where %s", m_szTableName, szWhereClause);
	}
	else
	{
		if( szUnitWhere )
			sprintf(szSql,"Select * From %s Where %s ", m_szTableName, szUnitWhere);
		else
			sprintf(szSql,"Select * From %s", m_szTableName);
	}

	if( szUnitWhere )
		delete[] szUnitWhere;

	//����FeatrueClass����
	*ppCls	= new CDpRecordset(m_pWks->m_pDB);
	BOOL bRet = (*ppCls)->Open(szSql);

	if (!bRet)
	{
		delete *ppCls;
		*ppCls = NULL;
	}
	
	return bRet;
}


int CDpUserDefinedGroupMgr::GetMaxID()
{
	if( !m_pWks )return 0;

	char szSql[1024] = {0};

	//�ȵõ���ǰ��������IDС��50000�����ID,��Ϊ����50000���Ǳ�ϵͳά���Ĳ���,��ID�������ݿ�ά����
	sprintf(szSql,"Select Max(ID) From %s ",m_szTableName);

	CDpRecordset rst(m_pWks->m_pDB);
	if (!rst.Open(szSql))
		return 0;
	if (rst.Next())
	{
		CDpDBVariant	var;
		if (!rst.GetFieldValue(1,var))
			return 0;
		int iMaxID = (int)var;
		return iMaxID;
	}
	
	return 0;
}


BOOL CDpUserDefinedGroupMgr::MakeInsertSql(CDpUserDefinedGroupObject *pObject, int nID, char *szSql, int nszLen)
{
	if (!pObject || !szSql || nszLen <= 0)
		return false;
	
	int		iCount	= pObject->GetAttrFieldCount();
	
	char	szTmp[10240];
	
	//�ٽ��������������ݵ�SQL���
	if (iCount > 0)
	{
		sprintf(szTmp,"Insert Into %s Values(%d,'%s'",m_szTableName,nID,pObject->GetUnitID());
		for (int i = 0; i < iCount; i++)
		{
			CDpDBVariant	var;
			pObject->GetAttrValue(i,var);
			
			char				szFieldValue[300];
			memset(szFieldValue,0,sizeof(szFieldValue));
			
			CDpCustomFieldInfo	info;
			GetAttrFieldInfo(i+2,&info);
			
			switch (info.m_CustomFieldType)
			{
			case DP_CFT_BYTE:
			case DP_CFT_SMALLINT:
			case DP_CFT_INTEGER:
			case DP_CFT_BOOL:
			case DP_CFT_COLOR:
			case DP_CFT_ENUM:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_BOOL && 
						var.m_dwType != DPDBVT_UCHAR && 
						var.m_dwType != DPDBVT_SHORT && 
						var.m_dwType != DPDBVT_LONG)
						return false;
					sprintf(szFieldValue,"%d",(int)var);
					break;
				}
			case DP_CFT_FLOAT:
			case DP_CFT_DOUBLE:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_SINGLE && 
						var.m_dwType != DPDBVT_DOUBLE)
						return false;
					sprintf(szFieldValue,"%f",(double)var);
					break;
				}
			case DP_CFT_VARCHAR:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_STRING)
						return false;
					sprintf(szFieldValue,"'%s'",(LPCTSTR)var);
					break;
				}
			case DP_CFT_DATE:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_DATE)
						return false;
					if (var.m_dwType == DPDBVT_DATE)
						BuildDateTimeSql(m_pWks->m_pDB->GetDBType(),var.m_pdate,szFieldValue);
					else
						sprintf(szFieldValue,"NULL");
					break;
				}
			case DP_CFT_BLOB:					//Ŀǰ�����ֶβ�֧�ֶ���������
				{
					sprintf(szFieldValue,"NULL");
					break;
				}
			default:
				return false;
				
			}
			
			strcat(szTmp," , ");
			strcat(szTmp,szFieldValue);
		}
		strcat(szTmp," ) ");
		
		if (strlen(szTmp) > (nszLen-1))
			return false;
		
		strcpy(szSql,szTmp);
		return true;
	}
	
	return false;
}

BOOL CDpUserDefinedGroupMgr::MakeUpdateSql(CDpUserDefinedGroupObject *pObject, char *szSql, int nszLen)
{
	if (!pObject || !szSql || nszLen <= 0)
		return false;
	
	int		iCount	= GetAttrFieldCount();
	
	char	szTmp[1024] = {0}, szSetValue[1024] = {0};
	
	//�ٽ��������������ݵ�SQL���
	if (iCount > 0)
	{
		for (int i = 0, j = 0; i < iCount; i++)
		{
			char				szFieldValue[300];
			memset(szFieldValue,0,sizeof(szFieldValue));
			
			CDpCustomFieldInfo	info;
			GetAttrFieldInfo(i,&info);

			if( stricmp(info.m_strName,"ID")==0 ||
				stricmp(info.m_strName,"GRID_ID")==0 )
				continue;

			CDpDBVariant	var;
			pObject->GetAttrValue(j++,var);
			
			switch (info.m_CustomFieldType)
			{
			case DP_CFT_BYTE:
			case DP_CFT_SMALLINT:
			case DP_CFT_INTEGER:
			case DP_CFT_BOOL:
			case DP_CFT_COLOR:
			case DP_CFT_ENUM:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_BOOL && 
						var.m_dwType != DPDBVT_UCHAR && 
						var.m_dwType != DPDBVT_SHORT && 
						var.m_dwType != DPDBVT_LONG)
						return false;
					sprintf(szFieldValue,"%d",(int)var);
					break;
				}
			case DP_CFT_FLOAT:
			case DP_CFT_DOUBLE:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_SINGLE && 
						var.m_dwType != DPDBVT_DOUBLE)
						return false;
					sprintf(szFieldValue,"%f",(double)var);
					break;
				}
			case DP_CFT_VARCHAR:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_STRING)
						return false;
					sprintf(szFieldValue,"'%s'",(LPCTSTR)var);
					break;
				}
			case DP_CFT_DATE:
				{
					if (var.m_dwType != DPDBVT_NULL &&
						var.m_dwType != DPDBVT_DATE)
						return false;
					if (var.m_dwType == DPDBVT_DATE)
						BuildDateTimeSql(m_pWks->m_pDB->GetDBType(),var.m_pdate,szFieldValue);
					else
						sprintf(szFieldValue,"NULL");
					break;
				}
			}

			if( strlen(szFieldValue)>0 )
			{
				if( j==1 )
					sprintf(szTmp, " %s = %s ", info.m_strName, szFieldValue );
				else
					sprintf(szTmp, ", %s = %s ", info.m_strName, szFieldValue );

				strcat(szSetValue, szTmp);
			}
		}

		memset(szTmp, 0, sizeof(szTmp));
		if( strlen(szSetValue)>0 )
		{
			sprintf(szTmp, "Update %s set %s where ID = %d AND GRID_ID = '%s' ", 
				m_szTableName, szSetValue, pObject->GetID(), pObject->GetUnitID() );
			strcpy(szSql,szTmp);
			
			return true;
		}		
	}
	
	return false;
}



/*******************************************************************/
/*                       դ�����������                          */
/*******************************************************************/
CDpRasterLayer::CDpRasterLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType)
			   :CDpLayer(pWks,iLayerType)
{


}

CDpRasterLayer::~CDpRasterLayer()
{

}


//�õ�Ҫ������դ���ļ��ڷ������ڵ��ļ���
//����˵��:  [in] szGridID --  դ���ͼ��ID    [out] szFileName -- ���ص�դ���ļ���
BOOL CDpRasterLayer::GetNewRasterFileNameInServer(const char* szGridID,char* szFileName)
{
	if (!szGridID || strlen(szGridID) <= 0 || !szFileName)
		return false;

	//ȡͼ��ID�ĺ���λ
	char*		szRight		= (char*)szGridID + 15;

	sprintf(szFileName,"%s\\%s%s",m_pWks->m_szDefaultDir,m_szID,szRight);

	//ȡͼ��������
	CDpUnit*		pUnit	= NULL;
	if (!m_pWks->GetUnit(szGridID,&pUnit) || !pUnit)
		return false;

	char			szExtName[20];
	char			szSubDirName[50];
	memset(szExtName,0,sizeof(szExtName));
	memset(szSubDirName,0,sizeof(szSubDirName));

	if (m_iLayerTypeEx == LAYER_TYPE_EX_DEM)
	{
		sprintf(szExtName,"DEM");
		sprintf(szSubDirName,"DEM");
	}
	else if (m_iLayerTypeEx == LAYER_TYPE_EX_DOM)
	{
		sprintf(szExtName,"tif");
		sprintf(szSubDirName,"DOM");
	}
	else if (m_iLayerTypeEx == LAYER_TYPE_EX_DRG)
	{
		sprintf(szExtName,"DRG");
		sprintf(szSubDirName,"DRG");
	}
	else if (m_iLayerTypeEx == LAYER_TYPE_EX_DVS)
	{
		sprintf(szExtName,"DVS");
		sprintf(szSubDirName,"DVS");
	}
	else
		return false;

	sprintf(szFileName,"%s\\%s\\%s.%s",m_pWks->m_szDefaultDir,szSubDirName,
			pUnit->m_szUnitName,szExtName);

	return true;
}



//��ͼ�������դ��
//����˵��:   [in]szGridID          -- Ҫ��ӵ�դ��������ͼ����ID 
//            [in]pszRasterFileName -- դ����ļ����·��
//            [in]lpRtBound         -- դ��Ŀռ䷶Χ            
//            [in,out]pNewOID       -- ��ӳɹ��󷵻��µĵ����OID,���ʧ�ܷ���0,�������Ҫ
//                                     ����ΪNULL
BOOL CDpRasterLayer::AddRaster(const char* szGridID,const char* pszRasterFileName,
							   CDpRect* lpRtBound,GUID* pNewOID,BOOL bInsertEmptyAttr)
{
	if (!szGridID || !pszRasterFileName || strlen(pszRasterFileName) <= 0)
		return false;

	if (pNewOID)
	{
		ZeroMem(*pNewOID);
	}

	CDpDatabase*	db		= m_pWks->m_pDB;

	//�ж��û���û����ͼ��szGridIDд��Ȩ��
	if (!CanModifyCurrUnit(szGridID))
		return false;

	GUID	uMaxID  = NewGUID();
	
	CDpRect		rt;
	if (lpRtBound)
		rt.CopyRect(lpRtBound);
	else
		rt.SetRect(0,0,0,0);
	
	char		szSql[2048];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Insert Into %s values('%s','%s',%f,%f,%f,%f,%d,?)",
		    m_szSpatialTableName,GUIDToString(uMaxID),szGridID,rt.m_dMinX,
			rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,(int)m_iLayerType);

	BOOL bRet = SetLargerFieldValue(db->GetDBHandle(),szSql,
		                            (BYTE*)pszRasterFileName,strlen(pszRasterFileName));

	
	if (bRet && bInsertEmptyAttr)		//�������ʸ�����ݳɹ�,���ٲ����Ӧ�������ֶ�
	{
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Insert Into %s(ATT_OID,ATT_GRID_ID) values('%s','%s')",
			    m_szAttributeTableName,GUIDToString(uMaxID),szGridID);
		bRet = (db->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
	}

	if (bRet && pNewOID)
	{
		*pNewOID = uMaxID;

		
	}
	

	if (bRet)
	{
		//�˴�����Ҫ�����������դ���ļ��Ĵ���
	}


	return bRet;
}


//���դ��
BOOL CDpRasterLayer::AddRaster(CDpRaster* pRaster)
{
	if (!pRaster)
		return false;

	int	iCount = this->GetAttrFieldCount();
	if (iCount != pRaster->GetAttrFieldCount())
		return false;

	char	szFileName[256];
	sprintf(szFileName,0,sizeof(szFileName));

	pRaster->GetRasterFileName(szFileName,sizeof(szFileName));

	CDpRect	rt;
	pRaster->GetRasterBound(&rt);

	char			szSql[10240];

	//�����Feature�Ŀ�ʼ��ʼ����
	m_pWks->m_pDB->BeginTrans();
	
	//�ȳ�����ǰ��ʸ�����ݱ�����ӵ�ǰʸ��,���а����˶�Ȩ�޵��ж�
	GUID		uiNewOID;
	ZeroMem(uiNewOID);
	if (!AddRaster((char*)(pRaster->GetUnitID()),szFileName,&rt,&uiNewOID,false) || IsZeroGUID(&uiNewOID))
		goto AddErr;

	//�ٽ��������������ݵ�SQL���
	if (!MakeInsertSql(pRaster,uiNewOID,szSql,sizeof(szSql)))
		goto AddErr;

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto AddErr;

	m_pWks->m_pDB->CommitTrans();
	pRaster->SetOID(uiNewOID);						//����ӵ�Raster�����OID�޸�
	return true;


AddErr:
	m_pWks->m_pDB->Rollback();
	return false;

}



//�༭դ��
BOOL CDpRasterLayer::SetRaster(GUID OID,const char* szUnitID,
							   const char* pszRasterFileName,CDpRect* lpRtBound)
{
	if (IsZeroGUID(&OID) || !pszRasterFileName || strlen(pszRasterFileName) <= 0 || 
		!szUnitID)
		return false;

	//�ж���û��ɾ���õ����Ȩ��
	if (!CanModifyCurrUnit(szUnitID))
		return false;

	CDpDatabase*	db		= m_pWks->m_pDB;

	CDpRect			rt;
	if (lpRtBound)
		rt.CopyRect(lpRtBound);
	else
		rt.SetRect(0,0,0,0);

	char		szSql[2048];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update %s set ST_MINX = %f,ST_MINY = %f,ST_MAXX = %f,ST_MAXY = %f,SPATIAL_DATA = ? where OID = '%s' AND ST_GRID_ID = '%s'",
		    m_szSpatialTableName,rt.m_dMinX,rt.m_dMinY,rt.m_dMaxX,rt.m_dMaxY,GUIDToString(OID),szUnitID);

	BOOL bRet = SetLargerFieldValue(db->GetDBHandle(),szSql,(BYTE*)pszRasterFileName,strlen(pszRasterFileName));

	if (bRet)
	{
		//�˴�����Ҫ��Ӹ�������դ���ļ��Ĵ���
	}

	return bRet;
}


//�༭դ��
BOOL CDpRasterLayer::UpdateRaster(CDpRaster* pRaster)
{
	if (!pRaster)
		return false;

	int	iCount = GetAttrFieldCount();

	if (iCount != pRaster->GetAttrFieldCount())
		return false;

	char	szFileName[256];
	sprintf(szFileName,0,sizeof(szFileName));

	pRaster->GetRasterFileName(szFileName,sizeof(szFileName));

	CDpRect	rt;
	pRaster->GetRasterBound(&rt);


	char			szSql[10240];

	//�����Feature�Ŀ�ʼ��ʼ����
	m_pWks->m_pDB->BeginTrans();
	
	//�ȳ��Ը��µ�ǰ��ʸ�����ݱ��е�ʸ������
	GUID		uiOID	= pRaster->GetOID();
	
	if (!SetRaster(uiOID,(char*)(pRaster->GetUnitID()),szFileName,&rt))
		goto UpdateErr;

	//�ٽ��������������ݵ�SQL���
	if (!MakeUpdateSql(pRaster,szSql,sizeof(szSql)))
		goto UpdateErr;

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		goto UpdateErr;


	m_pWks->m_pDB->CommitTrans();
	return true;

UpdateErr:
	m_pWks->m_pDB->Rollback();
	return false;
}




//ɾ��դ��
BOOL CDpRasterLayer::DelRaster(GUID OID,const char* szUnitID)
{
	if (IsZeroGUID(&OID) || !szUnitID)
		return false;

	//�ж���û��ɾ���õ����Ȩ��
	if (!CanModifyCurrUnit(szUnitID))
		return false;

	CDpDatabase*	db		= m_pWks->m_pDB;

	char		szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From %s Where OID = '%s' AND ST_GRID_ID = '%s'",m_szSpatialTableName,GUIDToString(OID),szUnitID);
	
	if (db->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Delete From %s Where ATT_OID = '%s' where ATT_GRIDID = '%s'",m_szAttributeTableName,GUIDToString(OID),szUnitID);
		if (db->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
		{
			//�˴�����Ҫ���ɾ������դ���ļ��Ĵ���
			return true;
		}
		else
			return false;
	}
	else
		return false;

}

//ɾ��դ��
BOOL CDpRasterLayer::DelRaster(CDpRaster* pRaster)
{
	if (!pRaster)
		return false;

	return DelRaster(pRaster->GetOID(),(char*)(pRaster->GetUnitID()));

}



//�򿪵�ǰͼ�����Ĳ�ѯ
//����˵��:  [in]  pSpaFilter    -- �ռ��������,�������Ҫ����,������NULL����
//           [out] ppRasterQuery -- ִ�гɹ����ز�ѯ��ѯ����,ʧ���򷵻�NULL
BOOL CDpRasterLayer::OpenRasterQuery(CDpSpatialQuery* pSpaFilter,CDpRasterQuery** ppRasterQuery)
{
	if (!ppRasterQuery || !LAYER_IS_RASTER(m_iLayerType))
		return false;

	*ppRasterQuery		= NULL;

	//�����ռ����ݲ�ѯ����
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select * From %s",m_szSpatialTableName);

	//Ȩ�޿��ƹ������
	char	szRightFilter[512];
	memset(szRightFilter,0,sizeof(szRightFilter));
	//�����ϵͳ����Ա���ߵ�ǰ���ݿ��Ǳ��ص��ļ����ݿ�ʱ,����Ҫ����Ȩ�޵Ĺ������
	if (!m_pWks->m_pDB->m_pRightMgr->MakeRightFilterClauseWhenOpenQuery(m_pWks->m_szID,m_szID,szRightFilter))
		return false;

	if (strlen(szRightFilter) > 0)
	{
		strcat(szSql," WHERE ");
		strcat(szSql,szRightFilter);
	}


	*ppRasterQuery = new CDpRasterQuery(m_pWks);
	
	if (!((*ppRasterQuery)->Open(szSql,pSpaFilter)))
	{
		delete *ppRasterQuery;
		*ppRasterQuery = NULL;	
		return false;
	}
	else
		return true;


}


//�򿪵�ǰͼ���Feature�Ĳ�ѯ
//����˵��:  [in]szWhereClause  -- ��ѯ��WHERE�Ӿ�,����ͨ�����Ӿ����Բ�ѯ������й���,
//                                 ���б�����WHERE��ͷ,ͬʱҲ���԰���ORDER BY
//                                 ��Ҫ����ͨ�������ֶ������˼�¼,�������Ҫ,������ΪNULL
//           [in]pSpaFilter     -- �ռ����,�������Ҫ,������ΪNULL
//           [out] ppFeatureCls -- �ɹ�ʱ���ش򿪵�CDpRasterClass����,ʧ��ʱ����NULL
BOOL CDpRasterLayer::OpenRasterClass(CDpStringArray* pUnitIDArray,const char* szWhereClause,
									 CDpSpatialQuery* pSpaFilter,CDpRasterClass** ppRasterCls)
{
	if (!ppRasterCls || !LAYER_IS_RASTER(m_iLayerType))
		return false;
	
	*ppRasterCls = NULL;

	//�ȵõ�RasterClass������
	char		szRasterClsName[50];
	memset(szRasterClsName,0,sizeof(szRasterClsName));
	if (!GetGeoClassName(szRasterClsName,sizeof(szRasterClsName)))
		return false;

	//Ȩ�޿��ƹ������
	char	szRightFilter[512];
	memset(szRightFilter,0,sizeof(szRightFilter));
	//�����ϵͳ����Ա���ߵ�ǰ���ݿ��Ǳ��ص��ļ����ݿ�ʱ,����Ҫ����Ȩ�޵Ĺ������
	if (!m_pWks->m_pDB->m_pRightMgr->MakeRightFilterClauseWhenOpenQuery(m_pWks->m_szID,m_szID,szRightFilter))
		return false;


	//����Ҫ��򿪵�ͼ����SQL��� 
	char*	szUnitWhere		= NULL;
	int		iUnitWhereSize	= 0;
//	memset(szUnitWhere,0,sizeof(szUnitWhere));

	if (pUnitIDArray)
	{
		int		iCount = pUnitIDArray->GetCount();
		if (iCount > 0)
		{
			iUnitWhereSize = iCount*22+128;
			szUnitWhere = new char[iUnitWhereSize];
			memset(szUnitWhere,0,iUnitWhereSize);
			sprintf(szUnitWhere," ST_GRID_ID IN (");

			for (int i = 0; i < iCount; i++)
			{
				if (i != 0)
					strcat(szUnitWhere,",");

				strcat(szUnitWhere,"'");
				strcat(szUnitWhere,pUnitIDArray->GetItem(i));
				strcat(szUnitWhere,"'");
			}
			strcat(szUnitWhere,")");
		}
	}


	//���ɲ�ѯSQL
	int			szSqlSize	= strlen(szRightFilter) + iUnitWhereSize + (szWhereClause?strlen(szWhereClause):0) + 256;
	char*		szSql		= new char[szSqlSize];
	memset(szSql,0,szSqlSize);


	if (szWhereClause && strlen(szWhereClause) > 6)
	{
		sprintf(szSql,"Select * From %s %s",szRasterClsName,szWhereClause);
		//����Ȩ�޹���
		if (strlen(szRightFilter) > 0)
		{
			strcat(szSql," AND ");
			strcat(szSql,szRightFilter);
		}
		//����ͼ������
		if (szUnitWhere && strlen(szUnitWhere) > 0)
		{
			strcat(szSql," AND ");
			strcat(szSql,szUnitWhere);
		}
	}
	else
	{
		sprintf(szSql,"Select * From %s ",szRasterClsName);
		//����Ȩ�޹���
		if (strlen(szRightFilter) > 0)
		{
			strcat(szSql," WHERE ");
			strcat(szSql,szRightFilter);

			//����ͼ������
			if (szUnitWhere && strlen(szUnitWhere) > 0)
			{
				strcat(szSql," AND ");
				strcat(szSql,szUnitWhere);
			}
		}
		else
		{
			//����ͼ������
			if (szUnitWhere && strlen(szUnitWhere) > 0)
			{
				strcat(szSql," WHERE ");
				strcat(szSql,szUnitWhere);
			}

		}

	}

	
	//����FeatrueClass����
	*ppRasterCls	= new CDpRasterClass(m_pWks,m_iLayerType,szRasterClsName);
	BOOL bRet = (*ppRasterCls)->Open(szSql,pSpaFilter);
	if (!bRet)
	{
		delete *ppRasterCls;
		*ppRasterCls = NULL;
	}

	if (szUnitWhere)
	{
		delete [] szUnitWhere;
		szUnitWhere = NULL;
	}
	if (szSql)			
	{
		delete [] szSql;
		szSql = NULL;
	}

	return bRet;
}




















/*******************************************************************/
/*						ʸ��������
/*	�������ĳ������ĵ�����з���
/*******************************************************************/
CDpShape::CDpShape(enum DP_LAYER_TYPE iShapeType)
{
	m_iShapeType	= iShapeType;
	m_pData			= NULL;
}

CDpShape::~CDpShape()
{
	ClearPoints();
}


//�õ�ʸ������
DP_LAYER_TYPE CDpShape::GetShapeType()
{
	return m_iShapeType;
}

//�õ������Ŀ
int CDpShape::GetPointCount()
{
	if (!m_pData)
		return 0;
	return m_pData->iNumPoints;
}


//�õ�PART����Ŀ
int CDpShape::GetPartCount()
{
	if (!m_pData)
		return 0;
	return m_pData->iNumParts;
}

//����㼯
void CDpShape::ClearPoints()
{
	if (m_pData)
	{
		if (m_pData->pParts)
		{
			delete [] m_pData->pParts;
			m_pData->pParts = NULL;
		}
		if (m_pData->pPoints)
		{
			delete [] m_pData->pPoints;
			m_pData->pPoints = NULL;
		}
		delete m_pData;
		m_pData = NULL;
	}
}


//ֱ�ӵõ�ʸ�����ݽṹ��ָ��,��ָ����ڴ�����߲����ͷ�,��Ϊ���ڴ���ͷ�Ӧ��CDpShape�������ͷ�
BOOL CDpShape::GetData(const CDpShapeData** pData)
{
	*pData = NULL;
	if (!m_pData)
		return false;
	*pData = m_pData;

	return true;
}


//ֱ�ӵõ�ʸ�����ݽṹ�������ڴ�,���������ݿ��з�������,���������ڴ���Ҫ���������ͷ�
//����˵��:   [out]ppBuff -- ������ʸ�����ݵĻ�����ָ��,[out]cbLen -- �������ĳ���
BOOL CDpShape::GetData(BYTE** ppBuff,int* cbLen)
{
	if (!ppBuff || !cbLen)
		return false;

	int		iSize = GetDataSize();	

	if (iSize <= 0)
		return false;

	*cbLen = iSize;
	*ppBuff = new BYTE[iSize];
	memset(*ppBuff,0,iSize);

	int		iOffset	= 0;
	int		iLen	= 2*sizeof(int)+4*sizeof(double);		//ͷ�ĳ���
	
	memcpy((*ppBuff),m_pData,iLen);
	iOffset += iLen;
	iLen	= m_pData->iNumParts*sizeof(int);

	memcpy((*ppBuff)+iOffset,m_pData->pParts,iLen);
	iOffset += iLen;

	memcpy((*ppBuff)+iOffset,m_pData->pPoints,m_pData->iNumPoints*sizeof(CDpPoint));

	return true;
}



//�õ��㼯����,pt-Ϊ���õ㼯�Ļ�����,iBuffCount--Ϊ��������С(��λ��BYTE)
//���ص���ʵ���Ǹö������ڴ����ݵ��ڴ�ָ��
BOOL CDpShape::GetPoints(CDpPoint** pt,int* iBuffCount) const
{
	*pt			= NULL;
	*iBuffCount	= 0;
	if (!m_pData)
		return false;

	*pt			= m_pData->pPoints;
	*iBuffCount	= m_pData->iNumPoints;

	return true;
}



//����ʸ������
BOOL CDpShape::SetData(CDpShapeData* pData)
{
	ClearPoints();

	m_pData = pData;

	return true;
}

//����ʸ������
BOOL CDpShape::CopyData(CDpShapeData* pData)
{
	if (!pData)
		return false;

	ClearPoints();

	m_pData = new CDpShapeData;
	memset(m_pData,0,sizeof(CDpShapeData));

	m_pData->iNumParts		= pData->iNumParts;
	m_pData->iNumPoints		= pData->iNumPoints;
	memcpy(m_pData->dBound,pData->dBound,4*sizeof(double));
	m_pData->pParts			= new int[m_pData->iNumParts];
	memcpy(m_pData->pParts,pData->pParts,pData->iNumParts*sizeof(int));
	m_pData->pPoints		= new CDpPoint[m_pData->iNumPoints];
	memcpy(m_pData->pPoints,pData->pPoints,pData->iNumPoints*sizeof(CDpPoint));

	return true;
}



//����ʸ������,buff - Ϊ�����ݿ���ȡ��������
BOOL CDpShape::CopyData(BYTE* buff,int iBuffSize)
{
	if (!buff || iBuffSize <= 0)
		return false;

	int		iParts	= 0;
	int		iPoints	= 0;

	memcpy(&iParts,buff,sizeof(int));
	memcpy(&iPoints,buff+sizeof(int),sizeof(int));

	InitData(iParts,iPoints);					//����ʸ�������ڴ�

	if (!m_pData)
		return false;

	int		iOffset	= 2 * sizeof(int);
	int		iLen	= 4 * sizeof(double);

	memcpy(m_pData->dBound,buff+iOffset,iLen);	//�����������
	iOffset += iLen;
	iLen	= iParts * sizeof(int);
	memcpy(m_pData->pParts,buff+iOffset,iLen);	//����PART��������
	iOffset += iLen;
	iLen	= iPoints * sizeof(CDpPoint);
	memcpy(m_pData->pPoints,buff+iOffset,iLen);	//���Ƶ㼯���� 

	return true;
}


//��ʼ��ʸ�����ݽṹ
void CDpShape::InitData(int iPartNum,int iPointNum)
{
	if (iPartNum <= 0 || iPointNum <= 0)
		return;

	ClearPoints();

	m_pData = new CDpShapeData;
	memset(m_pData,0,sizeof(CDpShapeData));
	m_pData->iNumParts	= iPartNum;
	m_pData->iNumPoints	= iPointNum;

	m_pData->pParts		= new int[iPartNum];
	m_pData->pPoints	= new CDpPoint[iPointNum];	
	memset(m_pData->pParts,0,sizeof(int)*iPartNum);
	memset(m_pData->pPoints,0,sizeof(CDpPoint)*iPointNum);

}



//����ʸ��������ռ���ڴ�Ĵ�С
int  CDpShape::GetDataSize()
{
	return GetShapeDataSize(m_pData);
}



//���õ㼯����,iPointCount�������Ŀǰʸ�����ݵĵ㼯��������
BOOL CDpShape::SetPoints(CDpPoint* pt,int iPointCount)
{
	if (!m_pData || !pt || m_pData->iNumPoints != iPointCount)
		return false;

	memcpy(m_pData->pPoints,pt,iPointCount*sizeof(CDpPoint));

	return true;

}


//����PART����������
BOOL CDpShape::SetParts(int* pParts,int iPartCount)
{
	if (!m_pData || !pParts || iPartCount != m_pData->iNumParts)
		return false;

	memcpy(m_pData->pParts,pParts,iPartCount*sizeof(int));

	return true;
}










/*******************************************************************/
/*                     ��������������
/*******************************************************************/
CDpParamMgr::CDpParamMgr(CDpDatabase* pDB)
{
	m_pDB			= pDB;
	memset(m_szParamTableName,0,sizeof(m_szParamTableName));
}


CDpParamMgr::~CDpParamMgr()
{
	Close();
}

//�򿪲�����
BOOL CDpParamMgr::Open(char* szParamTableName)
{
	if (!szParamTableName || strlen(szParamTableName) <= 0)
		return false;

	char		szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"SELECT A.CATEGORY_ID,A.CATEGORY_NAME,A.CATEGORY_DESC,B.PARAM_ID,B.PARAM_NAME,B.PARAM_DESC,B.PARAM_TYPE,B.PARAM_LEN,B.PARAM_PRECISION,B.PARAM_SELECT_ID,B.PARAM_VALUE FROM DP_ParamCategory_WH A,%s B WHERE A.CATEGORY_ID = B.CATEGORY_ID ORDER BY A.CATEGORY_ID",
		    szParamTableName);
	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
			{
				int					iCategoryID		= (int)var;
				//�ȵõ�����
				CDpParamCategory*		pCategory		= NULL;
				ParamCategoryArray::iterator itc = m_ParamData.find(iCategoryID);
				if (itc != m_ParamData.end())				//��ʾ���и÷���
					pCategory = (*itc).second;
				else										//�½�����
				{
					pCategory = new CDpParamCategory;
					pCategory->iCategoryID	= iCategoryID;
					CDpDBVariant	var1;
					if (rst.GetFieldValue(2,var1))
						CopyStr(&(pCategory->szCategoryName),(LPCTSTR)var1);
					if (rst.GetFieldValue(3,var1))
						CopyStr(&(pCategory->szCategoryDesc),(LPCTSTR)var1);

					//��������뵽MAP��					
					m_ParamData.insert(ParamCategoryArray::value_type(iCategoryID,pCategory));
				}

				if (!pCategory)
					continue;

				//�ٵõ�������
				CDpDBVariant	var2;
				CDpParamItem*		pParamItem			= NULL;
				if (rst.GetFieldValue(4,var2))
				{
					int	iSelID  = 0;
					pParamItem	= new CDpParamItem;

					//����������뵽��Ӧ�ķ�����
					pCategory->pParamItemArray.push_back(pParamItem);
						
					//ȡ���������Ϣ
					pParamItem->iParamID = (int)var2;
					if (rst.GetFieldValue(5,var2))				//��������
						CopyStr(&(pParamItem->pszParamName),(LPCTSTR)var2);
					if (rst.GetFieldValue(6,var2))
						CopyStr(&(pParamItem->pszParamDesc),(LPCTSTR)var2);
					if (rst.GetFieldValue(7,var2))				//������������
						pParamItem->nParamDataType	= DpCustomFieldType((int)var2);
					if (rst.GetFieldValue(8,var2))				//�������ݳ���
						pParamItem->iParamDataLen	= (int)var2;
					if (rst.GetFieldValue(9,var2))				//����С��λ	
						pParamItem->iParamPrecision	= (int)var2;
					if (rst.GetFieldValue(10,var2))				//������ѡ��ID
						iSelID = (int)var2;
					if (rst.GetFieldValue(11,var2))				//����ֵ
						CopyStr(&(pParamItem->pszValue),(LPCTSTR)var2);

					//ѡ����
					if (iSelID != 0)
					{
						char	szSelSql[128];
						memset(szSelSql,0,sizeof(szSelSql));
						sprintf(szSelSql,"Select * From DP_ParamValue_WH Where SELECT_ID = %d",iSelID);
						CDpRecordset	rstSel(m_pDB);
						if (rstSel.Open(szSelSql))
						{
							while (rstSel.Next())
							{
								CDpParamSelectItem*	pSelItem = new CDpParamSelectItem;
								pSelItem->iSelectID	= iSelID;
								
								//��ѡ��ֵ���뵽��������
								pParamItem->pSelectItem.push_back(pSelItem);

								CDpDBVariant	var3;
								if (rstSel.GetFieldValue(2,var3))			//ѡ��ֵID
									pSelItem->iValueID = (int)var3;
								if (rstSel.GetFieldValue(3,var3))			//ѡ��ֵ
									CopyStr(&(pSelItem->szSelectValue),(LPCTSTR)var3);
							}

							rstSel.Close();
						}
					}
				}

			}

		}

		rst.Close();
	}

	strcpy(m_szParamTableName,szParamTableName);
	return true;
}


//�رղ�����
void CDpParamMgr::Close()
{
	//�ͷŲ��������ݵ��ڴ�
	ParamCategoryArray::iterator itc;
	for (itc = m_ParamData.begin(); itc != m_ParamData.end(); ++itc)
	{
		CDpParamCategory* pCategory = (*itc).second;
		if (pCategory)
		{
			delete pCategory;								//�ͷŷ�����
			pCategory = NULL;
		}
	}
	
	m_ParamData.clear();
}




//�õ���ǰ�������Ŀ
int CDpParamMgr::GetCategoryCount()
{
	return m_ParamData.size();
}


//�õ���ǰ���������
//���е�szCategoryName��szCategoryDesc���ڴ治��Ҫ�������ͷ�
BOOL CDpParamMgr::GetCategory(int iIndex,int* iCategoryID,
								const char** szCategoryName,const char** szCategoryDesc)
{
	if (iIndex < 0 ||  (iIndex > (GetCategoryCount()-1)) || !iCategoryID || 
		!szCategoryName || !szCategoryDesc)
		return false;

	ParamCategoryArray::iterator itc;
	int		i = 0;
	for (itc = m_ParamData.begin(); itc != m_ParamData.end(); ++itc)
	{
		if (iIndex == i)
			break;
		i++;
	}

	CDpParamCategory*	pCategory	= (*itc).second;
	if (!pCategory)	
		return false;

	*iCategoryID	= pCategory->iCategoryID;
	*szCategoryName = pCategory->szCategoryName;
	*szCategoryDesc = pCategory->szCategoryDesc;

	return true;
}


//�õ���ǰ�����в�������Ŀ
int CDpParamMgr::GetParamItemCount(int iCategoryID)
{
	ParamCategoryArray::iterator itc	= m_ParamData.find(iCategoryID);
	if (itc == m_ParamData.end())
		return 0;
 
	CDpParamCategory*	pCategory	= (*itc).second;
	if (!pCategory)
		return 0;

	return pCategory->pParamItemArray.size();
}


//�õ�������Ϣ,���а���������������Ϣ��ֵ,pParamItem���ڴ�����߲����ͷ�
BOOL CDpParamMgr::GetParamInfo(const char* szWksID,int iCategoryID,int iParamIndex,
							   const CDpParamItem** ppParamItem)
{
	if (iParamIndex < 0 || !ppParamItem)
		return false;

	*ppParamItem = NULL;

	ParamCategoryArray::iterator itc	= m_ParamData.find(iCategoryID);
	if (itc == m_ParamData.end())
		return false;

	CDpParamCategory*	pCategory	= (*itc).second;
	if (!pCategory)
		return false;

	if (iParamIndex > (pCategory->pParamItemArray.size() - 1))
		return false;

	*ppParamItem = pCategory->pParamItemArray[iParamIndex];
	
	if (szWksID)
	{
		//�ж���û��Ȩ��
		DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
		if (m_pDB->m_pRightMgr->GetParamRight(szWksID,m_szParamTableName,
												  (*ppParamItem)->iParamID,&iRight) && 
			(iRight == DP_RIGHT_READONLY || iRight == DP_RIGHT_READWRITE))
			return true;
		else
		{
			*ppParamItem = NULL;
			return false;
		}
	}
	else
		return true;
}


//�õ�������Ϣ
BOOL CDpParamMgr::GetParamInfoByID(int iParamID,const CDpParamItem** ppParamItem)
{
	*ppParamItem	= NULL;	

	ParamCategoryArray::iterator itc;
	for (itc = m_ParamData.begin(); itc != m_ParamData.end(); ++itc)
	{
		CDpParamCategory* pCategory = (*itc).second;
		if (pCategory)
		{
			//������
			int iItemCount = pCategory->pParamItemArray.size();
			for (int i = 0; i < iItemCount; i++)
			{	
				CDpParamItem*	pItem = pCategory->pParamItemArray[i];
				if (pItem && pItem->iParamID == iParamID)
				{
					*ppParamItem = pItem;
					return true;
				}

			}
		}
	}

	return true;


}



//�Ӳ����ṹ�еĵ�����ֵ
BOOL CDpParamMgr::GetParamValue(const CDpParamItem* pParamItem,CDpDBVariant& var)
{
	if (!pParamItem)
		return false;

	var.Clear();

	switch (pParamItem->nParamDataType)
	{
		case DP_CFT_BYTE:
			var.m_dwType	= DPDBVT_UCHAR;
			var.m_chVal		= atoi(pParamItem->pszValue?pParamItem->pszValue:"");
			break;
		case DP_CFT_SMALLINT:
			var.m_dwType	= DPDBVT_SHORT;
			var.m_iVal		= atoi(pParamItem->pszValue?pParamItem->pszValue:"");
			break;
		case DP_CFT_COLOR:
		case DP_CFT_INTEGER:
			var.m_dwType	= DPDBVT_LONG;
			var.m_lVal		= atol(pParamItem->pszValue?pParamItem->pszValue:"");
			break;
		case DP_CFT_FLOAT:
		case DP_CFT_DOUBLE:
			var.m_dwType	= DPDBVT_DOUBLE;
			var.m_dblVal	= atof(pParamItem->pszValue?pParamItem->pszValue:"");
			break;
		case DP_CFT_VARCHAR:
			var.m_dwType	= DPDBVT_STRING;
			CopyStr(&(var.m_pString),pParamItem->pszValue?pParamItem->pszValue:"");
			break;
		case DP_CFT_DATE:					//���������ݵĴ�Ÿ�ʽYYYYMMDDHHMMSS		
		{
			var.m_dwType	= DPDBVT_DATE;
			var.m_pdate		= new TIMESTAMP_STRUCT;
			memset(var.m_pdate,0,sizeof(TIMESTAMP_STRUCT));
			if (pParamItem->pszValue && strlen(pParamItem->pszValue) == 14)
			{
				char	szTmp[5];
				memset(szTmp,0,sizeof(szTmp));
				//��
				memcpy(szTmp,pParamItem->pszValue,4);
				var.m_pdate->year	= atoi(szTmp);
				//��
				memset(szTmp,0,sizeof(szTmp));
				memcpy(szTmp,pParamItem->pszValue+4,2);
				var.m_pdate->month	= atoi(szTmp);
				//��
				memset(szTmp,0,sizeof(szTmp));
				memcpy(szTmp,pParamItem->pszValue+6,2);
				var.m_pdate->day	= atoi(szTmp);
				//ʱ
				memset(szTmp,0,sizeof(szTmp));
				memcpy(szTmp,pParamItem->pszValue+8,2);
				var.m_pdate->hour	= atoi(szTmp);
				//��
				memset(szTmp,0,sizeof(szTmp));
				memcpy(szTmp,pParamItem->pszValue+10,2);
				var.m_pdate->minute	= atoi(szTmp);
				//��
				memset(szTmp,0,sizeof(szTmp));
				memcpy(szTmp,pParamItem->pszValue+12,2);
				var.m_pdate->second	= atoi(szTmp);
			}
			break;
		}
		case DP_CFT_BOOL:
			var.m_dwType	= DPDBVT_BOOL;
			var.m_boolVal	= ((atoi(pParamItem->pszValue?pParamItem->pszValue:"")==0)?false:true);
			break;
		default:
			return false;
			break;
	}
	return true;
}


//���ò�������
BOOL CDpParamMgr::SetParamValue(const char* szWksID,int iParamID,CDpDBVariant var)
{
	if (iParamID < 0)
		return false;

	if (szWksID)
	{
		//�ж���û��Ȩ���޸Ĳ������ֵ
		DP_RIGHT	iRight	= DP_RIGHT_NOTRIGHT;
		if (!m_pDB->m_pRightMgr->GetParamRight(szWksID,m_szParamTableName,iParamID,&iRight) || 
			iRight != DP_RIGHT_READWRITE)
			return false;
	}

	char	szTmp[256];
	memset(szTmp,0,sizeof(szTmp));

	switch (var.m_dwType)
	{
		case DPDBVT_BOOL:
			sprintf(szTmp,"%d",var.m_boolVal?1:0);
			break;
		case DPDBVT_UCHAR:
			sprintf(szTmp,"%d",var.m_chVal);
			break;
		case DPDBVT_SHORT:
			sprintf(szTmp,"%d",var.m_iVal);
			break;
		case DPDBVT_LONG:
			sprintf(szTmp,"%d",var.m_lVal);
			break;
		case DPDBVT_SINGLE:
			sprintf(szTmp,"%f",var.m_fltVal);
			break;
		case DPDBVT_DOUBLE:
			sprintf(szTmp,"%f",var.m_dblVal);
			break;
		case DPDBVT_DATE:
			if (!var.m_pdate)
				return false;
			sprintf(szTmp,"%04d%02d%02d%02d%02d%02d",var.m_pdate->year,var.m_pdate->month,
				    var.m_pdate->day,var.m_pdate->hour,var.m_pdate->minute,var.m_pdate->second);
			break;
		case DPDBVT_STRING:
			if (!var.m_pString || strlen(var.m_pString) > 255)
				return false;
			strcpy(szTmp,var.m_pString);
			break;
		default:
			return false;
	}

	const CDpParamItem*		pItem = NULL;

	if (!GetParamInfoByID(iParamID,&pItem) || !pItem)
		return false;

	//�ȸ������ݿ�
	char	szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update %s set PARAM_VALUE = '%s' Where PARAM_ID = %d",
		    m_szParamTableName,szTmp,pItem->iParamID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	if (((CDpParamItem*)pItem)->pszValue)
	{
		delete [] ((CDpParamItem*)pItem)->pszValue;
		((CDpParamItem*)pItem)->pszValue = NULL;
	}

	CopyStr(&(((CDpParamItem*)pItem)->pszValue),szTmp);
	return true;
	
}

//���ò�������
BOOL CDpParamMgr::SetParamValue(const char* szWksID,int iParamID,DpCustomFieldType nDataType,const char* szValue)
{
	if (!szValue)
		return false;

	if (szWksID)
	{
		//�ж���û��Ȩ���޸Ĳ������ֵ
		DP_RIGHT	iRight	= DP_RIGHT_NOTRIGHT;
		if (!m_pDB->m_pRightMgr->GetParamRight(szWksID,m_szParamTableName,iParamID,&iRight) || 
			iRight != DP_RIGHT_READWRITE)
			return false;
	}
	
	const CDpParamItem*		pItem = NULL;
	if (!GetParamInfoByID(iParamID,&pItem) || !pItem)
		return false;

	if (nDataType != pItem->nParamDataType)
		return false;

	//�ȸ������ݿ�
	char	szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update %s Set PARAM_VALUE = '%s' Where PARAM_ID = %d",
		    m_szParamTableName,szValue,pItem->iParamID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	if (pItem->pszValue)
	{
		delete [] ((CDpParamItem*)pItem)->pszValue;
		((CDpParamItem*)pItem)->pszValue = NULL;
	}

	CopyStr(&(((CDpParamItem*)pItem)->pszValue),szValue);
	

	return true;

}

//��Ӳ�����
//����˵��:		[in]iCategoryID -- Ҫ��ӵĲ�������������,���û�з������0,
//              [in]iParamID    -- Ҫ��ӵĲ�����ID,����ò����ǴӲ���ά������ѡ��,
//								   ���ID��Ϊά���Ĳ���ID,������ǴӲ���ά������ѡ��,ID��Ϊ0
//              [in]szParamName -- ��������
//              [in]nDataType   -- ��������������
//              [in]iLen		-- ���������ݳ���,��Ҫ�������ַ���
//              [in]szParamValue-- ������ֵ
//				[in]bIsWHField  -- �Ƿ�����ά������ά��������Ӳ�����,TRUE-��ʾ��
//				[out]iNewParamID-- ���ش����ɹ��Ĳ�����ID,���ʧ�ܷ���0,�������Ҫ,����ΪNULL
//
BOOL CDpParamMgr::AddParamItem(const char* szWksID,int iCategoryID,int iParamID,
								 const char* szParamName,const char* szParamDesc,
								 DpCustomFieldType nDataType,int iLen,
								 int iPrecision,int iSelID,const char* szParamValue,
								 BOOL bIsWHField,int* iNewParamID)
{
	if (!szParamName || strlen(szParamName) < 0 || 
		nDataType == DP_CFT_NULL || nDataType == DP_CFT_BLOB || 
		nDataType == DP_CFT_ENUM || iLen < 0 || iPrecision < 0)
		return false;

	if (iNewParamID)
		*iNewParamID = 0;

	if (szWksID)
	{
		//�жϵ�ǰ�û���û����Ӳ������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
		//���û�жԹ�����д��Ȩ�޾Ͳ������������Ӳ�����
		DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
		if (!m_pDB->m_pRightMgr->GetWorkspaceRight(szWksID,&iRight) || 
			iRight != DP_RIGHT_READWRITE) 
			return false;
	}

	char	szSql[1024];
	memset(szSql,0,sizeof(szSql));

	int		iMaxID	= 0;

	if (bIsWHField)
		iMaxID	= 50000;
	else
		iMaxID = iParamID;

	//�ȸ������ݿ�
	if (iParamID == 0)					//�������ID����0,�ͱ�ʾ��Ҫϵͳ���ò�������һ��ID
	{
		//�ȵõ���ǰ��������IDС��50000�����ID,��Ϊ����50000���Ǳ�ϵͳά���Ĳ���,��ID�������ݿ�ά����
		if (bIsWHField)
			sprintf(szSql,"Select Max(PARAM_ID) From %s Where PARAM_ID >= 50000",m_szParamTableName);
		else
			sprintf(szSql,"Select Max(PARAM_ID) From %s Where PARAM_ID < 50000",m_szParamTableName);
		CDpRecordset	rst(m_pDB);
		if (!rst.Open(szSql))
			return false;
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (!rst.GetFieldValue(1,var))
				return false;
			iMaxID = (int)var;
			if (iMaxID >= 50000)
				return false;
		}

		iMaxID++;
		rst.Close();
	}

	memset(szSql,0,sizeof(szSql));
	char	szValue[256];
	if (szParamValue)
		sprintf(szValue,"'%s'",szParamValue);
	else
		sprintf(szValue,"NULL");

	sprintf(szSql,"Insert Into %s Values(%d,%d,'%s','%s',%d,%d,%d,%d,%s)",
			m_szParamTableName,iCategoryID,iMaxID,szParamName,szParamDesc?szParamDesc:"",nDataType,
			iLen,iPrecision,iSelID,szValue);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	//�ڸ������ݿ�ɹ���,�������ɹ��Ĳ�������뵽��ǰ����������������


	//�ȴ���������Ľڵ�
	CDpParamItem*		pParamItem			= new CDpParamItem;
	pParamItem->iParamID				= iMaxID;
	CopyStr(&(pParamItem->pszParamName),szParamName);
	CopyStr(&(pParamItem->pszParamDesc),szParamDesc);
	pParamItem->nParamDataType			= nDataType;
	pParamItem->iParamDataLen			= iLen;
	pParamItem->iParamPrecision			= iPrecision;
	CopyStr(&(pParamItem->pszValue),szParamValue);
	
	//����ò�����ֵ����Ҫ��ά����ѡ��ֵ��ȥѡ��Ļ�,����ѡ��ֵ������
	if (iSelID != 0)
	{
		memset(szSql,0,sizeof(szSql));
		sprintf(szSql,"Select VALUE_ID,VALUE From DP_ParamValue_WH Where SELECT_ID = %d",iSelID);
		CDpRecordset	rst(m_pDB);
		if (rst.Open(szSql))
		{
			CDpDBVariant		var;
			while (rst.Next())
			{
				CDpParamSelectItem*	pSelItem	= new CDpParamSelectItem;
				rst.GetFieldValue(1,var);
				pSelItem->iSelectID		= iSelID;
				pSelItem->iValueID		= (int)var;
				rst.GetFieldValue(2,var);
				CopyStr(&(pSelItem->szSelectValue),(LPCTSTR)var);
				pParamItem->pSelectItem.push_back(pSelItem);
			}
			rst.Close();
		}
	}


	//���Ҹ÷����Ƿ��Լ�����
	CDpParamCategory*	pCategory			= NULL;
	ParamCategoryArray::iterator itc	= m_ParamData.find(iCategoryID);
	if (itc == m_ParamData.end())					//���಻����
	{
		pCategory = new CDpParamCategory;
		pCategory->iCategoryID	= iCategoryID;

		//����������뵽������ȥ
		m_ParamData.insert(ParamCategoryArray::value_type(iCategoryID,pCategory));

		char		szSqlTmp[256];
		memset(szSqlTmp,0,sizeof(szSqlTmp));
		sprintf(szSqlTmp,"Select CATEGORY_NAME,CATEGORY_DESC From DP_ParamCategory_WH Where CATEGORY_ID = %d",
			    iCategoryID);

		//�õ��÷�������ƺͱ�ע
		CDpRecordset	rst(m_pDB);
		if (rst.Open(szSqlTmp))
		{
			if (rst.Next())
			{
				CDpDBVariant var;
				rst.GetFieldValue(1,var);
				CopyStr(&(pCategory->szCategoryName),(LPCTSTR)var);
				rst.GetFieldValue(2,var);
				CopyStr(&(pCategory->szCategoryDesc),(LPCTSTR)var);
			}
			rst.Close();
		}
	}											//�����Ѵ���
	else
		pCategory	= (*itc).second;

	//����������뵽������
	if (pCategory)
		pCategory->pParamItemArray.push_back(pParamItem);

	if (iNewParamID)
		*iNewParamID = iMaxID;

	return true;
}


//ɾ��һ��������,ͬʱ�÷����µĲ�����Ҳ��ɾ��
BOOL CDpParamMgr::DeleteCategory(const char* szWksID,int iCategoryID)
{
	if (szWksID)
	{
		//�жϵ�ǰ�û���û����Ӳ������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
		//���û�жԹ�����д��Ȩ�޾Ͳ������������Ӳ�����
		DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
		if (!m_pDB->m_pRightMgr->GetWorkspaceRight(szWksID,&iRight) || 
			iRight != DP_RIGHT_READWRITE) 
			return false;
	}
	
	//��ɾ�����ݿ��в��������Ӧ��¼
	char		szSql[128];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From %s Where CATEGORY_ID = %d",m_szParamTableName,iCategoryID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	CDpParamCategory*	pCategory			= NULL;
	ParamCategoryArray::iterator itc	= m_ParamData.find(iCategoryID);
	if (itc == m_ParamData.end())					//���಻����
		return false;
	else
	{
		pCategory	= (*itc).second;
		m_ParamData.erase(itc);
		delete pCategory;							//ɾ��������Ľڵ��Լ��ͷ����ڴ�
		pCategory = NULL;
	}

	return true;
}

//ɾ��һ��������,ͬʱɾ���ò������µ�ѡ��ֵ��,bDelEmptyCategory-��ʾ���ɾ����ķ�����Ϊ�յ�ʱ��,�Ƿ�ɾ���յķ�����,TRUEΪɾ��
BOOL CDpParamMgr::DeleteParamItem(const char* szWksID,int iParamID,BOOL bDelEmptyCategory)
{
	if (szWksID)
	{
		//�жϵ�ǰ�û���û����Ӳ������Ȩ��,Ҳ���ǵ�ǰ�û���û�жԹ�����д��Ȩ��
		//���û�жԹ�����д��Ȩ�޾Ͳ������������Ӳ�����
		DP_RIGHT	iRight = DP_RIGHT_NOTRIGHT;
		if (!m_pDB->m_pRightMgr->GetWorkspaceRight(szWksID,&iRight) || 
			iRight != DP_RIGHT_READWRITE) 
			return false;
	}
	
	CDpParamCategory*	pCategory		= NULL;
	CDpParamItem*		pItem			= NULL;
	BOOL				bFind			= false;

	//�Ȳ��Ҹò��������Ǹ���������
	ParamCategoryArray::iterator itc;
	ParamItemArray::iterator	 itp;

	for (itc = m_ParamData.begin(); itc != m_ParamData.end(); ++itc)
	{
		pCategory = (*itc).second;
		if (pCategory)
		{
			//���Ҳ�����
			for (itp = pCategory->pParamItemArray.begin();
			     itp != pCategory->pParamItemArray.end(); ++itp)
			{
				if ((*itp)->iParamID == iParamID)
				{
					bFind = true;
					pItem = (*itp);
					break;
				}
			}
		}
		if (bFind)
			break;
	}

	if (!bFind || !pCategory || !pItem)
		return false;

	//��ɾ�����ݿ��в��������Ӧ��¼
	char		szSql[128];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Delete From %s Where PARAM_ID = %d",m_szParamTableName,iParamID);
	if (m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	pCategory->pParamItemArray.erase(itp);	
	delete pItem;								//�ͷ��ڴ�
	pItem = NULL;

	if (bDelEmptyCategory)
	{
		if (pCategory->pParamItemArray.size() <= 0)
		{
			this->m_ParamData.erase(itc);
			delete pCategory;
			pCategory = NULL;
		}
	}

	return true;
}
















/****************************************************************/
/*           ���WBS�ṹ��Ƶ�����ά�������������              */
/****************************************************************/
CDpWBSCodeWH::CDpWBSCodeWH(CDpDatabase* pDB,DP_WORKSPACE_TYPE type)
{
	m_pDB		= pDB;
	m_WSPType	= type;
}

CDpWBSCodeWH::~CDpWBSCodeWH()
{
	int		iCount = m_pWHData.size();
	for (int i = 0; i < iCount; i++)
	{
		CDpWBSAPartWH* pTmp = m_pWHData[i];
		if (pTmp)
		{
			delete pTmp;
			pTmp = NULL;
		}
	}
	m_pWHData.clear();
}

//�����ݿ��м�������
BOOL CDpWBSCodeWH::LoadDataFromDB()
{
	if (!m_pDB)
		return false;

	CDpRecordset		rstA(m_pDB);
	char				szSql[256];
	memset(szSql,0,sizeof(szSql));

	//�ȵõ�A���ֵ�ά��
	char	szWSPType[1];
	memset(szWSPType,0,sizeof(szWSPType));
	_itoa((int)m_WSPType,szWSPType,10);
	sprintf(szSql,"Select WBS_A,WBS_A_NAME,WBS_A_DESC From DP_WBS_A_WH WHERE WBS_WORKSPACE_TYPE='%s' ORDER BY WBS_A",szWSPType);
	if (rstA.Open(szSql))
	{
		CDpDBVariant	var;
		while (rstA.Next())
		{
			if (!rstA.GetFieldValue(1,var))
				continue;
			char			szA[2];			
			memset(szA,0,sizeof(szA));						
			strcpy(szA,(LPCTSTR)var);

			CDpWBSAPartWH*	pA	= new CDpWBSAPartWH;
			m_pWHData.push_back(pA);			//���뵽A��Ŀ��������
			pA->iA_ID = atoi((LPCTSTR)var);
			//ȡA�ı�ʶ
			pA->A	= DpAPartType(atoi((LPCTSTR)var)%10);		
			//ȡA���� 
			rstA.GetFieldValue(2,var);
			CopyStr(&(pA->szName),(LPCTSTR)var);
			//ȡA����
			rstA.GetFieldValue(3,var);
			CopyStr(&(pA->szDesc),(LPCTSTR)var);		


			//ȡ��A�µ�C
			CDpRecordset	rstC(m_pDB);
			char			szSqlC[256];			
			sprintf(szSqlC,"Select WBS_C,WBS_C_NAME,WBS_C_DESC From DP_WBS_C_WH Where WBS_A = '%s' ORDER BY WBS_C",szA);
			CDpDBVariant	varC;
			if (rstC.Open(szSqlC))
			{
				while (rstC.Next())
				{
					if (!rstC.GetFieldValue(1,varC))
						continue;
					char			szC[3];
					memset(szC,0,sizeof(szC));					
					strcpy(szC,(LPCTSTR)varC);

					CDpWBSCPartWH*	pC = new CDpWBSCPartWH;
					//���뵽��Ӧ��A������
					pA->cArray.push_back(pC);
					pC->iC_ID = atoi((LPCTSTR)varC);
					//ȡC�ı�ʶ
					pC->C	= DP_LAYER_TYPE_EX(atoi((LPCTSTR)varC)%100);
					//ȡC����
					rstC.GetFieldValue(2,varC);
					CopyStr(&(pC->szName),(LPCTSTR)varC);
					//ȡC����
					rstC.GetFieldValue(3,varC);
					CopyStr(&(pC->szDesc),(LPCTSTR)varC);					

					//ȡ��C�µ�D
					CDpRecordset	rstD(m_pDB);
					char			szSqlD[256];		
					memset(szSqlD,0,sizeof(szSqlD));										
					sprintf(szSqlD,"Select WBS_D,WBS_D_NAME,WBS_D_DESC From DP_WBS_D_WH Where WBS_C = '%s'  ORDER BY WBS_D",szC);
					CDpDBVariant	varD;
					if (rstD.Open(szSqlD))
					{
						while (rstD.Next())
						{
							if (!rstD.GetFieldValue(1,varD))
								continue;
							CDpWBSDPartWH*	pD = new CDpWBSDPartWH;
							//���뵽��Ӧ��C������
							pC->dArray.push_back(pD);
							pD->iD_ID = atoi((LPCTSTR)varD);														
							//ȡD������
							rstD.GetFieldValue(2,varD);
							CopyStr(&(pD->szName),(LPCTSTR)varD);
							//ȡD������
							rstD.GetFieldValue(3,varD);
							CopyStr(&(pD->szDesc),(LPCTSTR)varD);
						}
						rstD.Close();
					}

				}
				rstC.Close();
			}

		}
		rstA.Close();
		return true;
	}

	return false;
}


//�õ�A�����е�ĳ���ڵ�CDpWBSAPartWH* GetANode(DpAPartType A);
CDpWBSCodeWH::CDpWBSAPartWH* CDpWBSCodeWH::GetANode(enum DpAPartType A)
{
	int	iCount = GetACount();
	for (int i = 0; i < iCount; i++)
	{
		CDpWBSAPartWH* pTmp = m_pWHData[i];
		if (pTmp && pTmp->A == A)
			return pTmp;
	}
	return NULL;
}

//�õ�ָ����C�Ľڵ�
CDpWBSCodeWH::CDpWBSCPartWH* CDpWBSCodeWH::GetCNode(int cID)
{
	DpAPartType A;
	A=DpAPartType((cID/100)%10);
	CDpWBSAPartWH* pA = GetANode(A);
	if (pA)
	{
		int iCount = pA->cArray.size();
		for (int i = 0; i < iCount; i++)
		{
			CDpWBSCPartWH* pTmp = pA->cArray[i];
			if (pTmp && pTmp->iC_ID == cID)
				return pTmp;
		}
	}
	return NULL;
}

CDpWBSCodeWH::CDpWBSDPartWH* CDpWBSCodeWH::GetDNode(int dID)
{
	int cID=dID/100;
	CDpWBSCPartWH* pC = GetCNode(cID);
	if (pC)
	{
		int iCount = pC->dArray.size();
		for (int i = 0; i < iCount; i++)
		{
			CDpWBSDPartWH* pTmp = pC->dArray[i];
			if (pTmp && pTmp->iD_ID == dID)
				return pTmp;
		}
	}
	return NULL;
}


//�õ�A����Ŀ
int CDpWBSCodeWH::GetACount()
{
	return m_pWHData.size();
}

//�õ�C����Ŀ,A -- Ϊ������A�ı�ʶ
int CDpWBSCodeWH::GetCCount(enum DpAPartType A)
{
	CDpWBSAPartWH* pA = GetANode(A);
	if (pA)
		return pA->cArray.size();
	else
		return 0;
}

//�õ�D����Ŀ,C--Ϊ������C�ı�ʶ
int CDpWBSCodeWH::GetDCount(int cID)
{
	CDpWBSCPartWH* pC = GetCNode(cID);
	if (pC)
		return pC->dArray.size();	
	return 0;
}


//�õ������A,iIndex -- Ϊ���,��0��ʼ. szName,szDesc���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetA(int iIndex,enum DpAPartType* pA,
						const char** ppszName,const char** ppszDesc)
{
	if (iIndex < 0 || iIndex > (GetACount()-1) || !pA || !ppszName)
		return false;
	CDpWBSAPartWH* pTmp = m_pWHData[iIndex];
	*pA			= pTmp->A;
	*ppszName	= pTmp->szName;
	if (ppszDesc)	
		*ppszDesc= pTmp->szDesc;
	return true;
}

//�õ������C,A -- Ϊ������A�ı�ʶ,iIndex -- Ϊ���,��0��ʼ.  szName,szDesc���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetC(enum DpAPartType A,int iIndex,int* cID,
						const char** ppszName,const char** ppszDesc)
{
	if (A == DP_A_NULL || !cID || !ppszName)
		return false;

	CDpWBSAPartWH*	pA = GetANode(A);
	if (!pA)
		return false;
	if (iIndex < 0 || iIndex > (pA->cArray.size()-1))
		return false;

	CDpWBSCPartWH*	pTmp = pA->cArray[iIndex];
	if (pTmp)
	{
		*cID		= pTmp->iC_ID;
		*ppszName	= pTmp->szName;
		if (ppszDesc)
			*ppszDesc = pTmp->szDesc;
		return true;
	}

	return false;
}

//�õ������D,C -- Ϊ������C�ı�ʶ,iIndex-���,��0��ʼ. szName,szDesc���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetD(int cID,int iIndex,int* dID,
						const char** ppszName,const char** ppszDesc)
{
	if (!dID|| !ppszName)
		return false;

	CDpWBSCPartWH*	pC		= GetCNode(cID);
	if (!pC)
		return false;

	if (iIndex < 0 || iIndex > (pC->dArray.size()-1))
		return false;

	CDpWBSDPartWH*	pTmp	= pC->dArray[iIndex];
	if (pTmp)
	{
		*dID		= pTmp->iD_ID;
		*ppszName	= pTmp->szName;
		if (ppszDesc)
			*ppszDesc = pTmp->szDesc;
		return true;
	}

	return false;
}


//ͨ��A�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetAName(enum DpAPartType A,const char** ppszName)
{
	if (A == DP_A_NULL || !ppszName)
		return false;

	*ppszName	= NULL;

	CDpWBSAPartWH*	pA = GetANode(A);
	if (pA)
	{
		*ppszName	= pA->szName;
		return true;
	}
	return false;
}	

//ͨ��A�ı�ʶ�õ�����,ppszDesc���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetADesc(enum DpAPartType A,const char** ppszDesc)
{
	if (A == DP_A_NULL || !ppszDesc)
		return false;

	*ppszDesc	= NULL;
	CDpWBSAPartWH*	pA = GetANode(A);
	if (pA)
	{
		*ppszDesc	= pA->szDesc;
		return true;
	}
	return false;
}



//ͨ��C�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetCName(int cID,const char** ppszName)
{
	if (!ppszName)
		return false;

	*ppszName	= NULL;
	CDpWBSCPartWH* pC= GetCNode(cID);
	if(pC)
	{
		*ppszName = pC->szName;
		return true;
	}
	return false;
}


//ͨ��C�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetCDesc(int cID,const char**  ppszDesc)
{
	if (!ppszDesc)
		return false;

	*ppszDesc	= NULL;
	CDpWBSCPartWH* pC= GetCNode(cID);
	if(pC)
	{
		*ppszDesc = pC->szDesc;
		return true;
	}
	return false;
}



//ͨ��D�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetDName(int dID,const char** ppszName)
{
	if (!ppszName)
		return false;

	*ppszName = NULL;
	CDpWBSDPartWH* pTemp=GetDNode(dID);
	if(pTemp)
	{
		*ppszName=pTemp->szName;
		return true;
	}
	return false;
}

//ͨ��D�ı�ʶ�õ�����,ppszDesc���ڴ�����߲���ȥ�ͷ�
BOOL CDpWBSCodeWH::GetDDesc(int dID,const char** ppszDesc)
{
	if (!ppszDesc)
		return false;

	*ppszDesc = NULL;
	CDpWBSDPartWH* pTemp=GetDNode(dID);
	if(pTemp)
	{
		*ppszDesc=pTemp->szDesc;
		return true;
	}
	return false;
}


//�ú���ͨ��ָ������·��,���D������Ӧ���������
//����:  index[out] ---��������ŷ��ص�index��,����:0,1,2,3.....
BOOL CDpWBSCodeWH::GetDIndex(int dID,int *index)
{
	if(NULL==index)
		return FALSE;
	CDpWBSCPartWH* cPart=GetCNode(dID/100);
	int cCount=GetDCount(dID/100);
	for(int i=0;i<cCount;i++)
	{
		CDpWBSDPartWH* pTmp = cPart->dArray[i];
		if (pTmp && pTmp->iD_ID == dID)
		{
			*index=i;
			return TRUE;
		}
	}
	return FALSE;

}

//�ú���ͨ��D���ֵ�ID�����Ӧ���������
//����:  index[out] ---��������ŷ��ص�index��,����:0,1,2,3.....
/*
BOOL CDpWBSCodeWH::GetDIndex(char *pD_ID, int *index)
{
	if(NULL==pD_ID||NULL==index)
		return FALSE;

	//D���ֵ�IDΪ����ַ�,��һΪ����������,�ڶ�ΪA��������
	//,����ΪC��������,������ΪD��������
	if(5!=strlen(pD_ID))
		return FALSE;

	DpAPartType A;
	DpCPartType C;
	DpDPartType D;
	A=(DpAPartType)(atoi(pD_ID+1)/1000);
	C=(DpCPartType)(atoi(pD_ID+2)/100);
	D=(DpDPartType)(atoi(pD_ID+3));
	return GetDIndex(A,C,D,index);
}
*/






/********************************************************************/
/*                          ����������                            */
/********************************************************************/
CDpTaskMgr::CDpTaskMgr(CDpWorkspace* pWks)
{
	m_pWks			= pWks;
	m_szTaskName	= NULL;
	m_szUserID		= NULL;
	memset(m_szTaskID,0,sizeof(m_szTaskID));
	memset(&m_WBSCode,0,sizeof(CDpTaskMgr::DpWBSCodeData));
}

CDpTaskMgr::~CDpTaskMgr()
{
	FreeMem();
}

//�ͷ��ڴ�
void CDpTaskMgr::FreeMem()
{
	if (m_szTaskName)
	{
		delete [] m_szTaskName;
		m_szTaskName = NULL;
	}
	if (m_WBSCode.szAName)
	{
		delete [] m_WBSCode.szAName;
		m_WBSCode.szAName = NULL;
	}
	if (m_WBSCode.B)
	{
		delete [] m_WBSCode.B;
		m_WBSCode.B = NULL;
	}
	if (m_WBSCode.C)
	{
		delete [] m_WBSCode.C;
		m_WBSCode.C = NULL;
	}
	if (m_WBSCode.szDName)
	{
		delete [] m_WBSCode.szDName;
		m_WBSCode.szDName = NULL;
	}
	if (m_WBSCode.szTypeEx)
	{
		delete [] m_WBSCode.szTypeEx;
		m_WBSCode.szTypeEx = NULL;
	}
	if (m_WBSCode.szStartTime)
	{
		delete [] m_WBSCode.szStartTime;
		m_WBSCode.szStartTime = NULL;
	}
	if (m_WBSCode.szEndTime)
	{
		delete [] m_WBSCode.szEndTime;
		m_WBSCode.szEndTime = NULL;
	}
	if (m_WBSCode.szTaskDesc)
	{
		delete [] m_WBSCode.szTaskDesc;
		m_WBSCode.szTaskDesc = NULL;
	}

	memset(&m_WBSCode,0,sizeof(DpWBSCodeData));

	if (m_szUserID)
	{
		delete [] m_szUserID;
		m_szUserID = NULL;
	}
}


//������
BOOL CDpTaskMgr::OpenTask(const char* szTaskID)
{
	if (!szTaskID ||strlen(szTaskID) < TASK_ID_LEN || !m_pWks)
		return false;

	char		szSql[512];
	memset(szSql,0,sizeof(szSql));

	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() == DP_ADMIN)
		sprintf(szSql,"Select * From DP_TASKMGR_MAIN where WORKSPACE_ID = '%s' AND TASK_ID = '%s'",
				m_pWks->m_szID,szTaskID);
	else
		sprintf(szSql,"Select * From DP_TASKMGR_MAIN where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND TASK_ID = '%s'",
				m_pWks->m_szID,m_pWks->m_pDB->m_szUserID,szTaskID);

	CDpRecordset	rst(m_pWks->m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(2,var))
			{	
				//ȡ�û�ID
				CopyStr(&(m_szUserID),(LPCTSTR)var);	

				//ȡ����
				rst.GetFieldValue(4,var);
				CopyStr(&m_szTaskName,(LPCTSTR)var);
				//ȡA
				rst.GetFieldValue(5,var);
				m_WBSCode.A		= DpAPartType((int)var);
				//ȡB
				rst.GetFieldValue(6,var);
				CopyStr(&(m_WBSCode.B),(LPCTSTR)var);
				//ȡC
				rst.GetFieldValue(7,var);
				CopyStr(&(m_WBSCode.C),(LPCTSTR)var);
				//ȡD
				rst.GetFieldValue(8,var);
				m_WBSCode.D		= (int)var;
				//ȡ��ʼʱ��
				rst.GetFieldValue(9,var);
				CopyStr(&(m_WBSCode.szStartTime),(LPCTSTR)var);
				//ȡ����ʱ��
				rst.GetFieldValue(10,var);
				CopyStr(&(m_WBSCode.szEndTime),(LPCTSTR)var);
				//ȡ����״̬
				rst.GetFieldValue(11,var);
				m_WBSCode.iPercent	= (int)var;
				//ȡ�Ƿ�ϸ�
				rst.GetFieldValue(12,var);
				m_WBSCode.iPass		= (int)var;
				//ȡ����ע
				rst.GetFieldValue(13,var);
				CopyStr(&(m_WBSCode.szTaskDesc),(LPCTSTR)var);
			}

		}
		else
			return false;

		rst.Close();

		//�²�:���A��Ӱ��,��m_WBSCode.CΪC���ֵ�����;���A��ͼ��,��,m_WBSCode.CΪC���ֵ�ID��.
		//�жϵ�ǰ���������
		if (m_WBSCode.A == DP_A_IMAGE)				//���ΪIMAGE,����������;�ȡȡ����
		{
			if (_stricmp(m_WBSCode.C,"IOP") == 0)
				m_WBSCode.TypeEx = LAYER_TYPE_EX_IOP;//HOUKUI,06,7,13
			else if (_stricmp(m_WBSCode.C,"AOP") == 0)
				m_WBSCode.TypeEx = LAYER_TYPE_EX_AOP;//HOUKUI,06,7,13
			else if (_stricmp(m_WBSCode.C,"IMAGEPOINT") == 0)
				m_WBSCode.TypeEx = LAYER_TYPE_EX_IMAGEPOINT;//HOUKUI,06,7,13
		}
		else								//����MAP������
		{
			//����C,Ҳ����ͼ��ID��֪����ͼ�������
			DP_LAYER_TYPE_EX		iLayerTypeEx	= LAYER_TYPE_EX_NULL;
			if (m_pWks->GetLayerType(m_WBSCode.C,&iLayerTypeEx) == GEO_NULL)
				return false;
			m_WBSCode.TypeEx	= iLayerTypeEx;
		}

		//�õ���������ά������
		CDpWBSCodeWH*		pWh = NULL;
		if (!m_pWks->m_pDB->OpenWBSWH(&pWh,(DP_WORKSPACE_TYPE)(m_pWks->m_nWksType)) || !pWh)
			return false;
		
		//�õ�A��������Ϣ
		const	char*	szTmp = NULL;
		pWh->GetADesc(m_WBSCode.A,&szTmp);
		if (!szTmp || strlen(szTmp) > 0)
			CopyStr(&(m_WBSCode.szAName),szTmp);
		
		//�õ�D��������Ϣ
		pWh->GetDDesc(m_WBSCode.D,&szTmp);
		if (!szTmp || strlen(szTmp) > 0)
			CopyStr(&(m_WBSCode.szDName),szTmp);
		
		if (m_WBSCode.A == DP_A_IMAGE)				
		{
			int cID_1;
			cID_1=((int)(m_pWks->m_nWksType))*1000+int(m_WBSCode.A)*100+m_WBSCode.TypeEx;
			pWh->GetCDesc(cID_1,&szTmp);
			if (!szTmp || strlen(szTmp) > 0)
				CopyStr(&(m_WBSCode.szTypeEx),szTmp);
			else
				CopyStr(&(m_WBSCode.szTypeEx),m_WBSCode.C);			//������ݿ�û��ά��,��ֱ�ӽ�C��Ϊ����������������
		}
		else
		{
			int cID_2;
			cID_2=((int)(m_pWks->m_nWksType))*1000+int(m_WBSCode.A)*100+m_WBSCode.TypeEx;
			pWh->GetCDesc(cID_2,&szTmp);
			if (!szTmp || strlen(szTmp) > 0)
				CopyStr(&(m_WBSCode.szTypeEx),szTmp);
		}

		delete pWh;

		strcpy(m_szTaskID,szTaskID);

		return true;
	}
	else
		return false;
}


//�õ�WBS��ĸ�������
BOOL CDpTaskMgr::Get(DpAPartType* A,const char** B,const char** C,int* D)
{
	*A	= m_WBSCode.A;
	*B	= m_WBSCode.B;
	*C	= m_WBSCode.C;
	*D	= m_WBSCode.D;
	return true;
}

//�õ�WBS��ĸ������ֵ�����
BOOL CDpTaskMgr::Get(const char** A,const char** B,const char** C,const char** D)
{
	*A = m_WBSCode.szAName;
	*B = m_WBSCode.B;
	*C = m_WBSCode.C;//ID
	*D = m_WBSCode.szDName;
	return true;
}

//����WBS��ĸ�������
BOOL CDpTaskMgr::Set(DpAPartType A,const char* B,const char* C,int D)
{
	if (A == DP_A_NULL || !B || !C)
		return false;
	
	char	szSql[512];
	char	szB[20];
	memset(szSql,0,sizeof(szSql));
	memset(szB,0,sizeof(szB));

	sprintf(szSql,"Update DP_TASKMGR_MAIN Set WBS_A = %d,WBS_B = '%s',WBS_C = '%s',WBS_D = %d WHERE TASK_ID = '%s'",
		    A,B,C,D,m_szTaskID);

	if (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{
		//�����ԭ���ڴ�
		FreeMem();
		OpenTask(m_szTaskID);
		return true;
	}
	else
		return false;
}

//���������״̬
BOOL CDpTaskMgr::SetTaskStatus(int iPercent,int iPass,char* szDesc,int iCurrentAllowFolwD)
{
	if (!m_szTaskID || strlen(m_szTaskID) <= 0)
		return false;

//	if (m_WBSCode.iPercent == 100)				//�����ǰ���ȱ���Ϊ100,����ϵͳ����Ա,�Ͳ����޸�״̬
//	{
//		if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
//			return false;
//	}

	//���ж��Ƿ��ܽ����޸�
	//���޸ĵ�ǰ����������ݵ�����״̬
	char			szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select CURRENTALLOW_D From DP_TASK_STATUS Where WORKSPACE_ID = '%s' AND WBS_A = %d AND WBS_B = '%s' AND WBS_C = '%s'",
			m_pWks->m_szID,m_WBSCode.A,m_WBSCode.B?m_WBSCode.B:"",m_WBSCode.C?m_WBSCode.C:"");

	int				iCount			= 0;
	int				iCurrentD		= 0;
	CDpDBVariant	var;
	CDpRecordset	rst(m_pWks->m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next() && rst.GetFieldValue("CURRENTALLOW_D",var))
		{
			iCount		= 1;
			iCurrentD	= (int)var;
		}
		rst.Close();
	}

	if (iCount > 0)
	{
		if (iCurrentD != m_WBSCode.D)				//�����ǰ�����D�ͱ������D��ͬ�Ͳ������޸�
			return false;
	}
	
	//���޸�����״̬
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_TASKMGR_MAIN Set TASK_PERCENT = %d,TASK_PASS = %d,TASK_DESC = '%s' where TASK_ID = '%s'",
			iPercent,iPass,szDesc?szDesc:"",m_szTaskID);

	if (m_pWks->m_pDB->ExecuteSQL(szSql) != DP_SQL_SUCCESS)
		return false;

	//���޸�����״̬��
	memset(szSql,0,sizeof(szSql));

	if (iCount == 0)
		sprintf(szSql,"Insert Into DP_TASK_STATUS Values('%s',%d,'%s','%s','%s',%d)",
		        m_pWks->m_szID,m_WBSCode.A,m_WBSCode.B?m_WBSCode.B:"",m_WBSCode.C?m_WBSCode.C:"",m_szTaskID,iCurrentAllowFolwD);
	else
	{

		sprintf(szSql,"Update DP_TASK_STATUS Set TASK_ID = '%s',CURRENTALLOW_D = %d Where WORKSPACE_ID = '%s' AND WBS_A = %d AND WBS_B = '%s' AND WBS_C = '%s'",
		        m_szTaskID,iCurrentAllowFolwD,m_pWks->m_szID,m_WBSCode.A,m_WBSCode.B?m_WBSCode.B:"",m_WBSCode.C?m_WBSCode.C:"");
	}

	return (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}





















/*******************************************************************/
/*                        ������������                           */
/*******************************************************************/
CDpTaskGroup::CDpTaskGroup()
{
	m_szGroupName		= NULL;		
	m_WBS_B				= NULL;
	m_szStartTime		= NULL;
	m_szEndTime			= NULL;
	m_szDesc			= NULL;
	m_iPass				= 0;
	m_iPercent			= 0;
	m_WBS_A				= DP_A_NULL;
	m_WBS_D				= 0;
	m_uGroupID			= 0;

}

CDpTaskGroup::~CDpTaskGroup()
{
	int		iCount	= m_TaskArray.size();

	for (int i = 0; i < iCount; i++)
	{	
		_DpTaskInfo*	pItem = m_TaskArray[i];
		if (pItem)
		{
			if (pItem->szTaskID)	
			{
				delete [] pItem->szTaskID;
				pItem->szTaskID = NULL;
			}
			if (pItem->szTaskName)		
			{
				delete [] pItem->szTaskName;
				pItem->szTaskName = NULL;
			}
			if (pItem->szWBS_C)	
			{
				delete [] pItem->szWBS_C;
				pItem->szWBS_C = NULL;
			}
			if (pItem->szStartTime)		
			{
				delete [] pItem->szStartTime;
				pItem->szStartTime = NULL;
			}
			if (pItem->szEndTime)	
			{
				delete [] pItem->szEndTime;
				pItem->szEndTime = NULL;
			}
			if (pItem->szDesc)
			{
				delete [] pItem->szDesc;
				pItem->szDesc = NULL;
			}
			delete pItem;
			pItem = NULL;

		}
	}	
	m_TaskArray.clear();

	if (m_szGroupName)	
	{
		delete [] m_szGroupName;
		m_szGroupName = NULL;
	}
	if (m_WBS_B)			
	{
		delete [] m_WBS_B;
		m_WBS_B = NULL;
	}
	if (m_szStartTime)	
	{
		delete [] m_szStartTime;
		m_szStartTime = NULL;
	}
	if (m_szEndTime)		
	{
		delete [] m_szEndTime;
		m_szEndTime = NULL;
	}
	if (m_szDesc)			
	{
		delete [] m_szDesc;
		m_szDesc = NULL;
	}

}


//�õ����������������Ŀ
int CDpTaskGroup::GetTaskCount()
{
	return m_TaskArray.size();
}


//�õ���������
const CDpTaskGroup::_DpTaskInfo* CDpTaskGroup::GetTaskItem(int iIndex)
{
	if (iIndex >= GetTaskCount())
		return NULL;

	return m_TaskArray[iIndex];
}





















/*******************************************************************/
/*                        ���񼯹������                           */
/*******************************************************************/
CDpTaskSet::CDpTaskSet(CDpWorkspace* pWks,const char* szUserID)
{
	m_pWks	= pWks;
	strcpy(m_szUserID,szUserID);
}

CDpTaskSet::~CDpTaskSet()
{
	FreeMem();
}

//�ͷ��ڴ�
void CDpTaskSet::FreeMem()
{
	DPTASKSET::iterator		i;

	for (i = m_TaskSet.begin(); i != m_TaskSet.end();++i)
	{
		CDpTaskGroup*	pGroup = (*i).second;

		if (pGroup)
		{
			delete pGroup;
			pGroup = NULL;
		}
	}
	m_TaskSet.clear();
}



//�����ݿ���ȡ��������,�ú�����Ҫ����,���ܳ���BUG,HOUKUI,06,7,10
BOOL CDpTaskSet::GetDataForDB(const _DpTaskFilter* pFilter/*const char* szFilterClause*/)
{
	if (!m_pWks)
		return false;

	//�õ���������ά������
	CDpWBSCodeWH*		pWh = NULL;
	if (!m_pWks->m_pDB->OpenWBSWH(&pWh,m_pWks->m_nWksType) || !pWh)
		return false;

	char		szSql[1024];
	memset(szSql,0,sizeof(szSql));

	//��ȡ��DLG������,������Ҫ�����������
	//sprintf(szSql,"Select * From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND ((WBS_D - 23000) < 0  OR (WBS_D - 23000) >= 1000)",
	//	    m_pWks->m_szID,m_szUserID);
	sprintf(szSql,"Select * From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND (WBS_D/100)%100<>23", //���ڱ���ı���,������Ҫ�޸�
		    m_pWks->m_szID,m_szUserID);



	char		szTaskFilter[512];
	memset(szTaskFilter,0,sizeof(szTaskFilter));
	char		szTmp[256];
	memset(szTmp,0,sizeof(szTmp));

	if (pFilter)
	{
		//A
		if (pFilter->AFilter != DP_A_IMAGE)
			sprintf(szTaskFilter,"WBS_A = %d",pFilter->AFilter);
		

		//B
		if (pFilter->BFilter && strlen(pFilter->BFilter) > 0)
		{
			sprintf(szTmp,"WBS_B = '%s'",pFilter->BFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}

		//C
		if (pFilter->CFilter && strlen(pFilter->CFilter))
		{
			sprintf(szTmp,"WBS_C = '%s'",pFilter->CFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}
		
		//D
		if (pFilter->DFilter != 0)
		{
			sprintf(szTmp,"WBS_D = %d",pFilter->DFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}


		//���������ѯ��������
		if (strlen(szTaskFilter) > 0)
		{
			strcat(szSql," AND ");
			strcat(szSql,szTaskFilter);
		}

	}

	CDpRecordset	rst(m_pWks->m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant		varTaskID;
			CDpDBVariant		varTaskName;
			CDpDBVariant		varTaskA;
			CDpDBVariant		varTaskB;
			CDpDBVariant		varTaskC;
			CDpDBVariant		varTaskD;
			CDpDBVariant		varTaskStartTime;
			CDpDBVariant		varTaskEndTime;
			CDpDBVariant		varTaskPercent;
			CDpDBVariant		varTaskPass;
			CDpDBVariant		varTaskDesc;

			if (rst.GetFieldValue(3,varTaskID) &&
				rst.GetFieldValue(4,varTaskName) &&
				rst.GetFieldValue(5,varTaskA) &&
				rst.GetFieldValue(6,varTaskB) &&
				rst.GetFieldValue(7,varTaskC) &&
				rst.GetFieldValue(8,varTaskD) &&
				rst.GetFieldValue(9,varTaskStartTime) &&
				rst.GetFieldValue(10,varTaskEndTime) &&
				rst.GetFieldValue(11,varTaskPercent) &&
				rst.GetFieldValue(12,varTaskPass) &&
				rst.GetFieldValue(13,varTaskDesc))
			{
				CDpTaskGroup*		pGroup	= new CDpTaskGroup;
				pGroup->m_uGroupID	= _atoi64((LPCTSTR)varTaskID);		//��DLG���������ID��������ID
				CopyStr(&(pGroup->m_szGroupName),(LPCTSTR)varTaskName);	//��DLG������������ƾ�����������
				pGroup->m_WBS_A		= DpAPartType((int)varTaskA);
				CopyStr(&(pGroup->m_WBS_B),(LPCTSTR)varTaskB);
				pGroup->m_WBS_D		= (int)varTaskD;
				pGroup->m_iPercent	= (int)varTaskPercent;
				pGroup->m_iPass		= (int)varTaskPass;
				CopyStr(&(pGroup->m_szStartTime),(LPCTSTR)varTaskStartTime);
				CopyStr(&(pGroup->m_szEndTime),(LPCTSTR)varTaskEndTime);
				CopyStr(&(pGroup->m_szDesc),(LPCTSTR)varTaskDesc);

				//����һ��������
				CDpTaskGroup::_DpTaskInfo* pItem = new CDpTaskGroup::_DpTaskInfo;
				memset(pItem,0,sizeof(CDpTaskGroup::_DpTaskInfo*));
				CopyStr(&(pItem->szTaskID),(LPCTSTR)varTaskID);
				CopyStr(&(pItem->szTaskName),pGroup->m_szGroupName);
				CopyStr(&(pItem->szWBS_C),(LPCTSTR)varTaskC);
				pItem->iPercent		= pGroup->m_iPercent;
				pItem->iPass		= pGroup->m_iPass;
				CopyStr(&(pItem->szStartTime),pGroup->m_szStartTime);
				CopyStr(&(pItem->szEndTime),pGroup->m_szEndTime);
				CopyStr(&(pItem->szDesc),pGroup->m_szDesc);
				
				//�������������
				pGroup->m_TaskArray.push_back(pItem);

				//����������뵽����
				m_TaskSet.insert(DPTASKSET::value_type(pGroup->m_uGroupID,pGroup));
			}

		}
		rst.Close();
	}
	

	//��ȡDLG�������
	memset(szSql,0,sizeof(szSql));

	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"SELECT WORKSPACE_ID,USER_ID,WBS_A,WBS_B,WBS_D,CSTR(IIF(ISNULL(MIN(TASK_START_TIME)),0,MIN(TASK_START_TIME))) AS StartTime,CSTR(MAX(IIF(ISNULL(TASK_END_TIME),0,TASK_END_TIME))) AS EndTime,CINT(AVG(IIF(ISNULL(TASK_PERCENT),0,TASK_PERCENT))) AS TaskFinishPercent,MIN(IIF(ISNULL(TASK_PASS),0,TASK_PASS)) as IsPass From DP_TASKMGR_MAIN");
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"SELECT WORKSPACE_ID,USER_ID,WBS_A,WBS_B,WBS_D,TO_CHAR(NVL(MIN(TASK_START_TIME),0)) AS StartTime,TO_CHAR(NVL(MAX(TASK_END_TIME),0)) AS EndTime,AVG(NVL(TASK_PERCENT,0))  AS TaskFinishPercent,MIN(NVL(TASK_PASS,0)) as IsPass From DP_TASKMGR_MAIN");
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"SELECT WORKSPACE_ID,USER_ID,WBS_A,WBS_B,WBS_D,STR(ISNULL(MIN(TASK_START_TIME),0))  AS StartTime,STR(ISNULL(MAX(TASK_END_TIME),0)) AS EndTime,AVG(ISNULL(TASK_PERCENT,0))  AS TaskFinishPercent,MIN(ISNULL(TASK_PASS,0))  as IsPass From DP_TASKMGR_MAIN");

	memset(szTaskFilter,0,sizeof(szTaskFilter));


	if (pFilter)
	{
		memset(szTmp,0,sizeof(szTmp));

		//����C����GroupBy�ı�����,������Ҫ��������
		//C
		if (pFilter->CFilter && strlen(pFilter->CFilter))
		{
			sprintf(szTmp," Where WBS_C = '%s'",pFilter->CFilter);
			strcat(szSql,szTmp);						
			memset(szTmp,0,sizeof(szTmp));
		}

		//A
		if (pFilter->AFilter != DP_A_IMAGE)
			sprintf(szTaskFilter,"WBS_A = %d",pFilter->AFilter);
		

		//B
		if (pFilter->BFilter && strlen(pFilter->BFilter) > 0)
		{
			sprintf(szTmp,"WBS_B = '%s'",pFilter->BFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}

		
		//D
		if (pFilter->DFilter != 0)
		{
			sprintf(szTmp,"WBS_D = %d",pFilter->DFilter);
			if (strlen(szTaskFilter) > 0)
				strcat(szTaskFilter," AND ");
			strcat(szTaskFilter,szTmp);
			memset(szTmp,0,sizeof(szTmp));
		}
	}


	memset(szTmp,0,sizeof(szTmp));
	sprintf(szTmp," GROUP BY WORKSPACE_ID,USER_ID,WBS_A,WBS_B,WBS_D HAVING WORKSPACE_ID = '%s' AND USER_ID = '%s' AND (WBS_D - 23000) >= 0 AND (WBS_D - 23000) < 1000 ",
		    m_pWks->m_szID,m_szUserID);

	strcat(szSql,szTmp);

	//���������ѯ��������
	if (strlen(szTaskFilter) > 0)
	{
		strcat(szSql," AND ");
		strcat(szSql,szTaskFilter);
	}


	//��ȡ���ͬʱ,�������ڵ�����ȡ��	
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant		varA;
			CDpDBVariant		varB;
			CDpDBVariant		varD;
			CDpDBVariant		varStart;
			CDpDBVariant		varEnd;
			CDpDBVariant		varPercent;
			CDpDBVariant		varPass;

			if (rst.GetFieldValue(3,varA) && 
				rst.GetFieldValue(4,varB) && 
				rst.GetFieldValue(5,varD) && 
				rst.GetFieldValue(6,varStart) && 
				rst.GetFieldValue(7,varEnd) && 
				rst.GetFieldValue(8,varPercent) && 
				rst.GetFieldValue(9,varPass))
			{
				CDpTaskGroup*	pGroup = new CDpTaskGroup;

				pGroup->m_WBS_A		= DpAPartType((int)varA);
				CopyStr(&(pGroup->m_WBS_B),(LPCTSTR)varB);
				pGroup->m_WBS_D		= (int)varD;
				CopyStr(&(pGroup->m_szStartTime),(LPCTSTR)varStart);
				CopyStr(&(pGroup->m_szEndTime),(LPCTSTR)varEnd);
				pGroup->m_iPercent	= (int)varPercent;
				pGroup->m_iPass		= (int)varPass;

				//ȡ�����ڵ�������
				CDpRecordset	rstItem(m_pWks->m_pDB);
				char		szTask[512];
				memset(szTask,0,sizeof(szTask));
				sprintf(szTask,"Select TASK_ID,TASK_NAME,WBS_C,TASK_START_TIME,TASK_END_TIME,TASK_PERCENT,TASK_PASS,TASK_DESC From DP_TASKMGR_MAIN Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND WBS_A = %d AND WBS_B = '%s' AND WBS_D = %d ORDER BY TASK_ID",
					    m_pWks->m_szID,m_szUserID,pGroup->m_WBS_A,pGroup->m_WBS_B,pGroup->m_WBS_D);

				int		iCount		= 0;

				if (rstItem.Open(szTask))
				{
					while (rstItem.Next())
					{
						CDpDBVariant		varTaskID;
						CDpDBVariant		varTaskName;
						CDpDBVariant		varC;
						CDpDBVariant		varItemStart;
						CDpDBVariant		varItemEnd;
						CDpDBVariant		varItemPercent;
						CDpDBVariant		varItemPass;
						CDpDBVariant		varDesc;
						
						if (rstItem.GetFieldValue(1,varTaskID) && 
							rstItem.GetFieldValue(2,varTaskName) && 
							rstItem.GetFieldValue(3,varC) && 
							rstItem.GetFieldValue(4,varItemStart) && 
							rstItem.GetFieldValue(5,varItemEnd) && 
							rstItem.GetFieldValue(6,varItemPercent) && 
							rstItem.GetFieldValue(7,varItemPass) && 
							rstItem.GetFieldValue(8,varDesc))
						{
							iCount++;
							CDpTaskGroup::_DpTaskInfo*	pItem = new CDpTaskGroup::_DpTaskInfo;
							memset(pItem,0,sizeof(CDpTaskGroup::_DpTaskInfo));

							CopyStr(&(pItem->szTaskID),(LPCTSTR)varTaskID);
							CopyStr(&(pItem->szTaskName),(LPCTSTR)varTaskName);
							CopyStr(&(pItem->szWBS_C),(LPCTSTR)varC);
							CopyStr(&(pItem->szStartTime),(LPCTSTR)varItemStart);
							CopyStr(&(pItem->szEndTime),(LPCTSTR)varItemEnd);
							pItem->iPercent		= (int)varItemPercent;
							pItem->iPass		= (int)varItemPass;
							CopyStr(&(pItem->szDesc),(LPCTSTR)varDesc);

							//�����ǰ�ǵ�һ��������,��������ID���������͵��ڸ���,��������ƾ͵���D������
							if (iCount == 1)
							{
								pGroup->m_uGroupID = _atoi64(pItem->szTaskID);
								CopyStr(&(pGroup->m_szDesc),pItem->szDesc);
								const char*	szDTmp	= NULL;
								//һ��Ҫ���Եĵط�
								/*
								DpCPartType cType;
								if(NULL==(LPCTSTR)varC)
									cType=DP_C_NULL;
								if(0==_stricmp((LPCTSTR)varC,"IOP"))
									cType=DP_C_IOP;
								if(0==_stricmp((LPCTSTR)varC,"AOP"))
									cType=DP_C_AOP;
								if(0==_stricmp((LPCTSTR)varC,"IMAGEPOINT"))
									cType=DP_C_IMAGEPOINT;
								if(0==_stricmp((LPCTSTR)varC,"DEM"))
									cType=DP_C_DEM;
								if(0==_stricmp((LPCTSTR)varC,"DOM"))
									cType=DP_C_DOM;
								if(0==_stricmp((LPCTSTR)varC,"DLG"))
									cType=DP_C_DLG;
								if(0==_stricmp((LPCTSTR)varC,"DRG"))
									cType=DP_C_DRG;
								if(0==_stricmp((LPCTSTR)varC,"AOP"))
									cType=DP_C_AOP;
								if(0==_stricmp((LPCTSTR)varC,"DVS"))
									cType=DP_C_DVS;
									*/


								//һ��Ҫ���Եĵط�
								
								pWh->GetDDesc(pGroup->m_WBS_D,&szDTmp);
								CopyStr(&(pGroup->m_szGroupName),szDTmp);
							}

							pGroup->m_TaskArray.push_back(pItem);
						}
					}
					rstItem.Close();
				}

				if (iCount > 0)
					m_TaskSet.insert(DPTASKSET::value_type(pGroup->m_uGroupID,pGroup));
				else
				{
					delete pGroup;
					pGroup = NULL;
				}

			}

		}
		rst.Close();
	}

	delete pWh;
	pWh = NULL;

	return true;
}

//ȡ�����Ŀ
int CDpTaskSet::GetGroupCount()
{
	return m_TaskSet.size();
}


//ȡ��,iIndex ��0��ʼ
const CDpTaskGroup* CDpTaskSet::GetGroup(int iIndex)
{
	DPTASKSET::iterator			i;
	int							j	= 0;

	if (iIndex >= GetGroupCount())
		return NULL;

	for (i = m_TaskSet.begin(); i != m_TaskSet.end(); ++i)
	{
		if (j == iIndex)
			return (const CDpTaskGroup*)((*i).second);

		j++;
	}
	return NULL;
}








/*******************************************************************/
/*                    ���û������񼯵�������                       */
/*******************************************************************/
CDpTaskSetArray::CDpTaskSetArray()
{

}


CDpTaskSetArray::~CDpTaskSetArray()
{
	int		iCount = GetArraySize();

	for (int i = 0; i < iCount; i++)
	{
		CDpTaskSet*	pSet = m_TaskSetArray[i];
		if (pSet)
		{
			delete pSet;
			pSet = NULL;
		}
	}
	m_TaskSetArray.clear();
}


//ȡ��ǰ�����ڵ����񼯵���Ŀ
int CDpTaskSetArray::GetArraySize()
{
	return m_TaskSetArray.size();
}
//ȡָ������������
const CDpTaskSet* CDpTaskSetArray::GetTaskSet(int iIndex)
{
	return (const CDpTaskSet*)m_TaskSetArray[iIndex];
}

//���������������
void CDpTaskSetArray::AddTaskSet(CDpTaskSet* pTaskSet)
{
	if (!pTaskSet)
		return;
	m_TaskSetArray.push_back(pTaskSet);
}














/***************************************************************************/
/*				�������ֵ������������,ͬʱ��������������ֵ��ʱ��Ҳ��
/*              ��Ҫ����¼�µ������ֶε�ֵszFilter�ڱ�������µ�����ֵ
/***************************************************************************/
CDpUniqueFilter::CDpUniqueFilter()
{

}

CDpUniqueFilter::~CDpUniqueFilter()
{
	RemoveAllFilter();
}

//��ӹ�������
BOOL CDpUniqueFilter::AddFilter(const char* szFieldName,const char* szFilter,
								DpCustomFieldType nType)
{
	if (!szFieldName || strlen(szFieldName) <= 0)
		return true;

	_DpUniqueFilter*	nFilter		= new _DpUniqueFilter;
	memset(nFilter,0,sizeof(_DpUniqueFilter));
	
	CopyStr(&nFilter->szFieldName,szFieldName);
	if (szFilter)
		CopyStr(&nFilter->szFilter,szFilter);
	nFilter->nFilterFieldType	= nType;

	m_nFilterArray.push_back(nFilter);
	return true;
}

//���ͼ����������,nUnitIDArray-��ΪҪ������ֵ���ʵ�ͼ����ID
BOOL CDpUniqueFilter::AddUnitFilter(CDpStringArray& nUnitIDArray)
{
	int		iCount	= nUnitIDArray.GetCount();
	if (iCount <= 0)
		return false;

	char*	szFilter		= NULL;
	int		szFilterSize	= 0;


	memset(szFilter,0,sizeof(szFilter));
	strcat(szFilter,"IN (");

	int		iAddCount	= 0;

	szFilterSize = iCount * 22 + 128;
	szFilter = new char[szFilterSize];
	memset(szFilter,0,szFilterSize);

	for (int i = 0; i < iCount; i++)
	{
		char*	szTmp = nUnitIDArray.GetItem(i);
		if (!szTmp)
			continue;

		char	szID[50];
		if (iAddCount != 0)
			strcat(szFilter,",");

		sprintf(szID,"'%s'",szTmp);
		strcat(szFilter,szID);

		iAddCount++;
	}

	if (iAddCount == 0)
	{
		if (szFilter)
		{
			delete [] szFilter;
			szFilter = NULL;
		}
		return false;
	}

	strcat(szFilter,")");

	//��ӵ���������������ȥ
	_DpUniqueFilter*	nFilter		= new _DpUniqueFilter;
	memset(nFilter,0,sizeof(_DpUniqueFilter));

	CopyStr(&nFilter->szFieldName,"ATT_GRID_ID");
	CopyStr(&nFilter->szFilter,szFilter);
	m_nFilterArray.push_back(nFilter);

	if (szFilter)
	{
		delete [] szFilter;
		szFilter = NULL;
	}

	return true;
}



//ɾ����������-��������
BOOL CDpUniqueFilter::RemoveFilter(int iIndex)
{
	int iCount = m_nFilterArray.size();
	if (iIndex >= iCount)
		return false;

	_DpUniqueFilter*	nFilter	= m_nFilterArray[iIndex];
	if (nFilter)
	{
		if (nFilter->szFieldName)
		{
			delete [] nFilter->szFieldName;
			nFilter->szFieldName = NULL;
		}
		if (nFilter->szFilter)
		{
			delete [] nFilter->szFilter;
			nFilter->szFilter = NULL;
		}
		delete nFilter;
		nFilter = NULL;
	}
	
	m_nFilterArray.erase(m_nFilterArray.begin()+iIndex);
	return true;
}



//ɾ����������-�����ֶ���
BOOL CDpUniqueFilter::RemoveFilter(const char* szFieldName)
{
	if (!szFieldName || strlen(szFieldName) <= 0)
		return false;

	int iCount = m_nFilterArray.size();

	for (int i = 0; i < iCount; i++)
	{
		_DpUniqueFilter* nFilter = m_nFilterArray[i];
		if (nFilter && nFilter->szFieldName && 
			_stricmp(szFieldName,nFilter->szFieldName) == 0)
			return RemoveFilter(i);
	}
	
	return false;
}



//ɾ�����еĹ�������
BOOL CDpUniqueFilter::RemoveAllFilter()
{
	int iCount = m_nFilterArray.size();

	for (int i = 0; i < iCount; i++)
	{
		_DpUniqueFilter*	nFilter	= m_nFilterArray[i];		
		if (nFilter)
		{
			if (nFilter->szFieldName)	
			{
				delete [] nFilter->szFieldName;
				nFilter->szFieldName = NULL;
			}
			if (nFilter->szFilter)			
			{
				delete [] nFilter->szFilter;
				nFilter->szFilter = NULL;
			}
			delete nFilter;
			nFilter = NULL;
		}
	}

	m_nFilterArray.clear();
	return true;
}



//�õ���������-��������
BOOL CDpUniqueFilter::GetFilter(int iIndex,const _DpUniqueFilter** ppcFilter)
{
	if (!ppcFilter)
		return false;

	*ppcFilter	= NULL;
	int iCount	= m_nFilterArray.size();
	
	if (iIndex >= iCount)
		return false;

	*ppcFilter	= m_nFilterArray[iIndex];
	return true;
}



//�õ���������--��������
BOOL CDpUniqueFilter::GetFilter(const char* szFieldName,const _DpUniqueFilter** ppcFilter)
{
	if (!szFieldName || strlen(szFieldName) <= 0 || !ppcFilter)
		return false;

	*ppcFilter	= NULL;
	int iCount	= m_nFilterArray.size();
	
	for (int i = 0; i < iCount; i++)
	{
		_DpUniqueFilter* nFilter = m_nFilterArray[i];
		if (nFilter && nFilter->szFieldName && 
			_stricmp(szFieldName,nFilter->szFieldName) == 0)
		{
			*ppcFilter = nFilter;
			return true;
		}	
	}
	return false;
}


//�������еĹ��������õ����Ϲ���������ͼ��
//����˵��:  [out]szLayerFilterSql  -- ���ڹ���ͼ���SQL����е�WHERE���,������WHERE,���ݹ����������г����ֶδ�
//                                      �����ֵ���в��Ҿ�����Щ�ֶε�ͼ�����Ա��ͼ��ID
//           [in] iBuffCount        --  szLayerFilterSql�Ļ���������
BOOL CDpUniqueFilter::MakeLayerFilterWhereClause(char* szLayerFilterSql,int iBuffCount)
{
	if (!szLayerFilterSql || iBuffCount <= 0) 
		return false;

	memset(szLayerFilterSql,0,sizeof(szLayerFilterSql));

	int		iCount		= m_nFilterArray.size();
	int		i			= 0;
	int		j			= 0;
	char	szTmp[2048]	= "\0";

	//���ɹ���ͼ���SQL
	memset(szTmp,0,sizeof(szTmp));
	sprintf(szTmp," FIELD_NAME in ('");

	for (i = 0; i < iCount; i++)
	{
		_DpUniqueFilter* nFilter = m_nFilterArray[i];
		if (nFilter && nFilter->szFieldName && strlen(nFilter->szFieldName) > 0)
		{
			if (j != 0)
				strcat(szTmp,",'");

			strcat(szTmp,nFilter->szFieldName);
			strcat(szTmp,"'");
			j++;
		}
	}
	
	if (j > 0)
	{
		strcat(szTmp,") ");
		if (iBuffCount > strlen(szTmp)+1)
		{
			strcpy(szLayerFilterSql,szTmp);
			return true;
		}
	}

	return false;

}

//�õ����˶���ֵ��¼��WHERE�Ӿ�,������WHERE 
BOOL CDpUniqueFilter::MakeUniqueFilterWhereClause(char* szRecordFilterWhereClause,int iBuffCount)
{

	if (!szRecordFilterWhereClause || iBuffCount <= 0)
		return false;

	memset(szRecordFilterWhereClause,0,sizeof(szRecordFilterWhereClause));

	int		iCount			= m_nFilterArray.size();
	int		i				= 0;
	char	szTmp[2048]		= "\0";
	memset(szTmp,0,sizeof(szTmp));
	
	if (iCount <= 0)
		return false;

	int		iWhereCount		= 0;
	//���ɹ���ͼ���SQL
	for (i = 0; i < iCount; i++)
	{
		_DpUniqueFilter* nFilter = m_nFilterArray[i];
		if (nFilter && nFilter->szFieldName && strlen(nFilter->szFieldName) > 0 ||
			nFilter->szFilter && strlen(nFilter->szFilter) > 0)
		{
			//���������������ַ���
			char	szAnalyseResult[256];
			memset(szAnalyseResult,0,sizeof(szAnalyseResult));
			if (!AnalyseFilterString(nFilter->szFilter,szAnalyseResult,sizeof(szAnalyseResult),nFilter->nFilterFieldType))
				continue;

			//�ϳɸù���������SQL�ִ�
			char	szSingleFilter[256];
			memset(szSingleFilter,0,sizeof(szSingleFilter));
			if (iWhereCount != 0)
				sprintf(szSingleFilter," AND %s %s ",nFilter->szFieldName,szAnalyseResult);
			else
				sprintf(szSingleFilter," %s %s ",nFilter->szFieldName,szAnalyseResult);

			strcat(szTmp,szSingleFilter);

			iWhereCount++;

		}
	}

	if (strlen(szTmp) > (iBuffCount - 1))
		return false;

	strcpy(szRecordFilterWhereClause,szTmp);

	return true;


}


//�������Ա����͹����������ɵõ������Ա���Ϲ��������Ķ���ֵ��¼����SQL���
BOOL CDpUniqueFilter::MakeGetUniqueValueCountSql(const char* szLayerID,char* szGetCountSql,
												 int iBuffCount)
{
	if (!szLayerID || strlen(szLayerID) <= 0 || 
		!szGetCountSql || iBuffCount <= 0)
		return false;

	char	szSelClause[2048];
	memset(szSelClause,0,sizeof(szSelClause));
	if (!MakeGetUniqueValueRecordSql(szLayerID,szSelClause,sizeof(szSelClause)) || 
		strlen(szSelClause) <= 0)
		return false;
		
	char	szSql[4096];
	memset(szSql,0,sizeof(szSql));

	sprintf(szSql,"Select Count(*) From (%s) ",szSelClause);

	if (strlen(szSql) > iBuffCount - 1)
		return false;
	strcpy(szGetCountSql,szSql);
	return true;
}






//�������Ա����͹����������ɵõ������Ա���Ϲ��������Ķ���ֵ��¼��SQL���
BOOL CDpUniqueFilter::MakeGetUniqueValueRecordSql(const char* szLayerID,char* szGetRecordSql,
												  int iBuffCount)
{
	if (!szLayerID || strlen(szLayerID) <= 0 || 
		!szGetRecordSql || iBuffCount <= 0)
		return false;

	memset(szGetRecordSql,0,sizeof(szGetRecordSql));

	int		iCount			= m_nFilterArray.size();
	int		i				= 0;
	char	szTmp[2048]		= "\0";
	char	szField[2048]	= "\0";
	memset(szField,0,sizeof(szField));

	memset(szTmp,0,sizeof(szTmp));
	
	if (iCount <= 0)
		return false;

	for (i = 0; i < iCount; i++)
	{
		_DpUniqueFilter* nFilter = m_nFilterArray[i];
		if (!nFilter || !(nFilter->szFieldName) || strlen(nFilter->szFieldName) <= 0)
			continue;
		
		if (i != 0)
			strcat(szField,",");
		strcat(szField,nFilter->szFieldName);
	}

	if (strlen(szField) <= 0)
		return false;

	strcat(szField," ");

	char	szWhereClause[2048];
	memset(szWhereClause,0,sizeof(szWhereClause));
	if (!MakeUniqueFilterWhereClause(szWhereClause,sizeof(szWhereClause)))
		memset(szWhereClause,0,sizeof(szWhereClause));

	//�����ɶ���ֵSQL��ѯ��ͬʱ����Ȩ�޵��ж�,�޸ı��:NO.050220
	char	szRet[4096];
	memset(szRet,0,sizeof(szRet));
	if (strlen(szWhereClause) > 0)
//		sprintf(szRet,"Select Distinct %s From A%s Where %s",szField,szLayerID,szWhereClause);
		//��������¼�����ֶ�
		sprintf(szRet,"Select Distinct %s,Count(*) AS RECCOUNT From A%s GROUP BY %s HAVING %s",szField,szLayerID,szField,szWhereClause);
	else
//		sprintf(szRet,"Select Distinct %s From A%s",szField,szLayerID);
		sprintf(szRet,"Select Distinct %s,Count(*) AS RECCOUNT From A%s GROUP BY %s",szField,szLayerID,szField);


	if (strlen(szRet) > (iBuffCount - 1))
		return false;

	strcpy(szGetRecordSql,szRet);

	return true;
}

//���������������ִ�. [in]szFilterStr--���������ִ� [out]szResult--�������
//                    [in]iBuffer    --szResult�Ļ�������С
BOOL CDpUniqueFilter::AnalyseFilterString(const char* szFilterStr,char* szResult,
										  int iBuffer,DpCustomFieldType nType)
{
	if (!szFilterStr || strlen(szFilterStr) <= 0 || !szResult || iBuffer <= 0)
		return false;

	char	szRet[1024];
	memset(szRet,0,sizeof(szRet));

	//��ȥ����ߵĿո�
	char*	szTmp = LeftTrim(szFilterStr);
	if (!szTmp || strlen(szTmp) <= 0)
		return false;
	
	//��ʾû��д�ȽϷ���,����ֱ�ӵ��� 
	if (*szTmp != '=' && *szTmp != '<' && *szTmp != '>' && 
		*szTmp != '!' && *szTmp != 'I' && *szTmp != 'i')
	{
		if (nType == DP_CFT_VARCHAR)
			sprintf(szRet," = '%s' ",szTmp);
		else
			sprintf(szRet," = %s ",szTmp);


		if (strlen(szRet) > iBuffer - 1)
			return false;
		strcpy(szResult,szRet);
		return true;
	}
	else if ((*szTmp == 'I' || *szTmp == 'i') && 
		     (*(szTmp+1) == 'N' || *(szTmp+1) == 'n'))			//IN
	{
		sprintf(szRet,"%s",szTmp);
		if (strlen(szRet) > iBuffer - 1)
			return false;
		strcpy(szResult,szRet);
		return true;
	}
	else			//�бȽϷ���
	{
		if (strlen(szTmp) <= 1)
			return false;

		if (*(szTmp+1) == '=')
		{
			char	szContext[512];
				
			if (nType == DP_CFT_VARCHAR)
				sprintf(szContext,"'%s'",LeftTrim(szTmp+2));
			else
				sprintf(szContext,"%s",LeftTrim(szTmp+2));

			if (!szContext || strlen(szContext) <= 0)
				return false;

			switch (*szTmp)
			{
				case '>':
					sprintf(szRet," >= %s ",szContext);
					break;
				case '<':
					sprintf(szRet," <= %s ",szContext);
					break;
				case '!':
					sprintf(szRet," <> %s ",szContext);
					break;
				case '=':
					sprintf(szRet," = %s ",szContext);
					break;
				default:
					return false;
			}

			if (strlen(szRet) > iBuffer - 1)
				return false;

			strcpy(szResult,szRet);
			return true;
		}
		else
		{
			char	szContext[512];
				
			if (nType == DP_CFT_VARCHAR)
				sprintf(szContext,"'%s'",LeftTrim(szTmp+1));
			else
				sprintf(szContext,"%s",LeftTrim(szTmp+1));

			if (!szContext || strlen(szContext) <= 0)
				return false;

			switch (*szTmp)
			{
				case '>':
					sprintf(szRet," > %s ",szContext);
					break;
				case '<':
					sprintf(szRet," < %s ",szContext);
					break;
				case '=':
					sprintf(szRet," = %s ",szContext);
					break;
				default:
					return false;
			}

			if (strlen(szRet) > iBuffer-1)
				return false;
			
			strcpy(szResult,szRet);
			return true;
		}


	}
	
	return false;
}



/****************************************************************************
/*       ��ѯ��ǰ���������������Ա����ж��ٲ�ͬ�������ֶεĽṹ
/****************************************************************************/

//���һ�������ֶε��������
BOOL CDpUniqueFieldResult::AddField(CDpCustomFieldInfo* pInfo)
{
	if (!pInfo)
		return false;
	m_nResultArray.push_back(pInfo);
	return true;
}


//ɾ������������е��ֶβ��ͷ����ڴ�
BOOL CDpUniqueFieldResult::RemoveAllField()
{
	int iCount = m_nResultArray.size();
	for (int i = 0; i < iCount; i++)
	{
		CDpCustomFieldInfo* pInfo = m_nResultArray[i];
		if (pInfo)
		{
			delete pInfo;
			pInfo = NULL;
		}
	}
	m_nResultArray.clear();
	return true;
}

//�õ��������ָ�����ֶε�����
BOOL CDpUniqueFieldResult::GetField(int iIndex,const CDpCustomFieldInfo** ppInfo)
{
	if (!ppInfo)
		return false;
	*ppInfo = NULL;

	int iCount = m_nResultArray.size();
	if (iIndex < 0 || iIndex >= iCount)
		return false;

	*ppInfo = m_nResultArray[iIndex];
	return true;
}




/****************************************************************************
/   ����ֵ��ѯ������ͼ��Ľ������,������ͼ���з��������Ķ���ֵ��ϵļ�¼ 
/****************************************************************************/

//�����������ͼ��Ľ��
BOOL CDpUniqueQueryLayerResult::AddLayerResult(const char* szLayerID,const char* szLayerName,
											   int iCount)
{
	if (!szLayerID || strlen(szLayerID) <= 0)
		return false;

	_DpUniqueQueryLayersResult*	duqlr = new _DpUniqueQueryLayersResult;
	memset(duqlr,0,sizeof(_DpUniqueQueryLayersResult));

	CopyStr(&(duqlr->szLayerID),szLayerID);
	CopyStr(&(duqlr->szLayerName),szLayerName);
	duqlr->iCount	= iCount;

	m_nResultArray.push_back(duqlr);

	return true;
}


//ɾ�����еĽ�������ͷ��ڴ�
BOOL CDpUniqueQueryLayerResult::RemoveAllResult()
{
	int	iCount = m_nResultArray.size();

	for (int i = 0; i < iCount; i++)
	{
		_DpUniqueQueryLayersResult*	duqlr = m_nResultArray[i];
		if (duqlr)
		{
			if (duqlr->szLayerID)
			{
				delete [] duqlr->szLayerID;
				duqlr->szLayerID = NULL;
			}
			if (duqlr->szLayerName)
			{
				delete [] duqlr->szLayerName;
				duqlr->szLayerName = NULL;
			}
			delete duqlr;
			duqlr = NULL;
		}
	}

	m_nResultArray.clear();
	return true;
}


//ȡ��ָ����ͼ����
BOOL CDpUniqueQueryLayerResult::GetResult(int iIndex,const _DpUniqueQueryLayersResult** ppcResult)
{
	if (!ppcResult)
		return false;
	*ppcResult = NULL;

	int	iCount = m_nResultArray.size();
	if (iIndex < 0 || iIndex >= iCount)
		return false;

	*ppcResult = m_nResultArray[iIndex];
	return true;

}



/****************************************************************************
/           ����ֵ��ѯ��ĳ������ͼ��Ķ���ֵ����ϵļ�¼�Ľṹ��
/****************************************************************************/
//���¼�������Ӽ�¼
BOOL CDpUniqueQueryValueResult::AddResult(COLUMN_VALUE_ARRAY* pRst,int iRecCount)
{
	if (!pRst || iRecCount<= 0)
		return false;

	_DpUniqueQueryValueResult*	result	= new _DpUniqueQueryValueResult;
	result->pColumnArray	= pRst;
	result->iRecCount		= iRecCount;

	m_nRecordArray.push_back(result);
	return true;
}


//ɾ����¼�������еļ�¼
BOOL CDpUniqueQueryValueResult::RemoveAllResult()
{
	int iCount = m_nRecordArray.size();

	for (int i = 0; i < iCount; i++)
	{
		_DpUniqueQueryValueResult*	pRow = m_nRecordArray[i];
		if (pRow)
		{
			int iColCount = pRow->pColumnArray->size();
			COLUMN_VALUE_ARRAY::iterator	j;
			for (j = pRow->pColumnArray->begin(); j != pRow->pColumnArray->end(); ++j)
			{
				CDpDBVariant*	pVar = (*j);
				if (pVar)
				{
					delete pVar;
					pVar = NULL;
				}
			}
			pRow->pColumnArray->clear();

			delete pRow->pColumnArray;
			pRow->pColumnArray = NULL;

			delete pRow;
			pRow = NULL;
		}

	}
	m_nRecordArray.clear();

	return true;
}

//�õ�ָ���ļ�¼
BOOL CDpUniqueQueryValueResult::GetResult(int iIndex,const _DpUniqueQueryValueResult** ppRst)
{
	if (!ppRst)
		return false;
	*ppRst = NULL;

	int iCount = m_nRecordArray.size();
	if (iIndex < 0 || iIndex >= iCount)
		return false;

	*ppRst = m_nRecordArray[iIndex];
	return true;
}



/********************************************************************/
/*                            ����ֵ������                     
/*   ���ڸ���N�������ֶ���ȥ���Ҿ�����N���������Ĳ�,Ҳ���Ը�����N��
/*   �����ֶεĹ�������ȥ�������в��з�����N�������Ĳ��Լ�����ļ�¼
/********************************************************************/
CDpUniqueQuery::CDpUniqueQuery(CDpWorkspace* pWks)
{
	m_pWks	= pWks;
}

CDpUniqueQuery::~CDpUniqueQuery()
{

}


//�õ���ǰ���ݿ������в�����������ֶ�,���а���ά���������ֶκͲ㶨��������ֶ�
BOOL CDpUniqueQuery::GetAllAttributeFields(CDpUniqueFieldResult& nFieldsInfo)
{
	CDpRecordset	rst(m_pWks->m_pDB);
	char			szSql[2048];
	memset(szSql,0,sizeof(szSql));

	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select DISTINCT FIELD_NAME,FIELD_ALIAS,FIELD_TYPE,FIELD_LENGTH,FIELD_PRECISION,FIELD_NOTNULLABLE,FIELD_ISUNIQUE,PARENT_CLASS FROM DP_Attr_Dictionary WHERE MID(ATTR_TABLE_NAME,2,14) = '%s'",// ORDER BY FIELD_NAME,FIELD_ALIAS",
		        m_pWks->m_szID);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select DISTINCT FIELD_NAME,FIELD_ALIAS,FIELD_TYPE,FIELD_LENGTH,FIELD_PRECISION,FIELD_NOTNULLABLE,FIELD_ISUNIQUE,PARENT_CLASS FROM DP_Attr_Dictionary WHERE SUBSTRING(ATTR_TABLE_NAME,2,14) = '%s'",// ORDER BY FIELD_NAME,FIELD_ALIAS",
		        m_pWks->m_szID);
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select DISTINCT FIELD_NAME,FIELD_ALIAS,FIELD_TYPE,FIELD_LENGTH,FIELD_PRECISION,FIELD_NOTNULLABLE,FIELD_ISUNIQUE,PARENT_CLASS FROM DP_Attr_Dictionary WHERE SUBSTR(ATTR_TABLE_NAME,2,14) = '%s'",// ORDER BY FIELD_NAME,FIELD_ALIAS",
		        m_pWks->m_szID);

	if (!rst.Open(szSql))
		return false;

	while (rst.Next())
	{
		CDpDBVariant	var;
		if (!rst.GetFieldValue((UINT)1,var))
			continue;
		
		CDpCustomFieldInfo*	pInfo = new CDpCustomFieldInfo;
		memset(pInfo,0,sizeof(CDpCustomFieldInfo));
		//�ֶ���
		strcpy(pInfo->m_strName,(LPCTSTR)var);
		//�ֶα���
		rst.GetFieldValue((UINT)2,var);
		strcpy(pInfo->m_szAliasName,(LPCTSTR)var);
		//�ֶ�����
		rst.GetFieldValue((UINT)3,var);
		pInfo->m_CustomFieldType = DpCustomFieldType((int)var);
		//�ֶγ���
		rst.GetFieldValue((UINT)4,var);
		pInfo->m_iSize			= (int)var;
		//�ֶξ���
		rst.GetFieldValue((UINT)5,var);
		pInfo->m_nPrecision		= (int)var;
		//�ֶ��Ƿ�����Ϊ��,1��ʾ������Ϊ��,0��ʾ����Ϊ��
		rst.GetFieldValue((UINT)6,var);
		pInfo->m_bAllowNull	= (((int)var) == 0); 
		//�ֶ��Ƿ������ظ�
		rst.GetFieldValue((UINT)7,var);
		pInfo->m_bIsUnique		= (((int)var) == 1);
		//�������ID
		rst.GetFieldValue((UINT)8,var);
		pInfo->m_iParent		= ((int)var);

		nFieldsInfo.AddField(pInfo);

	}

	rst.Close();

	return true;
}



//���������ֶ������б���Ҿ�����������ֶεĲ�
//����˵��: [in]  nFilter			--  Ҫ���ҵ��ֶεĹ�������
//          [out] nResultArray  	--  ���ҵĽ��
BOOL CDpUniqueQuery::FindLayers(CDpUniqueFilter* pFilter,
								CDpUniqueQueryLayerResult& nResultArray,
								BOOL bIsGetCountOfLayer)
{
	if (!pFilter)
		return false;

	char		szWhereClause[2048];
	memset(szWhereClause,0,sizeof(szWhereClause));

	if (!pFilter->MakeLayerFilterWhereClause(szWhereClause,sizeof(szWhereClause)))
		return false;

	char		szSql[4096];
	memset(szSql,0,sizeof(szSql));
	int		iFieldCount	= pFilter->GetFilterCount();

	sprintf(szSql,"Select LAYER_ID,LAYER_NAME From DP_LAYERSMGR where WORKSPACE_ID = '%s' AND ATTR_TABLE_NAME in (Select ATTR_TABLE_NAME FROM DP_Attr_Dictionary Where %s Group by ATTR_TABLE_NAME Having Count(ATTR_TABLE_NAME) = %d)",
		    m_pWks->m_szID,szWhereClause,iFieldCount);

	CDpRecordset	rstLayer(m_pWks->m_pDB);
	if (!rstLayer.Open(szSql))
		return false;

	while (rstLayer.Next())
	{
		CDpDBVariant	varID;
		CDpDBVariant	varName;
		int				iCount		= 0;
		if (!rstLayer.GetFieldValue((UINT)1,varID) || 
			!rstLayer.GetFieldValue((UINT)2,varName))
			continue;

		const char*			szLayerID	= (LPCTSTR)varID;

		//�ڴ˴�Ӧ�����Ƿ��ܲ�ѯ��ͼ���Ȩ�޿���,�޸ı��:NO.050220
		if (!m_pWks->m_pDB->m_pRightMgr->CanAccessLayer((char*)szLayerID))
			continue;

		//�õ�ͼ���о���ͼ��Ķ���ֵ����
		if (bIsGetCountOfLayer)
		{
			//�ڲ�ѯÿ��ͼ���еĶ���ֵǰ����nFilter�м�����ƶ�Ȩ�޵Ĺ�������,
			//��ͼ���ѯ����ٽ���ӵĹ�������ȥ��
			int		iFilterCount	= pFilter->GetFilterCount();
			if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
			{
				char	szRightFilter[1024];
				sprintf(szRightFilter," IN (Select GRID_ID From DP_V_USERRIGHT_UNIT_LAYER Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND LAYER_ID = '%s' AND MRIGHT > 0)",
						szLayerID,m_pWks->m_szID,m_pWks->m_pDB->m_szUserID,szLayerID);
				pFilter->AddFilter("ATT_GRID_ID",szRightFilter,DP_CFT_INTEGER);
			}

			//�ٲ�ѯ����ͼ�������з��������Ķ���ֵ����ϵ�������
			CDpRecordset	rstUniqueCount(m_pWks->m_pDB);
			char			szGetCountSql[2048];
			memset(szGetCountSql,0,sizeof(szGetCountSql));
			if (pFilter->MakeGetUniqueValueCountSql(szLayerID,szGetCountSql,sizeof(szGetCountSql)))
			{
				if (rstUniqueCount.Open(szGetCountSql))
				{
					if (rstUniqueCount.Next())
					{
						CDpDBVariant	varCount;
						if (rstUniqueCount.GetFieldValue((UINT)1,varCount))
						{
							iCount = (int)varCount;
						}

					}
					rstUniqueCount.Close();
				}
			}

			//ɾ����������
			if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
				pFilter->RemoveFilter(iFilterCount);	

			if (iCount < 0)
				iCount = 0;
		}

		//ͼ��ID
		if (!nResultArray.AddLayerResult((LPCTSTR)varID,(LPCTSTR)varName,iCount))
			continue;

	}

	rstLayer.Close();


	return true;
}


//�򿪾����ĳ���Ѿ��������˵�ͼ���е����ж���ֵ����ϵĽ���ļ�¼
//����˵��: [in]  nFilterArray		-- Ҫ���ҵ��ֶεĹ�������
//          [in]  szLayerID			-- ���ڵ�ͼ���ID
//			[out] nRecordset		-- ���ؾ���ļ�¼��
BOOL CDpUniqueQuery::OpenUniqueRecordInLayer(CDpUniqueFilter* nFilter,const char* szLayerID,
											 CDpUniqueQueryValueResult& nRecordset)
{
	if (!nFilter || !szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return false;	

	char	szSql[2048];
	memset(szSql,0,sizeof(szSql));


	//�ڲ�ѯ��ͼ���еĶ���ֵǰ����nFilter�м�����ƶ�Ȩ�޵Ĺ�������,
	//��ͼ���ѯ����ٽ���ӵĹ�������ȥ��
	int		iFilterCount	= nFilter->GetFilterCount();
	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
	{
		char	szRightFilter[1024];
		sprintf(szRightFilter," IN (Select GRID_ID From DP_V_USERRIGHT_UNIT_LAYER Where WORKSPACE_ID = '%s' AND USER_ID = '%s' AND LAYER_ID = '%s' AND MRIGHT > 0)",
				szLayerID,m_pWks->m_szID,m_pWks->m_pDB->m_szUserID,szLayerID);
		nFilter->AddFilter("ATT_GRID_ID",szRightFilter,DP_CFT_INTEGER);
	}

	BOOL bRet = nFilter->MakeGetUniqueValueRecordSql(szLayerID,szSql,sizeof(szSql));

	//ɾ����������
	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		nFilter->RemoveFilter(iFilterCount);	

	if (!bRet)
		return false;

	CDpRecordset	rst(m_pWks->m_pDB);
	if (!rst.Open(szSql))
		return false;

	int iFieldCount	= rst.GetFieldCount();

	if (iFieldCount <= 0)
		return false;

	while (rst.Next())
	{
		int					iRecCount	= 0;
		COLUMN_VALUE_ARRAY* pArray		= new COLUMN_VALUE_ARRAY;
		for (UINT i = 1; i <= iFieldCount; i++)
		{
			if (i != (iFieldCount))
			{
				CDpDBVariant*	pVar = new CDpDBVariant;
				rst.GetFieldValue(i,*pVar);
				pArray->push_back(pVar);
			}
			else					//ȡ��¼��
			{
				CDpDBVariant	var;
				rst.GetFieldValue(i,var);
				iRecCount	= (int)var;
			}
			
		}

		nRecordset.AddResult(pArray,iRecCount);
	}

	rst.Close();

	return true;
}







/***********************************************************/
/*     ���ڹ���Ҫ����CHECKOUT��CHECKIN�ľ�������ݵ��б�   */
/***********************************************************/
CDpCheckInfo::CDpCheckInfo(DpCheckType nCheckType)
{
	m_nCheckType	= nCheckType;
}


CDpCheckInfo::~CDpCheckInfo()
{
	RemoveAllItem();
}

void CDpCheckInfo::RemoveAllItem()
{
	int		iCount	= m_CheckArray.size();
	for (int i = 0; i < iCount; i++)
	{
		_DpCheckOpreaInfo*	pInfo = m_CheckArray[i];
		if (pInfo)
		{
			if (pInfo->szUnitID)
			{
				delete [] pInfo->szUnitID;
				pInfo->szUnitID = NULL;
			}
			if (pInfo->szLayerID)	
			{
				delete [] pInfo->szLayerID;
				pInfo->szLayerID = NULL;
			}
			if (pInfo->szErrorMsg)
			{
				delete [] pInfo->szErrorMsg;
				pInfo->szErrorMsg = NULL;
			}
			delete pInfo;
			pInfo = NULL;
		}
	}
	m_CheckArray.clear();

	m_DistinctLayer.clear();
	m_DistinctImage.clear();
	m_DistinctUnit.clear();
}

//�õ��ܹ�Ҫ������ITEM�ĸ���
int CDpCheckInfo::GetItemCount()
{
	return m_CheckArray.size();
}


//�õ�ָ����ŵ�ITEM
BOOL CDpCheckInfo::GetItem(int iIndex,const CDpCheckInfo::_DpCheckOpreaInfo** ppInfo)
{
	int		iCount = m_CheckArray.size();
	if (iIndex >= iCount)
		return false;
	*ppInfo = m_CheckArray[iIndex];
	return true;
}


//���Ҫ������XML��ǩ
void CDpCheckInfo::AddImageXmlItem(const char* szImageID,const char* szXmlTagName,DP_LOCK_TYPE nLockType)
{
	if (!szImageID || strlen(szImageID) != IMAGE_ID_LEN)
		return;
	_DpCheckOpreaInfo*		pInfo = new _DpCheckOpreaInfo;
	memset(pInfo,0,sizeof(_DpCheckOpreaInfo));
	CopyStr(&(pInfo->szUnitID),szImageID);
	if (szXmlTagName)
		CopyStr(&(pInfo->szLayerID),szXmlTagName);
	pInfo->nLockType	= nLockType;
	pInfo->iDataType	= 2;
	m_CheckArray.push_back(pInfo);

	ULONGLONG	uID = _atoi64(szImageID);

	DISTINCTLAYER::iterator	i = m_DistinctImage.find(uID);

	if (i == m_DistinctImage.end())				//���û���ҵ�
		m_DistinctImage.insert(DISTINCTIMAGE::value_type(uID,0));

}

//���Ҫ������ͼ����ͼ��
//AddUnitLayer(const char* szUnitID,const char* szLayerID,DP_LOCK_TYPE nLockType = DP_UT_UNLOCK)
//void CDpCheckInfo::AddUnitLayer(const char* szUnitID,const char* szLayerID,DP_LOCK_TYPE nLockType)
void CDpCheckInfo:: AddUnitLayer(const char* szUnitID,const char* szLayerID,DP_LOCK_TYPE nLockType)
{
 
	if (!szUnitID || !szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return;

	_DpCheckOpreaInfo*		pInfo = new _DpCheckOpreaInfo;
	memset(pInfo,0,sizeof(_DpCheckOpreaInfo));
	CopyStr(&(pInfo->szUnitID),szUnitID);
	CopyStr(&(pInfo->szLayerID),szLayerID);
	pInfo->nLockType	= nLockType;
	pInfo->iDataType	= 1;	

	m_CheckArray.push_back(pInfo);

	ULONGLONG	uID = _atoi64(szLayerID);

	DISTINCTLAYER::iterator	i = m_DistinctLayer.find(uID);

	if (i == m_DistinctLayer.end())				//���û���ҵ�
		m_DistinctLayer.insert(DISTINCTLAYER::value_type(uID,0));


	//�ڼ���ͼ��ID��MAP��
	uID = _atoi64(szUnitID);
	DISTINCTUNIT::iterator j = m_DistinctUnit.find(uID);
	if (j == m_DistinctUnit.end())
		m_DistinctUnit.insert(DISTINCTUNIT::value_type(uID,0));

}

//����ǰͼ���е�����ͼ�㶼���뵽Ҫ��������������
//bDlgOnly -- �Ƿ�ֻ����DLGͼ��,TRUE��ʾֻ����DLGͼ��,FALSE��ʾ�������е�ͼ��(DOM,DEM��)
BOOL CDpCheckInfo::AddUnit(CDpWorkspace* pWks,const char* szUnitID,
						   DP_LOCK_TYPE nLockType,BOOL bDlgOnly)
{
	if (!pWks || !szUnitID || strlen(szUnitID) != UNIT_ID_LEN)
		return false;

	CDpStringArray		idArray;
	CDpStringArray		nameArray;


	BOOL				bRet				= false;

	if (bDlgOnly)
		bRet = pWks->GetLayersIDAndName(idArray,nameArray,GEO_NULL,LAYER_TYPE_EX_DLG);
	else
		bRet = pWks->GetLayersIDAndName(idArray,nameArray);

	if (bRet)
	{
		int		iCount = idArray.GetCount();

		for (int i = 0; i < iCount; i++)
			AddUnitLayer(szUnitID,idArray.GetItem(i),nLockType);
	}
	else
		return false;

	return true;
}

//�õ���ǰ���е�Ҫ�����Ĳ�ͬ��ͼ��ID
void CDpCheckInfo::GetAllDistinctLayer(CDpStringArray& idArray)
{
	DISTINCTLAYER::iterator	i;
	for (i = m_DistinctLayer.begin(); i != m_DistinctLayer.end(); ++i)
	{
		ULONGLONG	uID = (*i).first;
		char		szTmp[30];
		sprintf(szTmp,"%019I64d",uID);
		idArray.AddString(szTmp);
	}
}

//�õ���ǰ���е�Ҫ�����Ĳ�ͬ��Ӱ��ID
void CDpCheckInfo::GetAllDistinctImage(CDpStringArray& idArray)
{
	DISTINCTIMAGE::iterator	i;
	for (i = m_DistinctImage.begin(); i != m_DistinctImage.end(); ++i)
	{
		ULONGLONG	uID = (*i).first;
		char		szTmp[30];
		sprintf(szTmp,"%019I64d",uID);
		idArray.AddString(szTmp);
	}
}

//�õ���ǰ���е�Ҫ�����Ĳ�ͬ��ͼ��ID
void CDpCheckInfo::GetAllDistinctUnit(CDpStringArray& idArray)
{
	DISTINCTUNIT ::iterator	i;
	for (i = m_DistinctUnit.begin(); i != m_DistinctUnit.end(); ++i)
	{
		ULONGLONG	uID = (*i).first;
		char		szTmp[30];
		sprintf(szTmp,"%019I64d",uID);
		idArray.AddString(szTmp);
	}
}


/*************************************************************************/
/*                          CHECK_OUT������                       
/* ���ڹ����������е�����CHECK_OUT���������ݿ��ļ�                     
/*************************************************************************/
//����˵��:pWks					-- ��Ҫִ��CheckOut�����Ĺ�����
//         szTemplateFileName	-- ģ�����ݿ������,����ȫ·��
CDpCheckOutMgr::CDpCheckOutMgr(CDpWorkspace* pWks,const char* szTemplateFileName)
{
	m_pWks		= pWks;
	memset(m_szTemplateFileName,0,sizeof(m_szTemplateFileName));
	if (szTemplateFileName && strlen(szTemplateFileName) > 0)
		strcpy(m_szTemplateFileName,szTemplateFileName);
	else
		strcpy(m_szTemplateFileName,"Template.gdb");

}


CDpCheckOutMgr::~CDpCheckOutMgr()
{

}



//�жϵ�ǰ�������ݿ��ļ��Ƿ����
BOOL CDpCheckOutMgr::DBFileIsExist(const char* szFileName)
{
	if (!szFileName || strlen(szFileName) <= 0)
		return false;

	_OFSTRUCT	of;
	memset(&of,0,sizeof(OFSTRUCT));

	HFILE hFile = OpenFile(szFileName,&of,OF_EXIST);

	if (hFile == 1)
		return true;
	else
		return false;

}



//���ɿյı������ݿ��ļ�
//����˵��: szFileName			-- Ҫ�������ļ�
BOOL CDpCheckOutMgr::MakeEmptyDBFile(const char* szFileName)
{
	if (!szFileName || strlen(szFileName) <= 0)
		return true;

	char	szTempFileName[_MAX_PATH];
	memset(szTempFileName,0,sizeof(szTempFileName));

	if (strlen(m_szTemplateFileName) > 1)
		strcpy(szTempFileName,m_szTemplateFileName);
	else								
		strcpy(szTempFileName,"Template.gdb");			//�õ�ǰĿ¼�е�ģ���ļ�

	//�ж�ģ���ļ��Ƿ����
	if (!DBFileIsExist(szTempFileName))
		return false;

	return CopyFile(szTempFileName,szFileName,FALSE);	//����ļ������򸲸�
}



//��Ŀ�����ݿ��ڸ��Ƶ�ǰ�û���Ȩ������
//��Ҫ��Ҫ���Ʊ�DP_RoleRight,DP_UserMgr,DP_UserRight
BOOL CDpCheckOutMgr::CopyUserInfoToDest(SQLHANDLE hConnDest)
{
	if (!hConnDest)
		return false;

	//�����Ŀ�����ݿ���ԭ�еļ�¼
	char				szSql[1024];
	sprintf(szSql,"Delete From DP_RoleRight Where WORKSPACE_ID = '%s'",m_pWks->m_szID);
	ExecuteSqlDirect(hConnDest,szSql);
	sprintf(szSql,"Delete From DP_USERMGR where USER_ID = '%s'",m_pWks->m_pDB->m_szUserID);
	ExecuteSqlDirect(hConnDest,szSql);
	sprintf(szSql,"Delete From DP_UserRight where WORKSPACE_ID = '%s' AND USER_ID = '%s'",
		    m_pWks->m_szID,m_pWks->m_pDB->m_szUserID);
	ExecuteSqlDirect(hConnDest,szSql);


	CDpRecordset		rst(m_pWks->m_pDB);

	sprintf(szSql,"Select * From DP_RoleRight Where WORKSPACE_ID = '%s'",m_pWks->m_szID);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var1;
			CDpDBVariant	var2;
			CDpDBVariant	var3;
			CDpDBVariant	var4;
			CDpDBVariant	var5;
			CDpDBVariant	var6;
			if (!rst.GetFieldValue((short)1,var1) || 
				!rst.GetFieldValue((short)2,var2) || 
				!rst.GetFieldValue((short)3,var3) || 
				!rst.GetFieldValue((short)4,var4) || 
				!rst.GetFieldValue((short)5,var5) || 
				!rst.GetFieldValue((short)6,var6))
				continue;

			sprintf(szSql,"Insert Into DP_RoleRight values('%s','%s',%d,'%s','%s',%d)",
				    (LPCTSTR)var1,(LPCTSTR)var2,(int)var3,(LPCTSTR)var4,(LPCTSTR)var5,(int)var6);
			ExecuteSqlDirect(hConnDest,szSql);
		}
		rst.Close();
	}
	
	sprintf(szSql,"Select * From DP_UserMgr where USER_ID = '%s'",m_pWks->m_pDB->m_szUserID);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var1;
			CDpDBVariant	var2;
			CDpDBVariant	var3;
			CDpDBVariant	var4;

			if (!rst.GetFieldValue((short)1,var1) || 
				!rst.GetFieldValue((short)2,var2) || 
				!rst.GetFieldValue((short)3,var3) ||
				!rst.GetFieldValue((short)4,var4))
				continue;

			sprintf(szSql,"Insert into DP_USERMGR Values('%s','%s','%s',%d)",
				    (LPCTSTR)var1,(LPCTSTR)var2,(LPCTSTR)var3,(int)var4);
			ExecuteSqlDirect(hConnDest,szSql);

		}
		rst.Close();
	}

	sprintf(szSql,"Select * From DP_UserRight  Where WORKSPACE_ID = '%s' AND USER_ID = '%s'",
		    m_pWks->m_szID,m_pWks->m_pDB->m_szUserID);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var1;
			CDpDBVariant	var2;
			CDpDBVariant	var3;
			CDpDBVariant	var4;
			CDpDBVariant	var5;
			CDpDBVariant	var6;
			CDpDBVariant	var7;

			if (!rst.GetFieldValue((short)1,var1) || 
				!rst.GetFieldValue((short)2,var2) || 
				!rst.GetFieldValue((short)3,var3) || 
				!rst.GetFieldValue((short)4,var4) || 
				!rst.GetFieldValue((short)5,var5) || 
				!rst.GetFieldValue((short)6,var6) || 
				!rst.GetFieldValue((short)7,var7))
				continue;
			
			sprintf(szSql,"Insert Into DP_UserRight Values('%s','%s','%s',%d,'%s','%s',%d)",
				    (LPCTSTR)var1,(LPCTSTR)var2,(LPCTSTR)var3,(int)var4,(LPCTSTR)var5,
					(LPCTSTR)var6,(int)var7);
			ExecuteSqlDirect(hConnDest,szSql);
		}
		rst.Close();		
	}

 	return true;
}

//��Ŀ�����ݿ��ڸ�����������������
BOOL CDpCheckOutMgr::CopyLockMgrDataToDest(SQLHANDLE hConnDest)
{
	if (!hConnDest)
		return false;

	char				szSql[1024];
	char				szWhere[128];
	
	sprintf(szWhere," Where WORKSPACE_ID = '%s'",m_pWks->m_szID);

	sprintf(szSql,"Delete From DP_IMAGE_LOCKMGR %s",szWhere);
	//��ɾ����DP_IMAGE_LOCKMGR���м�¼
	ExecuteSqlDirect(hConnDest,szSql);			
	
	//��ɾ����DP_UNIT_LOCKMGR���м�¼
	sprintf(szSql,"Delete From DP_UNIT_LOCKMGR %s",szWhere);
	ExecuteSqlDirect(hConnDest,szSql);			


	return BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,1,"DP_IMAGE_LOCKMGR",szWhere,4096);
}





//��Ŀ�����ݿ��ڸ��Ʋ���ά������������
BOOL CDpCheckOutMgr::CopyParamWhDataToDest(SQLHANDLE hConnDest)
{
	if (!hConnDest)
		return false;

	CDpRecordset		rst(m_pWks->m_pDB);
	char				szSql[1024];
	sprintf(szSql,"Delete From DP_ParamCategory_WH");

	//��ɾ����DP_ParamCategory_WH���м�¼
	ExecuteSqlDirect(hConnDest,szSql);			
	//����DP_ParamCategory_WH���¼
	if (rst.Open("Select * From DP_ParamCategory_WH"))
	{
		while (rst.Next())
		{
			CDpDBVariant		var;
			CDpDBVariant		var1;
			CDpDBVariant		var2;

			if (!rst.GetFieldValue((UINT)1,var) || 
				!rst.GetFieldValue((UINT)2,var1) || 
				!rst.GetFieldValue((UINT)3,var2))
				continue;

			sprintf(szSql,"Insert Into DP_ParamCategory_WH Values(%d,'%s','%s')",
				    (int)var,(LPCTSTR)var1,(LPCTSTR)var2);
			ExecuteSqlDirect(hConnDest,szSql);
		}
		rst.Close();
	}

	sprintf(szSql,"Delete From DP_ParamItem_WH");
	
	//ɾ����DP_ParamItem_WH���м�¼
	ExecuteSqlDirect(hConnDest,szSql);			
	//����DP_ParamItem_WH���¼
	if (rst.Open("Select * From DP_ParamItem_WH"))
	{
		while (rst.Next())
		{
			CDpDBVariant		var;
			CDpDBVariant		var1;
			CDpDBVariant		var2;
			CDpDBVariant		var3;
			CDpDBVariant		var4;
			CDpDBVariant		var5;
			CDpDBVariant		var6;
			
			if (!rst.GetFieldValue((UINT)1,var) || 
				!rst.GetFieldValue((UINT)2,var1) || 
				!rst.GetFieldValue((UINT)3,var2) || 
				!rst.GetFieldValue((UINT)4,var3) || 
				!rst.GetFieldValue((UINT)5,var4) || 
				!rst.GetFieldValue((UINT)6,var5) || 
				!rst.GetFieldValue((UINT)7,var6))
				continue;

			sprintf(szSql,"Insert Into DP_ParamItem_WH Values(%d,%d,'%s',%d,%d,%d,%d)",
					(int)var,(int)var1,(LPCTSTR)var2,(int)var3,(int)var4,(int)var5,(int)var6);
			ExecuteSqlDirect(hConnDest,szSql);

		}
		rst.Close();
	}
	
	sprintf(szSql,"Delete From DP_ParamValue_WH");

	//ɾ��DP_ParamValue_WH���еļ�¼
	ExecuteSqlDirect(hConnDest,szSql);

	if (rst.Open("Select * From DP_ParamValue_WH"))
	{
		while (rst.Next())
		{
			CDpDBVariant		var;
			CDpDBVariant		var1;
			CDpDBVariant		var2;
			
			if (!rst.GetFieldValue((UINT)1,var) || 
				!rst.GetFieldValue((UINT)2,var1) || 
				!rst.GetFieldValue((UINT)3,var2))
				continue;

			sprintf(szSql,"Insert Into DP_ParamValue_WH Values(%d,%d,'%s')",
				    (int)var,(int)var1,(LPCTSTR)var2);
			ExecuteSqlDirect(hConnDest,szSql);
		}
		rst.Close();
	}


	return true;
}

//��Ŀ�����ݿ��ڸ��������ֶ�ά������������
BOOL CDpCheckOutMgr::CopyAttrWhDataToDest(SQLHANDLE	hConnDest)
{
	if (!hConnDest)
		return false;

	char		szSql[512];
	//��ɾ��Ŀ�����ݿ���DP_AttrField_WH������м�¼
	ExecuteSqlDirect(hConnDest,"Delete From DP_AttrField_WH");

	CDpRecordset	rst(m_pWks->m_pDB);

	if (rst.Open("Select * From DP_AttrField_WH"))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			CDpDBVariant	var1;
			CDpDBVariant	var2;
			CDpDBVariant	var3;
			CDpDBVariant	var4;
			CDpDBVariant	var5;
			CDpDBVariant	var6;

			if (!rst.GetFieldValue((UINT)1,var) || 
				!rst.GetFieldValue((UINT)2,var1) || 
				!rst.GetFieldValue((UINT)3,var2) || 
				!rst.GetFieldValue((UINT)4,var3) || 
				!rst.GetFieldValue((UINT)5,var4) || 
				!rst.GetFieldValue((UINT)6,var5) || 
				!rst.GetFieldValue((UINT)7,var6))
				continue;

			sprintf(szSql,"Insert Into DP_AttrField_WH Values(%d,'%s,'%s',%d,%d,%d,%d)",
				    (int)var,(LPCTSTR)var1,(LPCTSTR)var2,(int)var3,(int)var4,(int)var5,
					(int)var6);
			ExecuteSqlDirect(hConnDest,szSql);
		}
		rst.Close();
		return true;
	}
	return false;
}


//��Ŀ�����ݿ��ڸ���WBS��ά����Ϣ
BOOL CDpCheckOutMgr::CopyWBSWhDataToDest(SQLHANDLE hConnDest)
{
	if (hConnDest == NULL)
		return false;

	//ɾ��Ŀ�����ݿ���ԭ�еĸ�ά����¼
	ExecuteSqlDirect(hConnDest,"Delete From DP_WBS_A_WH");
	ExecuteSqlDirect(hConnDest,"Delete From DP_WBS_C_WH");
	ExecuteSqlDirect(hConnDest,"Delete From DP_WBS_D_WH");

	if (BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,20,"DP_WBS_A_WH",NULL,4096) && 
		BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,20,"DP_WBS_C_WH",NULL,4096) && 
		BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,20,"DP_WBS_D_WH",NULL,4096))
		return true;
	else
		return false;
}




//��Ŀ�걾�����ݿ��и��ƹ�������¼
BOOL CDpCheckOutMgr::CopyWksDataToDest(SQLHANDLE hConnDest)
{
	if (hConnDest == NULL)
		return false;

	//ɾ��Ŀ�����ݿ���ԭ�еĸù������ļ�¼
	char		szDelSql[256];
	sprintf(szDelSql,"Delete From DP_WorkspaceMgr Where WORKSPACE_ID = '%s'",m_pWks->m_szID);
	if (!ExecuteSqlDirect(hConnDest,szDelSql))
		return false;

	char	szWhere[128];
	sprintf(szWhere," Where WORKSPACE_ID = '%s' ",m_pWks->m_szID);

	//���������������Ҫ���ڴ���С
	char	szBlockSize[256];
	memset(szBlockSize,0,sizeof(szBlockSize));
	int		iBlockSize	= DEFAULT_BINARY_BLOCK_SIZE;

	if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szBlockSize,"Select NVL(Length(CONTROL_POINT),0),NVL(Length(PROJECT_DATA),0) From DP_WorkspaceMgr %s",szWhere);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szBlockSize,"Select ISNULL(DATALENGTH(CONTROL_POINT),0),ISNULL(DATALENGTH(PROJECT_DATA),0) From DP_WorkspaceMgr %s",szWhere);

	if (strlen(szBlockSize) > 0)
	{
		CDpRecordset	rst(m_pWks->m_pDB);
		if (rst.Open(szBlockSize))
		{
			if (rst.Next())
			{
				CDpDBVariant	var;
				CDpDBVariant	var1;
				int				i		= 0;
				int				j		= 0;
				if (rst.GetFieldValue(1,var) && rst.GetFieldValue(2,var))
				{
					i = (int)var;
					j = (int)var1;
					if (i > j)
						iBlockSize = i;
					else
						iBlockSize = j;
				}
			}
			rst.Close();
		}

	}


	return BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,1,"DP_WorkspaceMgr",szWhere,iBlockSize);
}

//��Ŀ�����ݿ��и���ͼ������
BOOL CDpCheckOutMgr::CopyLayerDataToDest(SQLHANDLE hConnDest)
{
	if (hConnDest == NULL)
		return false;

	//ɾ��Ŀ�����ݿ���ԭ�еĸù������ļ�¼
	char		szDelSql[256];
	sprintf(szDelSql,"Delete From DP_LayersMgr Where WORKSPACE_ID = '%s'",m_pWks->m_szID);
	if (!ExecuteSqlDirect(hConnDest,szDelSql))
		return false;

	char	szWhere[128];
	sprintf(szWhere," Where WORKSPACE_ID = '%s' ",m_pWks->m_szID);

	//���������������Ҫ���ڴ���С
	char	szBlockSize[256];
	memset(szBlockSize,0,sizeof(szBlockSize));
	int		iBlockSize	= DEFAULT_BINARY_BLOCK_SIZE;

	if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szBlockSize,"Select Max(NVL(Length(SYMBOL_XML_DATA),0)) From DP_LayersMgr %s",szWhere);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szBlockSize,"Select ISNULL(DATALENGTH(SYMBOL_XML_DATA),0) From DP_LayersMgr %s",szWhere);

	if (strlen(szBlockSize) > 0)
	{
		CDpRecordset	rst(m_pWks->m_pDB);
		if (rst.Open(szBlockSize))
		{
			if (rst.Next())
			{
				CDpDBVariant	var;
				int				i		= 0;
				if (rst.GetFieldValue(1,var))
					iBlockSize = (int)var;
			}
			rst.Close();
		}

	}

	return BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,20,"DP_LayersMgr",szWhere,iBlockSize);

}





//��Ŀ�����ݿ��и���ͼ�������
BOOL CDpCheckOutMgr::CopyGridMgrDataToDest(SQLHANDLE hConnDest,CDpUnit* pUnit)
{
	if (!hConnDest)
		return false;

	BOOL		bRet		= false;

	if (pUnit)
	{
		//ɾ��Ŀ����е����и�ͼ����ͼ��������Ϣ
		char		szSql[1024];
		sprintf(szSql,"Delete From DP_GridInWks Where GRID_ID = '%s'",pUnit->m_szID);
		ExecuteSqlDirect(hConnDest,szSql);

		tm*			tmTmp = localtime(&(pUnit->m_tmUpdateDate));
		char		szUpdateDate[100];

		if (pUnit->m_tmUpdateDate)
//			BuildDateTimeSql(m_pWks->m_pDB->GetDBType(),tmTmp,szUpdateDate);
			BuildDateTimeSql(DP_ACCESS,tmTmp,szUpdateDate);
		else
			sprintf(szUpdateDate,"NULL");

		//����Ŀ����в����¼
		if(1 ==pUnit->m_pWks->GetWksVersion())
		{
			sprintf(szSql,"Insert Into DP_GridInWks Values('%s','%s','%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s,%d,'%s','%s')",
				m_pWks->m_szID,pUnit->m_szID,pUnit->m_szUnitName?pUnit->m_szUnitName:"",pUnit->m_rtBound.m_dMinX,pUnit->m_rtBound.m_dMinY,
				pUnit->m_rtBound.m_dMaxX,pUnit->m_rtBound.m_dMaxY,pUnit->m_ptQuadrangle[0].m_dx,pUnit->m_ptQuadrangle[0].m_dy,
				pUnit->m_ptQuadrangle[1].m_dx,pUnit->m_ptQuadrangle[1].m_dy,pUnit->m_ptQuadrangle[2].m_dx,pUnit->m_ptQuadrangle[2].m_dy,
				pUnit->m_ptQuadrangle[3].m_dx,pUnit->m_ptQuadrangle[3].m_dy,szUpdateDate,pUnit->m_iUnitStatus,
				pUnit->m_szParamTableName?pUnit->m_szParamTableName:"",pUnit->m_szLocker?pUnit->m_szLocker:"");
		}
		else if(2==pUnit->m_pWks->GetWksVersion())
		{
			sprintf(szSql,"Insert Into DP_GridInWks Values('%s','%s','%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s,%d,'%s','%s',%d,'%s','%s')",
				m_pWks->m_szID,pUnit->m_szID,pUnit->m_szUnitName?pUnit->m_szUnitName:"",pUnit->m_rtBound.m_dMinX,pUnit->m_rtBound.m_dMinY,
				pUnit->m_rtBound.m_dMaxX,pUnit->m_rtBound.m_dMaxY,pUnit->m_ptQuadrangle[0].m_dx,pUnit->m_ptQuadrangle[0].m_dy,
				pUnit->m_ptQuadrangle[1].m_dx,pUnit->m_ptQuadrangle[1].m_dy,pUnit->m_ptQuadrangle[2].m_dx,pUnit->m_ptQuadrangle[2].m_dy,
				pUnit->m_ptQuadrangle[3].m_dx,pUnit->m_ptQuadrangle[3].m_dy,szUpdateDate,pUnit->m_iUnitStatus,
				pUnit->m_szParamTableName?pUnit->m_szParamTableName:"",pUnit->m_szLocker?pUnit->m_szLocker:"",(int)(pUnit->m_Type),pUnit->m_szRefer,pUnit->m_szLocalName);
		}
		else
			return FALSE;

		return ExecuteSqlDirect(hConnDest,szSql);
	}
	else				//���Ƶ�ǰ�����������е�ͼ���Ĺ�������
	{
		//ɾ��Ŀ����е����и�ͼ����ͼ��������Ϣ
		char		szSql[1024];
		char		szWhere[128];
		sprintf(szWhere," Where WORKSPACE_ID = '%s'",m_pWks->m_szID);
		sprintf(szSql,"Delete From DP_GridInWks %s",szWhere);
		ExecuteSqlDirect(hConnDest,szSql);

		return BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,20,"DP_GridInWks",szWhere,4096);
	}
}

//��Ŀ�����ݿ��ڸ��������ֵ����������
BOOL CDpCheckOutMgr::CopyAttrFieldDircToDest(SQLHANDLE hConnDest)	
{
	if (!hConnDest)
		return false;

	char		szSql[512];
	char		szWhereClause[128];

	if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szWhereClause," Where SUBSTR(ATTR_TABLE_NAME,2,14) = '%s'",m_pWks->m_szID);
	else if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szWhereClause," Where MID(ATTR_TABLE_NAME,2,14) = '%s'",m_pWks->m_szID);
	else
		sprintf(szWhereClause," Where SUBSTRING(ATTR_TABLE_NAME,2,14) = '%s'",m_pWks->m_szID);

	sprintf(szSql,"Delete From DP_ATTR_DICTIONARY Where MID(ATTR_TABLE_NAME,2,14) = '%s'",m_pWks->m_szID);
	ExecuteSqlDirect(hConnDest,szSql);

	sprintf(szSql,"Select * From DP_ATTR_DICTIONARY %s",szWhereClause);

	CDpRecordset	rst(m_pWks->m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant		var;			
			CDpDBVariant		var1;			
			CDpDBVariant		var2;			
			CDpDBVariant		var3;			
			CDpDBVariant		var4;			
			CDpDBVariant		var5;			
			CDpDBVariant		var6;			
			CDpDBVariant		var7;			
			CDpDBVariant		var8;
			CDpDBVariant		var9;

			if (!rst.GetFieldValue((UINT)1,var) ||
				!rst.GetFieldValue((UINT)2,var1) ||
				!rst.GetFieldValue((UINT)3,var2) ||
				!rst.GetFieldValue((UINT)4,var3) ||
				!rst.GetFieldValue((UINT)5,var4) ||
				!rst.GetFieldValue((UINT)6,var5) ||
				!rst.GetFieldValue((UINT)7,var6) ||
				!rst.GetFieldValue((UINT)8,var7) ||
				!rst.GetFieldValue((UINT)9,var8) ||
				!rst.GetFieldValue((UINT)10,var9))
				continue;

			sprintf(szSql,"Insert Into DP_ATTR_DICTIONARY Values('%s',%d,'%s','%s',%d,%d,%d,%d,%d,%d)",
				    (LPCTSTR)var,(int)var1,(LPCTSTR)var2,(LPCTSTR)var3,(int)var4,(int)var5,(int)var6,
					(int)var7,(int)var8,(int)var9);
			ExecuteSqlDirect(hConnDest,szSql);
		}
		rst.Close();
		return true;
	}

	return false;
}

//��Ŀ�����ݿ⸴�Ʋ�����,���а����ȴ���������󿽱���¼
BOOL CDpCheckOutMgr::CopyParamTableToDest(SQLHANDLE hConnDest,const char* szParamTableName)
{
	if (!hConnDest || !szParamTableName || strlen(szParamTableName) < UNIT_ID_LEN)
		return false;

	char	szSql[512];
	//�ж�Ŀ�������Ƿ��иñ�
	sprintf(szSql,"Select * From %s",szParamTableName);
	CDpRecordset	rst(m_pWks->m_pDB);
	if (!rst.Open(szSql))
		return true;
	
	
	//�Ƚ�Ŀ�����ݿ������еĸò���ɾ��
	sprintf(szSql,"Drop Table %s",szParamTableName);
	ExecuteSqlDirect(hConnDest,szSql);
	
	//�ٴ����ñ�
/*
	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Select * Into %s From DP_PARAMTABLETEMPLATE WHERE 1 = 2",szParamTableName);
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table %s As Select * From DP_PARAMTABLETEMPLATE WHERE 1 = 2",szParamTableName);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select * Into %s From DP_PARAMTABLETEMPLATE WHERE 1 = 2",szParamTableName);
*/
/*
	if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Create Table %s(CATEGORY_ID INTEGER,PARAM_ID INTEGER PRIMARY KEY,PARAM_NAME CHAR(50) UNIQUE,PARAM_TYPE SMALLINT,PARAM_LEN SMALLINT,PARAM_PRECISION SMALLINT,PARAM_SELECT_ID INTEGER,PARAM_VALUE CHAR(200))",szParamTableName);
	else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table %s(CATEGORY_ID INTEGER,PARAM_ID INTEGER PRIMARY KEY,PARAM_NAME VARCHAR2(50) UNIQUE,PARAM_TYPE SMALLINT,PARAM_LEN SMALLINT,PARAM_PRECISION SMALLINT,PARAM_SELECT_ID INTEGER,PARAM_VALUE VARCHAR2(200))",szParamTableName);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Create Table %s(CATEGORY_ID INTEGER,PARAM_ID INTEGER PRIMARY KEY,PARAM_NAME CHAR(50) UNIQUE,PARAM_TYPE SMALLINT,PARAM_LEN SMALLINT,PARAM_PRECISION SMALLINT,PARAM_SELECT_ID INTEGER,PARAM_VALUE CHAR(200))",szParamTableName);
*/

	sprintf(szSql,"Create Table %s(CATEGORY_ID INTEGER,PARAM_ID INTEGER PRIMARY KEY,PARAM_NAME VARCHAR(50) UNIQUE,PARAM_DESC VARCHAR(200),PARAM_TYPE SMALLINT,PARAM_LEN SMALLINT,PARAM_PRECISION SMALLINT,PARAM_SELECT_ID INTEGER,PARAM_VALUE VARCHAR(200))",szParamTableName);
	ExecuteSqlDirect(hConnDest,szSql);

	//����ò������и��Ƽ�¼
	while (rst.Next())
	{
		CDpDBVariant		var;			
		CDpDBVariant		var1;			
		CDpDBVariant		var2;			
		CDpDBVariant		var3;			
		CDpDBVariant		var4;			
		CDpDBVariant		var5;			
		CDpDBVariant		var6;			
		CDpDBVariant		var7;			
		CDpDBVariant		var8;

		if (!rst.GetFieldValue((UINT)1,var) ||
			!rst.GetFieldValue((UINT)2,var1) ||
			!rst.GetFieldValue((UINT)3,var2) ||
			!rst.GetFieldValue((UINT)4,var3) ||
			!rst.GetFieldValue((UINT)5,var4) ||
			!rst.GetFieldValue((UINT)6,var5) ||
			!rst.GetFieldValue((UINT)7,var6) ||
			!rst.GetFieldValue((UINT)8,var7) || 
			!rst.GetFieldValue((UINT)9,var8))
			continue;

		sprintf(szSql,"Insert Into %s Values(%d,%d,'%s','%s',%d,%d,%d,%d,'%s')",
				szParamTableName,(int)var,(int)var1,(LPCTSTR)var2,(LPCTSTR)var3,(int)var4,
				(int)var5,(int)var6,(int)var7,(LPCTSTR)var8);
		ExecuteSqlDirect(hConnDest,szSql);
	}
	rst.Close();
	return true;
}


//��Ŀ�����ݿ��и���Ӱ������
BOOL CDpCheckOutMgr::CopyImageMgrDataToDest(SQLHANDLE hConnDest,const char* szImageID)
{
	if (!hConnDest)
		return false;

	BOOL		bRet		= false;

	//ɾ��Ŀ����е����и�ͼ����ͼ��������Ϣ
	char		szSql[1024];
	char		szWhere[128];
	if (szImageID)
		sprintf(szWhere," Where WORKSPACE_ID = '%s' AND IMAGE_ID = '%s'",m_pWks->m_szID,szImageID);
	else
		sprintf(szWhere," Where WORKSPACE_ID = '%s'",m_pWks->m_szID);

	sprintf(szSql,"Delete From DP_IMAGEMGR %s",szWhere);
	ExecuteSqlDirect(hConnDest,szSql);

	return BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,20,"DP_IMAGEMGR",szWhere,4096);

}


//��Ŀ�����ݿ��и���Ӱ����XML�����ݱ�
BOOL CDpCheckOutMgr::CopyImageXmlDataToDest(SQLHANDLE hConnDest,CDpImage* pSourceImage)
{
	if (hConnDest == NULL || !pSourceImage)  
		return false;

	BOOL			bRet		= false;

	//ɾ��Ŀ�����ݿ����ظ���¼
	char		szDelSql[128];
	sprintf(szDelSql,"Delete From DP_IMAGEMGR_XML where IMAGE_ID = '%s'",pSourceImage->m_szID);
	ExecuteSqlDirect(hConnDest,szDelSql);	

	//ȡ�õ�ǰӰ�������е�XML��ǩ������
	CDpStringArray	nArray;	
	pSourceImage->GetAllXmlTagName(nArray);
	
	int				iCount		= nArray.GetCount();

	char			szSql[256];
	for (int i = 0; i < iCount; i++)
	{
		memset(szSql,0,sizeof(szSql));
		char*		szTagName	= nArray.GetItem(i);

		BYTE*		pBuff		= NULL;
		int			iBuffLen	= 0;

		if (pSourceImage->GetImageXmlData(szTagName,&pBuff,&iBuffLen) && pBuff && iBuffLen > 0)
		{
			sprintf(szSql,"Insert Into DP_IMAGEMGR_XML values('%s','%s',?)",pSourceImage->m_szID,szTagName);
			SetLargerFieldValue(hConnDest,szSql,pBuff,iBuffLen);
		}
		else
		{
			sprintf(szSql,"Insert Into DP_IMAGEMGR_XML values('%s','%s',NULL)",pSourceImage->m_szID,szTagName);
			ExecuteSqlDirect(hConnDest,szSql);	
		}

		if (pBuff)
		{
			delete [] pBuff;
			pBuff = NULL;
		}
	}

	return true;
}


//��Ŀ�����ݿ��и���ָ����Ӱ����XML������
BOOL CDpCheckOutMgr::CopyImageXmlDataToDest(SQLHANDLE hConnDest,const char* szImageID,const char* szXmlTagName)
{
	if (!hConnDest || !szImageID || !szXmlTagName)
		return false;

	//��ɾ��ԭ�м�¼
	char		szSql[256];
	char		szWhere[200];
	sprintf(szWhere," Where IMAGE_ID = '%s' AND XML_TAGNAME = '%s'",
		    szImageID,szXmlTagName);

	sprintf(szSql,"Delete From DP_IMAGEMGR_XML %s",szWhere);
	ExecuteSqlDirect(hConnDest,szSql);


	int		iBlockSize	= DEFAULT_BINARY_BLOCK_SIZE;		

	//���Դ���ݿ�������ORACLE����SQLSERVER�Ļ�,���ȼ��㵱ǰ����ʸ�����ݵ�SIZE
	if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select Max(NVL(Length(XML_DATA),0)) From DP_IMAGEMGR_XML %s",szWhere);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select Max(ISNULL(DATALENGTH(XML_DATA),0)) From DP_IMAGEMGR_XML %s",szWhere);
	
	if (strlen(szSql) > 0)
	{
		CDpRecordset	rst(m_pWks->m_pDB);
		if (rst.Open(szSql))
		{
			if (rst.Next())
			{
				CDpDBVariant	var;
				if (rst.GetFieldValue(1,var))
					iBlockSize		= (int)var;
			}
			rst.Close();
		}
	}
	
	return BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,1,"DP_IMAGEMGR_XML",szWhere,iBlockSize);
}








//��Ŀ�����ݿ��ڴ����յĿռ����ݱ�
//�ڴ����ñ�֮ǰ���жϸñ���Ŀ�����ݿ����Ƿ��Ѿ�����,������ھͲ����д�������
BOOL CDpCheckOutMgr::CreateEmptySpatialTableInDest(SQLHANDLE hConnDest,const char* szSpaTableName)
{
	if (!hConnDest || !szSpaTableName || strlen(szSpaTableName) < LAYER_ID_LEN) 
		return false;

	//���жϸñ���Ŀ�����ݿ��Ƿ��Ѿ�����
	char		szSql[256];
	sprintf(szSql,"Select * From %s Where 1 = 2",szSpaTableName);

	//��������
	if (ExecuteSqlDirect(hConnDest,szSql))
		return true;

	//�����ռ����ݱ�
	sprintf(szSql,"Create Table %s(OID CHAR(32),ST_GRID_ID VARCHAR(19),ST_MINX FLOAT,ST_MINY FLOAT,ST_MAXX FLOAT,ST_MAXY FLOAT,ST_OBJECT_TYPE SMALLINT,SPATIAL_DATA IMAGE,PRIMARY KEY(OID,ST_GRID_ID))",
		    szSpaTableName);
/*
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Create Table %s(OID INTEGER,ST_GRID_ID CHAR(19),ST_MINX FLOAT,ST_MINY FLOAT,ST_MAXX FLOAT,ST_MAXY FLOAT,ST_OBJECT_TYPE SMALLINT,SPATIAL_DATA IMAGE,PRIMARY KEY(OID,ST_GRID_ID))",
		        szSpaTableName);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table %s(OID INTEGER,ST_GRID_ID VARCHAR2(19),ST_MINX FLOAT,ST_MINY FLOAT,ST_MAXX FLOAT,ST_MAXY FLOAT,ST_OBJECT_TYPE SMALLINT,SPATIAL_DATA BLOB,PRIMARY KEY(OID,ST_GRID_ID))",
		        szSpaTableName);	
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Create Table %s(OID INTEGER,ST_GRID_ID CHAR(19),ST_MINX FLOAT,ST_MINY FLOAT,ST_MAXX FLOAT,ST_MAXY FLOAT,ST_OBJECT_TYPE SMALLINT,SPATIAL_DATA IMAGE,PRIMARY KEY(OID,ST_GRID_ID))",
		        szSpaTableName);
*/

	return ExecuteSqlDirect(hConnDest,szSql);

}

//��Ŀ�����ݿ���ָ���Ŀռ����ݱ��и��Ƽ�¼
BOOL CDpCheckOutMgr::CopyFeatureRecordToDest(SQLHANDLE hConnDest,const char* szLayerID,const char* szGridID)
{
	if (!hConnDest || !szLayerID || !szGridID || 
		strlen(szLayerID) != LAYER_ID_LEN)
		return false;

	char		szSpaTableName[100];
	char		szAttrTableName[100];
	sprintf(szSpaTableName,"S%s",szLayerID);
	sprintf(szAttrTableName,"A%s",szLayerID);

	char		szWhere[1024];
	char		szWhereAttr[1024];

	sprintf(szWhere," Where ST_GRID_ID = '%s'",szGridID);
	sprintf(szWhereAttr," Where ATT_GRID_ID = '%s'",szGridID);

	int		iShapeBlockSize	= DEFAULT_BINARY_BLOCK_SIZE;		//Ϊ�˲��Լ���*4
	char	szSql[2048];

	//��ɾ��Ŀ�����ݿ���ԭ�еĿռ����ݺ���������
	//ɾ����������
	sprintf(szSql,"Delete From %s %s",szAttrTableName,szWhereAttr);
	ExecuteSqlDirect(hConnDest,szSql);
	//ɾ���ռ�����
	sprintf(szSql,"Delete From %s %s",szSpaTableName,szWhere);
	ExecuteSqlDirect(hConnDest,szSql);

	memset(szSql,0,sizeof(szSql));

	//���Դ���ݿ�������ORACLE����SQLSERVER�Ļ�,���ȼ��㵱ǰ����ʸ�����ݵ�SIZE
	if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Select Max(NVL(Length(SPATIAL_DATA),0)) From %s %s",szSpaTableName,szWhere);
	else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Select Max(ISNULL(DATALENGTH(SPATIAL_DATA),0)) From %s %s",szSpaTableName,szWhere);
	
	if (strlen(szSql) > 0)
	{
		CDpRecordset	rst(m_pWks->m_pDB);
		if (rst.Open(szSql))
		{
			if (rst.Next())
			{
				CDpDBVariant	var;
				if (rst.GetFieldValue(1,var))
					iShapeBlockSize		= (int)var;
			}
			rst.Close();
		}
	}

	if (iShapeBlockSize <= 0)
		return true;

	//��ʼ��������
	if (BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,50,szSpaTableName,szWhere,iShapeBlockSize) && 
		BulkCopyRecordToDest(m_pWks->m_pDB->GetDBHandle(),hConnDest,100,szAttrTableName,szWhereAttr,iShapeBlockSize))
	{
		return true;
	}
	else
		return false;


}


//��Ŀ�����ݿ��ڴ����յ��������ݱ�
//�ڴ�����֮ǰ���жϸñ���Ŀ�����ݿ����Ƿ��Ѿ�����,������ھͲ����д�������
BOOL CDpCheckOutMgr::CreateEmptyAttributeTableToDest(SQLHANDLE hConnDest,CDpLayer* pSourceLayer)
{
	if (!hConnDest || !pSourceLayer)
		return false;

	//���жϸñ���Ŀ�����ݿ��Ƿ��Ѿ�����
	char		szSql[8192];
	char		szField[128];
	char		szFieldDefine[256];
	sprintf(szSql,"Select * From %s where 1 = 2",pSourceLayer->m_szAttributeTableName);

	//��������
	if (ExecuteSqlDirect(hConnDest,szSql))
		return true;

	//�������SQL���
	memset(szSql,0,sizeof(szSql));

/*
	if (m_pDB->GetDBType() == DP_ACCESS)
		sprintf(szSql,"Create Table %s(ATT_OID INTEGER,ATT_GRID_ID CHAR(19)",pSourceLayer->m_szAttributeTableName);
	else if (m_pDB->GetDBType() == DP_ORACLE)
		sprintf(szSql,"Create Table %s(ATT_OID INTEGER,ATT_GRID_ID VARCHAR2(19)",pSourceLayer->m_szAttributeTableName);
	else if (m_pDB->GetDBType() == DP_SQLSERVER)
		sprintf(szSql,"Create Table %s(ATT_OID INTEGER,ATT_GRID_ID CHAR(19)",pSourceLayer->m_szAttributeTableName);
*/

	sprintf(szSql,"Create Table %s(ATT_OID CHAR(32),ATT_GRID_ID VARCHAR(19)",pSourceLayer->m_szAttributeTableName);

	int		iCount	= pSourceLayer->GetAttrFieldCount();
	if (iCount == 0)
		strcat(szSql,"ATT_OID CHAR(32) PRIMARY KEY)");
	else
	{
		for (int i = 0; i < iCount; i++)
		{
			CDpCustomFieldInfo	info;
			if (!pSourceLayer->GetAttrFieldInfo(i,&info))
				return false;
			
			switch (info.m_CustomFieldType)
			{
				case DP_CFT_BYTE:
				case DP_CFT_SMALLINT:
				case DP_CFT_BOOL:
					sprintf(szField,"SMALLINT");
					break;
				case DP_CFT_INTEGER:
				case DP_CFT_COLOR:
				case DP_CFT_ENUM:
					sprintf(szField,"INTEGER");
					break;
				case DP_CFT_FLOAT:
				case DP_CFT_DOUBLE:
					sprintf(szField,"FLOAT");
					break;
				case DP_CFT_DATE:
					sprintf(szField,"DATE");
					break;
				case DP_CFT_VARCHAR:
				{
				/*
					if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
						sprintf(szField,"CHAR(%d)",info.m_iSize);
					else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
						sprintf(szField,"VARCHAR2(%d)",info.m_iSize);
					else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
						sprintf(szField,"VARCHAR(%d)",info.m_iSize);
					else
						return false;
				*/
					sprintf(szField,"VARCHAR(%d)",info.m_iSize);
					break;
				}
				case DP_CFT_BLOB:
				{
				/*
					if (m_pWks->m_pDB->GetDBType() == DP_ACCESS)
						sprintf(szField,"IMAGE");
					else if (m_pWks->m_pDB->GetDBType() == DP_ORACLE)
						sprintf(szField,"BLOB");
					else if (m_pWks->m_pDB->GetDBType() == DP_SQLSERVER)
						sprintf(szField,"IMAGE");
					else
						return false;
				*/
					sprintf(szField,"IMAGE");
					break;
				}
				default:
					return false;
			}

			//�Ƿ�����Ϊ��
			if (!(info.m_bAllowNull))
				strcat(szField," NOT NULL");

			if (info.m_bIsUnique && info.m_CustomFieldType != DP_CFT_BLOB)
				strcat(szField," UNIQUE");

			sprintf(szFieldDefine," ,%s %s ",info.m_strName,szField);
			strcat(szSql,szFieldDefine);

		}
		strcat(szSql,",PRIMARY KEY(ATT_OID,ATT_GRID_ID))");
	}

	//ִ�д���������
	return ExecuteSqlDirect(hConnDest,szSql);
}


//ִ��һ��CHECKOUT����
//szFileName--CheckOut�����ص��ļ����ݿ������
//pInfo--ҪGET�ľ������ݵ�ID�Ѿ���־����Ϣ
BOOL CDpCheckOutMgr::GetOut(const char* szFileName,CDpCheckInfo* pInfo,PWORKPROCESS pFun,_DPWP_Param* pParam)
{
	if (!szFileName || strlen(szFileName) <= 0 
		|| !pInfo || pInfo->GetCheckType() != DP_CHECKOUT)
		return false;
	
	int		iCount	= pInfo->GetItemCount();
	if (iCount <= 0)
		return false;

	//�ж�ҪCHECKOUT��Ŀ���ļ��Ƿ����,��������ھ͸���һ���յı������ݿ��ļ�
	if (!DBFileIsExist(szFileName))
	{
		if (!MakeEmptyDBFile(szFileName))
			return false;
	}
	
	//���½��Ŀ����ݿ��
	SQLHANDLE		hEnv				= NULL;					//�������
	SQLHANDLE		hConn				= NULL;					//���Ӿ��
	SQLCHAR			szConnString[200];
	SQLCHAR			szOutConn[1024]		= "\0";
	SQLSMALLINT		siOutLen			= 0;

	BOOL			bRet				= false;
	int				iGetCount			= 0;
	int				i					= 0;

	//��ԭ���ݿ�
	if (!ResumeDBFile(szFileName))
		return FALSE;
	
	//��ʼ����
	memset(szConnString,0,sizeof(szConnString));
	sprintf((char*)szConnString,"Driver={Microsoft Access Driver (*.mdb)};Dbq=%s;Pwd=DataPlatform",szFileName); //;Pwd=DataPlatform

	if (SQLAllocHandle(SQL_HANDLE_ENV,NULL,&hEnv) == SQL_SUCCESS)
	{
		//��ODBC���ó�Ϊ�汾3,����ĳЩODBC API �������ܱ�֧��
		SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
		if (SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hConn) == SQL_SUCCESS)
		{
			//�������ݿ�����
			if (SQLDriverConnect(hConn,NULL,szConnString,sizeof(szConnString),szOutConn,sizeof(szOutConn),
							 &siOutLen,SQL_DRIVER_NOPROMPT) == SQL_SUCCESS)
				bRet	= true;
		}
	}

	if (!bRet)
		goto Err;

	//�ڿ�ʼCHECKOUT����֮ǰ,�ȿ�ʼ����
	if (SQLSetConnectAttr(hConn,SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_OFF,0) != SQL_SUCCESS)
		goto Err;



	//�ȸ��Ƹ��������ļ�¼
	if (!CopyUserInfoToDest(hConn) || 						//�����û���Ϣ
		!CopyWksDataToDest(hConn) || 						//������������������
//		!CopyGridMgrDataToDest(hConn) ||					//����ͼ�����������
		!CopyLayerDataToDest(hConn) || 						//����ͼ����������
		!CopyParamWhDataToDest(hConn) ||					//��������ά�������� 
		!CopyAttrWhDataToDest(hConn) || 					//���������ֶ�ά��������
		!CopyWBSWhDataToDest(hConn) || 
		!CopyAttrFieldDircToDest(hConn))					//���������ֵ������
	{
		bRet = false;
		goto Err;
	}
	
	//����ͼ���Ĺ�����Ϣ
	{
		CDpStringArray idArray;
		pInfo->GetAllDistinctUnit(idArray);
		int		iCount = idArray.GetCount();

		for (int i = 0; i < iCount; i++)
		{
			CDpUnit*	pUnit = NULL;
			if (m_pWks->GetUnit(idArray.GetItem(i),&pUnit) && pUnit)
			{
				CopyGridMgrDataToDest(hConn,pUnit);
				delete pUnit;
				pUnit = NULL;
			}
		}
	}



	//������Ҫ���ƵĲ����������
	{
		CDpStringArray		idArray;
		pInfo->GetAllDistinctLayer(idArray);
		int		iCount = idArray.GetCount();
		for (int i = 0; i < iCount; i++)
		{
			double		dTmp			= ((double)i) / ((double)iCount) * 100;
			if (pFun && pParam)
			{
				pParam->iPrecent	= (int)dTmp;
				(*pFun)(pParam);
				if (pParam->bIsBreak)
				{
					bRet	= false;
					goto Err;
				}
			}


			char*		szLayerID		= idArray.GetItem(i);
			char		szPTabName[50];
			char		szSTabName[50];
			char		szFeaCls[256];

			sprintf(szPTabName,"P%s",szLayerID);
			sprintf(szSTabName,"S%s",szLayerID);

			//��Ŀ�����ݿ��н�����Ӧ������������������
			if (!CopyParamTableToDest(hConn,szPTabName))
				goto Err;

			//�����յĿռ����ݱ�
			if (!CreateEmptySpatialTableInDest(hConn,szSTabName))
				goto Err;

			//�����յ��������ݱ�
			CDpLayer*		pLayer	= NULL;
			if (m_pWks->GetLayer((char*)idArray.GetItem(i),&pLayer) && pLayer)
			{
				if (!CreateEmptyAttributeTableToDest(hConn,pLayer))
				{
					delete pLayer;
					pLayer = NULL;
					goto Err;
				}
				delete pLayer;
				pLayer = NULL;
			}
			else
				goto Err;

			//����FeaCls��ͼ
			sprintf(szFeaCls,"Create View FeaCls%s AS Select A.*,B.* From S%s A,A%s B where A.OID = B.ATT_OID AND A.ST_GRID_ID = B.ATT_GRID_ID",
					szLayerID,szLayerID,szLayerID);

			ExecuteSqlDirect(hConn,szFeaCls);

		}
	}

	//��������Ҫ���Ƶ�Ӱ��Ĳ�����͹�������,����һ����Ҫ����ļ�����
	{
		CDpStringArray		idArray;
		pInfo->GetAllDistinctImage(idArray);
		int		iCount = idArray.GetCount();
		for (int i = 0; i < iCount; i++)
		{
			char*		szImageID		= idArray.GetItem(i);
			char		szPTabName[50];
			sprintf(szPTabName,"P%s",szImageID);

			//����Ӱ���������
			if (!CopyImageMgrDataToDest(hConn,szImageID))
				goto Err;

			//��Ŀ�����ݿ��н�����Ӧ������������������
			if (!CopyParamTableToDest(hConn,szPTabName))
				goto Err;
			
		}

	}




	//����ƾ��������
	{
		for (i = 0; i < iCount; i++)
		{
			const CDpCheckInfo::_DpCheckOpreaInfo* pItem = NULL;
			pInfo->GetItem(i,&pItem);
			if (pItem)
			{
				if (pItem->iDataType == 2)				//IMAGE
				{
					//���XML��ǩ��Ϊ��,��˵������ҪGET��Ӱ��ı�ǩ,Ҳֻ����ҪGETԭʼӰ��
					if (!(pItem->szLayerID) || strlen(pItem->szLayerID) <= 0)
					{
						((CDpCheckInfo::_DpCheckOpreaInfo*)pItem)->bSuccess = true;
						continue;
					}
						
					//���ж�Ȩ��
					DP_RIGHT		nRight = DP_RIGHT_NOTRIGHT;
					m_pWks->m_pDB->m_pRightMgr->GetImageXmlRight(pItem->szUnitID,
						                         pItem->szLayerID,&nRight);
					if (nRight <= DP_RIGHT_NOTRIGHT)
					{
						CopyStr((char**)(&(pItem->szErrorMsg)),"û�ж���Ȩ��");			//������1:û�ж���Ȩ��
						continue;
					}

					if (pItem->nLockType == DP_UT_LOCKED)
					{
						if (nRight < DP_RIGHT_READWRITE)
						{
							CopyStr((char**)(&(pItem->szErrorMsg)),"û��д��Ȩ��");		//������2:û��д��
							continue;
						}

						//�жϸ�XML�Ƿ��Ѿ�������������
						char		szOtherLocker[100];
						m_pWks->m_pDB->m_pRightMgr->GetImageXmlLockerName(m_pWks->m_szID,pItem->szUnitID,
							              pItem->szLayerID,szOtherLocker);
						if (strlen(szOtherLocker) > 0 && _stricmp(szOtherLocker,m_pWks->m_pDB->m_szUserID) != 0)
						{
							CopyStr((char**)(&(pItem->szErrorMsg)),"ͼ�㱻�����û�����");		//������3:��Ŀ���ѱ������û�����
							continue;
						}
					}

					if (CopyImageXmlDataToDest(hConn,pItem->szUnitID,pItem->szLayerID))
					{
						if (pItem->nLockType == DP_UT_LOCKED)
						{
							if (!m_pWks->m_pDB->m_pRightMgr->LockImageXml(m_pWks->m_szID,pItem->szUnitID,pItem->szLayerID))
							{
								CopyStr((char**)(&(pItem->szErrorMsg)),"����ͼ��ʧ��");		//������4:����Ŀ��ʧ��
								continue;
							}
						}
						((CDpCheckInfo::_DpCheckOpreaInfo*)pItem)->bSuccess = true;
					}
					else
						CopyStr((char**)(&(pItem->szErrorMsg)),"δ֪����");		//������10:δ֪����

				}
				else
				{
					//���ж�Ȩ��
					DP_RIGHT		nRight = DP_RIGHT_NOTRIGHT;
					m_pWks->m_pDB->m_pRightMgr->GetUnitLayerRight(pItem->szUnitID,
						                         pItem->szLayerID,&nRight);
					if (nRight <= DP_RIGHT_NOTRIGHT)
					{
						CopyStr((char**)(&(pItem->szErrorMsg)),"û�ж���Ȩ��");			//������1:û�ж���Ȩ��
						continue;
					}

					if (pItem->nLockType == DP_UT_LOCKED)
					{
						if (nRight < DP_RIGHT_READWRITE)
						{
							CopyStr((char**)(&(pItem->szErrorMsg)),"û��д��Ȩ��");		//������2:û��д��
							continue;
						}

						//�жϸ�XML�Ƿ��Ѿ�������������
						char		szOtherLocker[100];
						m_pWks->m_pDB->m_pRightMgr->GetUnitLayerLockerName(m_pWks->m_szID,pItem->szUnitID,
							              pItem->szLayerID,szOtherLocker);
						if (strlen(szOtherLocker) > 0 && _stricmp(szOtherLocker,m_pWks->m_pDB->m_szUserID) != 0)
						{
							CopyStr((char**)(&(pItem->szErrorMsg)),"ͼ�㱻�����û�����");		//������3:��Ŀ���ѱ������û�����
							continue;
						}
					}

					if (CopyFeatureRecordToDest(hConn,pItem->szLayerID,pItem->szUnitID))
					{
						if (pItem->nLockType == DP_UT_LOCKED)
						{
							if (!m_pWks->m_pDB->m_pRightMgr->LockUnitLayer(m_pWks->m_szID,pItem->szUnitID,pItem->szLayerID))
							{
								CopyStr((char**)(&(pItem->szErrorMsg)),"����ͼ��ʧ��");		//������4:����Ŀ��ʧ��
								continue;
							}
						}
						((CDpCheckInfo::_DpCheckOpreaInfo*)pItem)->bSuccess = true;
					}
					else
						CopyStr((char**)(&(pItem->szErrorMsg)),"δ֪����");		//������10:δ֪����

				}
			}
		}
	}

	//ȫ���ɹ���,�ύ����
	SQLEndTran(SQL_HANDLE_DBC,hConn,SQL_COMMIT);


Err:
	if (!bRet)				//�ع�����
		SQLEndTran(SQL_HANDLE_DBC,hConn,SQL_ROLLBACK);

	//�ڽ��������,�ٽ��Զ��ύ���ܴ�
	SQLSetConnectOption(hConn, SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_ON);

	if (hConn)		
	{
		SQLDisconnect(hConn);
		SQLFreeHandle(SQL_HANDLE_DBC,hConn);
	}

	if (hEnv)		SQLFreeHandle(SQL_HANDLE_ENV,hEnv);

	//�������ݿ�
	if (!EncryptDBFile(szFileName))
		bRet = false;

	return bRet;
}

















/*************************************************************************/
/*                          CHECK_IN������                       
/* ���ڹ���Դ�ļ����ݿ��е�����CHECK_IN�����ݿ��������
/*************************************************************************/
CDpCheckInMgr::CDpCheckInMgr(CDpWorkspace* pSourceWks,CDpWorkspace* pDestWks)
{
	m_pSourceWks		= pSourceWks;
	m_pDestWks			= pDestWks;
	m_pSourceDB			= NULL;
}

//���Դ�������ڹ����ʱ��û�д�,��ô�ͱ����ڴ˴���
BOOL CDpCheckInMgr::OpenSourceWks(const char* szDBFile,const char* szWksID)
{
	if (!szDBFile || strlen(szDBFile) < 2)
		return false;

	m_pSourceDB			= new CDpDatabase("00001");
	if (!m_pSourceDB->ConnectAccess(m_pDestWks->m_pDB->m_szUserID,m_pDestWks->m_pDB->m_szPwd,
		                       szDBFile))
		return false;

	if (szWksID)
		return m_pSourceDB->GetWksByID(szWksID,&m_pSourceWks); 
	else
	{
		CDpStringArray	IdArray;
		CDpStringArray	NameArray;
		if (m_pSourceDB->GetWksIDAndName(IdArray,NameArray))
			return m_pSourceDB->GetWksByID(IdArray.GetItem(0),&m_pSourceWks);
		else
			return false;
	}
	return false;
}


CDpCheckInMgr::~CDpCheckInMgr()
{
	if (m_pSourceDB)
	{
		if (m_pSourceWks)
		{
			delete m_pSourceWks;
			m_pSourceWks = NULL;
		}
		delete m_pSourceDB;
		m_pSourceDB = NULL;
	}

}


//ִ��CHECK����
BOOL CDpCheckInMgr::CheckIn(CDpCheckInfo* pInfo,PWORKPROCESS pFun,_DPWP_Param* pParam)
{
	if (!m_pDestWks || !m_pSourceWks || !pInfo || pInfo->GetCheckType() != DP_CHECKIN)
		return false;
	
	int		iCount	= pInfo->GetItemCount();
	if (iCount <= 0)
		return false;

	//�ڿ�ʼCHECKIN����֮ǰ,�ȿ�ʼ����
	m_pDestWks->m_pDB->BeginTrans();

	for (int i = 0; i < iCount; i++)
	{
		double		dTmp		= ((double)i) / ((double)iCount) * 100;
		if (pFun && pParam)
		{
			pParam->iPrecent	= (int)dTmp;
			(*pFun)(pParam);
			if (pParam->bIsBreak)
				return false;
		}


		const CDpCheckInfo::_DpCheckOpreaInfo* pItem = NULL;
		pInfo->GetItem(i,&pItem);
		if (pItem)
		{
			if (pItem->iDataType == 2)					//IMAGE
			{
				DP_RIGHT		nRight = DP_RIGHT_NOTRIGHT;
				m_pDestWks->m_pDB->m_pRightMgr->GetImageXmlRight(pItem->szUnitID,
					           pItem->szLayerID,&nRight);
				if (nRight < DP_RIGHT_READWRITE)
				{	
					CopyStr((char**)(&(pItem->szErrorMsg)),"û��д��Ȩ��");		//������2:û��д��
					continue;
				}	
				
				char			szLocker[100];			//����������
				memset(szLocker,0,sizeof(szLocker));
				m_pDestWks->m_pDB->m_pRightMgr->GetImageXmlLockerName(m_pSourceWks->m_szID,pItem->szUnitID,
																	  pItem->szLayerID,szLocker);
				//��������˲��ǵ�ǰ�û�,��������CHECKIN����
				if (_stricmp(szLocker,m_pDestWks->m_pDB->m_szUserID) != 0)
				{
					CopyStr((char**)(&(pItem->szErrorMsg)),"û������ͼ�����ͼ�㱻��������");		//������2:û��д��
					continue;
				}
				
				if (CopyImageXmlDataToDest(pItem->szUnitID,pItem->szLayerID))
				{
					if (pItem->nLockType == DP_UT_UNLOCK)			//����
						m_pDestWks->m_pDB->m_pRightMgr->LockImageXml(m_pSourceWks->m_szID,pItem->szUnitID,
							                              pItem->szLayerID,false);

					((CDpCheckInfo::_DpCheckOpreaInfo*)pItem)->bSuccess = true;
				}

			}
			else										//UNIT
			{

				DP_RIGHT		nRight = DP_RIGHT_NOTRIGHT;
				m_pDestWks->m_pDB->m_pRightMgr->GetUnitLayerRight(pItem->szUnitID,
					           pItem->szLayerID,&nRight);
				if (nRight < DP_RIGHT_READWRITE)
				{	
					CopyStr((char**)(&(pItem->szErrorMsg)),"û��д��Ȩ��");		//������2:û��д��
					continue;
				}	
				
				char			szLocker[100];			//����������
				memset(szLocker,0,sizeof(szLocker));
				m_pDestWks->m_pDB->m_pRightMgr->GetUnitLayerLockerName(m_pSourceWks->m_szID,pItem->szUnitID,
																	  pItem->szLayerID,szLocker);
				//��������˲��ǵ�ǰ�û�,��������CHECKIN����
				if (_stricmp(szLocker,m_pDestWks->m_pDB->m_szUserID) != 0)
								{
									CopyStr((char**)(&(pItem->szErrorMsg)),"û������ͼ�����ͼ�㱻��������");		//������2:û��д��
									continue;
							}

				//�������ݿ���ͼ����ID��Ŀ�����ݿ��е�ͼ��ID��ͬ,����CHECKIN
				CDpStringArray stringArray;
				m_pSourceWks->GetUnitsID(stringArray);
				BOOL	IsUnitExist = FALSE;
				for(int i = 0 ; i < stringArray.GetCount(); i++)
				{
					if(!strcmp( stringArray.GetItem(i),pItem->szUnitID ))
					{
						IsUnitExist = TRUE;
						break;
					}
				}

				if(!IsUnitExist)
				{
					CopyStr((char**)(&(pItem->szErrorMsg)),"��Ŀ�����ݿ��е�ͼ�����Ҳ����������ݿ���ͼ����ID");
					m_pDestWks->m_pDB->Rollback();
					return FALSE;	
				}

				
				if (CopyUnitLayerDataToDest(pItem->szUnitID,pItem->szLayerID))
				{
					if (pItem->nLockType == DP_UT_UNLOCK)			//����
						m_pDestWks->m_pDB->m_pRightMgr->LockUnitLayer(m_pSourceWks->m_szID,pItem->szUnitID,
												                              pItem->szLayerID,false);
					

					((CDpCheckInfo::_DpCheckOpreaInfo*)pItem)->bSuccess = true;
				}

			}

		}

	}


	m_pDestWks->m_pDB->CommitTrans();

	return true;
}


//����Ӱ��XML����
BOOL CDpCheckInMgr::CopyImageXmlDataToDest(const char* szImageID,const char* szXmlTagName)
{
	if (!szImageID || strlen(szImageID) != IMAGE_ID_LEN || !szXmlTagName)
		return false;

	//��ɾ��ԭ�м�¼
	char		szSql[256];
	char		szWhere[200];
	sprintf(szWhere," Where IMAGE_ID = '%s' AND XML_TAGNAME = '%s'",
		    szImageID,szXmlTagName);

	sprintf(szSql,"Delete From DP_IMAGEMGR_XML %s",szWhere);
	m_pDestWks->m_pDB->ExecuteSQL(szSql);

	int		iBlockSize	= DEFAULT_BINARY_BLOCK_SIZE*2;		
	
	return BulkCopyRecordToDest(m_pSourceWks->m_pDB->GetDBHandle(),m_pDestWks->m_pDB->GetDBHandle(),
		                        1,"DP_IMAGEMGR_XML",szWhere,iBlockSize);
}


//����ͼ��ͼ������
BOOL CDpCheckInMgr::CopyUnitLayerDataToDest(const char* szUnitID,const char* szLayerID)
{
	if (!szUnitID || !szLayerID || strlen(szLayerID) != LAYER_ID_LEN)
		return false;

	char		szSpaTableName[100];
	char		szAttrTableName[100];
	sprintf(szSpaTableName,"S%s",szLayerID);
	sprintf(szAttrTableName,"A%s",szLayerID);

	char		szWhere[1024];
	char		szWhereAttr[1024];

	sprintf(szWhere," Where ST_GRID_ID = '%s'",szUnitID);
	sprintf(szWhereAttr," Where ATT_GRID_ID = '%s'",szUnitID);

	int		iShapeBlockSize	= DEFAULT_BINARY_BLOCK_SIZE*4;		//Ϊ�˲��Լ���*4
	char	szSql[2048];

	//��ɾ��Ŀ�����ݿ���ԭ�еĿռ����ݺ���������
	//ɾ����������
	sprintf(szSql,"Delete From %s %s",szAttrTableName,szWhereAttr);
	m_pDestWks->m_pDB->ExecuteSQL(szSql);
	//ɾ���ռ�����
	sprintf(szSql,"Delete From %s %s",szSpaTableName,szWhere);
	m_pDestWks->m_pDB->ExecuteSQL(szSql);

	//��ʼ��������
	if (BulkCopyRecordToDest(m_pSourceWks->m_pDB->GetDBHandle(),m_pDestWks->m_pDB->GetDBHandle(),50,
		                     szSpaTableName,szWhere,iShapeBlockSize) && 
		BulkCopyRecordToDest(m_pSourceWks->m_pDB->GetDBHandle(),m_pDestWks->m_pDB->GetDBHandle(),50,
		                     szAttrTableName,szWhereAttr,iShapeBlockSize))
	{
		return true;
	}
	else
		return false;
}





/********************************************************/
/*                �����ֶ�ά���������                  */
/********************************************************/
CDpAttrWHMgr::CDpAttrWHMgr(CDpDatabase* pDB)
{
	m_pDB	= pDB;
}

CDpAttrWHMgr::~CDpAttrWHMgr()
{
	FreeMem();
}

//�����ݿ��м�������,�����Ҫ���ع��������еĲ�ͬ�ֶ���Ϣ,��pWks����Ϊ��
//�������Ҫ���ع������������ֶ���Ϣ,��pWks����ΪNULL
BOOL CDpAttrWHMgr::LoadDataFromDB(CDpWorkspace* pWks)
{
	//�ȼ�������Ϣ
	char			szSql[256];
	sprintf(szSql,"Select * From DP_ATTR_GROUP_WH");

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var1;
			CDpDBVariant	var2;
			
			if (rst.GetFieldValue(1,var1) && 
				rst.GetFieldValue(2,var2))
			{
				_DpAttrWHGroup*		pGroup	= new _DpAttrWHGroup;
				pGroup->szAttrGroupName		= NULL;
				pGroup->szAttrGroupDesc		= NULL;
				CopyStr(&(pGroup->szAttrGroupName),(LPCTSTR)var1);
				CopyStr(&(pGroup->szAttrGroupDesc),(LPCTSTR)var2);
				
				m_GroupArray.push_back(pGroup);

				char			szSql[256];
				CDpRecordset	rstCount(m_pDB);

				//ȡ���ڵ��ֶ���Ϣ

				sprintf(szSql,"Select * From DP_ATTR_WH Where ATTR_GROUP_NAME = '%s'",
						pGroup->szAttrGroupName);

				if (rstCount.Open(szSql))
				{
					while (rstCount.Next())
					{
						CDpDBVariant		varItem2;
						CDpDBVariant		varItem3;
						CDpDBVariant		varItem4;
						CDpDBVariant		varItem5;
						CDpDBVariant		varItem6;
						CDpDBVariant		varItem7;
						CDpDBVariant		varItem8;
						
						if (rstCount.GetFieldValue(2,varItem2) && 
							rstCount.GetFieldValue(3,varItem3) && 
							rstCount.GetFieldValue(4,varItem4) && 
							rstCount.GetFieldValue(5,varItem5) && 
							rstCount.GetFieldValue(6,varItem6) && 
							rstCount.GetFieldValue(7,varItem7) && 
							rstCount.GetFieldValue(8,varItem8))
						{
							CDpCustomFieldInfo*	pInfo	= new CDpCustomFieldInfo;
							memset(pInfo,0,sizeof(CDpCustomFieldInfo));
							pGroup->nFieldArray.push_back(pInfo);

							//����
							strcpy(pInfo->m_strName,(LPCTSTR)varItem2);
							//����
							strcpy(pInfo->m_szAliasName,(LPCTSTR)varItem3);
							//����
							pInfo->m_CustomFieldType	= DpCustomFieldType((int)varItem4);
							//�ֶγ���
							pInfo->m_iSize				= (int)varItem5;
							//�ֶξ���
							pInfo->m_nPrecision			= (int)varItem6;
							//�Ƿ�����Ϊ��
							pInfo->m_bAllowNull			= (((int)varItem7) == 0);
							//�Ƿ���UNIQUE�ֶ�
							pInfo->m_bIsUnique			= (((int)varItem8) == 1);

						}

					}

					rstCount.Close();


				}

			}
		}	   

		NULL;

		rst.Close();
	}


	//ȡ���������е��ֶε��б�
	if (pWks)
	{
		_DpAttrWHGroup*		pGroup	= new _DpAttrWHGroup;
		CopyStr(&(pGroup->szAttrGroupName),"�����������ֶ�");
		CopyStr(&(pGroup->szAttrGroupDesc),"�����������ֶ�");
		
		m_GroupArray.push_back(pGroup);

		char		szQuery[512];
		char		szWksID[128];
		memset(szQuery,0,sizeof(szQuery));
		memset(szWksID,0,sizeof(szWksID));

		if (m_pDB->GetDBType() == DP_ACCESS)
			sprintf(szWksID,"MID(ATTR_TABLE_NAME,2,14) = '%s'",pWks->m_szID);
		else if (m_pDB->GetDBType() == DP_ORACLE)
			sprintf(szWksID,"SUBSTR(ATTR_TABLE_NAME,2,14) = '%s'",pWks->m_szID);
		else if (m_pDB->GetDBType() == DP_SQLSERVER)
			sprintf(szWksID,"SUBSTRING(ATTR_TABLE_NAME,2,14) = '%s'",pWks->m_szID);
	

		sprintf(szQuery,"Select Distinct FIELD_NAME,FIELD_ALIAS,FIELD_TYPE,FIELD_LENGTH,FIELD_PRECISION,FIELD_NOTNULLABLE,FIELD_ISUNIQUE From DP_ATTR_DICTIONARY where %s AND PARENT_CLASS = 0",szWksID);
		if (rst.Open(szQuery))
		{

			while (rst.Next())
			{
				CDpDBVariant		varItem1;
				CDpDBVariant		varItem2;
				CDpDBVariant		varItem3;
				CDpDBVariant		varItem4;
				CDpDBVariant		varItem5;
				CDpDBVariant		varItem6;
				CDpDBVariant		varItem7;
	
				if (rst.GetFieldValue(1,varItem1) && 
					rst.GetFieldValue(2,varItem2) && 
					rst.GetFieldValue(3,varItem3) && 
					rst.GetFieldValue(4,varItem4) && 
					rst.GetFieldValue(5,varItem5) && 
					rst.GetFieldValue(6,varItem6) && 
					rst.GetFieldValue(7,varItem7))
				{
					CDpCustomFieldInfo*	pInfo	= new CDpCustomFieldInfo;
					memset(pInfo,0,sizeof(CDpCustomFieldInfo));
					pGroup->nFieldArray.push_back(pInfo);
					
					//����
					strcpy(pInfo->m_strName,(LPCTSTR)varItem1);
					//����
					strcpy(pInfo->m_szAliasName,(LPCTSTR)varItem2);
					//����
					pInfo->m_CustomFieldType	= DpCustomFieldType((int)varItem3);
					//�ֶγ���
					pInfo->m_iSize				= (int)varItem4;
					//�ֶξ���
					pInfo->m_nPrecision			= (int)varItem5;
					//�Ƿ�����Ϊ��
					pInfo->m_bAllowNull			= (((int)varItem6) == 0);
					//�Ƿ���UNIQUE�ֶ�
					pInfo->m_bIsUnique			= (((int)varItem7) == 1);

				}
			}				
			rst.Close();
		}


	}



	return true;

}



//�ͷ��ڴ�
void CDpAttrWHMgr::FreeMem()
{
	int		iCount  = m_GroupArray.size();

	for (int i = 0; i < iCount; i++)
	{
		_DpAttrWHGroup*	pGroup	= m_GroupArray[i];
		if (pGroup)
		{
			if (pGroup->szAttrGroupName)
			{
				delete [] pGroup->szAttrGroupName;
				pGroup->szAttrGroupName = NULL;
			}
			if (pGroup->szAttrGroupDesc)		
			{
				delete [] pGroup->szAttrGroupDesc;
				pGroup->szAttrGroupDesc = NULL;
			}

			int		iItemCount	= pGroup->nFieldArray.size();
			for (int j = 0; j < iItemCount; j++)
			{
				CDpCustomFieldInfo*	pInfo = pGroup->nFieldArray[j];
				if (pInfo)
				{
					delete pInfo;
					pInfo = NULL;
				}
			}
			pGroup->nFieldArray.clear();

			delete pGroup;
			pGroup = NULL;
		}
	}
	
	m_GroupArray.clear();
}


//ȡ��ĸ���
int CDpAttrWHMgr::GetGroupCount()
{
	return m_GroupArray.size();
}

//ȡ�������
const CDpAttrWHMgr::_DpAttrWHGroup* CDpAttrWHMgr::GetGruop(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_GroupArray.size())
		return NULL;
	return m_GroupArray[iIndex];
}

//ȡ���ڵ��ֶ���Ŀ
int	CDpAttrWHMgr::GetFieldItemCount(const _DpAttrWHGroup* pGroup)
{
	if (!pGroup)
		return 0;
	return pGroup->nFieldArray.size();
}



//ȡ���ھ�����ֶ���Ϣ
const CDpCustomFieldInfo* CDpAttrWHMgr::GetFieldInfo(const CDpAttrWHMgr::_DpAttrWHGroup* pGroup,
																   int iIndex)
{
	if (!pGroup || iIndex < 0 || iIndex >= pGroup->nFieldArray.size())
		return NULL;
	return (pGroup->nFieldArray[iIndex]);
}





/********************************************************/
/*                ��Ĺ������Թ������                  */
/********************************************************/
CDpFixedClassAttrMgr::CDpFixedClassAttrMgr(CDpDatabase* pDB)
{
	m_pDB	= pDB;
}

CDpFixedClassAttrMgr::~CDpFixedClassAttrMgr()
{
	FreeMem();
}

void CDpFixedClassAttrMgr::FreeMem()
{
	int		iCount = m_nClassArray.size();

	for (int i = 0; i < iCount; i++)
	{
		_DpFixedClass*	pClass = m_nClassArray[i];
		if (pClass)
		{
			int		iFieldCount	= pClass->nFieldArray.size();

			for (int j = 0; j < iFieldCount; j++)
			{
				CDpCustomFieldInfo*	pInfo = pClass->nFieldArray[j];
				if (pInfo)	
				{
					delete pInfo;
					pInfo = NULL;
				}
			}

			pClass->nFieldArray.clear();


			if (pClass->szClassName)		
			{
				delete [] pClass->szClassName;
				pClass->szClassName = NULL;
			}
			if (pClass->szDescName)		
			{
				delete [] pClass->szDescName;
				pClass->szDescName = NULL;
			}
			delete pClass;
			pClass = NULL;
		}
	}
}

int	CDpFixedClassAttrMgr::GetClassCount()
{
	return m_nClassArray.size();
}

const CDpFixedClassAttrMgr::_DpFixedClass* CDpFixedClassAttrMgr::GetClassByIndex(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_nClassArray.size())
		return NULL;

	return m_nClassArray[iIndex];
}


const CDpFixedClassAttrMgr::_DpFixedClass* CDpFixedClassAttrMgr::GetClassByID(int iClassID)
{
	if (iClassID <= 0)
		return NULL;

	int		iCount	= m_nClassArray.size();

	for (int i = 0; i < iCount; i++)
	{
		_DpFixedClass*	pClass = m_nClassArray[i];
		if (pClass->iClassID == iClassID)
			return (const _DpFixedClass*)pClass;
	}
	return NULL;
}


BOOL CDpFixedClassAttrMgr::LoadDataFromDB()
{
	if (!m_pDB)
		return false;

	char	szSql[256];
	sprintf(szSql,"Select * From DP_FIXED_CLASS_WH ORDER BY CLASSID");

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			CDpDBVariant	var;
			CDpDBVariant	var1;
			CDpDBVariant	var2;
			CDpDBVariant	var3;
			CDpDBVariant	var4;

			if (rst.GetFieldValue(1,var) && 
				rst.GetFieldValue(2,var1) && 
				rst.GetFieldValue(3,var2) && 
				rst.GetFieldValue(4,var3) && 
				rst.GetFieldValue(5,var4))
			{
				_DpFixedClass*		pClass	= new _DpFixedClass;
				pClass->iClassID			= (int)var;
				CopyStr(&(pClass->szClassName),(LPCTSTR)var1);
				CopyStr(&(pClass->szDescName),(LPCTSTR)var2);
				pClass->iClassType			= (int)var3;
				pClass->iParentClassID		= (int)var4;	

				m_nClassArray.push_back(pClass);
			}
		}

		rst.Close();
	}


	for (int i = 0; i < m_nClassArray.size(); i++)
	{
		_DpFixedClass*	pClass = m_nClassArray[i];
		if (!pClass)
			continue;
		sprintf(szSql,"Select * From DP_FIXED_CLASS_ATTR_WH Where CLASSID = %d",
				pClass->iClassID);

		if (rst.Open(szSql))
		{
			while (rst.Next())
			{
				CDpDBVariant		varItem1;
				CDpDBVariant		varItem2;
				CDpDBVariant		varItem3;
				CDpDBVariant		varItem4;
				CDpDBVariant		varItem5;
				CDpDBVariant		varItem6;
				CDpDBVariant		varItem7;
				CDpDBVariant		varItem8;
				
				if (rst.GetFieldValue(1,varItem1) && 
					rst.GetFieldValue(2,varItem2) && 
					rst.GetFieldValue(3,varItem3) && 
					rst.GetFieldValue(4,varItem4) && 
					rst.GetFieldValue(5,varItem5) && 
					rst.GetFieldValue(6,varItem6) && 
					rst.GetFieldValue(7,varItem7) && 
					rst.GetFieldValue(8,varItem8))
				{
					CDpCustomFieldInfo*	pInfo	= new CDpCustomFieldInfo;
					memset(pInfo,0,sizeof(CDpCustomFieldInfo));
					pClass->nFieldArray.push_back(pInfo);

					//Class ID
					pInfo->m_iParent			= (int)varItem1;
					//����
					strcpy(pInfo->m_strName,(LPCTSTR)varItem2);
					//����
					strcpy(pInfo->m_szAliasName,(LPCTSTR)varItem3);
					//����
					pInfo->m_CustomFieldType	= DpCustomFieldType((int)varItem4);
					//�ֶγ���
					pInfo->m_iSize				= (int)varItem5;
					//�ֶξ���
					pInfo->m_nPrecision			= (int)varItem6;
					//�Ƿ�����Ϊ��
					pInfo->m_bAllowNull			= (((int)varItem7) == 0);
					//�Ƿ���UNIQUE�ֶ�
					pInfo->m_bIsUnique			= (((int)varItem8) == 1);
				}
			}

			rst.Close();

		}


	}
	
	return true;
}


//��ӱ�ǩ,ͬʱ�����XML������ ,HOUKUI,06.6.21
//(const char* szTagName,BYTE** buff,int* piBuffLen)
BOOL CDpWorkspace::SetXMLData(const char* szTagName,BYTE* buff,int iBuffLen)
{
	if (!szTagName || strlen(szTagName) <= 0 || NULL==buff || 0>=iBuffLen )
		return false;

	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;

	BOOL bRet	= false;
	char	szSql[256];	
	
	//���ж���û�����øñ�ǩ��Ȩ��
	if(m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return FALSE;//Ȩ�޿��ƣ�����Ҫ����Ա�����

	
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"DELETE  DP_Workspace_XML WHERE WORKSPACE_ID='%s' AND XML_TAGNAME='%s'",m_szID,szTagName);//��ɾ���ظ���¼ 	
	m_pDB->BeginTrans();			//��ʼ����	
	
	bRet = m_pDB->ExecuteSQL(szSql);	
	if(!buff)
		return bRet;

	//�����µļ�¼
	memset(szSql,0,sizeof(szSql));
	if (buff && iBuffLen > 0)
	{
		sprintf(szSql,"Insert Into DP_Workspace_XML Values('%s','%s',?)",m_szID,szTagName);
		bRet = SetLargerFieldValue(m_pDB->GetDBHandle(),szSql,buff,iBuffLen);		
	}
	else
	{
		sprintf(szSql,"Insert Into DP_Workspace_XML Values('%s','%s',NULL)",m_szID,szTagName);
		bRet = (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
	}


	if (bRet)
		m_pDB->CommitTrans();
	else
		m_pDB->Rollback();

	return bRet;
}

/*
//�ڹ�������ɾ��XML����
BOOL CDpWorkspace::DelXMLData(const char *szTagName)
{
	if (!szTagName || strlen(szTagName) <= 0)
		return false;
	
	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;
	
	BOOL bRet	= false;
	char	szSql[256];	
	
	//���ж���û��ɾ���ñ�ǩ��Ȩ��
	if(m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return FALSE;//Ȩ�޿��ƣ�����Ҫ����Ա�����	
	
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"DELETE  DP_Workspace_XML WHERE WORKSPACE_ID='%s' AND XML_TAGNAME='%s'",m_szID,szTagName);//ɾ���ظ���¼ 	
	m_pDB->BeginTrans();			//��ʼ����	
	
	bRet = m_pDB->ExecuteSQL(szSql);	
	if (bRet)
		m_pDB->CommitTrans();
	else
		m_pDB->Rollback();
	
	return bRet;	
}*/



//�õ��������е�XML�����ݣ�houkui,06.6.19
//����˵��: [in] ppBuff		-- ����XML���ĵ�����,�ڴ���Ҫ�������ͷ�
//          [out]piBuffSize -- ����XML�����ݵĳ���
BOOL CDpWorkspace::GetXMLData(const char* szTagName,BYTE** ppBuff,int* piBuffSize)
{
	if (!szTagName || strlen(szTagName) <= 0 )
		return false;
	
	if (!ppBuff || !piBuffSize)
		return false;

	*piBuffSize		= 0;

	CDpDBVariant		var;
	if (GetXMLData(szTagName,var))
	{
		if (var.m_dwType == DPDBVT_BINARY && var.m_pLongBinary->m_dwSize > 0)  	
		{
			*piBuffSize = var.m_pLongBinary->m_dwSize;
			*ppBuff = new BYTE[*piBuffSize];
			memcpy(*ppBuff,var.m_pLongBinary->GetBuffer(),*piBuffSize);
		}
		return true;
	}
	else
		return false;
}



/*�����ܼƶ�������¼��houkui,06.6.19*/
int CDpDatabase::GetDataServerCount()
{
	int         ret;
	char		szSql[256];	
	ret=0;
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"select count(*) from DP_DATASERVERINFO");
	CDpRecordset	rst(this);
	CDpDBVariant	var;
	if (rst.Open(szSql))
	{
		while(rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
				ret		= (int)var;
			//ret++;
		}	
	}
	return ret;
}

/*�����ݿ���ȡ�����м�¼���ṹ�������У�houkui,06.6.19*/
BOOL CDpDatabase::GetDataServer(CDpStringArray& szName,CDpStringArray& szAddr,CDpStringArray& szDesc)
{
	if(NULL==&szName||NULL==&szAddr||NULL==&szDesc)
		return FALSE;

	
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"select SERVER_NAME,SERVER_ADDR,SERVER_DESC from DP_DATASERVERINFO");

	CDpRecordset	rst(this);
	if (rst.Open(szSql))
	{
		CDpDBVariant	var1;
		CDpDBVariant	var2;
		CDpDBVariant	var3;
		var1.Clear();
		var2.Clear();
		var3.Clear();
		szName.DelAllItem();
		szAddr.DelAllItem();
		szDesc.DelAllItem();

		for(int i=0;rst.Next();i++)
		{
			if (rst.GetFieldValue(1,var1)&&rst.GetFieldValue(2,var2)&&rst.GetFieldValue(3,var3))
			{
				szName.AddString((char*)((LPCTSTR)var1));
				szAddr.AddString((char*)((LPCTSTR)var2));
				szDesc.AddString((char*)((LPCTSTR)var3));
			}
		}
	}	
	else
		return FALSE;
	return TRUE;
}

/*�����ݿ������һ����¼��houkui,06.6.19*/
BOOL CDpDatabase::CreateDataServer(char *szServerName, char *szServerAddr, char *szServerDesc)
{	
	if(NULL==szServerName||strlen(szServerName) <= 0
		||NULL==szServerAddr||strlen(szServerAddr) <= 0)
		return FALSE;//һ���Լ��
	
	if(m_pRightMgr->GetUserType() != DP_ADMIN)
		return FALSE;//Ȩ�޿��ƣ�����Ҫ����Ա�����

	BeginTrans();
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"INSERT INTO DP_DATASERVERINFO VALUES('%s','%s','%s')",szServerName,szServerAddr,szServerDesc);
	if(ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{	
		CommitTrans();
		return TRUE;
	}
	else
	{
		Rollback();
		return FALSE;
	}
}

/*ɾ�����м�¼��houkui,06.6.19*/
BOOL CDpDatabase::RemoveAllDataServer()
{

	if(m_pRightMgr->GetUserType() != DP_ADMIN)
		return FALSE;	//Ȩ�޿��ƣ�����Ҫ����Ա�����

	BeginTrans();
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"DELETE from DP_DATASERVERINFO");
	if(ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{	
		CommitTrans();
		return TRUE;
	}
	else
	{
		Rollback();
		return FALSE;
	}
	
}

/*ɾ��ָ���ļ�¼��houkui,06.6.19*/
BOOL CDpDatabase::DeleteDataServer(char *szServerName)
{
	
	if(NULL==szServerName||strlen(szServerName) <= 0)
		return FALSE;//һ���Լ��

	
	if(m_pRightMgr->GetUserType() != DP_ADMIN)
		return FALSE;//Ȩ�޿��ƣ�����Ҫ����Ա�����

	BeginTrans();
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"DELETE FROM DP_DATASERVERINFO WHERE SERVER_NAME='%s'",szServerName);
	if(ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{	
		CommitTrans();
		return TRUE;
	}
	else
	{
		Rollback();
		return FALSE;
	}
	
}

//Ԥ��һ���ӿڣ��Ժ��޸�,houkui,06.6.19
//�����������ַ�����������Ա�����У��ٸ��µ����ݿ��С�
BOOL CDpWorkspace::SetServerName(char *szServerName)
{
	if (szServerName && strlen(szServerName) > 0)
		strcpy(m_szServerName,szServerName);

	if (m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;//��Ҫ����ԱȨ��

	char		szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_WorkspaceMgr SET ServerName = '%s' Where WORKSPACE_ID = '%s'",m_szServerName,m_szID);

	return (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}

//Ԥ��һ���ӿڣ��Ժ��޸�,houkui,06.6.19
//�����ݿ��еķ��������ַ�����������Ա�����С�
BOOL CDpWorkspace::GetServerName(CDpDBVariant& var)
{
	char		szSql[512];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select ServerName From DP_WorkspaceMgr Where WORKSPACE_ID = '%s'",m_szID);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
				strcpy(m_szServerName,var.m_pString);
		}
		rst.Close();
		return TRUE;
	}
	return FALSE;
}


//houkui,06.6.21,����û��Զ����XML������
//(const char* szTagName,BYTE** buff,int* piBuffLen)
BOOL CDpWorkspace::GetXMLData(const char* szTagName,CDpDBVariant &var)
{
	if (!szTagName || strlen(szTagName) <= 0 )
		return false;
	char	szSql[1024];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select XML_DATA  From DP_Workspace_XML Where WORKSPACE_ID = '%s' AND XML_TAGNAME = '%s'" ,m_szID,szTagName);

	CDpRecordset		rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			if (rst.GetFieldValue(1,var))
			{	
				return true;
			}
		}	
	}	
	return FALSE;
}


//�޸�m_szLocalName,houkui,06.6.21
BOOL CDpUnit::SetLocalName(const char *szLocalName)
{
	if(1==m_pWks->GetWksVersion())
		return FALSE;

	if (szLocalName && strlen(szLocalName) > 0)
		strcpy(m_szLocalName,szLocalName);

	if (m_pWks->m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return false;//��Ҫ����ԱȨ��

	char		szSql[200];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_GridInWks SET LocalName  = '%s' Where WORKSPACE_ID = '%s'",m_szLocalName,m_szID);

	return (m_pWks->m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
}

/*
int CDpUnit::GetUnitCount(enum DpAPartType unitType)
{

	return 1;
}
*/


//���������������͵Ĳ�����houkui,06.6.27
BOOL CDpWorkspace::SetMetricType(UINT iMetricType)
{
	if (0==iMetricType)
		return false;
	
	//���ж���û�����øò������͵�Ȩ��
	if(m_pDB->m_pRightMgr->GetUserType() != DP_ADMIN)
		return FALSE;

	BOOL bRet	= false;
	char	szSql[256];		
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"update DP_WorkspaceMgr set METRICTYPE=%d where WORKSPACE_ID='%s'",iMetricType,m_szID);//���¼�¼ 	
	m_pDB->BeginTrans();		//��ʼ����	
	bRet = (m_pDB->ExecuteSQL(szSql) == DP_SQL_SUCCESS);
	if (bRet)
	{
		m_pDB->CommitTrans();
		m_iMetricType = iMetricType;
	}
	
	else
		m_pDB->Rollback();

	return bRet;
}

UINT CDpWorkspace::GetMetricType()
{
	char			szSql[256];		

	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Select METRICTYPE From DP_WorkspaceMgr where WORKSPACE_ID = '%s'",m_szID);

	CDpRecordset	rst(m_pDB);
	if (rst.Open(szSql))
	{
		if (rst.Next())
		{
			CDpDBVariant	var;
			if (rst.GetFieldValue(1,var))
			{
				m_iMetricType = (int)var;
				return m_iMetricType;
			}
		}
	}

	return m_iMetricType;

}

//����ͼ��������һ����Χ�ڵ�ͼ��,�����ṩ����ǰ�İ汾�ĳ���ʹ��.
//���ݿռ䷶Χȡ����������ͼ����ID
//szIDArray������ż����Ľ��
//rtBoundΪҪ�����ķ�Χ,Ϊ�����ʾ�����ǿռ�λ��
//bIntersect��ʾ��������,
//ΪTRUE��ʾֻҪ���ཻ��ͼ���͵õ�,ΪFALSE���ʾ��Ҫ��ȫ�����ŵõ�
//bIsLocked��ʾ�Ƿ񷵻ر�������ͼ��,ΪTRUE��ʾ����

BOOL CDpWorkspace::GetUnitsID(CDpStringArray& szIDArray,
							  DpAPartType Type,
							  CDpRect* pRect ,
							  BOOL bIntersect,
							  DP_LOCK_TYPE UnitType)
							 
{
	char		szSql[512];						//sql��ѯ���
	memset(szSql,0,sizeof(szSql));

	if(Type	!=	DP_A_UNIT && Type	!=	DP_A_STEREO  && Type	!=	DP_A_NULL  )
		return FALSE;

	int			apartType;
	apartType=int(Type);

	//������SQL���
	if(2 == m_iVersion ) //����Ŀǰ�汾�Ĺ�����
	{
		if(Type	!= DP_A_NULL) //����ģ��ID��ͼ��ID
		{
			if (UnitType != DP_UT_ANY)				//���Ƿ������е�ͼ��				
				sprintf(szSql,"Select GRID_ID,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY From DP_GridInWks Where WORKSPACE_ID = '%s' AND ISLOCK = %d AND TYPE= %d",m_szID,UnitType == DP_UT_UNLOCK?1:2,apartType);
			else
				sprintf(szSql,"Select GRID_ID,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY From DP_GridInWks Where WORKSPACE_ID = '%s'  AND TYPE= %d ",m_szID,apartType);
		}
		else if(Type	==	DP_A_NULL)//����ģ��ID����ͼ��ID
		{
			if (UnitType != DP_UT_ANY)				//���Ƿ������е�ͼ��				
				sprintf(szSql,"Select GRID_ID,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY From DP_GridInWks Where WORKSPACE_ID = '%s' AND ISLOCK = %d ",m_szID,UnitType == DP_UT_UNLOCK?1:2);
			else
				sprintf(szSql,"Select GRID_ID,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY From DP_GridInWks Where WORKSPACE_ID = '%s'  ",m_szID);// AND TYPE= %d ",m_szID,apartType);
		}
		
	}
	else if(1 == m_iVersion || m_iVersion == 0 )//������ǰ�汾�Ĺ�����
	{		
		if (UnitType != DP_UT_ANY)				//���Ƿ������е�ͼ��				
			sprintf(szSql,"Select GRID_ID,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY From DP_GridInWks Where WORKSPACE_ID = '%s' AND ISLOCK = %d ",m_szID,UnitType == DP_UT_UNLOCK?1:2);
		else
			sprintf(szSql,"Select GRID_ID,LTX,LTY,LBX,LBY,RBX,RBY,RTX,RTY From DP_GridInWks Where WORKSPACE_ID = '%s'   ",m_szID);
	}
	else
		return FALSE;

	//��ʼ�������ݿ�
	CDpRecordset	rst(m_pDB);

	CDpDBVariant	varID;

	CDpDBVariant	varLTX;
	CDpDBVariant	varLTY;
	CDpDBVariant	varLBX;
	CDpDBVariant	varLBY;

	CDpDBVariant	varRBX;
	CDpDBVariant	varRBY;
	CDpDBVariant	varRTX;
	CDpDBVariant	varRTY;

	if (rst.Open(szSql))
	{
		while (rst.Next())
		{
			if (rst.GetFieldValue(1,varID) && rst.GetFieldValue(2,varLTX) && 
				rst.GetFieldValue(3,varLTY) && rst.GetFieldValue(4,varLBX) && 
				rst.GetFieldValue(5,varLBY) && rst.GetFieldValue(6,varRBX) &&
				rst.GetFieldValue(7,varRBY) &&rst.GetFieldValue(8,varRTX) &&
				rst.GetFieldValue(9,varRTY)) 

			{
				CDpRgn			rgn;
				_DPRealPoint	d[4];

				d[0].x	= (double)varLBX;		d[0].y	= (double)varLBY;
				d[1].x	= (double)varRBX;		d[1].y	= (double)varRBY;
				d[2].x	= (double)varRTX;		d[2].y	= (double)varRTY;
				d[3].x	= (double)varLTX;		d[3].y	= (double)varLTY;
				rgn.CreatePolygonRgn(d,4);

				if (pRect)						//���пռ䷶Χ����
				{
					if (bIntersect)				//�ཻ
					{
						if (rgn.Intersect(pRect))
							szIDArray.AddString((char*)(LPCSTR)varID);
					}
					else						//����
					{
						if (pRect->PtInRect(d[0].x,d[0].y) &&
							pRect->PtInRect(d[1].x,d[1].y) &&
							pRect->PtInRect(d[2].x,d[2].y) &&
							pRect->PtInRect(d[3].x,d[3].y))
							szIDArray.AddString((char*)(LPCSTR)varID);
					}

				}
				else
					szIDArray.AddString((char*)(LPCSTR)varID);					//����������ID���뵽���������
			}

		}

		return true;
	}
	else
		return false;
}


BOOL CDpDatabase::ModifyDataServer(char *szServerName, char *szServerAddr, char *szServerDesc)
{
	
	if(NULL==szServerName||NULL==szServerAddr||NULL==szServerDesc)
		return FALSE;//һ���Լ��
	
	if(m_pRightMgr->GetUserType() != DP_ADMIN)
		return FALSE;//Ȩ�޿��ƣ�����Ҫ����Ա�����

	BeginTrans();
	char		szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"UPDATE DP_DATASERVERINFO SET SERVER_ADDR='%s',SERVER_DESC ='%s' WHERE SERVER_NAME='%s'",
		szServerAddr,szServerDesc,szServerName);
	if(ExecuteSQL(szSql) == DP_SQL_SUCCESS)
	{	
		CommitTrans();
		return TRUE;
	}
	else
	{
		Rollback();
		return FALSE;
	}
	

}

BOOL CDpImage::SetServer(const char *szServerName)//houkui,06.7.3
{
	char	szSql[256];
	memset(szSql,0,sizeof(szSql));
	sprintf(szSql,"Update DP_IMAGEMGR Set SERVER_NAME = '%s' Where IMAGE_ID = '%s'",szServerName,m_szID);	
	return (m_pWks->m_pDB->ExecuteSQL(szSql)== DP_SQL_SUCCESS);
}

//ͨ��C��ID��ö�Ӧ����չ����
BOOL CDpWBSCodeWH::GetCTypeEX(int cID,DP_LAYER_TYPE_EX* cTypeEx)
{ 
	if(NULL==cTypeEx)
		return FALSE;
	CDpWBSCPartWH* pC=GetCNode(cID);
	if(pC)
	{
		*cTypeEx=pC->C;
		return TRUE;
	}
	return FALSE;
}



UINT CDpWorkspace::GetWksVersion()
{
	return m_iVersion;
}



