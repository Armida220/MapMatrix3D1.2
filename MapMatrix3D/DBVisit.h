
/**************************************************************************************************
 This is a part of the Visiontek MapMatrix family.
 Copyright (C) 2005-2010 Visiontek Inc.
 All rights reserved.

 Module name:	���ݷ����м��ӿڶ���(GDMI)
 Author:		����ǿ
 Description:	���ڶԿռ����ݿ���з��ʵĽӿ�

**************************************************************************************************/

#ifndef _DBVISIT712304823904802398490238409238
#define _DBVISIT712304823904802398490238409238

#include <comdef.h>
#include "Scheme.h"

#pragma warning(push)
#pragma warning(disable : 4786)

#pragma warning (disable : 4201)
	#include <sql.h>
	#include <sqlext.h>
	#pragma comment(lib,"ODBC32.lib ") 
	#pragma message("Automatically linking with ODBC32.lib")

	#include <lm.h>
	#include <Winnetwk.h>

	#pragma comment(lib,"Netapi32.lib ") 
	#pragma message("Automatically linking with Netapi32.lib")

	#pragma comment(lib,"netapi32.lib") 
	#pragma message("Automatically linking with Advapi32.lib")

	#pragma comment(lib,"Mpr.lib") 
	#pragma message("Automatically linking with Mpr.lib")

	#ifdef __cplusplus
	extern "C" {
	#endif
		WINADVAPI
		BOOL
		WINAPI
		ConvertStringSecurityDescriptorToSecurityDescriptorA(
			IN  LPCSTR StringSecurityDescriptor,
			IN  DWORD StringSDRevision,
			OUT PSECURITY_DESCRIPTOR  *SecurityDescriptor,
			OUT PULONG  SecurityDescriptorSize OPTIONAL
			);
		WINADVAPI
		BOOL
		WINAPI
		ConvertStringSecurityDescriptorToSecurityDescriptorW(
			IN  LPCWSTR StringSecurityDescriptor,
			IN  DWORD StringSDRevision,
			OUT PSECURITY_DESCRIPTOR  *SecurityDescriptor,
			OUT PULONG  SecurityDescriptorSize OPTIONAL
			);
		#ifdef UNICODE
		#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorW
		#else
		#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorA
		#endif // !UNICODE

		WINADVAPI
		BOOL
		WINAPI
		ConvertSidToStringSidA(
			IN  PSID     Sid,
			OUT LPSTR  *StringSid
			);
		WINADVAPI
		BOOL
		WINAPI
		ConvertSidToStringSidW(
			IN  PSID     Sid,
			OUT LPWSTR  *StringSid
			);
		#ifdef UNICODE
		#define ConvertSidToStringSid  ConvertSidToStringSidW
		#else
		#define ConvertSidToStringSid  ConvertSidToStringSidA
		#endif // !UNICODE


	#ifdef __cplusplus
	}
	#endif

	#define SDDL_REVISION_1     1


	#pragma comment(lib,"Advapi32.lib") 
	#pragma message("Automatically linking with Advapi32.lib")

/*	typedef enum _SE_OBJECT_TYPE
	{
		SE_UNKNOWN_OBJECT_TYPE = 0,
		SE_FILE_OBJECT,
		SE_SERVICE,
		SE_PRINTER,
		SE_REGISTRY_KEY,
		SE_LMSHARE,
		SE_KERNEL_OBJECT,
		SE_WINDOW_OBJECT,
		SE_DS_OBJECT,
		SE_DS_OBJECT_ALL,
		SE_PROVIDER_DEFINED_OBJECT,
		SE_WMIGUID_OBJECT,
		SE_REGISTRY_WOW64_32KEY
	} SE_OBJECT_TYPE;*/



	#ifdef __cplusplus
	extern "C" {
	#endif

	WINADVAPI
	DWORD
	WINAPI
	SetNamedSecurityInfoA(
		IN LPSTR					pObjectName,
		IN SE_OBJECT_TYPE			ObjectType,
		IN SECURITY_INFORMATION		SecurityInfo,
		IN PSID						psidOwner,
		IN PSID						psidGroup,
		IN PACL						pDacl,
		IN PACL						pSacl
		);
	WINADVAPI
	DWORD
	WINAPI
	SetNamedSecurityInfoW(
		IN LPWSTR					pObjectName,
		IN SE_OBJECT_TYPE			ObjectType,
		IN SECURITY_INFORMATION		SecurityInfo,
		IN PSID						psidOwner,
		IN PSID						psidGroup,
		IN PACL						pDacl,
		IN PACL						pSacl
		);
	#ifdef UNICODE
	#define SetNamedSecurityInfo  SetNamedSecurityInfoW
	#else
	#define SetNamedSecurityInfo  SetNamedSecurityInfoA
	#endif // !UNICODE

	#ifdef __cplusplus
	}
	#endif


#pragma warning (default : 4201)

#define DPCLS_PERMANENT			0x00000
#define DPCLS_GEOMETRY			0x00001
#define DPCLS_GEOPOINT			0x01000
#define DPCLS_GEODIRPOINT		0x01100
#define DPCLS_GEOCURVE			0x02000
#define DPCLS_PARALLELCURVE		0x02100
#define DPCLS_GEOMULTIPOINT		0x02200
#define DPCLS_GEODEMPOINT		0x02300
#define DPCLS_GEOSURFACE		0x03000
#define DPCLS_GEOMULTISURFACE	0x03100
#define DPCLS_GEO3DM			0x04000
#define DPCLS_GEOTEXT			0x08000

#include <vector>
#include <map>					//ʹ��HASH��
//#include <algorithm>
using namespace std;

BOOL SetLargerFieldValue(SQLHANDLE hConn,char* szSql,BYTE* buff,int iBuffLen);

//��һ����ѯ��SQL����еõ�Ҫ��ѯ�ı���
BOOL GetTableNameFromSql(char* szSql,char* szTableName);

BOOL AddAccessRights(const TCHAR *lpszFileName, const TCHAR *lpszAccountName,DWORD dwAccessMask);

GUID GUIDFromString(LPCTSTR text);

CString GUIDToString(GUID id);

BOOL IsZeroGUID(GUID *id);

GUID NewGUID();

#define ZeroMem(a) memset(&(a),0,sizeof(a))

//���ж�����д���ʱ��,һ��ȱʡ�Ŀ�Ĵ�С
#define DEFAULT_WRITE_CHUNK		8192
//���ж����ƶ�ȡ��ʱ��,һ��ȱʡ�Ŀ�Ĵ�С
#define DEFAULT_READ_CHUNK		8192


//������ID��˳��ŵ����ֵ
#define MAX_WORKSPACE_ID		998
//����ID��˳��ŵ����ֵ
#define MAX_TASK_ID				99998
//ͼ��ID��˳��ŵ����ֵ
#define MAX_UINT_ID				9998
//ͼ��ID��˳��ŵ����ֵ
#define MAX_LAYER_ID			9998
//ʸ������OID�����ֵ
#define MAX_OID					0xFFFFFFF0

//������ID�ĳ���
#define WKS_ID_LEN				14
//ͼ��ID�ĳ���
#define UNIT_ID_LEN				19
//ͼ��ID�ĳ���		
#define LAYER_ID_LEN			19
//Ӱ��ID�ĳ���
#define IMAGE_ID_LEN			19
//����ID�ĳ���
#define TASK_ID_LEN				19

//ȱʡ�Ľ��ж��������ݽ���ʱ,�ڴ��Ĵ�С,ȱʡΪ512K,������һ������ĵ������ܳ���18700��
#define DEFAULT_BINARY_BLOCK_SIZE	524288


//ִ��SQL���ķ�����
enum DP_EXECSQL_RETURNCODE
{
	DP_SQL_SUCCESS		= 0,			//ִ�гɹ�
	DP_SQL_STAT_ERR		= 1,			//SQL������
	DP_SQL_NOT_SUPPORT	= 2,			//SQL��䲻֧��
	DP_SQL_DBRIGHT_ERR	= 3,			//�û�Ȩ�޲���(ָ���ݿ⼶��)
	DP_SQL_DPRIGHT_ERR	= 4				//�û�Ȩ�޲���(ָӦ�ü�)
};

//���ӵ����ݿ������
enum DP_DATABASE_TYPE
{
	DP_NULL				= 0,			//û�д����ݿ�
	DP_ACCESS			= 1,			//ACCESS���ݿ�
	DP_ORACLE			= 2,			//ORACLE���ݿ�
	DP_SQLSERVER		= 3
};

//����������
enum DP_WORKSPACE_TYPE
{
	DP_WKS_FULL			= 1,			//����������
	DP_WKS_PART			= 2,			//���ֹ�����
	DP_WKS_ANY			= 3,			//��������������͹�����
	DP_WKS_IMAGEPRO		= 4             //houkui,06.6.27,��ImagePro�ã�����ģ�����ݴ���
};

//ȡͼ���Ĺ�������
enum DP_GETUNIT_FILTER
{
	GETUNIT_Intersect	= 0x01,			//�ཻ
	GETUNIT_Include		= 0x02			//��ȫ����

};

//�ռ��ѯ�ĵȼ�
enum DP_SPATIAL_QUERY_LEVEL
{
	Level_Simple		= 0x01,			//�򵥲�ѯ,ֻ���ݵ����������ж�
	Level_Advance		= 0x02,			//���Ӳ�ѯ,���ݵ����ÿ���˵����ж�
};

//��ǰ�Ĺ�����������
enum DP_OBJECT_TYPE
{
	DP_OBJ_NULL			= 0,
	DP_OBJ_UNIT			= 1,			//ͼ��
	DP_OBJ_LAYER		= 2,			//ͼ��
	DP_OBJ_IMAGE		= 3				//ԭʼӰ��
};


//ͼ�������
enum DP_LAYER_TYPE
{
	GEO_NULL			= 0,			//������
	GEO_Point			= 1,			//��״ͼ��
	GEO_MultiPoint		= 2,			//MultiPoint
	GEO_Line			= 3,			//��״ͼ��
	GEO_CircularArc		= 4,			//һ��Բ�����
	GEO_EllipticArc		= 5,			//һ����Բ�����
	GEO_Bezier3Curve	= 6,			//����������,4�����Ƶ�
	GEO_Polyline		= 7,	
	GEO_Ring			= 8,			//��
	GEO_Polygon			= 9,			//�����
	GEO_Annotation		= 91,			//ע��,ע�ǵ�ʵ�ʵ�ʸ������������Polygon,����ע�ǲ��CDpShape��������ͻ�����GEO_Annotation
	GEO_Raster			= 10			//դ��
};

//ͼ����չ����
enum DP_LAYER_TYPE_EX
{
	LAYER_TYPE_EX_NULL	= 0,			//������

	LAYER_TYPE_EX_IOP	= 11,			//IOP,houkui,06,7,13
	LAYER_TYPE_EX_AOP	= 12,			//AOP,houkui,06,7,13
	LAYER_TYPE_EX_IMAGEPOINT= 13,		//IMAGEPOINT,houkui,06,7,13

	LAYER_TYPE_EX_DEM	= 21,			//DEM
	LAYER_TYPE_EX_DOM	= 22,			//DOM
	LAYER_TYPE_EX_DLG	= 23,			//DLG
	LAYER_TYPE_EX_DRG	= 24,			//DRG
	LAYER_TYPE_EX_DVS	= 25			//DVS
};


//�α�����
enum DP_CURSOR_TYPE
{
	DP_ForwardOnly		= SQL_CURSOR_FORWARD_ONLY,			//������ǰ����ֻ�����α�
	DP_Dynaset			= SQL_CURSOR_KEYSET_DRIVEN,			//��̬�α�,��CURSOR_KEYSET_DRIVEN
	DP_Dynamic			= SQL_CURSOR_DYNAMIC,				//��̬�α�,���п�����ЩODBC������֧��
	DP_Snapshot 		= SQL_CURSOR_STATIC					//��̬�α�
};

//ͼ��������
enum DP_LOCK_TYPE
{
	DP_UT_ANY			= 0,			//����״̬	
	DP_UT_UNLOCK		= 1,			//δ��������ͼ��
	DP_UT_LOCKED		= 2				//��������ͼ��
};

//��½�û�������
enum DP_USER_RIGHTTYPE
{
	DP_NORMAL			= 0,			//��ͨ�û�
	DP_ADMIN			= 1				//ϵͳ����Ա
};

//�û�Ȩ��
enum DP_RIGHT
{
	DP_RIGHT_NOTRIGHT	= 0,			//û�з��ʵ�Ȩ��
	DP_RIGHT_READONLY	= 1,			//ֻ��Ȩ��
	DP_RIGHT_READWRITE	= 2				//��дȨ��
};


//WBS CODE ��A���ֵ�ö��
enum DpAPartType
{
	DP_A_NULL			= 0,
	DP_A_IMAGE			= 1,	
	DP_A_UNIT			= 2,
	DP_A_STEREO			= 3 //��ʶģ�Ͷ��󣬶�Ӧ��CDpUnit�еĳ�Աm_iType,houkui,06.6.23,
							//��Ӧ��CDpUnit�еĳ�Աm_iType
};


//WBS CODE ��C���ֵ�ö��
/*
enum DpCPartType
{
	DP_C_NULL			= 0,//
	DP_C_IOP			= 1,//�ڶ���	
	DP_C_AOP			= 2,//���Զ���
	DP_C_IMAGEPOINT		= 3,//Ӱ����Ƶ�
	DP_C_DEM			= 4,//���ָ߲�ģ��
	DP_C_DOM			= 5,//��������ģ��
	DP_C_DLG			= 6,//�����߻�ͼ
	DP_C_DRG			= 7,//����դ��ͼ
	DP_C_DVS			= 8 //���ֿ��ӳ���
};
*/

//WBS CODE ��D���ֵ�ö��
/*
enum DpDPartType
{
	DP_D_NULL				= 10,//

	DP_D_PRODUCE			= 11,//����	
	DP_D_CHECK				= 12,//���

	DP_D_DEM_GENERATE		= 13,//�������ָ߲�ģ��,DP_D_DEM����
	DP_D_DEM_EDIT			= 14,//�༭���ָ߲�ģ��,DP_D_DEM�༭
	DP_D_DEM_MOSAIC			= 15,//���ָ߲�ģ�ͽӱ�,DP_D_DEM�ӱ�
	DP_D_DEM_CHECK			= 16,//������ָ߲�ģ��,DP_D_DEM���

	DP_D_DOM_GENERATE		= 17,//������������ģ��,DP_D_DOM����
	DP_D_DOM_EDIT			= 18,//�޲���������ģ��,DP_D_DOM�޲�
	DP_D_DOM_DODGE			= 19,//��������ģ���ȹ�,DP_D_DOM�ȹ�
	DP_D_DOM_MOSAIC			= 20,//ƴ����������ģ��,DP_D_DOMƴ��
	DP_D_DOM_CUT			= 21,//������������ģ��,DP_D_DOM����

	DP_D_DLG_COLLECT		= 22,//�����߻�ͼ�ɼ�,DP_D_DLG�ɼ�
	DP_D_DLG_EDIT			= 23,//�����߻�ͼ�༭,DP_D_DLG�༭
	DP_D_DLG_MOSAIC			= 24,//�����߻�ͼ�ӱ�,DP_D_DLG�ӱ�
	DP_D_DLG_CUT			= 25,//������������ģ��,DP_D_DLG����
	DP_D_DLG_MAPPING		= 26,//�����߻�ͼͼ������,DP_D_DLGͼ������
	DP_D_DLG_TOPOLOGY		= 27,//�����߻�ͼ����,DP_D_DLG����
	DP_D_DLG_CHECK			= 28,//�����߻�ͼ�ɼ�,DP_D_DLG���

	DP_D_DRG_PRINT			= 29,//����դ��ͼ��ͼ,DP_D_DRG��ͼ
	DP_D_DRG_CHECK			= 30,//����դ��ͼ���,DP_D_DRG���

	DP_D_DVS_GENERATE		= 31,//���ֿ��ӳ���ʸ��ģ�ͽ���,DP_D_DVSʸ��ģ�ͽ���
	DP_D_DVS_TEXTURE		= 32,//���ֿ��ӳ���DVS������ȡ,DP_D_DVS������ȡ
	DP_D_DVS_SCENE			= 33//DVS���γ�������,DP_D_DVS���γ�������
};
*/


//GET������
enum DpCheckType
{
	DP_NULLOPERA		= 0,			//�ղ���
	DP_CHECKOUT			= 1,			//CHECKOUT
	DP_CHECKIN			= 2				//CHECKIN
};

//��������Feature�Ļ��������״̬
enum DpBatchOperStatus
{
	DP_NOOPERA			= 0,			//û���κβ���
	DP_BATCHADD			= 1,			//������Ӳ��� 
	DP_BATCHUPDATE		= 2				//�������µ������
};


//�û���Ϣ�Ľṹ
struct _DpUserInfo
{
	char*					szUserID;		//�û�ID
	char*					szUserDesc;		//�û�����
	DP_USER_RIGHTTYPE		nUserType;		//�û�����
};



//�ֶ����ͽṹ
struct CDpFieldInfo
{
   char		m_strName[80];				//�ֶ�����
   short	m_nSQLType;					//�ֶ�����
   SQLULEN	m_iSize;					//�ֶεĵĴ�С
   short	m_nPrecision;				//�ֶξ���,���������͵��ֶ�,��С����λ��
   BOOL		m_bAllowNull;				//�Ƿ�����Ϊ��,ΪTRUE��ʾ����Ϊ��,ΪFALSE��ʾ������Ϊ��
};

//�Զ���������ֶ����͵��ֶ���Ϣ,�ڴ����Զ���������ֶ���Ϣ��,���ÿ���m_nSQLType�������,
//��Ϊ�ڴ���ʱ����m_CustomFieldType���ж��ֶεľ�������ݿ��
//��ͨ��ODBCȡ�����Ա��о����ĳ���Զ���������ֶ�����ʱ,m_nSQLType���и��ֶα�������ݿ���
//��������
struct CDpCustomFieldInfo:public CDpFieldInfo
{
   char		m_szAliasName[80];			//�ֶα���
   DpCustomFieldType m_CustomFieldType;	//��Ӧ���Զ����ֶ�����
   BOOL		m_bIsUnique;				//�Ƿ������ظ�,TRUE��ʾ�ֶ�ΪUNIQUE�ֶ�,���������ظ�,FALSE��ʾ�����ظ�
   int		m_iParent;					//�������ֶ������Ĺ���������ϵ���������ID,����Ƕ�̬����,��Ϊ0	
};


//��ṹ
struct CDpPoint
{
   double	m_dx;						//X����
   double	m_dy;						//Y���� 
   double	m_dz;						//Z����
   int		m_iExtensionCode;			//�����չ��
};




//����ѡ��ֵ�ṹ,����������ö�ٵ�ֵ�Ļ�,�ýṹ����ö������
struct CDpParamSelectItem
{
	CDpParamSelectItem()
	{
		iSelectID		= 0;
		iValueID		= 0;
		szSelectValue	= NULL;
	}

	~CDpParamSelectItem()
	{
		if (szSelectValue)
		{
			delete [] szSelectValue;
			szSelectValue = NULL;
		}
	}


	int					iSelectID;					//ѡ��ID
	int					iValueID;					//ѡ��ֵ��ID
	char*				szSelectValue;				//ѡ��ֵ
};
typedef vector<CDpParamSelectItem*> SelectItemArray;

//�����ṹ  adf
struct CDpParamItem
{
	CDpParamItem()
	{
		iParamID			= 0;
		pszParamName		= NULL;
		pszParamDesc		= NULL;
		nParamDataType		= DP_CFT_NULL;
		iParamDataLen		= 0;
		iParamPrecision		= 0;
		pszValue			= NULL;
	}

	~CDpParamItem()
	{
		if (pszParamName)
		{
			delete [] pszParamName;
			pszParamName = NULL;
		}
		if (pszParamDesc)
		{
			delete [] pszParamDesc;
			pszParamDesc = NULL;
		}
		if (pszValue)
		{
			delete [] pszValue;
			pszValue = NULL;
		}
		int iCount	= pSelectItem.size();
		for (int i = 0; i < iCount; i++)
		{
			CDpParamSelectItem* pSelItem = pSelectItem[i];
			if (pSelItem)
			{
				delete pSelItem;
				pSelItem = NULL;
			}
		}
		pSelectItem.clear();
	}

	int					iParamID;					//����ID
	char*				pszParamName;				//��������
	char*				pszParamDesc;				//��������
	DpCustomFieldType	nParamDataType;				//������������
	int					iParamDataLen;				//�������ݳ���
	int					iParamPrecision;			//�������ΪDOUBLE�Ļ�,����С����λ��
	char*				pszValue;					//����ֵ
	SelectItemArray		pSelectItem;				//�ò���������ѡ��ֵ,���û�����VECTOR������Ϊ�� 
};
typedef  vector<CDpParamItem*> ParamItemArray;

//��������
struct CDpParamCategory
{
	CDpParamCategory()
	{
		iCategoryID		= 0;
		szCategoryName	= NULL;
		szCategoryDesc	= NULL;
	}

	~CDpParamCategory()
	{
		if (szCategoryName)
		{
			delete [] szCategoryName;
			szCategoryName = NULL;
		}
		if (szCategoryDesc)
		{
			delete [] szCategoryDesc;
			szCategoryDesc = NULL;
		}

		int iCount = pParamItemArray.size();
		for (int i = 0; i < iCount; i++)
		{
			CDpParamItem* pItem = pParamItemArray[i];
			if (pItem)
			{
				delete pItem;
				pItem = NULL;
			}
		}
		pParamItemArray.clear();
	}


	int					iCategoryID;				//����ID
	char*				szCategoryName;				//������
	char*				szCategoryDesc;				//��������
	ParamItemArray		pParamItemArray;			//�÷����µĲ�����
};
typedef map<int,CDpParamCategory*> ParamCategoryArray;


//���ڰ󶨼�¼���İ���Ϣ
struct _DpBindFieldInfo
{
	CDpFieldInfo	info;				//�ֶε�ԭʼSQL��Ϣ
	SQLSMALLINT		nBindSqlType;		//Ҫ��Ŀ���ֶε���������
	int				iIndex;				//�ֶ�����
	int				iMemorySize;		//���ֶ����ڴ������ռ�ڴ���
	int				iPosInBlock;		//���ֶ����ڴ���е�λ��
	int				iLenPosInBlock;		//�ֶεĳ����ֶ����ڴ���е�λ��
	BOOL			bIsCanBind;			//���ֶ��Ƿ���԰󶨵��ڴ����
};

/*�����ڹ�������ö������*/
struct _DpTaskFilter
{
	DpAPartType		AFilter;			//���ΪDP_A_NULL,���ʾ����
	char*			BFilter;			//���ΪNULL,���ʾ����
	char*			CFilter;			//���ΪNULL,���ʾ����
	int				DFilter;			//���ΪO,���ʾ����
};

/*������ݷ���������Ϣ,houkui,06.6.19*/
struct _DpDataServerInfo
{
	_DpDataServerInfo()
	{
		memset(szServerName,0,sizeof(szServerName));
		memset(szServerAddr,0,sizeof(szServerAddr));
		memset(szServerDesc,0,sizeof(szServerDesc));
	}
	~_DpDataServerInfo()
	{
	}
	char			szServerName[10];		//��������,10
	char			szServerAddr[15];		//��������ַ,15
	char			szServerDesc[30];		//������������Ϣ,30
};



//��Դ���ݿ��е�ָ���ı����������Ƽ�¼��Ŀ�����ݿ�����
BOOL BulkCopyRecordToDest(SQLHANDLE hConnSource,SQLHANDLE hConnDest,
						  int iRecordCountOfPerOper,char* szTableName,
						  char* szWhereClause,int iDefaultBinaryBlockSize = DEFAULT_BINARY_BLOCK_SIZE);
//�ڵõ��ֶ����ͺ�,����¼����Ϣ��������
int CleanUpBindInfo(_DpBindFieldInfo* pBindInfo,int iCount,int iDefaultBinaryBlockSize);










typedef DP_EXECSQL_RETURNCODE DpReturnCode;


class CDpDatabase;
class CDpRightMgr;
class CDpWBSCodeWH;
class CDpTaskMgr;
class CDpTaskSetArray;
class CDpTaskSet;
class CDpTaskGroup;
class CDpWorkspace;
class CDpUserMgr;
class CDpGeoQuery;
class CDpGeometryQuery;
class CDpRasterQuery;
class CDpMgrObject;
class CDpLayer;
class CDpObject;
class CDpFeatureLayer;
class CDpFeature;
class CDpRasterLayer;
class CDpRaster;
class CDpUnit;
class CDpImage;
class CDpGeoClass;
class CDpFeatureClass;
class CDpRasterClass;
class CDpRecordset;
class CDpDBVariant;
class CDpLongBinary;
class CDpParamTable;
class CDpAttribute;
class CDpSpatialQuery;
class CDpShape;
class CDpUniqueFilter;
class CDpUniqueQueryLayerResult;
class CDpUniqueQueryValueResult;
class CDpUniqueQuery;
class CDpCheckInfo;
class CDpCheckOutMgr;
class CDpCheckInMgr;
class CDpStringArray;
class CDpUserArray;
class CDpParamMgr;
class CDpParamTable;
class CDpParamWH;
class CDpFeatureCache;
class CDpRgn;
class CDpRect;
class CDpObjectEx;
/********************************************/
/*                 ͨ����                   */
/********************************************/


/***********************************/
/*            ����������           */
/***********************************/
class CDpIntArray
{
	public:	
		CDpIntArray();
		~CDpIntArray();
	private:
		typedef vector<int> intArray;
		intArray		m_nArray;
	public:
		int				GetCount();
		int				GetItem(int iIndex);
		void			AddItem(int iItem);
		BOOL			ItemIsExist(int iItem);
};


/***********************************/
/*		   �ַ���������			   */
/***********************************/				 
class CDpStringArray
{
	public:
		CDpStringArray();
		~CDpStringArray();
	private:
		typedef vector<char*> strArray;

	private:
		strArray		m_strArray;

	public:
		int				GetCount();					//�õ���ǰ�ַ����������Ŀ��
		char*			GetItem(int	iIndex);		//�õ�ָ��λ�õ��ַ���,���㿪ʼ
		BOOL			AddString(const char* szItem);	//���ַ�������������ַ���
		void			DelString(int iIndex);		//ɾ��ָ����������
		void			DelAllItem();				//ɾ�����е�������
		void			Copy(CDpStringArray &other);
};


/***********************************/
/*		   �û���Ϣ������		   */
/***********************************/				 
class CDpUserArray
{
	public:
		CDpUserArray();
		~CDpUserArray();
	private:
		typedef vector<_DpUserInfo*> UserInfoArray;

		UserInfoArray		m_Array;
	public:
		int					GetCount();						//�õ���ǰ�ַ����������Ŀ��
		const _DpUserInfo*	GetItem(int	iIndex);			//�õ�ָ��λ�õ��û���Ϣ,���㿪ʼ
		BOOL				AddUserInfo(_DpUserInfo* pInfo);	//������������û���Ϣ
		void				DelItem(int iIndex);			//ɾ��ָ������Ϣ��
		void				DelItem(_DpUserInfo* pInfo);	//ɾ��ָ������Ϣ��	
		void				DelAllItem();					//ɾ�����е���Ϣ��
};


/*����εĵ�Ľṹ*/
struct _DPRealPoint 
{
   double		x;
   double		y;
};


/*ʵ���ľ��εĽṹ*/
struct _DpRealRect
{
	double		left;
	double		right;
	double		bottom;
	double		top;

	_DpRealRect()
	{
		left=0;
		right=0;
		bottom=0;
		top=0;
	};
	_DpRealRect(double left,double bottom,double right,double top)
	{
		this->left=left;
		this->right=right;
		this->bottom=bottom;
		this->top=top;
	};
};

/***********************************/
/*           �������              */
/***********************************/
class CDpRgn
{
public:
	double*		m_pX;					//X�㼯
	double*		m_pY;					//Y�㼯
	bool*		m_bVertex;				//
	bool*		m_VertexState;			//
	int			m_nBufferSize;			//�㼯��Ŀ
	_DpRealRect	m_Box;					//���

private:
	int			m_nDirection;
	int			CalRgnDirection();
	bool		InitVertexState();
	int			IsPtInRegion(double& x,double& y);
	inline int	Intersect(double& x0,double& y0,double& x1,double& y1,double& x2,double& y2,double& x3,double& y3,int ptIndex2,int ptIndex3,double *x,double *y,double& minXSrc,double& minYSrc,double& maxXSrc,double& maxYSrc,int nCount);
public:
	CDpRgn();
	virtual ~CDpRgn();
	inline int	GetRgnDirection(){return m_nDirection;};//0 ˳ʱ�� 1��ʱ�� -1 error
	bool		CreatePolygonRgn(_DPRealPoint* pRealPt,int nCount);//pRealPtָ�����ڹ���Ա��εĵ�,nCountΪ��ĸ���
	bool		CreatePolygonRgn(double* pX,double* pY,int nCount);//pX,pY�ֱ�ָ��count�����x�����y����
	int			GetRgnBox(_DpRealRect* pRgnBox);//�õ�����ε��������
	int			PtInRegion(double& x,double& y);//�ж�һ�����Ƿ��ڶ�����ڲ�,0�ⲿ 1 �ڲ� -1 �߽���
	int			PtInRegion(_DPRealPoint& point);//�ж�һ�����Ƿ��ڶ�����ڲ�

	double		GetCircumference();
	double		GetArea();
	bool		IsValid();	

	bool		Intersect(CDpRect* pRect);
};




/***********************************/
/*		        ������			   */
/***********************************/				 
class CDpRect
{
	public:
		CDpRect();
		CDpRect(double dMinX,double dMinY,double dMaxX,double dMaxY);
	public:
		double	m_dMinX;						//��СX����,����ǵѿ�������ϵ�Ļ�,��Ϊ���½ǵ�X����
		double	m_dMinY;						//��СY����,����ǵѿ�������ϵ�Ļ�,��Ϊ���½ǵ�Y����
		double	m_dMaxX;						//���X����,����ǵѿ�������ϵ�Ļ�,��Ϊ���Ͻǵ�X����
		double	m_dMaxY;						//���Y����,����ǵѿ�������ϵ�Ļ�,��Ϊ���Ͻǵ�Y����

	public:
		//���þ��ε�λ��
		void SetRect(double dMinX,double dMinY,double dMaxX,double dMaxY);
		void CopyRect(CDpRect* pRt);
		double	 Width() const;						//���ο��
		double   Height() const;					//���θ߶� 
		BOOL PtInRect(CDpPoint pt) const;		//�жϵ��Ƿ��ھ���������
		BOOL PtInRect(double dx,double dy) const;
		BOOL IsIntersect(CDpRect* rt) const;	//�ж�rt�Ƿ��뵱ǰ�����ཻ
		BOOL IsIntersect(double dMinX,double dMinY,double dMaxX,double dMaxY) const;
		BOOL IsInclusive(CDpRect* rt) const;	//�ж�rt�Ƿ񱻵�ǰ����������
		BOOL IsInclusive(double dMinX,double dMinY,double dMaxX,double dMaxY) const;
};



/********************************************/
/*              ���ݿ������
/*   ����Ĺ���������MFC�е�CDatabase
/********************************************/
class CDpDatabase
{
	friend class CDpCheckInMgr;

	public:
		CDpDatabase(char* szGlobalID);
		virtual ~CDpDatabase();
	public:
		//��������ACCESS���ݿ�,UserID--�û�ID,Pwd-����,���е��û�ID�Ϳ���������ݿ�ϵͳ��
		//�û�ID�Ϳ���,���Ǳ�ϵͳ�Լ������
		BOOL ConnectAccess(const char* UserID,const char* Pwd,const char* szAccessFileName);
		//��������ORACLE���ݿ�,UserID--�û�ID,Pwd-����,szAddress-ORACLE�ķ�����,szDBUserID -- ���ݿ��û�ID,szDBPwd -- ���ݿ��û�����
		BOOL ConnectDB(const char* UserID,const char* Pwd,const char* szAddress,const char* szDBUserID = "Dataplatform",const char* szDBPwd = "liwq");
		//��������SQLSERVER���ݿ⡣UserID--�û�ID,Pwd-����,szAddress-SQLServer���ݿ�����������ƻ�IP,szDBUserID -- ���ݿ��û�ID,szDBPwd -- ���ݿ��û����szDBName-���ݿ�����
		BOOL ConnectSqlServer(const char* UserID,const char* Pwd,const char* szAddress,const char* szDBUserID = "Dataplatform",const char* szDBPwd = "liwq",const char* szDBName = "Dataplatform");
		//��������Sybase���ݿ�.UserID--�û�ID,Pwd-����,szAddress-SQLServer���ݿ�����������ƻ�IP,uServerPort -- ���ݿ�������Ķ˿ں�,szDBUserID -- ���ݿ��û�ID,szDBPwd -- ���ݿ��û����szDBName-���ݿ�����
		BOOL ConnectSybase(const char* UserID,const char* Pwd,const char* szServerNetAddress,UINT uServerPort = 5000,const char* szDBUserID = "Dataplatform",const char* szDBPwd = "liwq",const char* szDBName = "Dataplatform");
		
		//����ʹ����ע����������,����ORACLE
		BOOL ConnectDB(const char* UserID,const char* Pwd);
		//����ʹ����ע����������,����SqlServer
		BOOL ConnectSqlServer(const char* UserID,const char* Pwd);
		//��������ʹ����ע����������,����Sybase
		BOOL ConnectSybase(const char* UserID,const char* Pwd);
		BOOL AddANode(enum DP_WORKSPACE_TYPE WspType,enum DpAPartType APartType);
		
		
		BOOL AddCNode(enum DP_WORKSPACE_TYPE WspType, enum DpAPartType APartType,
			enum DP_LAYER_TYPE_EX CPartTypeEx);
		
		BOOL AddDNode(enum DP_WORKSPACE_TYPE WspType, enum DpAPartType APartType,
			enum DP_LAYER_TYPE_EX CPartTypeEx, int dIndex, char* szDName,char* szDDesc);
		

		//�ر����ݿ�
		void CloseDB();

		//ִ��SQL���
		DpReturnCode ExecuteSQL(const char* szSQL);
		//���õ�¼�ĳ�ʱʱ��,dSeconds�ĵ�λΪ��,����������ǰ���� 
		void SetLoginTimeOut(DWORD dSeconds);
	public:
		//�Ƿ���ִ��������
		BOOL CanTransact();
		//��ʼ����
		BOOL BeginTrans();
		//�ύ����
		BOOL CommitTrans();
		//�ع�����
		BOOL Rollback();

	public:
		//ȡ�õ�ǰ���ݿ��й���������Ŀ
		int	 GetWksCount(DP_WORKSPACE_TYPE nWksType);
		//ö�ٵ�ǰ���ݿ��е�ǰ�û��ܹ����ʵù�������ID��������
		BOOL GetWksIDAndName(CDpStringArray& szIDArray,CDpStringArray& szNameArray);
		//����ID�õ�������
		BOOL GetWksByID(const char* szID,CDpWorkspace** ppWks);
		//�������Ƶõ�������,����������ظ���,ֻȡ��һ��
		BOOL GetWksByName(const char* szName,CDpWorkspace** ppWks);

	public:
		//���ݿ��Ƿ�����,������ΪTRUE,����ΪFALSE
		BOOL GetConnectStatus();
		//�ж����ݿ��Ƿ��
		BOOL IsOpen();
		//��ǰ���ӵ����ݿ������,���û���������ݿ�,��ֵΪ0
		DP_DATABASE_TYPE GetDBType();
		//�����ǰ���ӵ����ݿ�ΪACCESS,�������Ϊ���ݿ��ļ�������·��
		//����˵��:  szFileName --Ϊ�����ļ����Ļ�����,iBuffCount --Ϊ��������С
		BOOL GetDBFileName(char* szFileName,int iBuffCount);
		//�õ����ݿ��ODBC���
		SQLHANDLE GetDBHandle();

		//����������,bIsSetDefaultRight -- �Ƿ�����е��û�����Ըù�����������ϵͳ���õ�ȫ��Ȩ��,TRUE��ʾ����
		BOOL CreateWorksapce(const char* szName,CDpRect* pRtBound,double dScale,const char*	szDefaultDir,const char* szDataserver,int iWspType,char* szID=NULL,BOOL bIsSetDefaultRight = false);
		//BOOL CreateWorksapce(const char* szName,CDpRect* pRtBound,double dScale,const char* szDefaultDir,char* szID=NULL,BOOL bIsSetDefaultRight = false);
		//ɾ��������
		BOOL DeleteWorkspace(const char* szWksID,BOOL bDeleteDefaultDir = false);

		//�õ�WBS CODE ά�������,pWBSCodeWH��Ҫ�������ͷ�
		BOOL OpenWBSWH(CDpWBSCodeWH** ppWBSCodeWH,DP_WORKSPACE_TYPE type);

		//������ɫ
		BOOL CreateRole(const char* szRoleID,const char* szDesc);
		//�����û�,bIsSetDefaultRight -- �Ƿ����û���������ϵͳ���õ�ȫ��Ȩ��,TRUE��ʾ����
		BOOL CreateUser(const char* szUserID,const char* szDesc,const char* szPwd,DP_USER_RIGHTTYPE nUserType,BOOL bIsSetDefaultRight = false);
		//ö�ٵ�ǰ���ݿ��ڵ��û�
		BOOL GetUsersInfo(CDpUserArray& nArray);
		//ö�ٵ�ǰ���ݿ��ڵĽ�ɫ,nRoleID -- ��ɫ��,nRoleDesc -- ��ɫ����
		BOOL GetRolesInfo(CDpStringArray& nRoleID,CDpStringArray& nRoleDesc);
		//ɾ����ɫ
		BOOL DeleteRole(const char* szRoleID);
		//ɾ���û�
		BOOL DeleteUser(const char* szUserID);

		//�޸��û�����
		BOOL ModifyUserPwd(const char* szUserID,const char* szNewPwd,const char* szDesc,int iUserType);

		//ˢ��Ȩ�޹�����
		BOOL RefreshRightMgr();


	private:
		void				GetConnectInfo();			//ȡ�õ�ǰ���ӵ�������Ϣ
		//�ж��û�ID�Ϳ����Ƿ���ȷ
		BOOL				UserInfoIsCorrect(SQLHANDLE hConn,const char* UserID,const char* Pwd);
		//ͨ��ָ����SQL���õ�һ����������
		BOOL				GetWorkspace(const char* szSql,CDpWorkspace** ppWks);
		//��ʼ����Ա����
		void				InitMember();
		//�ͷ��ڴ�
		void				FreeMem();

	private:
		char				m_szPwd[64];				//��¼ϵͳ������

		char				m_szFileName[_MAX_PATH];	//���ݿ��ļ���
	
		SQLHANDLE			m_hEnv;						//�������
		SQLHANDLE			m_hConn;					//���Ӿ��

		DWORD				m_dwLoginTimeOut;			//��¼��ʱʱ��

		BOOL				m_bIsOpen;					//��ʾ�����Ƿ��
		BOOL				m_bCanTrans;				//�Ƿ���������
		BOOL				m_bCanGetDataAnyOrder;		//�ܷ����ݼ�¼�����ֶε�˳��(��С����)ȥȡ�ֶε�ֵ,���ǿ���������˳��ȡ

		DP_DATABASE_TYPE	m_dtDatabaseType;			//���ݿ�����

		char				m_szGlobalID[6];			//ȫ��ID,Ҫ�ڱ�־������


	public:
		BOOL ModifyDataServer(char *szServerName, char *szServerAddr, char *szServerDesc);
		BOOL DeleteDataServer (char* szServerName);
		BOOL RemoveAllDataServer ();
		BOOL CreateDataServer (char* szServerName,char* szServerAddr,char* szServerDesc);
		BOOL GetDataServer (CDpStringArray& szName,CDpStringArray& szAddr,CDpStringArray& szDesc);
		int GetDataServerCount();
		char				m_szUserID[64];				//��¼ϵͳ���û�ID,
		CDpRightMgr*		m_pRightMgr;				//Ȩ�޹�����

};


/************************************************************/
/*                         Ȩ�޹�����                       */
/************************************************************/
class CDpRightMgr
{
	private:
		//����Ȩ�޽ṹ
		struct _ParamRight
		{
			ULONGLONG	uWksID;						//������ID
			char*		szParamTableName;			//�����������
			UINT		uID;						//����ID
			DP_RIGHT	sRight;						//Ȩ��
		};

		//����ͼ����ͼ���Ȩ��,�޸ı��:NO.050220
		struct _Unit_Layer_Right
		{
			ULONGLONG	uUnitID;					//ͼ��ID
			ULONGLONG	ulLayerID;					//ͼ��ID
			DP_RIGHT	sRight;						//Ȩ��
		};

		//����Ӱ���ָ����ǩ��Ȩ��,�޸ı��:NO.050220
		struct _Image_XMLTag_Right
		{
			ULONGLONG	uImageID;					//Ӱ��ID
			char*		szXMLTagName;				//Ӱ��ı�ǩ����
			DP_RIGHT	sRight;						//Ȩ��
		};

		//�Ƿ�ӵ�ж�����Ӱ��,ͼ��,�����Ķ�Ȩ�޵Ľṹ
		struct _Wks_AllRead_Right
		{
			ULONGLONG	uWksID;						//������ID
			BOOL		m_bIsCanReadAllImage;		//�Ƿ�ӵ�ж�����Ӱ��Ķ�Ȩ��,��ӵ����"All Image Read Right"
			BOOL		m_bIsCanReadAllUnitLayer;	//�Ƿ�ӵ�ж�����ͼ��ͼ��Ķ�Ȩ��,��ӵ����"All Map Read Right"
			BOOL		m_bIsCanReadAllParam;		//�Ƿ�ӵ�ж����в����Ķ�Ȩ��,��ӵ����"All Parament Read Right"
		};


		typedef vector<char*> WKSRIGHT;						//������Ȩ������
		typedef vector<_Unit_Layer_Right*> LAYERARRAY;		//ͼ����ͼ�������,�޸ı��:NO.050220
		typedef vector<_Image_XMLTag_Right*> XMLTAGARRAY;	//Ӱ���XML��ǩ����,�޸ı��:NO.050220

		typedef vector<_ParamRight*> PARAMARRAY;			//�����������Ĳ���Ȩ�޵�����

		typedef vector<PARAMARRAY*> PARAMMAP;				//���еĹ������Ĳ���Ȩ��
		typedef vector<LAYERARRAY*> UNITMAP;			//ͼ��Ȩ��,MAP�е�KEYΪͼ����ID,secondΪ�����Ȩ��
		typedef vector<XMLTAGARRAY*> IMAGEMAP;			//Ӱ��Ȩ��,MAP�е�KEYΪӰ���ID,secondΪ�����Ȩ��
		typedef vector<_Wks_AllRead_Right*> WKSALLREADMAP;

		DP_USER_RIGHTTYPE	m_iUserType;			//��½�û�������
		WKSRIGHT			m_mapWorkspace;			//������ʵĹ�����������
		UNITMAP				m_mapUnit;				//������ʵ�ͼ����HASH��
		IMAGEMAP			m_mapImage;				//������ʵ�Ӱ���HASH��
		PARAMMAP			m_mapParam;				//������ʵĲ�����HASH��,��Щͼ��һ������ָ���Ĺ������µ�
		WKSALLREADMAP		m_mapWksAllRead;		//ӵ�����ж�Ȩ�޵ķ���������


	public:
		CDpRightMgr(CDpDatabase* pDB,const char* szUserID = NULL);
		virtual ~CDpRightMgr();
	public:
		//�����ݿ��м���Ȩ�޹�����Ϣ
		BOOL LoadRightInfo();
		//�ͷ�Ȩ�޹����¼��ռ���ڴ�
		void RemoveRightInfo();

		//�õ���ǰ�û��ĵ�½����
		DP_USER_RIGHTTYPE	GetUserType(){return m_iUserType;}
		//�жϸ�SQL����Ƿ���Ȩ�޽���ִ��,����оͷ���TRUE,���򷵻�FALSE;
		BOOL SqlCanExecute(const char* szSql);
		//�õ���ĳ����������Ȩ��	
		BOOL GetWorkspaceRight(const char* szWksID,DP_RIGHT* piRight);
		//�õ���ĳ��ͼ����Ȩ��
		BOOL GetUnitRight(const char* szUnitID,DP_RIGHT* piRight);
		//�õ�ͼ����ͼ���Ȩ��
		BOOL GetUnitLayerRight(const char* szUnitID,const char* szLayerID,DP_RIGHT* piRight);
		//�õ���ĳ��Ӱ���Ȩ��
		BOOL GetImageRight(const char* szImageID,DP_RIGHT* piRight);
		//�õ�Ӱ���ĳ��XML��ǩ��Ȩ��
		BOOL GetImageXmlRight(const char* szImageID,const char* szXmlTagName,DP_RIGHT* piRight);
		//ȷ���Ƿ��ܷ��ʸ�ͼ��,�жϵ����ݾ��Ǹ��û��Ƿ��ܷ��ʸ�ͼ�������һ��ͼ������ʾ�ܷ��ʸ�ͼ��
		BOOL CanAccessLayer(const char* szLayerID);

		//�õ�ĳ����������ĳ��������Ȩ��
		BOOL GetParamRight(const char* szWksID,const char* szParamTabName,UINT uParamID,DP_RIGHT* piRight);

		//����ĳ��Ӱ���ĳ��XML��ǩ
		//bLock -- TRUE:����,FALSE:����
		BOOL LockImageXml(const char* szWksID,const char* szImageID,const char* szXmlTagName,BOOL bLock = true);

		//����ĳ��ͼ����ĳ��ͼ��
		//bLock -- TRUE:����,FALSE:����
		BOOL LockUnitLayer(const char* szWksID,const char* szUnitID,const char* szLayerID,BOOL bLock = true);

		//�õ�����ָ��Ӱ���ָ��XML��ǩ�������˵��û�����
		//szRetLockerName -- ���������˵�����,���szRetLockerNameΪ���ַ������ʾ�ñ�ǩû�б�����
		BOOL GetImageXmlLockerName(const char* szWksID,const char* szImageID,const char* szXmlTagName,char* szRetLockerName);

		//�õ�����ָ��ͼ����ָ��ͼ��������˵��û�����
		//szRetLockerName -- ���������˵�����,���szRetLockerNameΪ���ַ������ʾ��ͼ����ͼ��û�б�����
		BOOL GetUnitLayerLockerName(const char* szWksID,const char* szUnitID,const char* szLayerID,char* szRetLockerName);

		//���������ڴ򿪲�ѯʱ(���FeatureClass,GeoQuery��)�����Ȩ�޹������
		//szRetClause -- ���ڷ��ع������
		BOOL MakeRightFilterClauseWhenOpenQuery(const char* szWksID,const char* szLayerID,char* szRetClause);

	private:
		//�ҵ�ָ����������ȫ��Ȩ��������
		const _Wks_AllRead_Right* FindAllReadByWksID(const char* szWksID);

	private:
		CDpDatabase*		m_pDB;
		char				m_szUserID[51];						//�û�ID
};


/************************************************************/
/*                        ������������                      */
/************************************************************/   
class CDpWorkspace
{
	public:
		CDpWorkspace(CDpDatabase* pDB);
		virtual ~CDpWorkspace();

	public:
		//�õ��������Ŀռ䷶Χ
		BOOL GetWksBound(CDpRect* pBound);
		//�޸Ĺ������Ŀռ䷶Χ
		BOOL SetWksBound(CDpRect* pBound);
		//�õ��������ı�����
		double GetWksScale(){return m_dblScale;}
		//���ù������ı�����
		BOOL SetWksScale(double dScale);

		//�õ��������Ŀ��Ƶ�XML
		BOOL GetWksCtlrPointXML(CDpDBVariant& var);
		//�õ��������Ŀ��Ƶ�XML,ppBuff���ڴ���Ҫ�������ͷ�
		BOOL GetWksCtlrPointXML(BYTE** ppBuff,int* piBuffSize);
		//�������ݿ��п��Ƶ�XML�ֶε�����
		BOOL SetWksCtrlPointXML(BYTE* pBuff,int iBuffSize);

		//�õ�������������ļ�����
		BOOL GetWksCameraData(CDpDBVariant& var);
		//�õ�������������ļ�����,ppBuff���ڴ���Ҫ�������ͷ�
		BOOL GetWksCameraData(BYTE** ppBuff,int* piBuffSize);
		//���¹�����������ļ�����
		BOOL SetWksCameraData(BYTE* pBuff,int iBuffSize);


		//�õ���������ͶӰ����ϵ����XML
		BOOL GetProjectXML(CDpDBVariant& var);
		//�õ���������ͶӰ����ϵ����XML,ppBuff���ڴ���Ҫ�������ͷ�
		BOOL GetProjectXML(BYTE** ppBuff,int* iBuffSize);
		//�������ݿ��е�ͶӰ����ϵ����XML
		BOOL SetProjectXML(BYTE* pBuff,int iBuffSize);


		//�õ���ǰ��������ǰ�û��������ID�����Ƶ��б�
		BOOL GetTasksIDAndName(CDpStringArray& szIDsArray,CDpStringArray& szNamesArray);
		//�õ���ӦID���������,ppMgr��Ҫ�������ͷ�
		BOOL GetTask(const char* szID,CDpTaskMgr** ppMgr);
		//�õ���ӦID���������,ppTaskSetArray--���񼯵�����
		//szUserID--�û���,���Ϊ��,��ȡ���е��û�������,
		//pFilter - ���ڲ�ѯ����Ĺ�������,���Ϊ��,���ʾ����Ҫ����
		BOOL GetTask(CDpTaskSetArray& nTaskSetArray,const char* szUserID = NULL,const _DpTaskFilter* pFilter = NULL);
		//�õ���ӦID�Ͷ�Ӧ�û���������
		BOOL GetTask(CDpTaskSet** ppTaskSet,const char* szUserID,const _DpTaskFilter* pFilter = NULL);
		//ö�ٸ��û���ӵ�еĽ�ɫ
		BOOL GetRoles(CDpStringArray& nRoleArray,const char* szuserID);
		


		//ɾ��ĳ������
		BOOL DeleteTask(const char* szID);
		//����һ������,[in]szName-Ϊ���������,[in]A,B,C,D�ֱ�ΪWBS CODE ���ĸ�����,
		//[out]szNewIDΪ�����ɹ�����µ������ID,�������Ҫ������ΪNULL 
		BOOL CreateTask(const char* szName,DpAPartType A,const char* B,const char* C,int D,const char* szStartTime,const char* szEndTime,char* szNewID);
		//����������ĳ���û�,���szUserIDΪNULLʱ,��ʾɾ��������ķ���,�ø�����ԭ��Ϊ�������״̬,
		//                     ��������������Ѿ�������(��������ɰٷֱȴ���0),��Ϊ����ʧ��
		BOOL AssignTaskToUser(const char* szTaskID,const char* szUserID);
		//��һ������������ĳ���û�,���szUserIDΪNULLʱ,��ʾɾ��������ķ���,�ø�����ԭ��Ϊ�������״̬,
		//                     ��������������Ѿ�������(��������ɰٷֱȴ���0),��Ϊ����ʧ��
		BOOL AssignTaskToUser(CDpTaskGroup* pGroup,const char* szUserID);
	
		//��ʱ��ӵ�һ������,���¸��汾��������
		//�ýӿڵ��ô��ǵ�����������ǻ���DLG������ʱ,Ϊ�Զ��Ľ��������е�B(������ͼ��)�µ�����DLGͼ���
		//дȨ�޶������szUserID����ʾ���û�	
		BOOL AssignTaskToUserTmp(const char* szTaskID,const char* szUserID,BOOL bDlgOnly = true);

				
		//����ͼ��,[in]szName-ͼ������,[in]pRtBound-ͼ���ռ䷶Χ,qqq
		//         [in]ptValiRegion-ͼ����Ч��Χ,[out]szUnitID-���ش����ɹ���ͼ����ID
		BOOL CreateUnit(const char* szName,CDpRect* pRtBound,CDpPoint* ptValiRegion,char* szUnitID = NULL,DpAPartType iType = DP_A_UNIT,char* szRefer = NULL,	char* szLocalName = NULL);
		//�ڹ������д���ԭʼӰ��,[in]szName-Ӱ������,,ʵ�ʾ���Ӱ����ļ���(������չ��)
		//                       [in]szStrip-��������
		//			             [out]szFilePath-Ӱ������ļ���    [in]pRtBound-Ӱ��ռ䷶Χ 
		//                       [out]szImageID-���ش����ɹ���Ӱ��ID,ʧ���򷵻ؿ��ִ�,�������Ҫ������ΪNULL
		//                       [in]iType-��ʶ��ģ�ͻ���ͼ��
		//                       [in]szRefer-�ο����ݣ���ģ��ʹ�ã�������ʶ����Ƭ
		//                       [in]szLocalName-��������ʶͼ���ĵ���
		//BOOL CreateUnit(const char* szName,CDpRect* pRtBound,CDpPoint* ptValiRegion,char* szUnitID = NULL);
		//�ڹ������д���ԭʼӰ��,[in]szName-Ӱ������,,ʵ�ʾ���Ӱ����ļ���(������չ��)
		//                       [in]szStrip-��������
		//			             [out]szFilePath-Ӱ������ļ���    [in]pRtBound-Ӱ��ռ䷶Χ 
		//                       [out]szImageID-���ش����ɹ���Ӱ��ID,ʧ���򷵻ؿ��ִ�,�������Ҫ������ΪNULL

		BOOL CreateImage(const char* szName,const char* szStrip,UINT iIndexInStrip,char* szFilePath,CDpRect* pRtBound = NULL,char* szImageID = NULL);
		//�ڹ������д���ͼ��,[in]szName-ͼ������      
		//                   [in]iLayerType-ͼ������  [out]szLayerID-���ش����ɹ���ͼ��ID,ʧ���򷵻ؿ��ִ�,�������Ҫ������ΪNULL
		BOOL CreateLayer(const char* szName,DP_LAYER_TYPE iLayerType,DP_LAYER_TYPE_EX iLayerTypeEx,char* szLayerID = NULL);

		//�ӹ�������ɾ��ͼ��
		BOOL DeleteUnit(const char* szUnitID,BOOL bDelFeature = false);
		//�ӹ�������ɾ��ԭʼӰ��
		BOOL DeleteImage(const char* szImageID);
		//�ӹ�������ɾ��ͼ��
		BOOL DeleteLayer(const char* szLayerID);


		//�õ��������е�ͼ������ĿDpAPartType ApartType,
		//int	 GetUnitCount(DP_LOCK_TYPE UnitType = DP_UT_ANY);
		
		
		//�õ��������е�ͼ������Ŀ
		int	 GetUnitCount(DP_LOCK_TYPE UnitType = DP_UT_ANY,DpAPartType ApartType =DP_A_UNIT);

		//���ݿռ䷶Χȡ����������ͼ����ID
		//szIDArray������ż����Ľ��
		//rtBoundΪҪ�����ķ�Χ,Ϊ�����ʾ�����ǿռ�λ��
		//bIntersect��ʾ��������,
		//ΪTRUE��ʾֻҪ���ཻ��ͼ���͵õ�,ΪFALSE���ʾ��Ҫ��ȫ�����ŵõ�
		//bIsLocked��ʾ�Ƿ񷵻ر�������ͼ��,ΪTRUE��ʾ����
		//BOOL GetUnitsID(CDpStringArray& szIDArray,CDpRect* pRect = NULL,BOOL bIntersect = TRUE,DP_LOCK_TYPE UnitType = DP_UT_ANY);
		//����ͼ��IDȡ��ͼ��,ppUnit����ͼ�������ָ��,�ö�����ʹ�����,��Ҫ�ֹ��ͷ�
//		BOOL GetUnitsID(DpAPartType APartType,CDpStringArray& szIDArray,CDpRect* pRect = NULL,BOOL bIntersect = TRUE,DP_LOCK_TYPE UnitType = DP_UT_ANY);
		BOOL GetUnit(const char* szUnitID,CDpUnit** ppUnit);//,UINT iType,char* szRefer = NULL,	char* szLocalName = NULL		


		//�õ��������е�Ӱ�����Ŀ
		int	 GetImageCount(DP_LOCK_TYPE UnitType = DP_UT_ANY);
		//���ݿռ䷶Χȡ����������Ӱ���ID
		//szIDArray������ż����Ľ��
		//rtBoundΪҪ�����ķ�Χ,Ϊ�����ʾ�����ǿռ�λ��
		//bIntersect��ʾ��������,
		//ΪTRUE��ʾֻҪ���ཻ��ͼ���͵õ�,ΪFALSE���ʾ��Ҫ��ȫ�����ŵõ�
		//bIsLocked��ʾ�Ƿ񷵻ر�������ͼ��,ΪTRUE��ʾ����
		BOOL GetImagesID(CDpStringArray& szIDArray,CDpRect* pRect = NULL,BOOL bIntersect = TRUE,DP_LOCK_TYPE UnitType = DP_UT_ANY);
		//���ݺ������Ƶõ��ú����ڵ�����Ӱ��
		BOOL GetImagesIDInStrip(CDpStringArray& szIDArray,const char* szStrip);
		//����Ӱ��IDȡ��Ӱ�����,ppImage����Ӱ������ָ��,�ö�����ʹ�����,��Ҫ�ֹ��ͷ�
		BOOL GetImage(const char* szImageID,CDpImage** ppImage);		//houkui,06.7.3
		//�õ���ǰ��������Ӱ��ĺ����б�
		BOOL GetStrips(CDpStringArray& szStripArray);



		//�õ��������в����Ŀ
		int GetLayersCount();
		//����ͼ������(������˳��,��ײ�Ϊ0)�õ�ͼ��ID
		BOOL GetLayerID(UINT uIndex,char** szLayerID);
		//�õ���ǰ�����������е�ͼ���ID,
		//����˵��: szIDArray -- ���صõ���ͼ��ID������, szNameArray -- ���صõ���ͼ�����Ƶ�����
		//          nLayerType   -- ���õõ���ͼ������͵Ĺ�������,���ΪGEO_NULL,���ʾ���øò���
		//          nLayerTypeEx -- ���õõ���ͼ��ĳɹ����͵Ĺ�������,���ΪLAYER_TYPE_EX_NULL,���ʾ���øò���
		BOOL GetLayersIDAndName(CDpStringArray& szIDArray,CDpStringArray& szNameArray,DP_LAYER_TYPE nLayerType = GEO_NULL,DP_LAYER_TYPE_EX nLayerTypeEx = LAYER_TYPE_EX_NULL);
		//�õ�ͼ�������
		DP_LAYER_TYPE GetLayerType(const char* szLayerID,DP_LAYER_TYPE_EX* iLayerTypeEx = NULL);

		//����ͼ���ID�õ�ͼ�����,�õ���LAYER������ʹ�����Ҫ�ֹ��ͷ�
		BOOL GetLayer(const char* szID,CDpLayer** ppLayer);

		//�õ�ָ��ͼ��ID����ر�ı���
		BOOL GetLayerTableName(const char* szID,char** ppszSpatialTableName,char** ppszParamTableName,char** ppszAttributeTableName);

		BOOL GetGroupTableName(char** ppszUDLTableName,char** ppszGRPTableName);

		//ȡ���ݵ�����״��,
		//szWBSB    -- ����Ӱ������ʱ,Ϊָ��Ӱ��ID,����MAP����ʱΪͼ��ID
		//szWBSC    -- ����Ӱ������ʱ,Ϊָ��Ӱ��XML��ǩ��,����MAP����ʱΪͼ��ID,��Ϊdlg��ʱ,�����Ǹ�ͼ���µ���һ��ͼ��ID
		//szTaskID	-- �������ڲ��������ݵ�����ID,���û���κ�����ͷ���NULL
		//D			-- ���ز�����ǰ���ݵ������D��
		//iPercent  -- ���ز�����ǰ���ݵ��������ɰٷֱ�
		//iPass     -- ���ز�����ǰ���ݵ�������Ƿ�ϸ�
		//szDesc	-- ���ز�����ǰ���ݵ�����ı�ע,��󲻳���200���ֽ�,�ڴ��Ƿ�����߷����,����Ƿ���һ��401���ֽڳ��ȵ��ַ�����
		BOOL GetDataStatus(const char* szWBSB,const char* szWBSC,char* szTaskID,int& D,int& iCurrentAllowFolwD,int& iPercent,int& iPass,char* szDesc);

		//���õ�ǰ�������������״��
		BOOL SetDataStatus(DpAPartType A,const char* szWBSB,const char* szWBSC,int iCurrentAllowFolwD);

		//���ݴ���Ľӿ�

		//��ʼ���ӵ�������
		BOOL StartLinkToServer();
		//ȡ������������
		BOOL EndLinkServer();
		BOOL GetXMLData(const char* szTagName,BYTE** ppBuff,int* piBuffSize);
		BOOL SetXMLData(const char* szTagName,BYTE* buff,int iBuffLen);	
		BOOL GetXMLData(const char* szTagName,CDpDBVariant& var);
		BOOL GetServerName(CDpDBVariant& var);
		BOOL SetServerName(char* szServerName);

	public:
// 		BOOL DelXMLData(const char* szTagName);
		UINT GetWksVersion();
		BOOL GetUnitsID(CDpStringArray&	szIDArray, DpAPartType Type=DP_A_NULL,CDpRect* pRect = NULL,BOOL bIntersect = TRUE,DP_LOCK_TYPE UnitType = DP_UT_ANY);//houkui,06.6.27
		UINT GetMetricType();
		BOOL SetMetricType(UINT iMetricType);
		UINT					m_iMetricType;			//������������,houkui,06.6.27

		CDpDatabase*			m_pDB;					//���ݿ�
		char*					m_szID;					//������ID
		char*					m_szName;				//����������
		char*					m_szDesc;				//�������ı�ע
		char*					m_szCreater;			//����������������
		char*					m_szDefaultDir;			//������ȱʡ�Ĺ���Ŀ¼
//		char*					m_szPhysicalDir;		//�������ļ�����Ŀ¼
		time_t					m_tmCreate;				//��������������
		time_t					m_tmUpdate;				//��������������
		UINT					m_iVersion;				//����ȥ�汾��						
		DP_WORKSPACE_TYPE		m_nWksType;				//����������
		CDpRect					m_rtBound;				//�������ķ�Χ
		double					m_dblScale;				//������
		UINT					m_iImageType;			//��ʶӰ�����ͣ�������������ķ���,houkui,2006.6.19
		char					m_szServerName[10];				// ���ݷ��������֣�������ȡ�ɹ�����,houkui,2006.6.19

	private:
		//����SQL���õ�LAYER	
		BOOL GetLayerBySql(char* szSql,CDpLayer** ppLayer);

};


/*****************************************************************/
/*                 ���ڹ����ɫ���û�Ȩ�޵���                    */
/*****************************************************************/
class CDpUserMgr
{
	public:
		CDpUserMgr(CDpWorkspace* pWks);
		virtual ~CDpUserMgr();

		//����ɫ���ͼ����ͼ��Ȩ��
		BOOL AddUnitLayerRightToRole(const char* szRoleID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);
		//����ɫ���Ӱ���XML��ǩȨ��
		BOOL AddImageXmlRightToRole(const char* szRoleID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//����ɫ��Ӳ���Ȩ��
		BOOL AddParamRightToRole(const char* szRoleID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);

		//���û�ֱ�ӷ����ɫ
		BOOL AddRoleToUser(const char* szUserID,const char* szRoleID);
		//���û����ͼ����ͼ���Ȩ��
		BOOL AddUnitLayerRightToUser(const char* szUserID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);
		//����ɫ���Ӱ���XML��ǩȨ��
		BOOL AddImageXmlRightToUser(const char* szUserID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//����ɫ��Ӳ���Ȩ��
		BOOL AddParamRightToUser(const char* szUserID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);

		//����ĳ����ɫ��ͼ����ͼ���Ȩ��
		BOOL DeleteUnitLayerRightFromRole(const char* szRoleID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);

		//����ĳ����ɫ��Ӱ���XML��ǩ��Ȩ��
		BOOL DeleteImageXmlRightFromRole(const char* szRoleID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//����ĳ����ɫ�Բ�����Ȩ��
		BOOL DeleteParamRightFromRole(const char* szRoleID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);


		//����ĳ���û���ͼ����ͼ���Ȩ��
		BOOL DeleteUnitLayerRightFromUser(const char* szUserID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);

		//����ĳ���û���Ӱ���XML��ǩ��Ȩ��
		BOOL DeleteImageXmlRightFromUser(const char* szUserID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//����ĳ���û��Բ�����Ȩ��
		BOOL DeleteParamRightFromUser(const char* szUserID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);

	public:

		CDpWorkspace*		m_pWks;

	private:
		//���ɫ�������Ȩ��
		BOOL AddRightToRole(const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);
		//���û��������Ȩ��
		BOOL AddRightToUser(const char* szUserID,const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);

		//���û���ĳ��Ȩ�޻���
		BOOL DeleteRightFromUser(const char* szUserID,const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);
		//����ɫ��ĳ��Ȩ�޻���
		BOOL DeleteRightFromRole(const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);

};





/****************************************************************/
/*           ���WBS�ṹ��Ƶ�����ά�������������              */
/****************************************************************/
class CDpWBSCodeWH
{ 
	//WBS_A��WBS_B��WBS_C��WBS_D,����ָ����ĸ�����Ϣ ,A:��������:���ǻ���MAP�Ļ��ǻ���IMAGE��
	//B:ͼ��ID ,C:ͼ��ID ,D:�����������,�������ɻ��Ǽ�黹�ǽӱߵ�  

	private:

		//WBS CODE ��D���ֵ�ά���б�
		struct CDpWBSDPartWH
		{
			CDpWBSDPartWH()
			{
				//D		= DP_D_NULL;
				szName	= NULL;
				szDesc	= NULL;
			}
			~CDpWBSDPartWH()
			{
				if (szName)
				{
					delete [] szName;
					szName = NULL;
				}
				if (szDesc)	
				{
					delete [] szDesc;
					szDesc = NULL;
				}
			}
			int		iD_ID;					//D���ֵ�ID
			//DpDPartType	D;					//D���ֵı�ʶ
			char*		szName;				//D���ֵ�����
			char*		szDesc;				//D���ֵ�����
		};
		typedef vector<CDpWBSDPartWH*> DPARTARRAY;		//D����ά��������

		//WBS CODE ��C���ֵ�ά���б�
		struct CDpWBSCPartWH
		{
			CDpWBSCPartWH()
			{
				C		= LAYER_TYPE_EX_NULL;
				szName	= NULL;
				szDesc	= NULL;
			}
			~CDpWBSCPartWH()
			{
				int iCount = dArray.size();
				for (int i = 0; i < iCount; i++)
				{
					CDpWBSDPartWH* pTmp = dArray[i];
					if (pTmp)
					{
						delete pTmp;
						pTmp = NULL;
					}
				}
				dArray.clear();

				if (szName)	
				{
					delete [] szName;
					szName = NULL;
				}
				if (szDesc) 
				{
					delete [] szDesc;
					szDesc = NULL;
				}
			}
			int					iC_ID;              //C���ֵ�ID
			DP_LAYER_TYPE_EX	C;					//C���ֵ���չ����
			char*				szName;				//C���ֵ�����
			char*				szDesc;				//C���ֵ�����			
			DPARTARRAY			dArray;				//���ڸ�C���ֵ�D���б�
		};
		typedef vector<CDpWBSCPartWH*> CPARTARRAY;		//C����ά��������


		//WBS CODE ��A���ֵ�ά���б�
		struct CDpWBSAPartWH
		{
			CDpWBSAPartWH()
			{
				A		= DP_A_NULL;
				szName	= NULL;
				szDesc	= NULL;
			}
			~CDpWBSAPartWH()
			{
				int iCount = cArray.size();
				for (int i = 0; i < iCount; i++)
				{
					CDpWBSCPartWH* pTmp = cArray[i];
					if (pTmp)
					{
						delete pTmp;
						pTmp = NULL;
					}
				}
				cArray.clear();

				if (szName)
				{
					delete [] szName;
					szName = NULL;
				}
				if (szDesc)
				{
					delete [] szDesc;
					szDesc = NULL;
				}
			}
			int		iA_ID;                 //A���ֵ�ID
			DpAPartType	A;					//A���ֵı�ʶ
			char*		szName;				//A���ֵ�����
			char*		szDesc;				//A���ֵ�����
			CPARTARRAY	cArray;				//���ڸ�A���ֵ�C���б�
		};
		typedef vector<CDpWBSAPartWH*> APARTARRAY;		//A����ά��������


		
		APARTARRAY				m_pWHData;				//ά��������
		CDpDatabase*			m_pDB;					//���ݿ����

	private:
		//�õ�A�����е�ĳ���ڵ�
		CDpWBSAPartWH* GetANode(DpAPartType A);
		//�õ�ָ����C�Ľڵ�
		CDpWBSCPartWH* GetCNode(int cID);
		//�õ�ָ����D�Ľڵ�
		CDpWBSDPartWH* GetDNode(int dID);
	public:
		CDpWBSCodeWH(CDpDatabase* pDB,DP_WORKSPACE_TYPE type=DP_WKS_FULL);
		~CDpWBSCodeWH();
	public:
		BOOL GetCTypeEX(int cID,DP_LAYER_TYPE_EX* cTypeEx);
		//BOOL GetDIndex(char* pD_ID,int* index);
		BOOL GetDIndex(int dID,int* index);
		//BOOL GetCWorkSpaceType(short C,int* pWSK,short wskType);
		DP_WORKSPACE_TYPE m_WSPType;
		BOOL LoadDataFromDB();							//�����ݿ��м�������
		//�õ�A����Ŀ
		int GetACount();
		//�õ�C����Ŀ,A -- Ϊ������A�ı�ʶ
		int GetCCount(DpAPartType A);
		//�õ�D����Ŀ,C--Ϊ������C�ı�ʶ
		int GetDCount(int cID);


		


		//�õ������A,iIndex -- Ϊ���,��0��ʼ. szName,szDesc���ڴ�����߲���ȥ�ͷ�
		BOOL GetA(int iIndex,DpAPartType* pA,const char** ppszName,const char** ppszDesc = NULL);
		//�õ������C,A -- Ϊ������A�ı�ʶ,iIndex -- Ϊ���,��0��ʼ.  szName,szDesc���ڴ�����߲���ȥ�ͷ�
		BOOL GetC(enum DpAPartType A,int iIndex,int* cID,const char** ppszName,const char** ppszDesc=NULL);
		//�õ������D,C -- Ϊ������C�ı�ʶ,iIndex-���,��0��ʼ. szName,szDesc���ڴ�����߲���ȥ�ͷ�
		BOOL GetD(int cID,int iIndex,int* dID,const char** ppszName,const char** ppszDesc=NULL);

		//ͨ��A�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
		BOOL GetAName(DpAPartType A,const char** ppszName);
		//ͨ��A�ı�ʶ�õ�����,ppszDesc���ڴ�����߲���ȥ�ͷ�
		BOOL GetADesc(DpAPartType A,const char** ppszDesc);
		//ͨ��C�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
		BOOL GetCName(int cID,const char** ppszName);
		//ͨ��C�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
		BOOL GetCDesc(int cID,const char**  ppszDesc);
		//ͨ��D�ı�ʶ�õ�����,szName���ڴ�����߲���ȥ�ͷ�
		BOOL GetDName(int dID,const char** ppszName);
		//ͨ��D�ı�ʶ�õ�����,ppszDesc���ڴ�����߲���ȥ�ͷ�
		BOOL GetDDesc(int dID,const char** ppszDesc);
};


		




/********************************************************************/
/*                          ����������                            */
/********************************************************************/
class CDpTaskMgr
{
	public:
		CDpTaskMgr(CDpWorkspace* pWks);
		~CDpTaskMgr();

		CDpWorkspace*		m_pWks;						//������ID
		char				m_szTaskID[50];				//����ID
		char*				m_szTaskName;				//��������
		char*				m_szUserID;					//ӵ�и�������û�ID

		struct DpWBSCodeData
		{
			DpAPartType		A;							//A����
			char*			B;							//B����
			char*			C;							//C����
			int				D;							//D����,ID
			int				TypeEx;						//����������,����DEM,DOM��,Ŀǰ�Ķ�������:
			                                            //     
			char*			szAName;					//A���ֵ�����
			char*			szDName;					//D���ֵ�����
			char*			szTypeEx;					//���������������,��DEM,DOM��
			char*			szStartTime;				//����ƻ���ʼʱ��
			char*			szEndTime;					//����ƻ�����ʱ��
			int				iPercent;					//�������
			int				iPass;						//�����Ƿ�ϸ�
			char*			szTaskDesc;					//����ı�ע
		};
		DpWBSCodeData		m_WBSCode;					//WBS CODE


	public:
		//������
		BOOL OpenTask(const char* szTaskID);
		//�õ�WBS��ĸ�������
		BOOL Get(DpAPartType* A,const char** B,const char** C,int* D);
		//�õ�WBS��ĸ������ֵ�����,A,D:������,B,C:�Ǿ����������ڲ����ݵ�ID
		BOOL Get(const char** A,const char** B,const char** C,const char** D);
		//����WBS��ĸ�������
		BOOL Set(DpAPartType A,const char* B,const char* C,int D);

		//���������״̬
		BOOL SetTaskStatus(int iPercent,int iPass,char* szDesc,int iCurrentAllowFolwD);

		//ȡ�������״̬
//		BOOL GetTaskStatus(int iPercent,int iPass,char* szDesc);

		//�ͷ��ڴ�
		void FreeMem();
};


/*******************************************************************/
/*                        ������������                           */
/*******************************************************************/
class CDpTaskGroup
{
	public:
		CDpTaskGroup();
		~CDpTaskGroup();
	public:
		struct _DpTaskInfo
		{
			char*		szTaskID;					//����ID
			char*		szTaskName;					//��������
			char*		szWBS_C;					//WBS C�Ĳ���
			char*		szStartTime;				//����ƻ���ʼʱ��,(�������ж������ʱ,ȡ��Сʱ��)
			char*		szEndTime;					//����ƻ�����ʱ��,(�������ж������ʱ,ȡ���ʱ��)
			int			iPercent;					//�������״̬
			int			iPass;						//�Ƿ�ϸ�
			char*		szDesc;						//����ı�ע,(�������ж������ʱ,ȡ��һ��)
		};

		/*������������*/
		typedef vector<_DpTaskInfo*> DPTASKARRAY;

	public:
		//�õ����������������Ŀ
		int GetTaskCount();

		//�õ���������
		const _DpTaskInfo* GetTaskItem(int iIndex);

	public:
		ULONGLONG			m_uGroupID;				//��ID,(����DLG������ʱ,��ID�͵��ڸ����ڵ�һ�������ID)
		char*				m_szGroupName;			//������,(����DLG������ʱ,���ƾ�ΪD������)
		DpAPartType			m_WBS_A;				//WBS A�Ĳ���
		char*				m_WBS_B;				//WBS B�Ĳ���
		int					m_WBS_D;				//WBS D�Ĳ��� 
		DPTASKARRAY			m_TaskArray;			//��������� 
		int					m_iPercent;				//�������״̬,(�������ж������ʱ,ȡ��ֵ)
		int					m_iPass;				//�Ƿ�ϸ�,(�������ж������ʱ,���������񶼺ϸ�,�źϸ�)
		char*				m_szStartTime;			//����ƻ���ʼʱ��,(�������ж������ʱ,ȡ��Сʱ��)
		char*				m_szEndTime;			//����ƻ�����ʱ��,(�������ж������ʱ,ȡ���ʱ��)
		char*				m_szDesc;				//����ı�ע,(�������ж������ʱ,ȡ��һ��)
};



/*******************************************************************/
/*                        ���񼯹������                           */
/*******************************************************************/
class CDpTaskSet
{
	public:
		typedef map<ULONGLONG,CDpTaskGroup*>	DPTASKSET;

		CDpTaskSet(CDpWorkspace* pWks,const char* szUserID);
		~CDpTaskSet();
	public:
		//�����ݿ���ȡ��������
		BOOL GetDataForDB(const _DpTaskFilter* pFilter/*const char* szFilterClause*/);

		//ȡ�����Ŀ
		int GetGroupCount();
		//ȡ��,iIndex ��0��ʼ
		const CDpTaskGroup* GetGroup(int iIndex);

		char			m_szUserID[50];
	private:
		void FreeMem();

	private:
		CDpWorkspace*	m_pWks;
		DPTASKSET		m_TaskSet;						//����
};



/*******************************************************************/
/*                    ���û������񼯵�������                       */
/*******************************************************************/
class CDpTaskSetArray
{
	public:
		typedef vector<CDpTaskSet*> DPTASKSETARRAY;

		CDpTaskSetArray();
		~CDpTaskSetArray();
	public:
		//ȡ��ǰ�����ڵ����񼯵���Ŀ
		int GetArraySize();
		//ȡָ������������
		const CDpTaskSet* GetTaskSet(int iIndex);
		//���������������
		void AddTaskSet(CDpTaskSet* pTaskSet);
	private:
		DPTASKSETARRAY			m_TaskSetArray;
};







/*******************************************************************/
/*        �������ڹ���Ԫ����(��ͼ��,ͼ��,ԭʼӰ��)               */
/*******************************************************************/
class CDpMgrObject
{
	public:
		CDpMgrObject(CDpWorkspace* pWks);
		~CDpMgrObject();

	public:
		//����������
		virtual BOOL CreateParamTable();
		//ɾ��������
		virtual BOOL DeleteParamTable();
		//�򿪲�����,ppParamTable���ڴ���Ҫ�������ͷ�
		BOOL OpenParamTable(CDpParamTable** ppParamTable);
		
		//������������ȱʡ�����ò���
		BOOL AddDefaultBulitinParam(CDpParamTable* pTab = NULL);

	protected:
		//�õ���ǰ������������,ͼ��/ͼ��/ԭʼӰ��
		virtual DP_OBJECT_TYPE GetObjType(){return DP_OBJ_NULL;}
		//�õ���ǰ�������ĳɹ�����,ֻ�е����������ͼ��ʱ����Ч
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx(){return LAYER_TYPE_EX_NULL;}


	public:
		CDpWorkspace*	m_pWks;
		char*			m_szID;						//ID
		char*			m_szParamTableName;			//����������
};



/************************************************************/
/*                        ͼ�������                        */
/************************************************************/   
class CDpLayer:public CDpMgrObject
{
	public:
		CDpLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType);
		virtual ~CDpLayer();
	public:
		//�õ��������
		DP_LAYER_TYPE GetLayerType();
		//�õ���ĳɹ�����
		DP_LAYER_TYPE_EX GetLayerTypeEx();
		//�õ����ID
		BOOL GetLayerID(char* szLayerID,int iBuffCount);
		//�õ��������
		BOOL GetLayerName(char* szLayerName,int iBuffCount);
		//�õ�����������
		BOOL GetParamTableName(char* szParamTabName,int iBuffCount);
		//�õ����ʸ�����ݱ������
		BOOL GetSpatialTableName(char* szSpatialTabName,int iBuffCount);
		//�õ����Ա������
		BOOL GetAttrTableName(char* szAttrTabName,int iNameBuffCount);
		//�õ�ͼ���Ӧ��FeatureClass������
		BOOL GetGeoClassName(char* szFeaClsName,int iNameBuffCount);

		//�õ������ֶεĸ���,������ATT_OID
		int GetAttrFieldCount();
		//�õ����Ա�ĳ�������ֶε���Ϣ
		BOOL GetAttrFieldInfo(char* szAttrFieldName,CDpCustomFieldInfo* pInfo);
		//�õ����Ա�ĳ�������ֶε���Ϣ,uIndex -- ��ʾ�ֶε�����,�ֶε�������0��ʼ
		BOOL GetAttrFieldInfo(UINT uIndex,CDpCustomFieldInfo* pInfo);
		//�õ�ָ�����ֶ����������ֶε�����,�����ֶε�������0��ʼ
		int  GetAttrFieldIndex(char* szAttrFieldName);
		//��������ֶ�
		BOOL AddAttrField(CDpCustomFieldInfo* pInfo,bool bRefreshView = true);
		//��������ֶ�,һ����Ӷ�������ֶ�
		BOOL AddAttrField(CDpCustomFieldInfo* pFieldInfoArray,int iFieldCount,bool bRefreshView = true);
		//ɾ�������ֶ�,iIndex - Ϊ�ֶε�������,��0��ʼ
		BOOL RemoveAttrField(int iIndex);
		//�õ����������ֶε�ֵ
		BOOL GetAttrFieldValue(GUID uOID,const char* szUnitID,const char* szFieldName,CDpDBVariant& var);
		//���õ��������ֶε�ֵ
		BOOL SetAttrFieldValue(GUID uOID,const char* szUnitID,const char* szFieldName,CDpDBVariant& var);
		
		//ˢ��FeatureClass����ͼ,��Ҫ��ˢ�½ṹ.��Ϊ�����Ա��޸������������Ҫ����ͼ����ˢ��
		BOOL RefreshFeaClsView();


		//�������÷���ĳ�ֲ�ѯ����������ֵ
		BOOL BatchSetAttrFieldValue(CDpUniqueFilter* nFilter,CDpUniqueFilter* pNewAttValue);

		//�õ���ǰ����֧�ֵĲ�
		BOOL GetSupportClassIDs(CDpIntArray& nClassIDArray);

		//����������
		virtual BOOL CreateParamTable();
		//ɾ��������
		virtual BOOL DeleteParamTable();

		//�޸�ͼ������
		BOOL ModifyLayerName(const char* szNewLayerName);

		int GetRecordCount();

		
	protected:
		//�õ���ǰ������������,ͼ��/ͼ��/ԭʼӰ��
		virtual DP_OBJECT_TYPE GetObjType();
		//�õ���ǰ�������ĳɹ�����,ֻ�е����������ͼ��ʱ����Ч
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx();
		

	public:
		char*				m_szName;					//ͼ�������
		char*				m_szSpatialTableName;		//�ռ����ݱ�����
		char*				m_szAttributeTableName;		//���Ա�����
		int					m_iIndex;					//ͼ�����ʾ˳��,0Ϊ��Ͳ�
		DP_LAYER_TYPE_EX	m_iLayerTypeEx;				//�ɹ�����,��DEM,DLG,DOM��


	protected:
		//ȷ���Ƿ��ܱ༭��ͼ���е�����
		BOOL CanModifyCurrUnit(const char* szUnitID);
		//ͨ��Feature������ͼ�����Ա��в����¼��SQL���
		BOOL MakeInsertSql(CDpObject* pObject,GUID nOID,char* szSql,int iBuffCount);
		//ͨ��Feature������ͼ�����Ա���¼�¼��SQL���
		BOOL MakeUpdateSql(CDpObject* pObject,char* szSql,int iBuffCount);

	protected:
		DP_LAYER_TYPE		m_iLayerType;				//ͼ��ĵ�������
		CDpCustomFieldInfo*	m_pCustomFieldInfo;			//�����ֶε���������
		int					m_iAttrFieldCount;			//�����ֶεĸ���

	private:
		//�õ����������ֶε��ֶ���Ϣ
		BOOL GetAllAttrFieldInfo();
		//���������ֶ��Ƿ��ܹ��������
		BOOL CanAddAttrField(CDpCustomFieldInfo* pInfo);
		//�������������ֶε�SQL���,����ͬʱ��Ӷ���ֶ�
		//pInfo -- Ҫ��ӵ��ֶ����Ե�����,iFieldCount -- Ҫ��ӵ��ֶ���
		BOOL MakeAddAttrFieldSql(CDpCustomFieldInfo* pInfo,int iFieldCount,char* szSqlOut);
		//�����������ֵ������¼��SQL���
		BOOL MakeInsertAttrDircSql(CDpCustomFieldInfo* pInfo,char* szSqlOut);



};


/*******************************************************************/
/*                       ʸ������������                          */
/*******************************************************************/
class CDpFeatureLayer:public CDpLayer
{
	public:
		CDpFeatureLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType);	
		virtual ~CDpFeatureLayer();
	public:
		//��ӵ���, �������uOID��Ϊ0,���ʾ����ӵ����ʱ����Ҫȥ�������ĵ���ID,����ֱ��ʹ�øò���,���Ҳ����ж��Ƿ��б༭ͼ����Ȩ��
		BOOL AddShape(const char* szGridID,CDpShape* pShape,GUID* pNewOID = NULL,BOOL bInsertEmptyAttr = true,GUID *uOID = NULL);
		//�༭����
		BOOL SetShape(GUID OID,const char* szUnitID,CDpShape* pShape);
		//ɾ������,ͬʱɾ����������
		BOOL DelShape(GUID OID,const char* szUnitID);

		//��ǰʸ���������һ��Feature
		BOOL AddFeature(CDpFeature* pFeature);
		//����һ��Feature
		BOOL UpdateFeature(CDpFeature* pFeature);
		//ɾ��һ��Feature
		BOOL DeleteFeature(CDpFeature* pFeature);

		BOOL FindFeature(GUID OID);
		
		//�򿪵�ǰͼ�����Ĳ�ѯ
		BOOL OpenGeometryQuery(CDpSpatialQuery* pSpaFilter,CDpGeometryQuery** ppGeometryQuery);
		//�򿪵�ǰͼ���Feature�Ĳ�ѯ
		BOOL OpenFeatureClass(CDpStringArray* pUnitIDArray,const char* szWhereClause,CDpSpatialQuery* pSpaFilter,CDpFeatureClass** ppFeatureCls);
		/************************************************************************/
		/* SELECT[ALL|DISTINCT|DISTINCTROW|TOP] 
		{*|talbe.*|[table.]field1[AS alias1][,[table.]field2[AS alias2][,��]]} 
		FROM tableexpression[,��][IN externaldatabase] 
		[WHERE��] 
		[GROUP BY��] 
		[HAVING��] 
		[ORDER BY��] 
		[WITH OWNERACCESS OPTION]
		
		[ALL|DISTINCT|DISTINCTROW|TOP]---->pPredicate
		WHERE��------>szWhereClause*/
		/************************************************************************/
		//ִ��ĳ��sql���(��sql�����������ڱ��������У�pSelectColΪ��ѯ�е��ֶ�����
		BOOL QueryFeatureClass(CDpStringArray* pSelectCol,const char *pPredicate,const char* szWhereClause,CDpFeatureClass** ppFeatureCls);
};


/*******************************************************************/
/*                       �û����������ݶ�����					   */
/*******************************************************************/

class CDpUserDefinedGroupObject
{
public:
	CDpUserDefinedGroupObject(CDpRecordset *pRecSet, CDpObjectEx *pItem);
	virtual ~CDpUserDefinedGroupObject();	

	int GetID();
	int SetID(int id);
	const char* GetUnitID();
	void SetUnitID(const char* szUnitID);

	//���������ֶ�����,�õ�����ֵ
	BOOL GetAttrValue(UINT uIndex,CDpDBVariant& var);
	//����ָ���ֶ�����������ֵ
	BOOL SetAttrValue(UINT uIndex,CDpDBVariant& var);

	//�õ���ǰFeature�ڵ������ֶεĸ���
	int	GetAttrFieldCount();

public:
	int									m_iOID;						//OID
	char								m_szUnitID[20];				//����������ͼ��ID
	int									m_iAttrFieldCount;
	CDpDBVariant*						m_pAttrFieldValueArray;		//�����ֶ�ֵ������
};


/*******************************************************************/
/*                       �û������Ĺ�����								   */
/*******************************************************************/
class CDpUserDefinedGroupMgr
{
public:
	CDpUserDefinedGroupMgr();	
	virtual ~CDpUserDefinedGroupMgr();

	BOOL Attach(CDpWorkspace* pWks, CDpStringArray &arrUnitIDs, int table);

public:
	//��ӵ���, �������uOID��Ϊ0,���ʾ����ӵ����ʱ����Ҫȥ�������ĵ���ID,����ֱ��ʹ�øò���,���Ҳ����ж��Ƿ��б༭ͼ����Ȩ��
	BOOL AddGroup(CDpUserDefinedGroupObject* pObj, int* pNewId = NULL);

	//ɾ������,ͬʱɾ����������
	BOOL DelGroup(int nid);

	int GetGroupCount();

	//���������ֶ�����,�õ�����ֵ
	CDpUserDefinedGroupObject* GetGroup(int nid);

	//����ָ���ֶ�����������ֵ
	BOOL UpdateGroup(CDpUserDefinedGroupObject* pObj);

	int GetAttrFieldCount();	

	BOOL GetAttrFieldInfo(const char* szAttrFieldName,CDpCustomFieldInfo* pInfo);

	BOOL GetAttrFieldInfo(int idx,CDpCustomFieldInfo* pInfo);

	int  GetAttrFieldIndex(const char* szAttrFieldName);
	
	//��ȡһ��Group�Ĳ�ѯ
	BOOL OpenGroupClass(const char* szWhereClause,CDpRecordset** ppCls);

protected:
	int GetMaxID();
	BOOL GetFieldInfo();
	BOOL MakeInsertSql(CDpUserDefinedGroupObject *pObject, int nID, char *szSql, int nszLen);
	BOOL MakeUpdateSql(CDpUserDefinedGroupObject *pObj, char *szSql, int nszLen);

	char* MakeUnitIDWhere();

protected:
	CDpWorkspace*	m_pWks;
	CDpStringArray	m_arrUnitIDs;						//ID
	char*			m_szTableName;

	CDpCustomFieldInfo*	m_pCustomFieldInfo;			//�����ֶε���������
	int					m_iAttrFieldCount;			//�����ֶεĸ���
};

/*******************************************************************/
/*                       դ�����������                          */
/*******************************************************************/
class CDpRasterLayer:public CDpLayer
{
	public:
		CDpRasterLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType = GEO_Raster);	
		virtual ~CDpRasterLayer();

	public:
		//�õ�Ҫ������դ���ļ��ڷ������ڵ��ļ���
		//����˵��:  [in] szGridID --  դ���ͼ��ID    [out] szFileName -- ���ص�դ���ļ���
		BOOL GetNewRasterFileNameInServer(const char* szGridID,char* szFileName);

		//���դ��
		BOOL AddRaster(const char* szGridID,const char* pszRasterFileName,CDpRect* lpRtBound,GUID* pNewOID,BOOL bInsertEmptyAttr=true);
		//���դ��
		BOOL AddRaster(CDpRaster* pRaster);

		//�༭դ��
		BOOL SetRaster(GUID OID,const char* szUnitID,const char* pszRasterFileName,CDpRect* lpRtBound);
		//�༭դ��
		BOOL UpdateRaster(CDpRaster* pRaster);
	
		//ɾ��դ��
		BOOL DelRaster(GUID OID,const char* szUnitID);
		//ɾ��դ��
		BOOL DelRaster(CDpRaster* pRaster);



	public:
		//�򿪵�ǰͼ�����Ĳ�ѯ
		BOOL OpenRasterQuery(CDpSpatialQuery* pSpaFilter,CDpRasterQuery** ppRasterQuery);
		//�򿪵�ǰͼ���Feature�Ĳ�ѯ
		BOOL OpenRasterClass(CDpStringArray* pUnitIDArray,const char* szWhereClause,CDpSpatialQuery* pSpaFilter,CDpRasterClass** ppRasterCls);

};


/*******************************************************************/
/*                         ͼ��������
/*******************************************************************/
class CDpUnit:public CDpMgrObject
{
	public:	
		CDpUnit(CDpWorkspace* pWks);
		~CDpUnit();
	public:
//		int GetUnitCount(enum DpAPartType unitType);
		BOOL SetLocalName(const char *szLocalName);
	
		//�õ�ͼ����ָ��������ͼ��ID
		BOOL GetLayerID(UINT iIndex,char** ppszLayerID);
		//����ͼ��ID�õ�ʸ������,ʸ����¼������ʹ�������Ҫ�ֹ��ͷ�
		BOOL OpenGeometryQuery(const char* szLayerID,CDpGeometryQuery** ppGeometryQuery,CDpSpatialQuery* pSpaFilter = NULL);

		//����դ��ͼ���ID�õ�դ�����
		BOOL OpenRasterQuery(const char* szRasterLayerID,CDpRasterQuery** ppRasterQuery,CDpSpatialQuery* pSpaFilter = NULL);

		//����ͼ���Ŀռ䷶Χ
		BOOL SetUnitBound(CDpRect* pRect);

		//����ͼ������Ч��Χ
		BOOL SetUnitVailRegion(CDpPoint* pVailReg);
		
		//����������
		virtual BOOL CreateParamTable();
		//ɾ��������
		virtual BOOL DeleteParamTable();

		DpAPartType			m_Type;                    //��ʶ��ģ�ͻ���ͼ��,modified by houkui,2006.6.19
		char*				m_szRefer;					//�ο����ݣ���ģ��ʹ�ã�������ʶ����Ƭ,modified by houkui,2006.6.19
		char*				m_szLocalName;				//��ʶͼ���ĵ���,modified by houkui,2006.6.19

		char*				m_szUnitName;				//ͼ������
		CDpRect				m_rtBound;					//ͼ����Χ
		CDpPoint			m_ptQuadrangle[4];			//ͼ�����ı�����Ч����
		time_t				m_tmUpdateDate;				//��������,��ȷ����
		DP_LOCK_TYPE		m_iUnitStatus;				//ͼ����״̬,1-δ����,2-����
		char*				m_szLocker;					//������ͼ����û����û�ID
	    
		//�õ���ǰͼ�����е�ͼ�����Ŀ
		int	 GetLayerCount(){return m_pWks->GetLayersCount();}

	protected:
		//�õ���ǰ������������,ͼ��/ͼ��/ԭʼӰ��
		virtual DP_OBJECT_TYPE GetObjType();
		//�õ���ǰ�������ĳɹ�����,ֻ�е����������ͼ��ʱ����Ч
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx();
		
};



/*******************************************************************/
/*                         Ӱ�������
/*******************************************************************/
class CDpImage:public CDpMgrObject
{
	public:
		CDpImage(CDpWorkspace* pWks);
		~CDpImage();
	public:
		//����������
		virtual BOOL CreateParamTable();
		//ɾ��������
		virtual BOOL DeleteParamTable();
		//�õ���ǰӰ�������еı�ǩ������
		BOOL GetAllXmlTagName(CDpStringArray& nArray);
		//�õ�Ӱ���ָ�����Ƶ�XML������
		BOOL GetImageXmlData(const char* szTagName,BYTE** buff,int* piBuffLen);
		//���Ӱ���ǩ,ͬʱ�����XML������
		BOOL SetImageXmlData(const char* szTagName,BYTE* buff,int iBuffLen);
		//ɾ��Ӱ���ǩ
		BOOL DeleteImageXmlData(const char* szTagName);
		//����Image�Ŀռ䷶Χ
		BOOL SetImageBound(double LTX,double LTY,double RTX,double RTY,double RBX,double RBY,double LBX,double LBY);

	protected:
		//�õ���ǰ������������,ͼ��/ͼ��/ԭʼӰ��
		virtual DP_OBJECT_TYPE GetObjType();
		//�õ���ǰ�������ĳɹ�����,ֻ�е����������ͼ��ʱ����Ч
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx();


	public:
		BOOL SetServer(const char* szServerName);
	
		char*				m_szServerName;				//��¼���ݷ��������֣�������ȡ����,modified by houkui,2006.6.19
		
		char*				m_szImageName;				//Ӱ������
		char*				m_szImageDesc;				//Ӱ������
		char*				m_szStrip;					//������Ϣ
		char*				m_szFilePath;				//Ӱ����ļ�·��
		char*				m_szLocker;					//������ͼ����û����û�ID
		UINT				m_iIndexInStrip;			//Ӱ���ں����е�����
//		CDpRect				m_rtBound;					//Ӱ��Χ
		time_t				m_tmUpdateDate;				//��������,��ȷ����
		DP_LOCK_TYPE		m_iImageStatus;				//Ӱ���״̬,1-δ����,2-����
		CDpRgn				m_nRgn;						//Ӱ��ķ�Χ,Ϊ�ı���
};






/*******************************************************************/
/*                ���ڴ����¼����
/*******************************************************************/
class CDpRecordset
{
	public:
		CDpRecordset(CDpDatabase* pDB);
		virtual ~CDpRecordset();
	public:
		//�򿪲�ѯ,szSql -- ��ʾSQL���
		BOOL Open(const char* szSql,DP_CURSOR_TYPE nCursorType = DP_Dynaset);
		//�α��Ƿ��
		BOOL IsOpen();
		//�ر��α�
		void Close();
		//�õ���¼��,�����ѯ����¼�ú���Ӧ�÷���1,���򷵻�0
		int  GetODBCRecordCount();
		//�õ��ֶ���Ŀ
		int GetFieldCount();
		//�õ��ֶζ�Ӧ���ֶ�������
		int GetFieldIndex(char* szFieldName);
		//�õ��ֶ�������Ӧ���ֶε�����
		BOOL GetFieldName(UINT uIndex,char* szFieldName);
		//�õ��ֶε�SQL��������
		SQLSMALLINT GetFieldSqlType(UINT uIndex);
		//�õ��ֶε���Ϣ
		BOOL GetFieldInfo(UINT uIndex,CDpFieldInfo* pInfo);


		//�����ƶ�һ����¼
		virtual BOOL Next();

		virtual CDpObjectEx* Next(int iEmpty);

		//�õ�ODBC���ξ��
		SQLHANDLE GetSTMT();

		//�õ��ֶ�ֵ,������1��ʼ
		BOOL GetFieldValue(UINT uIndex,CDpDBVariant& var);
		//�õ��ֶ�ֵ
		BOOL GetFieldValue(char* szFieldName,CDpDBVariant& var);


	protected:
		CDpDatabase*		m_pDB;						//���ݿ����ָ��
		CDpFieldInfo*		m_pFieldInfoArray;			//��¼���ֶ���Ϣ�б�
		int					m_iFieldCount;				//��ǰ��¼�����ֶ���
		SQLHANDLE			m_hStmt;					//����ִ�в�ѯ�ľ�� 
		BOOL				m_bIsOpen;					//��ʾ�α��Ƿ��

	protected:

		//������Դ����������ת���������������������
		SQLSMALLINT	DSDataTypeToDrv(CDpFieldInfo* pInfo);

		BOOL GetAllFieldInfo();
		//�õ���¼���ֶ���Ŀ
		int GetColsCount();
		//�õ���ǰ���������ݵ�ʵ�ʳ���
		long GetActualSize(UINT uIndex);
		//�õ��ֶ�ֵ�Ļ�����
		BOOL GetDataBuffer(/*in*/CDpDBVariant& varValue,/*in*/short nFieldType,
						   /*out*/const void** ppBuff,/*in,out*/long* pnLen);

};


#define DPDBVT_NULL				0x00000000
#define DPDBVT_BOOL				0x00000001
#define DPDBVT_UCHAR			0x00000002
#define DPDBVT_SHORT			0x00000003
#define DPDBVT_LONG				0x00000004
#define DPDBVT_SINGLE			0x00000005
#define DPDBVT_DOUBLE			0x00000006
#define DPDBVT_DATE				0x00000007
#define DPDBVT_STRING			0x00000008
#define DPDBVT_BINARY			0x00000009

/*******************************************************************/
/*                ���ڴ����¼����
/*******************************************************************/
class CDpDBVariant
{
public:
	CDpDBVariant();

	DWORD m_dwType;

	union
	{
		BOOL				m_boolVal;
		unsigned char		m_chVal;
		short				m_iVal;
		long				m_lVal;
		float				m_fltVal;
		double				m_dblVal;
		TIMESTAMP_STRUCT*	m_pdate;
		char*				m_pString;
		CDpLongBinary*		m_pLongBinary;
	};

	//����ڴ�
	void Clear();

	operator int() const;
	operator double() const;
	operator LPCTSTR() const;
	const CDpDBVariant& operator =(CDpDBVariant& var); 
	const CDpDBVariant& operator =(LPCTSTR szVal);
	const CDpDBVariant& operator = (bool bVal);
	const CDpDBVariant& operator = (UCHAR ucVal);
	const CDpDBVariant& operator = (short sVal);
	const CDpDBVariant& operator = (WORD sVal);
	const CDpDBVariant& operator = (long lVal);
	const CDpDBVariant& operator = (DWORD lVal);
	const CDpDBVariant& operator = (float fVal);
	const CDpDBVariant& operator = (double dblVal);
	const CDpDBVariant& operator = (TIMESTAMP_STRUCT* tmVal);


	//

public:
	virtual ~CDpDBVariant();

};


/*******************************************************************/
/*                ���ڴ����Ķ������ֶε���
/*******************************************************************/
class CDpLongBinary
{
	public:
		CDpLongBinary();
		~CDpLongBinary();
	public:
		HGLOBAL		m_hData;
		DWORD		m_dwSize;

	public:
		BYTE*		GetBuffer();
		void		ReleaseBuffer();
	private:
		BYTE*		m_pBuff;
};


/*******************************************************************/
/*                     ���ڽ��пռ����ݲ�ѯ����                    */
/*       ���������ʸ�����ݺ�դ�����ݲ�ѯ�Ļ���                    */
/*******************************************************************/
class CDpGeoQuery:public CDpRecordset
{
	public:
		CDpGeoQuery(CDpWorkspace* pWks);
		virtual ~CDpGeoQuery();
	public:
		//���ÿռ������ֶε�����,ȱʡ�Ŀռ��ֶε�����Ϊ"Spatial_Data",
		//���������Ҫ���ÿռ��ֶ���,Ӧ����ִ��Open֮ǰ��������,��Ϊ��OPEN��ʱ���
		//�������õĿռ��ֶ�������ռ��ֶε�������
		void SetGeoFieldName(const char* szFieldName);
		//�õ���ǰGeo��OID
		GUID GetCurrOID(){return m_iCurOID;}
		//�õ���ǰGeo������ͼ��ID
		BOOL GetCurrUnitID(char* szUnitID);

	public:
		//�򿪿ռ��ѯ,szSql-�������ͨSQL�Ĳ�ѯ����,���԰���ORDER BY,CDpSpatialQuery --�ռ��ѯ����
		virtual BOOL Open(const char* szSql,CDpSpatialQuery* pSpaFilter = NULL);
		//�����ƶ�һ����¼
		virtual BOOL Next();

	protected:
		//�����ݿ��е�����ֶ�ֱ�ӵõ���Geo�����
		BOOL GetGeoBound(CDpRect* pRect);
		//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
		BOOL GetGeo(CDpDBVariant& var);
		//�õ���ǰGEO����������
		DP_LAYER_TYPE GetCurGeoType();

	protected:
		CDpWorkspace*		m_pWks;						//�����Ĺ�����
		char				m_szQueryTableName[50];		//�򿪵ı������
		char				m_szGeoFieldName[50];		//�ռ��ֶε�����
		char				m_szSql[8196];				//������ѯ��SQL���
		char				m_szCurUnitID[20];			//��ǰ��¼��ͼ��ID
		UINT				m_iSpatialFieldIndex;		//ʸ���ֶε�������,��OPEN��ʱ��Ӹ���ʸ���ֶ��������ݿ���ȡ��
		UINT				m_iOIDIndex;				//OID���ֶ�������
		UINT				m_iUnitIDIndex;				//ͼ��ID���ֶ�������
		UINT				m_iGeoTypeIndex;			//Shape�����ֶε�������
		GUID				m_iCurOID;					//��ǰ��OID,������ÿ���ƶ���¼ʱ��¼�µ�ǰFeature��OID

	private:
		//�õ���ǰ�����OID
		GUID GetOID();
		//�õ���ǰ��ͼ��ID
		BOOL GetUnitID();
};



/*******************************************************************/
/*                     ���ڽ��пռ����ݲ�ѯ����                    */
/*       �������ֻ�����ڶԿռ����ݵĲ�ѯ,����ʵ�ֿռ����ݵ��޸�   */
/*******************************************************************/
class CDpGeometryQuery:public CDpGeoQuery
{
	public:
		CDpGeometryQuery(CDpWorkspace* pWks);
		virtual ~CDpGeometryQuery();

	public:
		//�õ���ǰGEO����������
		DP_LAYER_TYPE GetCurShapeType();
		//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
		BOOL GetShapeBound(CDpRect* pRect);
		//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
		BOOL GetShape(CDpShape** ppShape);

};


/*******************************************************************/
/*                     ���ڽ��пռ����ݲ�ѯ����                    */
/*       �������ֻ�����ڶԿռ����ݵĲ�ѯ,����ʵ�ֿռ����ݵ��޸�   */
/*******************************************************************/
class CDpRasterQuery:public CDpGeoQuery
{
	public:
		CDpRasterQuery(CDpWorkspace* pWks);
		virtual ~CDpRasterQuery();

	public:
		//�õ���ǰGEO����������
		DP_LAYER_TYPE GetCurRasterType();
		//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
		BOOL GetRasterBound(CDpRect* pRect);
		//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
		BOOL GetRasterFileName(char* pszRasterFileName);
};







/*******************************************************************/
/*                 �������ݷ�����    							   */
/*   ���������ɶԿռ����ݺ��������ݵĲ�ѯ,����Ҳ�ܹ����         */ 
/*      �ռ����ݺ��������ݵ��޸�                                   */ 
/*******************************************************************/
class CDpGeoClass:public CDpGeoQuery
{
	public:
		//FeaClsType -- ��FeatrueClass�ĵ�������
		CDpGeoClass(CDpWorkspace* pWks,DP_LAYER_TYPE GeoClsType,const char* szFeaClsName);
		virtual ~CDpGeoClass();
	public:
		//�򿪿ռ��ѯ,szSql-�������ͨSQL�Ĳ�ѯ����,���԰���ORDER BY,CDpSpatialQuery --�ռ��ѯ����
		//Ŀǰ����FeatureClass�ı�ṹ��ǰ����ֶ�Ӧ����(�����ֶ�˳��):
		//OID,ST_GRID_ID,ST_MINX,ST_MINY,ST_MAXX,ST_MAXY,ST_OBJECT_TYPE,SPATIAL_DATA,ATT_OID
		//����������ֶ�
		virtual BOOL Open(const char* szSql,CDpSpatialQuery* pSpaFilter = NULL);
		//�õ������ֶε���Ŀ
		int GetAttrFieldCount();
		//�õ�ָ�������ֶε�����
		int GetAttrFieldIndex(char* szAttrFieldName);

		//���������ֶε�˳��õ�ָ�������ֶε��������ֶ���Ϣ
		int GetAttrFieldInfoByOrder(int iAttrOrder,CDpFieldInfo* pInfo);
		//ȡ����ֵ,��ȡֵ��ʱ���鰴���ֶε�˳��(��С����)ȥȡ
		//iAttrFieldIndex -- �����ֶ�����,��0��ʼ
		BOOL GetAttrValue(WORD iAttrFieldIndex,CDpDBVariant& var);
		//ȡ����ֵ,��ȡֵ��ʱ���鰴���ֶε�˳��(��С����)ȥȡ
		BOOL GetAttrValue(char* szFieldName,CDpDBVariant& var);
		//��������ֵ,ֻ�����÷Ƕ����Ƶ��ֶ�
		BOOL SetAttrValue(WORD iAttrFieldIndex,CDpDBVariant& var);
		//��������ֵ,ֻ�����÷Ƕ����Ƶ��ֶ�
		BOOL SetAttrValue(char* szFieldName,CDpDBVariant& var);

		
		
	private:
		//�õ������ֶε�˳��
		int GetAttrFieldStart();
		//�жϵ�ǰ�û��Ƿ���Ȩ���޸ĸ�FeatureClass
		BOOL CanModifyRight();


	private:
		int					m_iSpaFieldIndexStart;			//�ռ������ֶεĿ�ʼ����
		int					m_iAttrFieldIndexStart;			//�����ֶεĿ�ʼ����,OID�����������ֶ�,���ڿռ������ֶ�	
		DP_LAYER_TYPE		m_iGeoClsType;					//GeoClass��Geo������
		char				m_szFeaClsName[50];				//��FeatureClass������	

};



/*******************************************************************/
/*                 ���������									   */
/*   ���������ɶԿռ����ݺ��������ݵĲ�ѯ,����Ҳ�ܹ����         */ 
/*      �ռ����ݺ��������ݵ��޸�                                   */ 
/*******************************************************************/
class CDpFeatureClass:public CDpGeoClass
{
	public:
		//FeaClsType -- ��FeatrueClass�ĵ�������
		CDpFeatureClass(CDpWorkspace* pWks,DP_LAYER_TYPE FeaClsType,const char* szFeaClsName);
		virtual ~CDpFeatureClass();
	public:
		//�õ���ǰGEO����������
		DP_LAYER_TYPE GetCurShapeType();
		//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
		BOOL GetShapeBound(CDpRect* pRect);
		//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
		BOOL GetShape(CDpShape** ppShape);
		//�õ�һ��Feature
		BOOL GetFeature(CDpFeature** ppFeature);
		//�����ƶ�һ����¼
		BOOL NextRecord();
};


/***********************************************************************/
/*                  ���ڽ�����������Feature��ʱ��ʹ�õĶ���            */
/***********************************************************************/
class CDpFeatureCache
{

	public:
		CDpFeatureCache(CDpFeatureLayer* pFeaLayer);
		~CDpFeatureCache();

	public:
		//��Feature���������Feature
		BOOL				AddFeature(CDpFeature* pFeature);
		//����ÿ�β�������Ŀ��
		void				SetCountOfPerOper(int iCount);			
		//��ʼ���������
		BOOL				StartBatchOpera(DpBatchOperStatus nStatus);	
		//�������������������
		BOOL				EndAndSaveBatchOpera();						
		//ȡ��������������Ҳ�����
		BOOL				CancelBatchOpera();							

	private:
		//����ڴ�
		void				FreeMem();								
		//�ر�SQL�ֱ�
		void				CloseSqlHandle();
		//�õ���ǰͼ�������ͼ�������OID�Լ�������״̬
		BOOL				GetAllUnitInfo();	
		//�������ݿ��,ͬʱ���仺����,�Լ����ɰ���Ϣ
		BOOL				Bind();	
		//��ռ����ݵĻ������������
		BOOL				FillSpaCache(_DpBindFieldInfo* pInfo,CDpFeature* pFeature,int iRecNum);
		//���������ݵĻ���ȥ���������
		BOOL				FillAttrCache(_DpBindFieldInfo* pInfo,CDpFeature* pFeature,int iRecNum);
		//�������������ݸ��µ����ݿ���
		BOOL				UpdateCacheToDB();


	private:
		CDpFeatureLayer*	m_pFeaLayer;
		DpBatchOperStatus	m_nCurOperStatus;						//��ǰ������״̬
		int					m_iCountOfPer;							//ÿ�β�������Ŀ��
		int					m_iCurItemCount;						//��ǰ�������ڵ���Ŀ��
		int					m_iSpatialSizeOfFtr;					//���㵱ǰҪ������Feature�Ŀռ����ݱ�����İ��ڴ��Ĵ�С
		int					m_iAttrSizeOfFtr;						//���㵱ǰҪ������Feature���������ݱ�����İ��ڴ��Ĵ�С
		SQLSMALLINT			m_iSpaColCount;							//�ռ����ݱ���ֶ���
		SQLSMALLINT			m_iAttrColCount;						//�������ݱ���ֶ���
		BYTE*				m_pSpatialCache;						//ʵ�ʵĻ����ڴ�(�ռ�����)
		BYTE*				m_pAttrCache;							//ʵ�ʵĻ����ڴ�(��������)
		_DpBindFieldInfo*	m_pSpatialBindInfo;						//�ռ����ݱ�İ���Ϣ
		_DpBindFieldInfo*	m_pAttrBindInfo;						//�������ݱ�İ���Ϣ
		SQLHANDLE			m_hSpaStmt;								//�ռ����ݱ��SQL�ֱ�
		SQLHANDLE			m_hAttrStmt;							//�������ݱ��SQL�ֱ�

};



/*******************************************************************/
/*                 դ�������									   */
/*   ���������ɶ�դ�����ݺ��������ݵĲ�ѯ,����Ҳ�ܹ����         */ 
/*      դ�����ݺ��������ݵ��޸�                                   */ 
/*******************************************************************/
class CDpRasterClass:public CDpGeoClass
{
	public:
		CDpRasterClass(CDpWorkspace* pWks,DP_LAYER_TYPE FeaClsType,const char* szRasterClsName);
		virtual ~CDpRasterClass();
	public:
		//�õ���ǰGEO����������
		DP_LAYER_TYPE GetCurRasterType();
		//�����ݿ��е�����ֶ�ֱ�ӵõ��õ�������
		BOOL GetRasterBound(CDpRect* pRect);
		//�õ�����ĵ���,�����ǰû�м�¼���Լ�����¼β,�򷵻�false
		BOOL GetRasterFileName(char* pszRasterFileName);

		//�õ�Raster
		BOOL GetRaster(CDpRaster** ppRaster);
};


/*************************************************************/
/*                  ���������ԵĶ���Ĺ�����
/*************************************************************/
class CDpObject
{
	public:
		CDpObject(int iAttrFieldCount);
		virtual ~CDpObject();
	public:
		//���������ֶ�����,�õ�����ֵ
		BOOL GetAttrValue(UINT uIndex,CDpDBVariant& var);
		//����ָ���ֶ�����������ֵ
		BOOL SetAttrValue(UINT uIndex,CDpDBVariant& var);

		//�õ���ǰFeature��OID
		GUID GetOID();
		//���õ�ǰFeature��OID
		void SetOID(GUID uiOID);

		//�õ���ǰFeature��������ͼ��ID
		const char* GetUnitID();
		//���õ�ǰFeature��������ͼ��ID
		void SetUnitID(const char* szUnitID);

		//�õ���ǰFeature�ڵ������ֶεĸ���
		int	GetAttrFieldCount();

	private:
		GUID								m_iOID;						//OID
		char								m_szUnitID[20];				//����������ͼ��ID
		int									m_iAttrFieldCount;
		CDpDBVariant*						m_pAttrFieldValueArray;		//�����ֶ�ֵ������

};

class CDpObjectEx
{
public:
	CDpDBVariant*		m_pFields;

public:
	CDpObjectEx(int iCount)
	{
		m_pFields = new CDpDBVariant[iCount];
	}
	~CDpObjectEx()
	{
		if (m_pFields)
		{
			delete [] m_pFields;
			m_pFields = NULL;
		}
		
	}

};

/*************************************************************/
/*                 ��������Ĺ�����
/*          ���а��������ʸ�����ݺ���������
/*************************************************************/
class CDpFeature:public CDpObject
{
	public:
		CDpFeature(int iAttrFieldCount);
		virtual ~CDpFeature();
	public:
		//�õ�ʸ������,ֻ�ǵõ�ָ��,ppShape���ڴ�����߲����ͷ�
		BOOL GetShape(const CDpShape** ppShape);
		//����ʸ������,ֻ�ǽ�ָ����и���,�����ǰFeature������Shape,����ɾ��ԭ�е�Shape
		BOOL SetShapeByRef(CDpShape* pShape);

	private:
		CDpShape*							m_pShape;
};


/**************************************************************/
/*                   ����դ��Ĺ�����        
/*         ���а���դ���ļ�������,դ�������Լ���������      
/**************************************************************/
class CDpRaster:public CDpObject
{
	public:
		CDpRaster(int iAttrFieldCount);
		virtual ~CDpRaster();
	public:
		//�õ�դ�����ݵ��ļ���
		BOOL GetRasterFileName(char* szFileName,int iBuffCount);
		//�õ�դ������
		BOOL GetRasterBound(CDpRect* pRtBound);
		//����դ���ļ����ļ���
		BOOL SetRasterFileName(const char* szFileName);
		//����դ������
		BOOL SetRasterBound(CDpRect* pRtBound);

	private:
		char*							m_szRasterFileName;
		CDpRect							m_rtBound;
};



/*******************************************************************/
/*                     ��������������
/*******************************************************************/
class CDpParamMgr
{
	public:
		CDpParamMgr(CDpDatabase* pDB);
		virtual ~CDpParamMgr();

		//�򿪲�����
		BOOL Open(char* szParamTableName);
		//�رղ�����
		void Close();

		//�õ���ǰ�������Ŀ
		int  GetCategoryCount();
		//�õ����������
		BOOL GetCategory(int iIndex,int* iCategoryID,const char** szCategoryName,const char** szCategoryDesc);
		//�õ���ǰ�����в�������Ŀ
		int  GetParamItemCount(int iCategoryID);
		//�Ӳ����ṹ�еõ�����ֵ
		BOOL GetParamValue(const CDpParamItem* pParamItem,CDpDBVariant& var);

		//�õ�������Ϣ
		BOOL GetParamInfoByID(int iParamID,const CDpParamItem** ppParamItem);
	
	protected:
		//�õ�������Ϣ,���а���������������Ϣ��ֵ,pParamItem���ڴ�����߲����ͷ�
		BOOL GetParamInfo(const char* szWksID,int iCategoryID,int iParamIndex,const CDpParamItem** ppParamItem);

		//���ò�������
		BOOL SetParamValue(const char* szWksID,int iParamID,CDpDBVariant var);
		//���ò�������
		BOOL SetParamValue(const char* szWksID,int iParamID,DpCustomFieldType nDataType,const char* szValue);
		//��Ӳ�����
		BOOL AddParamItem(const char* szWksID,int iCategoryID,int iParamID,const char* szParamName,const char* szParamDesc,DpCustomFieldType nDataType,int iLen,int iPrecision,int iSelID,const char* szParamValue,BOOL bIsWHField,int* iNewParamID);
		//ɾ��һ��������,ͬʱ�÷����µĲ�����Ҳ��ɾ��
		BOOL DeleteCategory(const char* szWksID,int iCategoryID);
		//ɾ��һ��������,ͬʱɾ���ò������µ�ѡ��ֵ��,bDelEmptyCategory-��ʾ���ɾ����ķ�����Ϊ�յ�ʱ��,�Ƿ�ɾ���յķ�����,TRUEΪɾ��
		BOOL DeleteParamItem(const char* szWksID,int iParamID,BOOL bDelEmptyCategory);
	

	private:

	protected:
		CDpDatabase*			m_pDB;

		ParamCategoryArray		m_ParamData;				//��������		

		char					m_szParamTableName[50];		//�����������

};






/*******************************************************************/
/*                     ͼ������������
/*******************************************************************/
class CDpParamTable:public CDpParamMgr
{
	public:
		CDpParamTable(CDpWorkspace* pWks)
			:CDpParamMgr(pWks->m_pDB)
		{
			m_pWks		= pWks;
		}


		//�õ�������Ϣ,���а���������������Ϣ��ֵ,pParamItem���ڴ�����߲����ͷ�
		BOOL GetParamInfo(int iCategoryID,int iParamIndex,const CDpParamItem** ppParamItem)
		{
			return CDpParamMgr::GetParamInfo(m_pWks->m_szID,iCategoryID,iParamIndex,ppParamItem);
		}
		//���ò�������
		BOOL SetParamValue(int iParamID,CDpDBVariant var)
		{
			return CDpParamMgr::SetParamValue(m_pWks->m_szID,iParamID,var);
		}
		//���ò�������
		BOOL SetParamValue(int iParamID,DpCustomFieldType nDataType,const char* szValue)
		{
			return CDpParamMgr::SetParamValue(m_pWks->m_szID,iParamID,nDataType,szValue);
		}

		//��Ӳ�����
		BOOL AddParamItem(int iCategoryID,int iParamID,const char* szParamName,const char* szParamDesc,DpCustomFieldType nDataType,int iLen,int iPrecision,int iSelID,const char* szParamValue,int* iNewParamID)
		{
			return CDpParamMgr::AddParamItem(m_pWks->m_szID,iCategoryID,iParamID,szParamName,szParamDesc,nDataType,iLen,iPrecision,iSelID,szParamValue,false,iNewParamID);
		}

		//ɾ��һ��������,ͬʱ�÷����µĲ�����Ҳ��ɾ��
		BOOL DeleteCategory(int iCategoryID)
		{
			return CDpParamMgr::DeleteCategory(m_pWks->m_szID,iCategoryID);
		}

		//ɾ��һ��������,ͬʱɾ���ò������µ�ѡ��ֵ��
		BOOL DeleteParamItem(int iParamID)
		{
			return CDpParamMgr::DeleteParamItem(m_pWks->m_szID,iParamID,true);
		}


	private:	
		CDpWorkspace*			m_pWks;						//������	
};


/*******************************************************************/
/*                    ����ά���������
/*******************************************************************/
class CDpParamWH:public CDpParamMgr
{
	public:
		CDpParamWH(CDpDatabase* pDB)
				:CDpParamMgr(pDB)
		{
		}

		//�õ�������Ϣ,���а���������������Ϣ��ֵ,pParamItem���ڴ�����߲����ͷ�
		BOOL GetParamInfo(int iCategoryID,int iParamIndex,const CDpParamItem** ppParamItem)
		{
			return CDpParamMgr::GetParamInfo(NULL,iCategoryID,iParamIndex,ppParamItem);
		}
		//���ò�������
		BOOL SetParamValue(int iParamID,CDpDBVariant var)
		{
			return CDpParamMgr::SetParamValue(NULL,iParamID,var);
		}
		//���ò�������
		BOOL SetParamValue(int iParamID,DpCustomFieldType nDataType,const char* szValue)
		{
			return CDpParamMgr::SetParamValue(NULL,iParamID,nDataType,szValue);
		}

		//��Ӳ�����
		BOOL AddParamItem(int iCategoryID,int iParamID,const char* szParamName,const char* szParamDesc,DpCustomFieldType nDataType,int iLen,int iPrecision,int iSelID,const char* szParamValue,int* iNewParamID)
		{
			return CDpParamMgr::AddParamItem(NULL,iCategoryID,iParamID,szParamName,szParamDesc,nDataType,iLen,iPrecision,iSelID,szParamValue,true,iNewParamID);
		}

		//ɾ��һ��������,ͬʱ�÷����µĲ�����Ҳ��ɾ��
		BOOL DeleteCategory(int iCategoryID)
		{
			return CDpParamMgr::DeleteCategory(NULL,iCategoryID);
		}

		//ɾ��һ��������,ͬʱɾ���ò������µ�ѡ��ֵ��
		BOOL DeleteParamItem(int iParamID)
		{
			return CDpParamMgr::DeleteParamItem(NULL,iParamID,false);
		}




};












/*******************************************************************/
/*						ʸ��������
/*	�������ĳ������ĵ�����з���
/*******************************************************************/


//ʸ���������ݵĽṹ,�������Ϊ��Ļ�,��iNumParts��iNumPoints������1
//Ŀǰ�����ݿ��еĴ����ṹΪ:

// -----------------------------------------------------------------------------
//     PART��Ŀ  |  Point��Ŀ |   ���   |     PART������    |     Point������
// -----------------------------------------------------------------------------
//     4 BYTE    |   4 BYTE   | 4*8 BYTE |   PART��Ŀ*4 BYTE |  Point��Ŀ*28 BYTE

struct CDpShapeData
{
	int				iNumParts;				//PART��Ŀ
	int				iNumPoints;				//�����Ŀ
	double			dBound[4];				//��Χ���,����ΪMINX,MINY,MAXX,MAXY		
	int*			pParts;					//ÿ��PART����ʼ��ĵ��,������±�Ӧ����iNumParts
	CDpPoint*		pPoints;				//����ĵ�,������±�Ӧ����iNumPoints
};
//�õ�һ��ʸ�����ݵĴ�С
int	GetShapeDataSize(CDpShapeData* pData);

class CDpShape
{
	public:
		CDpShape(DP_LAYER_TYPE iShapeType);	//����ʱ��Ҫָ��SHAPE������
		~CDpShape();
	public:
		//�õ�ʸ������
		DP_LAYER_TYPE GetShapeType();
		//�õ������Ŀ
		int GetPointCount();
		//�õ�PART����Ŀ
		int GetPartCount();
		//ֱ�ӵõ�ʸ�����ݽṹ��ָ��
		BOOL GetData(const CDpShapeData** pData);
		//ֱ�ӵõ�ʸ�����ݽṹ�������ڴ�,���������ݿ��з�������
		BOOL GetData(BYTE** ppBuff,int* cbLen);

		//�õ��㼯����,pt-Ϊ���õ㼯�Ļ�����,iBuffCount--Ϊ��������С(��λ��BYTE)
		BOOL GetPoints(CDpPoint** pt,int* iBuffCount) const;
		//���õ㼯����
		BOOL SetPoints(CDpPoint* pt,int iPointCount);
		//����PART����������
		BOOL SetParts(int* pParts,int iPartCount);
		//����ʸ������
		BOOL SetData(CDpShapeData* pData);
		//����ʸ������
		BOOL CopyData(CDpShapeData* pData);
		//����ʸ������,buff - Ϊ�����ݿ���ȡ��������
		BOOL CopyData(BYTE* pBuff,int iBuffSize);
		//��ʼ��ʸ�����ݽṹ
		void InitData(int iPartNum,int iPointNum);
		//����ʸ��������ռ���ڴ�Ĵ�С
		int  GetDataSize();

		//����㼯
		void ClearPoints();

	private:
		DP_LAYER_TYPE	m_iShapeType;
		CDpShapeData*	m_pData;		//ʸ������
};


/*******************************************************************/
/*						�ռ����ݲ�ѯ��
/*	���������ڽ��пռ��ѯʱ����ռ��ѯ����			
/*******************************************************************/
class CDpSpatialQuery
{
	public:
		//���ò�ѯ����
		void SetQueryCont(DP_GETUNIT_FILTER nFilter);
		//���ò�ѯ�ĵȼ�:1.�򵥲�ѯ,��ֻͨ���Ե����������бȽ������,�򵥲�ѯ���ٶȿ쵫���Ȳ�
		//               2.�߼���ѯ,���ݵ����ÿ���˵����ж�,�߼���ѯ���ȸߵ��ٶ���
		void SetQueryLevel(DP_SPATIAL_QUERY_LEVEL nLevel);
		//���ò�ѯ�ķ�ʽ
		void SetQueryType(DP_GETUNIT_FILTER nFilter);
		//���ò�ѯ��Ŀ�������,bRect -- Ϊtrueʱ,��ʾ��ѯ��ΧΪ����,Ϊfalseʱ,��ʾ��ѯ��ΧΪ�����
		void SetQueryBoundType(BOOL bRect = true);
		//���ò�ѯ�ķ�Χ
		void SetQueryBound(RECT* rt);
		//���ò�ѯ�ķ�Χ 
		void SetQueryBound(POINT* pt,int iPtCount);
};








//����ֵ��ѯ�е��ֶι��������Ľṹ
struct	_DpUniqueFilter
{
	char*			szFieldName;			//�ֶ���
	char*			szFilter;				//��������,�����������ΪNULL��Ϊ���ִ�,���ʾȡ���ֶε�����ֵ
	DpCustomFieldType	nFilterFieldType;	//Ҫ���˵ĸ��ֶε���������
};


/***************************************************************************/
/*				�������ֵ������������,ͬʱ��������������ֵ��ʱ��Ҳ��
/*              ��Ҫ����¼�µ������ֶε�ֵszFilter�ڱ�������µ�����ֵ
/***************************************************************************/
class CDpUniqueFilter
{
	private:
		typedef vector<_DpUniqueFilter*> FilterArray;	//��������������
		FilterArray		m_nFilterArray;
	public:
		CDpUniqueFilter();
		~CDpUniqueFilter();

		//��ӹ�������
		BOOL AddFilter(const char* szFieldName,const char* szFilter,DpCustomFieldType nType);
		//���ͼ����������,nUnitIDArray-��ΪҪ������ֵ���ʵ�ͼ����ID
		BOOL AddUnitFilter(CDpStringArray& nUnitIDArray);
		//ɾ����������-��������
		BOOL RemoveFilter(int iIndex);
		//ɾ����������-�����ֶ���
		BOOL RemoveFilter(const char* szFieldName);
		//ɾ�����еĹ�������
		BOOL RemoveAllFilter();
		//�õ���������-��������
		BOOL GetFilter(int iIndex,const _DpUniqueFilter** ppcFilter);
		//�õ���������--��������
		BOOL GetFilter(const char* szFieldName,const _DpUniqueFilter** ppcFilter);
		//�õ�������������Ŀ
		int GetFilterCount(){return m_nFilterArray.size();}
		//�������еĹ��������õ����Ϲ���������ͼ��
		BOOL MakeLayerFilterWhereClause(char* szLayerFilterSql,int iBuffCount);
		//�������Ա����͹����������ɵõ������Ա���Ϲ��������Ķ���ֵ��¼��SQL���
		BOOL MakeGetUniqueValueRecordSql(const char* szLayerID,char* szGetRecordSql,int iBuffCount);
		//�������Ա����͹����������ɵõ������Ա���Ϲ��������Ķ���ֵ��¼����SQL���
		BOOL MakeGetUniqueValueCountSql(const char* szLayerID,char* szGetCountSql,int iBuffCount);

		//�õ����˶���ֵ��¼��WHERE�Ӿ� 
		BOOL MakeUniqueFilterWhereClause(char* szRecordFilterWhereClause,int iBuffCount);

	private:
		//���������������ִ�. [in]szFilterStr--���������ִ� [out]szResult--�������
		//                    [in]iBuffer    --szResult�Ļ�������С
		BOOL AnalyseFilterString(const char* szFilterStr,char* szResult,int iBuffer,DpCustomFieldType nType);

};



//��ѯ��ǰ���������������Ա����ж��ٲ�ͬ�������ֶεĽṹ
class CDpUniqueFieldResult
{
	public:
		CDpUniqueFieldResult()
		{

		}
		~CDpUniqueFieldResult()
		{
			RemoveAllField();
		}
	public:
		//���һ�������ֶε��������
		BOOL AddField(CDpCustomFieldInfo* pInfo);
		//ɾ������������е��ֶβ��ͷ����ڴ�
		BOOL RemoveAllField();
		//�õ��������ָ�����ֶε�����
		BOOL GetField(int iIndex,const CDpCustomFieldInfo** ppInfo);
		//�õ�������еļ�¼��
		int  GetCount(){return m_nResultArray.size();}
	private:
		typedef vector<CDpCustomFieldInfo*> UniqueFieldResultArray;	//�����ֶβ�ѯ���������
		UniqueFieldResultArray		m_nResultArray;
};




//����ֵ��ѯ������ͼ��Ľ���Ľṹ,������ͼ���з��������Ķ���ֵ��ϵļ�¼
struct _DpUniqueQueryLayersResult
{
	char*			szLayerID;				//ͼ��ID
	char*			szLayerName;			//ͼ������
	int				iCount;					//���еķ��Ϲ��������Ķ���ֵ�ĸ���,����ͬ��ϵĸ���
};


//����ֵ��ѯ������ͼ��Ľ������,������ͼ���з��������Ķ���ֵ��ϵļ�¼
class CDpUniqueQueryLayerResult
{

	public:
		CDpUniqueQueryLayerResult()
		{

		}
		~CDpUniqueQueryLayerResult()
		{
			RemoveAllResult();	
		}


		//�����������ͼ��Ľ��
		BOOL AddLayerResult(const char* szLayerID,const char* szLayerName,int iCount);
		//ɾ�����еĽ�������ͷ��ڴ�
		BOOL RemoveAllResult();
		//ȡ��ָ����ͼ����
		BOOL GetResult(int iIndex,const _DpUniqueQueryLayersResult** ppcResult);
		//�õ���ǰ�ļ�¼���ļ�¼��
		int GetCount(){return m_nResultArray.size();}

	private:
		typedef vector<_DpUniqueQueryLayersResult*> UniqueLayerResultArray;	//����ֵ��ѯ���������
		UniqueLayerResultArray		m_nResultArray;

};


typedef vector<CDpDBVariant*>	COLUMN_VALUE_ARRAY;


struct _DpUniqueQueryValueResult
{
	COLUMN_VALUE_ARRAY*	pColumnArray;			//�ֶ�ֵ 
	int					iRecCount;				//��¼��
};



//����ֵ��ѯ��ĳ������ͼ��Ķ���ֵ����ϵļ�¼�Ľṹ
class CDpUniqueQueryValueResult
{
	public:
		CDpUniqueQueryValueResult()
		{

		}
		~CDpUniqueQueryValueResult()
		{
			RemoveAllResult();
		}

		//���¼�������Ӽ�¼
		BOOL AddResult(COLUMN_VALUE_ARRAY* pRst,int iRecCount);					
		//ɾ����¼�������еļ�¼
		BOOL RemoveAllResult();									
		//�õ�ָ���ļ�¼
		BOOL GetResult(int iIndex,const _DpUniqueQueryValueResult** ppRst);
		//�õ���¼��
		int  GetCount(){return m_nRecordArray.size();}

	private:	
		typedef vector<_DpUniqueQueryValueResult*> UNIQUE_RECORDSET;//����ֵ��ϵļ�¼ֵ
		UNIQUE_RECORDSET	m_nRecordArray;
};


/********************************************************************/
/*   
/*                            ����ֵ������                     
/*   ���ڸ���N�������ֶ���ȥ���Ҿ�����N���������Ĳ�,Ҳ���Ը�����N��
/*   �����ֶεĹ�������ȥ�������в��з�����N�������Ĳ��Լ�����ļ�¼
/********************************************************************/
class CDpUniqueQuery
{
	
	public:
		CDpUniqueQuery(CDpWorkspace* pWks);
		~CDpUniqueQuery();
	public:
		//�õ���ǰ���ݿ������в�����������ֶ�,���а���ά���������ֶκͲ㶨��������ֶ�
		BOOL GetAllAttributeFields(CDpUniqueFieldResult& nFieldsInfo);

		//���������ֶ������б���Ҿ�����������ֶεĲ�
		//����˵��: [in]  nFilter			--  Ҫ���ҵ��ֶεĹ�������
		//          [out] nResultArray  	--  ���ҵĽ��
		//          [in]  bIsGetCountOfLayer -- �Ƿ�õ������ÿ��ͼ���ڵĶ���ֵ�ĸ���
		BOOL FindLayers(CDpUniqueFilter* pFilter,CDpUniqueQueryLayerResult& nResultArray,BOOL bIsGetCountOfLayer = true);

		//�򿪾����ĳ���Ѿ��������˵�ͼ���е����ж���ֵ����ϵĽ���ļ�¼
		//����˵��: [in]  nFilterArray		-- Ҫ���ҵ��ֶεĹ�������
		//          [in]  szLayerID			-- ���ڵ�ͼ���ID
		//			[out] nRecordset		-- ���ؾ���ļ�¼��
		BOOL OpenUniqueRecordInLayer(CDpUniqueFilter* nFilter,const char* szLayerID,CDpUniqueQueryValueResult& nRecordset);

	private:
		CDpWorkspace*	m_pWks;
};


/***********************************************************/
/*     ���ڹ���Ҫ����CHECKOUT��CHECKIN�ľ�������ݵ��б�   */
/***********************************************************/
class CDpCheckInfo
{
	public:
		//ҪCHECKOUT�������б�
		struct	_DpCheckOpreaInfo
		{
			char*			szUnitID;			//ͼ��ID����Ӱ��ID
			char*			szLayerID;			//ͼ��ID��XML��ǩ
			int				iDataType;			//���������ݵ�����,1-UNIT,2-IMAGE
			DP_LOCK_TYPE	nLockType;			//��GET�Ĺ�����Ҫ���GET�������״̬
			BOOL			bSuccess;			//�����Ƿ�ɹ�,TRUE-��ʾ�ɹ�,FALSE-ʧ��
			char*			szErrorMsg;			//������Ϣ����
		};


	public:
		CDpCheckInfo(DpCheckType nCheckType);
		~CDpCheckInfo();

		//ȡ��ǰ��GET����,����CHECKOUT����CHECKIN 
		DpCheckType GetCheckType(){return m_nCheckType;}

		//ɾ�������б���
		void RemoveAllItem();

		//�õ��ܹ�Ҫ������ITEM�ĸ���
		int GetItemCount();

		//�õ���ǰ���е�Ҫ�����Ĳ�ͬ��ͼ��ID
		void GetAllDistinctLayer(CDpStringArray& idArray);

		//�õ���ǰ���е�Ҫ�����Ĳ�ͬ��Ӱ��ID
		void GetAllDistinctImage(CDpStringArray& idArray);

		//�õ���ǰ���е�Ҫ�����Ĳ�ͬ��ͼ��ID
		void GetAllDistinctUnit(CDpStringArray& idArray);
		
		//�õ�ָ����ŵ�ITEM
		BOOL GetItem(int iIndex,const _DpCheckOpreaInfo** ppInfo);

		//���Ҫ������XML��ǩ
		void AddImageXmlItem(const char* szImageID,const char* szXmlTagName,DP_LOCK_TYPE nLockType = DP_UT_UNLOCK);
		//���Ҫ������ͼ����ͼ��
		void AddUnitLayer(const char* szUnitID,const char* szLayerID,DP_LOCK_TYPE nLockType = DP_UT_UNLOCK);
		//����ǰͼ���е�����ͼ�㶼���뵽Ҫ��������������
		BOOL AddUnit(CDpWorkspace* pWks,const char* szUnitID,DP_LOCK_TYPE nLockType = DP_UT_UNLOCK,BOOL bDlgOnly = true);

	private:
		typedef vector<_DpCheckOpreaInfo*> CHECKARRAY;
		typedef map<ULONGLONG,int> DISTINCTLAYER;		
		typedef map<ULONGLONG,int> DISTINCTIMAGE;
		typedef map<ULONGLONG,int> DISTINCTUNIT;

		CHECKARRAY			m_CheckArray;		//ҪCHECK����Ϣ���б�
		DISTINCTLAYER		m_DistinctLayer;	//ҪCHECK�Ĳ�ͬͼ����б�
		DISTINCTIMAGE		m_DistinctImage;	//ҪCHECK�Ĳ�ͬ��Ӱ����б�
		DISTINCTUNIT		m_DistinctUnit;		//ҪCHECK�Ĳ�ͬ��ͼ�����б�

		DpCheckType			m_nCheckType;		//CHECK������

};

struct _DPWP_Param
{
	int		iPrecent;
	BOOL	bIsBreak;
	void*	pWnd;
};

typedef void (CALLBACK *PWORKPROCESS) ( _DPWP_Param* );

/*************************************************************************/
/*                          CHECK_OUT������                       
/* ���ڹ����������е�����CHECK_OUT���������ݿ��ļ�                     
/*************************************************************************/
class CDpCheckOutMgr
{
	public:
		//pWks -- ���ݿ�������ϵ�ĳ���Ѵ򿪵Ĺ�����,������CheckOut
		//szTemplateFileName -- �����ļ����ݿ��ģ�����ݿ��ļ���,�����ڴ����ձ����ļ�ʱʹ��
		CDpCheckOutMgr(CDpWorkspace* pWks,const char* szTemplateFileName = NULL);
		~CDpCheckOutMgr();
	public:
		//ִ��һ��CHECKOUT����
		//szFileName--CheckOut�����ص��ļ����ݿ������
		//pInfo--ҪGET�ľ������ݵ�ID�Ѿ���־����Ϣ
		BOOL GetOut(const char* szFileName,CDpCheckInfo* pInfo,PWORKPROCESS pFun = NULL,_DPWP_Param* pParam = NULL);	
		
	private:
		//�жϵ�ǰ�������ݿ��ļ��Ƿ����
		BOOL DBFileIsExist(const char* szFileName);
		//���ɿյı������ݿ��ļ�
		BOOL MakeEmptyDBFile(const char* szFileName);


		//��Ŀ�����ݿ��ڸ��Ƶ�ǰ�û���Ȩ������
		BOOL CopyUserInfoToDest(SQLHANDLE hConnDest);
		//��Ŀ�����ݿ��ڸ�����������������
		BOOL CopyLockMgrDataToDest(SQLHANDLE hConnDest);
		//��Ŀ�����ݿ��ڸ��Ʋ���ά������������
		BOOL CopyParamWhDataToDest(SQLHANDLE hConnDest);	
		//��Ŀ�����ݿ��ڸ��������ֶ�ά������������
		BOOL CopyAttrWhDataToDest(SQLHANDLE	hConnDest);
		//��Ŀ�����ݿ��ڸ���WBS��ά����Ϣ
		BOOL CopyWBSWhDataToDest(SQLHANDLE hConnDest);

		//��Ŀ�걾�����ݿ��и��ƹ�������¼
		BOOL CopyWksDataToDest(SQLHANDLE hConnDest);
		//��Ŀ�����ݿ��и���ͼ�������
		BOOL CopyGridMgrDataToDest(SQLHANDLE hConnDest,CDpUnit* pUnit = NULL);
		//��Ŀ�����ݿ��и���Ӱ������
		BOOL CopyImageMgrDataToDest(SQLHANDLE hConnDest,const char* szImageID = NULL);
		//��Ŀ�����ݿ��и���Ӱ����XML�����ݱ�
		BOOL CopyImageXmlDataToDest(SQLHANDLE hConnDest,CDpImage* pSourceImage);
		//��Ŀ�����ݿ��и���ָ����Ӱ����XML������
		BOOL CopyImageXmlDataToDest(SQLHANDLE hConnDest,const char* szImageID,const char* szXmlTagName);


		//��Ŀ�����ݿ��ڸ��������ֵ����������
		BOOL CopyAttrFieldDircToDest(SQLHANDLE hConnDest);	
		//��Ŀ�����ݿ⸴�Ʋ�����,���а����ȴ���������󿽱���¼
		BOOL CopyParamTableToDest(SQLHANDLE hConnDest,const char* szParamTableName);
		//��Ŀ�����ݿ��и���ͼ������
		BOOL CopyLayerDataToDest(SQLHANDLE hConnDest);

		//��Ŀ�����ݿ��ڴ����յĿռ����ݱ�
		//�ڴ�����֮ǰ���жϸñ���Ŀ�����ݿ����Ƿ��Ѿ�����,������ھͲ����д�������
		BOOL CreateEmptySpatialTableInDest(SQLHANDLE hConnDest,const char* szSpaTableName);
		//��Ŀ�����ݿ���ָ���Ŀռ����ݱ���������ݱ��и��Ƽ�¼
		BOOL CopyFeatureRecordToDest(SQLHANDLE hConnDest,const char* szLayerID,const char* szGridID);

		//��Ŀ�����ݿ��ڴ����յ��������ݱ�
		//�ڴ�����֮ǰ���жϸñ���Ŀ�����ݿ����Ƿ��Ѿ�����,������ھͲ����д�������
		BOOL CreateEmptyAttributeTableToDest(SQLHANDLE hConnDest,CDpLayer* pSourceLayer);


	private:
		CDpWorkspace*		m_pWks;
		char				m_szTemplateFileName[_MAX_PATH];
		
};


/*************************************************************************/
/*                          CHECK_IN������                       
/* ���ڹ���Դ�ļ����ݿ��е�����CHECK_IN�����ݿ��������
/*************************************************************************/
class CDpCheckInMgr
{
	public:
		//pSourceWks -- ��ҪCHECKIN��Դ���ݿ�Ĺ�����,Ŀǰ�򿪵�ACCESS���ļ����ݿ�
		//pDestWks   -- ��ҪCHECKIN��Ŀ�����ݿ�Ĺ�����,Ŀǰ�򿪵�ORACLE�����ݿ������
		CDpCheckInMgr(CDpWorkspace* pSourceWks,CDpWorkspace* pDestWks);
		~CDpCheckInMgr();
	public:
		//���Դ�������ڹ����ʱ��û�д�,��ô�ͱ����ڴ˴���
		BOOL OpenSourceWks(const char* szDBFile,const char* szWksID = NULL);

		//ִ��CHECK����
		BOOL CheckIn(CDpCheckInfo* pInfo,PWORKPROCESS pFun = NULL,_DPWP_Param* pParam = NULL);

		//����Ӱ��XML����
		BOOL CopyImageXmlDataToDest(const char* szImageID,const char* szXmlTagName);
	
		//����ͼ��ͼ������
		BOOL CopyUnitLayerDataToDest(const char* szUnitID,const char* szLayerID);

	private:
		CDpWorkspace*		m_pSourceWks;
		CDpWorkspace*		m_pDestWks;
		CDpDatabase*		m_pSourceDB;

};



/********************************************************/
/*                ��Ĺ������Թ������                  */
/********************************************************/
class CDpFixedClassAttrMgr
{
public:
	CDpFixedClassAttrMgr(CDpDatabase* pDB);
	~CDpFixedClassAttrMgr();

	typedef vector<CDpCustomFieldInfo*> DP_CLASSFIELDARRAY;

	//��Ľṹ
	struct _DpFixedClass
	{
		_DpFixedClass()
		{
			iClassID		= 0;
			szClassName		= NULL;
			szDescName		= NULL;
			iClassType		= 0;
			iParentClassID	= 0;
		}

		int					iClassID;				//��ID
		char*				szClassName;			//����
		char*				szDescName;				//������
		int					iClassType;				//������� 
		int					iParentClassID;			//��ĸ���ID
		DP_CLASSFIELDARRAY	nFieldArray;			//�����ڵ��ֶ�����
	};

	typedef vector<_DpFixedClass*>	DP_FIXEDCLASS_ARRAY;


	
	BOOL		LoadDataFromDB();

	int			GetClassCount();

	const _DpFixedClass* GetClassByIndex(int iIndex);
	const _DpFixedClass* GetClassByID(int iClassID);

private:
	void					FreeMem();
	DP_FIXEDCLASS_ARRAY		m_nClassArray;
	CDpDatabase*			m_pDB;

};



/********************************************************/
/*                �����ֶ�ά���������                  */
/********************************************************/
class CDpAttrWHMgr
{
	public:
		CDpAttrWHMgr(CDpDatabase* pDB);
		~CDpAttrWHMgr();

		typedef vector<CDpCustomFieldInfo*>	DP_WHFIELDARRAY;

		//ά��������ṹ
		struct _DpAttrWHGroup
		{
			char*				szAttrGroupName;		//������
			char*				szAttrGroupDesc;		//������	
			DP_WHFIELDARRAY		nFieldArray;			//�����ֶ���Ϣ����
		};

		typedef vector<_DpAttrWHGroup*>	DP_ATTRWHGROUP_ARRAY;

		DP_ATTRWHGROUP_ARRAY	m_GroupArray;

		//�����ݿ��м�������,�����Ҫ���ع��������еĲ�ͬ�ֶ���Ϣ,��pWks����Ϊ��
		//�������Ҫ���ع������������ֶ���Ϣ,��pWks����ΪNULL
		BOOL LoadDataFromDB(CDpWorkspace* pWks = NULL);

		//�ͷ��ڴ�
		void FreeMem();

		//ȡ��ĸ���
		int	 GetGroupCount();

		//ȡ�������
		const _DpAttrWHGroup* GetGruop(int iIndex);

		//ȡ���ڵ��ֶ���Ŀ
		int	 GetFieldItemCount(const _DpAttrWHGroup* pGroup); 

		//ȡ���ھ�����ֶ���Ϣ
		const CDpCustomFieldInfo* GetFieldInfo(const _DpAttrWHGroup* pGroup,int iIndex);


	private:
		CDpDatabase*			m_pDB;
};



#endif //_DBVISIT712304823904802398490238409238





