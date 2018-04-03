
/**************************************************************************************************
 This is a part of the Visiontek MapMatrix family.
 Copyright (C) 2005-2010 Visiontek Inc.
 All rights reserved.

 Module name:	数据访问中间层接口定义(GDMI)
 Author:		李文强
 Description:	用于对空间数据库进行访问的接口

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
#include <map>					//使用HASH表
//#include <algorithm>
using namespace std;

BOOL SetLargerFieldValue(SQLHANDLE hConn,char* szSql,BYTE* buff,int iBuffLen);

//从一个查询的SQL语句中得到要查询的表名
BOOL GetTableNameFromSql(char* szSql,char* szTableName);

BOOL AddAccessRights(const TCHAR *lpszFileName, const TCHAR *lpszAccountName,DWORD dwAccessMask);

GUID GUIDFromString(LPCTSTR text);

CString GUIDToString(GUID id);

BOOL IsZeroGUID(GUID *id);

GUID NewGUID();

#define ZeroMem(a) memset(&(a),0,sizeof(a))

//进行二进制写入的时候,一个缺省的块的大小
#define DEFAULT_WRITE_CHUNK		8192
//进行二进制读取的时候,一个缺省的块的大小
#define DEFAULT_READ_CHUNK		8192


//工作区ID的顺序号的最大值
#define MAX_WORKSPACE_ID		998
//任务ID的顺序号的最大值
#define MAX_TASK_ID				99998
//图幅ID的顺序号的最大值
#define MAX_UINT_ID				9998
//图层ID的顺序号的最大值
#define MAX_LAYER_ID			9998
//矢量地物OID的最大值
#define MAX_OID					0xFFFFFFF0

//工作区ID的长度
#define WKS_ID_LEN				14
//图幅ID的长度
#define UNIT_ID_LEN				19
//图层ID的长度		
#define LAYER_ID_LEN			19
//影像ID的长度
#define IMAGE_ID_LEN			19
//任务ID的长度
#define TASK_ID_LEN				19

//缺省的进行二进制数据交换时,内存块的大小,缺省为512K,即限制一个地物的点数不能超过18700个
#define DEFAULT_BINARY_BLOCK_SIZE	524288


//执行SQL语句的返回码
enum DP_EXECSQL_RETURNCODE
{
	DP_SQL_SUCCESS		= 0,			//执行成功
	DP_SQL_STAT_ERR		= 1,			//SQL语句错误
	DP_SQL_NOT_SUPPORT	= 2,			//SQL语句不支持
	DP_SQL_DBRIGHT_ERR	= 3,			//用户权限不够(指数据库级别)
	DP_SQL_DPRIGHT_ERR	= 4				//用户权限不够(指应用级)
};

//连接的数据库的类型
enum DP_DATABASE_TYPE
{
	DP_NULL				= 0,			//没有打开数据库
	DP_ACCESS			= 1,			//ACCESS数据库
	DP_ORACLE			= 2,			//ORACLE数据库
	DP_SQLSERVER		= 3
};

//工作区类型
enum DP_WORKSPACE_TYPE
{
	DP_WKS_FULL			= 1,			//完整工作区
	DP_WKS_PART			= 2,			//部分工作区
	DP_WKS_ANY			= 3,			//可以是任意的类型工作区
	DP_WKS_IMAGEPRO		= 4             //houkui,06.6.27,给ImagePro用，增加模型数据处理
};

//取图幅的过滤条件
enum DP_GETUNIT_FILTER
{
	GETUNIT_Intersect	= 0x01,			//相交
	GETUNIT_Include		= 0x02			//完全包含

};

//空间查询的等级
enum DP_SPATIAL_QUERY_LEVEL
{
	Level_Simple		= 0x01,			//简单查询,只根据地物的外包来判断
	Level_Advance		= 0x02,			//复杂查询,根据地物的每个端点来判断
};

//当前的管理对象的类型
enum DP_OBJECT_TYPE
{
	DP_OBJ_NULL			= 0,
	DP_OBJ_UNIT			= 1,			//图幅
	DP_OBJ_LAYER		= 2,			//图层
	DP_OBJ_IMAGE		= 3				//原始影像
};


//图层的类型
enum DP_LAYER_TYPE
{
	GEO_NULL			= 0,			//空类型
	GEO_Point			= 1,			//点状图层
	GEO_MultiPoint		= 2,			//MultiPoint
	GEO_Line			= 3,			//线状图层
	GEO_CircularArc		= 4,			//一个圆的外包
	GEO_EllipticArc		= 5,			//一个椭圆的外包
	GEO_Bezier3Curve	= 6,			//贝塞尔曲线,4个控制点
	GEO_Polyline		= 7,	
	GEO_Ring			= 8,			//环
	GEO_Polygon			= 9,			//多边形
	GEO_Annotation		= 91,			//注记,注记的实际的矢量数据类型是Polygon,但是注记层的CDpShape对象的类型还是用GEO_Annotation
	GEO_Raster			= 10			//栅格
};

//图层扩展属性
enum DP_LAYER_TYPE_EX
{
	LAYER_TYPE_EX_NULL	= 0,			//空类型

	LAYER_TYPE_EX_IOP	= 11,			//IOP,houkui,06,7,13
	LAYER_TYPE_EX_AOP	= 12,			//AOP,houkui,06,7,13
	LAYER_TYPE_EX_IMAGEPOINT= 13,		//IMAGEPOINT,houkui,06,7,13

	LAYER_TYPE_EX_DEM	= 21,			//DEM
	LAYER_TYPE_EX_DOM	= 22,			//DOM
	LAYER_TYPE_EX_DLG	= 23,			//DLG
	LAYER_TYPE_EX_DRG	= 24,			//DRG
	LAYER_TYPE_EX_DVS	= 25			//DVS
};


//游标类型
enum DP_CURSOR_TYPE
{
	DP_ForwardOnly		= SQL_CURSOR_FORWARD_ONLY,			//方向向前并且只读的游标
	DP_Dynaset			= SQL_CURSOR_KEYSET_DRIVEN,			//动态游标,即CURSOR_KEYSET_DRIVEN
	DP_Dynamic			= SQL_CURSOR_DYNAMIC,				//动态游标,但有可能有些ODBC驱动不支持
	DP_Snapshot 		= SQL_CURSOR_STATIC					//静态游标
};

//图幅的类型
enum DP_LOCK_TYPE
{
	DP_UT_ANY			= 0,			//任意状态	
	DP_UT_UNLOCK		= 1,			//未被锁定的图幅
	DP_UT_LOCKED		= 2				//被锁定的图幅
};

//登陆用户的类型
enum DP_USER_RIGHTTYPE
{
	DP_NORMAL			= 0,			//普通用户
	DP_ADMIN			= 1				//系统管理员
};

//用户权限
enum DP_RIGHT
{
	DP_RIGHT_NOTRIGHT	= 0,			//没有访问的权限
	DP_RIGHT_READONLY	= 1,			//只读权限
	DP_RIGHT_READWRITE	= 2				//读写权限
};


//WBS CODE 的A部分的枚举
enum DpAPartType
{
	DP_A_NULL			= 0,
	DP_A_IMAGE			= 1,	
	DP_A_UNIT			= 2,
	DP_A_STEREO			= 3 //标识模型对象，对应在CDpUnit中的成员m_iType,houkui,06.6.23,
							//对应在CDpUnit中的成员m_iType
};


//WBS CODE 的C部分的枚举
/*
enum DpCPartType
{
	DP_C_NULL			= 0,//
	DP_C_IOP			= 1,//内定向	
	DP_C_AOP			= 2,//绝对定向
	DP_C_IMAGEPOINT		= 3,//影像控制点
	DP_C_DEM			= 4,//数字高层模型
	DP_C_DOM			= 5,//数字正射模型
	DP_C_DLG			= 6,//数字线画图
	DP_C_DRG			= 7,//数字栅格图
	DP_C_DVS			= 8 //数字可视场景
};
*/

//WBS CODE 的D部分的枚举
/*
enum DpDPartType
{
	DP_D_NULL				= 10,//

	DP_D_PRODUCE			= 11,//生产	
	DP_D_CHECK				= 12,//检查

	DP_D_DEM_GENERATE		= 13,//生成数字高层模型,DP_D_DEM生成
	DP_D_DEM_EDIT			= 14,//编辑数字高层模型,DP_D_DEM编辑
	DP_D_DEM_MOSAIC			= 15,//数字高层模型接边,DP_D_DEM接边
	DP_D_DEM_CHECK			= 16,//检查数字高层模型,DP_D_DEM检查

	DP_D_DOM_GENERATE		= 17,//生成数字正射模型,DP_D_DOM生成
	DP_D_DOM_EDIT			= 18,//修补数字正射模型,DP_D_DOM修补
	DP_D_DOM_DODGE			= 19,//数字正射模型匀光,DP_D_DOM匀光
	DP_D_DOM_MOSAIC			= 20,//拼接数字正射模型,DP_D_DOM拼接
	DP_D_DOM_CUT			= 21,//裁切数字正射模型,DP_D_DOM裁切

	DP_D_DLG_COLLECT		= 22,//数字线画图采集,DP_D_DLG采集
	DP_D_DLG_EDIT			= 23,//数字线画图编辑,DP_D_DLG编辑
	DP_D_DLG_MOSAIC			= 24,//数字线画图接边,DP_D_DLG接边
	DP_D_DLG_CUT			= 25,//裁切数字正射模型,DP_D_DLG裁切
	DP_D_DLG_MAPPING		= 26,//数字线画图图廓整饰,DP_D_DLG图廓整饰
	DP_D_DLG_TOPOLOGY		= 27,//数字线画图拓扑,DP_D_DLG拓扑
	DP_D_DLG_CHECK			= 28,//数字线画图采集,DP_D_DLG检查

	DP_D_DRG_PRINT			= 29,//数字栅格图出图,DP_D_DRG出图
	DP_D_DRG_CHECK			= 30,//数字栅格图检查,DP_D_DRG检查

	DP_D_DVS_GENERATE		= 31,//数字可视场景矢量模型建立,DP_D_DVS矢量模型建立
	DP_D_DVS_TEXTURE		= 32,//数字可视场景DVS纹理提取,DP_D_DVS纹理提取
	DP_D_DVS_SCENE			= 33//DVS漫游场景生成,DP_D_DVS漫游场景生成
};
*/


//GET的类型
enum DpCheckType
{
	DP_NULLOPERA		= 0,			//空操作
	DP_CHECKOUT			= 1,			//CHECKOUT
	DP_CHECKIN			= 2				//CHECKIN
};

//批量处理Feature的缓冲操作的状态
enum DpBatchOperStatus
{
	DP_NOOPERA			= 0,			//没有任何操作
	DP_BATCHADD			= 1,			//批量添加操作 
	DP_BATCHUPDATE		= 2				//批量更新地物操作
};


//用户信息的结构
struct _DpUserInfo
{
	char*					szUserID;		//用户ID
	char*					szUserDesc;		//用户描述
	DP_USER_RIGHTTYPE		nUserType;		//用户类型
};



//字段类型结构
struct CDpFieldInfo
{
   char		m_strName[80];				//字段名称
   short	m_nSQLType;					//字段类型
   SQLULEN	m_iSize;					//字段的的大小
   short	m_nPrecision;				//字段精度,用于数字型的字段,即小数的位数
   BOOL		m_bAllowNull;				//是否允许为空,为TRUE表示允许为空,为FALSE表示不允许为空
};

//自定义的属性字段类型的字段信息,在创建自定义的属性字段信息中,不用考虑m_nSQLType这个属性,
//因为在创建时根据m_CustomFieldType来判断字段的具体的数据库的
//在通过ODBC取出属性表中具体的某个自定义的属性字段类型时,m_nSQLType含有该字段本身的数据库内
//数据类型
struct CDpCustomFieldInfo:public CDpFieldInfo
{
   char		m_szAliasName[80];			//字段别名
   DpCustomFieldType m_CustomFieldType;	//对应的自定义字段类型
   BOOL		m_bIsUnique;				//是否允许重复,TRUE表示字段为UNIQUE字段,即不允许重复,FALSE表示允许重复
   int		m_iParent;					//该属性字段所属的固有派生体系的派生类的ID,如果是动态属性,则为0	
};


//点结构
struct CDpPoint
{
   double	m_dx;						//X坐标
   double	m_dy;						//Y坐标 
   double	m_dz;						//Z坐标
   int		m_iExtensionCode;			//点的扩展码
};




//参数选择值结构,即当参数有枚举的值的话,该结构存贮枚举内容
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


	int					iSelectID;					//选择ID
	int					iValueID;					//选择值的ID
	char*				szSelectValue;				//选择值
};
typedef vector<CDpParamSelectItem*> SelectItemArray;

//参数结构  adf
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

	int					iParamID;					//参数ID
	char*				pszParamName;				//参数名称
	char*				pszParamDesc;				//参数描述
	DpCustomFieldType	nParamDataType;				//参数数据类型
	int					iParamDataLen;				//参数数据长度
	int					iParamPrecision;			//参数如果为DOUBLE的话,保留小数的位数
	char*				pszValue;					//参数值
	SelectItemArray		pSelectItem;				//该参数关联的选择值,如果没有则该VECTOR的项数为零 
};
typedef  vector<CDpParamItem*> ParamItemArray;

//参数分类
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


	int					iCategoryID;				//分类ID
	char*				szCategoryName;				//分类名
	char*				szCategoryDesc;				//分类描述
	ParamItemArray		pParamItemArray;			//该分类下的参数项
};
typedef map<int,CDpParamCategory*> ParamCategoryArray;


//用于绑定记录集的绑定信息
struct _DpBindFieldInfo
{
	CDpFieldInfo	info;				//字段的原始SQL信息
	SQLSMALLINT		nBindSqlType;		//要绑定目标字段的数据类型
	int				iIndex;				//字段索引
	int				iMemorySize;		//该字段在内存块中所占内存数
	int				iPosInBlock;		//该字段在内存块中的位置
	int				iLenPosInBlock;		//字段的长度字段在内存块中的位置
	BOOL			bIsCanBind;			//该字段是否可以绑定到内存块中
};

/*用于在工作区中枚举任务*/
struct _DpTaskFilter
{
	DpAPartType		AFilter;			//如果为DP_A_NULL,则表示不用
	char*			BFilter;			//如果为NULL,则表示不用
	char*			CFilter;			//如果为NULL,则表示不用
	int				DFilter;			//如果为O,则表示不用
};

/*存放数据服务器的信息,houkui,06.6.19*/
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
	char			szServerName[10];		//服务器名,10
	char			szServerAddr[15];		//服务器地址,15
	char			szServerDesc[30];		//服务器描述信息,30
};



//从源数据库中的指定的表中批量复制记录到目标数据库中内
BOOL BulkCopyRecordToDest(SQLHANDLE hConnSource,SQLHANDLE hConnDest,
						  int iRecordCountOfPerOper,char* szTableName,
						  char* szWhereClause,int iDefaultBinaryBlockSize = DEFAULT_BINARY_BLOCK_SIZE);
//在得到字段类型后,将记录绑定信息进行整理
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
/*                 通用类                   */
/********************************************/


/***********************************/
/*            整数数组类           */
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
/*		   字符串数组类			   */
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
		int				GetCount();					//得到当前字符串数组的条目数
		char*			GetItem(int	iIndex);		//得到指定位置的字符串,从零开始
		BOOL			AddString(const char* szItem);	//向字符串数组中添加字符串
		void			DelString(int iIndex);		//删除指定的数组项
		void			DelAllItem();				//删除所有的数组项
		void			Copy(CDpStringArray &other);
};


/***********************************/
/*		   用户信息数组类		   */
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
		int					GetCount();						//得到当前字符串数组的条目数
		const _DpUserInfo*	GetItem(int	iIndex);			//得到指定位置的用户信息,从零开始
		BOOL				AddUserInfo(_DpUserInfo* pInfo);	//向数组中添加用户信息
		void				DelItem(int iIndex);			//删除指定的信息项
		void				DelItem(_DpUserInfo* pInfo);	//删除指定的信息项	
		void				DelAllItem();					//删除所有的信息项
};


/*多边形的点的结构*/
struct _DPRealPoint 
{
   double		x;
   double		y;
};


/*实数的矩形的结构*/
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
/*           多边形类              */
/***********************************/
class CDpRgn
{
public:
	double*		m_pX;					//X点集
	double*		m_pY;					//Y点集
	bool*		m_bVertex;				//
	bool*		m_VertexState;			//
	int			m_nBufferSize;			//点集数目
	_DpRealRect	m_Box;					//外包

private:
	int			m_nDirection;
	int			CalRgnDirection();
	bool		InitVertexState();
	int			IsPtInRegion(double& x,double& y);
	inline int	Intersect(double& x0,double& y0,double& x1,double& y1,double& x2,double& y2,double& x3,double& y3,int ptIndex2,int ptIndex3,double *x,double *y,double& minXSrc,double& minYSrc,double& maxXSrc,double& maxYSrc,int nCount);
public:
	CDpRgn();
	virtual ~CDpRgn();
	inline int	GetRgnDirection(){return m_nDirection;};//0 顺时针 1逆时针 -1 error
	bool		CreatePolygonRgn(_DPRealPoint* pRealPt,int nCount);//pRealPt指向用于构造对边形的点,nCount为点的个数
	bool		CreatePolygonRgn(double* pX,double* pY,int nCount);//pX,pY分别指向count个点的x坐标和y坐标
	int			GetRgnBox(_DpRealRect* pRgnBox);//得到多边形的最大外廓
	int			PtInRegion(double& x,double& y);//判断一个点是否在多边形内部,0外部 1 内部 -1 边界上
	int			PtInRegion(_DPRealPoint& point);//判断一个点是否在多边形内部

	double		GetCircumference();
	double		GetArea();
	bool		IsValid();	

	bool		Intersect(CDpRect* pRect);
};




/***********************************/
/*		        矩形类			   */
/***********************************/				 
class CDpRect
{
	public:
		CDpRect();
		CDpRect(double dMinX,double dMinY,double dMaxX,double dMaxY);
	public:
		double	m_dMinX;						//最小X坐标,如果是笛卡儿坐标系的话,则为左下角的X坐标
		double	m_dMinY;						//最小Y坐标,如果是笛卡儿坐标系的话,则为左下角的Y坐标
		double	m_dMaxX;						//最大X坐标,如果是笛卡儿坐标系的话,则为右上角的X坐标
		double	m_dMaxY;						//最大Y坐标,如果是笛卡儿坐标系的话,则为右上角的Y坐标

	public:
		//设置矩形的位置
		void SetRect(double dMinX,double dMinY,double dMaxX,double dMaxY);
		void CopyRect(CDpRect* pRt);
		double	 Width() const;						//矩形宽度
		double   Height() const;					//矩形高度 
		BOOL PtInRect(CDpPoint pt) const;		//判断点是否在矩形区域内
		BOOL PtInRect(double dx,double dy) const;
		BOOL IsIntersect(CDpRect* rt) const;	//判断rt是否与当前矩形相交
		BOOL IsIntersect(double dMinX,double dMinY,double dMaxX,double dMaxY) const;
		BOOL IsInclusive(CDpRect* rt) const;	//判断rt是否被当前矩形所包含
		BOOL IsInclusive(double dMinX,double dMinY,double dMaxX,double dMaxY) const;
};



/********************************************/
/*              数据库访问类
/*   该类的功能类似于MFC中的CDatabase
/********************************************/
class CDpDatabase
{
	friend class CDpCheckInMgr;

	public:
		CDpDatabase(char* szGlobalID);
		virtual ~CDpDatabase();
	public:
		//用于连接ACCESS数据库,UserID--用户ID,Pwd-口令,其中的用户ID和口令都不是数据库系统的
		//用户ID和口令,而是本系统自己分配的
		BOOL ConnectAccess(const char* UserID,const char* Pwd,const char* szAccessFileName);
		//用于连接ORACLE数据库,UserID--用户ID,Pwd-口令,szAddress-ORACLE的服务名,szDBUserID -- 数据库用户ID,szDBPwd -- 数据库用户口令
		BOOL ConnectDB(const char* UserID,const char* Pwd,const char* szAddress,const char* szDBUserID = "Dataplatform",const char* szDBPwd = "liwq");
		//用于连接SQLSERVER数据库。UserID--用户ID,Pwd-口令,szAddress-SQLServer数据库服务器的名称或IP,szDBUserID -- 数据库用户ID,szDBPwd -- 数据库用户口令，szDBName-数据库名称
		BOOL ConnectSqlServer(const char* UserID,const char* Pwd,const char* szAddress,const char* szDBUserID = "Dataplatform",const char* szDBPwd = "liwq",const char* szDBName = "Dataplatform");
		//用于连接Sybase数据库.UserID--用户ID,Pwd-口令,szAddress-SQLServer数据库服务器的名称或IP,uServerPort -- 数据库服务器的端口号,szDBUserID -- 数据库用户ID,szDBPwd -- 数据库用户口令，szDBName-数据库名称
		BOOL ConnectSybase(const char* UserID,const char* Pwd,const char* szServerNetAddress,UINT uServerPort = 5000,const char* szDBUserID = "Dataplatform",const char* szDBPwd = "liwq",const char* szDBName = "Dataplatform");
		
		//用于使用了注册表的连接项,连接ORACLE
		BOOL ConnectDB(const char* UserID,const char* Pwd);
		//用于使用了注册表的连接项,连接SqlServer
		BOOL ConnectSqlServer(const char* UserID,const char* Pwd);
		//用于连接使用了注册表的连接项,连接Sybase
		BOOL ConnectSybase(const char* UserID,const char* Pwd);
		BOOL AddANode(enum DP_WORKSPACE_TYPE WspType,enum DpAPartType APartType);
		
		
		BOOL AddCNode(enum DP_WORKSPACE_TYPE WspType, enum DpAPartType APartType,
			enum DP_LAYER_TYPE_EX CPartTypeEx);
		
		BOOL AddDNode(enum DP_WORKSPACE_TYPE WspType, enum DpAPartType APartType,
			enum DP_LAYER_TYPE_EX CPartTypeEx, int dIndex, char* szDName,char* szDDesc);
		

		//关闭数据库
		void CloseDB();

		//执行SQL语句
		DpReturnCode ExecuteSQL(const char* szSQL);
		//设置登录的超时时间,dSeconds的单位为秒,必需在连接前调用 
		void SetLoginTimeOut(DWORD dSeconds);
	public:
		//是否能执行事务功能
		BOOL CanTransact();
		//开始事务
		BOOL BeginTrans();
		//提交事务
		BOOL CommitTrans();
		//回滚事务
		BOOL Rollback();

	public:
		//取得当前数据库中工作区的数目
		int	 GetWksCount(DP_WORKSPACE_TYPE nWksType);
		//枚举当前数据库中当前用户能够访问得工作区的ID个和名称
		BOOL GetWksIDAndName(CDpStringArray& szIDArray,CDpStringArray& szNameArray);
		//根据ID得到工作区
		BOOL GetWksByID(const char* szID,CDpWorkspace** ppWks);
		//根据名称得到工作区,如果名称有重复的,只取第一个
		BOOL GetWksByName(const char* szName,CDpWorkspace** ppWks);

	public:
		//数据库是否连接,连接上为TRUE,否则为FALSE
		BOOL GetConnectStatus();
		//判断数据库是否打开
		BOOL IsOpen();
		//当前连接的数据库的类型,如果没有连接数据库,该值为0
		DP_DATABASE_TYPE GetDBType();
		//如果当前连接的数据库为ACCESS,则该属性为数据库文件的完整路径
		//参数说明:  szFileName --为放置文件名的缓冲区,iBuffCount --为缓冲区大小
		BOOL GetDBFileName(char* szFileName,int iBuffCount);
		//得到数据库的ODBC句柄
		SQLHANDLE GetDBHandle();

		//创建工作区,bIsSetDefaultRight -- 是否给所有的用户分配对该工作区有三个系统内置的全读权限,TRUE表示分配
		BOOL CreateWorksapce(const char* szName,CDpRect* pRtBound,double dScale,const char*	szDefaultDir,const char* szDataserver,int iWspType,char* szID=NULL,BOOL bIsSetDefaultRight = false);
		//BOOL CreateWorksapce(const char* szName,CDpRect* pRtBound,double dScale,const char* szDefaultDir,char* szID=NULL,BOOL bIsSetDefaultRight = false);
		//删除工作区
		BOOL DeleteWorkspace(const char* szWksID,BOOL bDeleteDefaultDir = false);

		//得到WBS CODE 维护表对象,pWBSCodeWH需要调用者释放
		BOOL OpenWBSWH(CDpWBSCodeWH** ppWBSCodeWH,DP_WORKSPACE_TYPE type);

		//创建角色
		BOOL CreateRole(const char* szRoleID,const char* szDesc);
		//创建用户,bIsSetDefaultRight -- 是否能用户分配三个系统内置的全读权限,TRUE表示分配
		BOOL CreateUser(const char* szUserID,const char* szDesc,const char* szPwd,DP_USER_RIGHTTYPE nUserType,BOOL bIsSetDefaultRight = false);
		//枚举当前数据库内的用户
		BOOL GetUsersInfo(CDpUserArray& nArray);
		//枚举当前数据库内的角色,nRoleID -- 角色名,nRoleDesc -- 角色描述
		BOOL GetRolesInfo(CDpStringArray& nRoleID,CDpStringArray& nRoleDesc);
		//删除角色
		BOOL DeleteRole(const char* szRoleID);
		//删除用户
		BOOL DeleteUser(const char* szUserID);

		//修改用户口令
		BOOL ModifyUserPwd(const char* szUserID,const char* szNewPwd,const char* szDesc,int iUserType);

		//刷新权限管理器
		BOOL RefreshRightMgr();


	private:
		void				GetConnectInfo();			//取得当前连接的属性信息
		//判断用户ID和口令是否正确
		BOOL				UserInfoIsCorrect(SQLHANDLE hConn,const char* UserID,const char* Pwd);
		//通过指定的SQL语句得到一个工作区类
		BOOL				GetWorkspace(const char* szSql,CDpWorkspace** ppWks);
		//初始化成员变量
		void				InitMember();
		//释放内存
		void				FreeMem();

	private:
		char				m_szPwd[64];				//登录系统的密码

		char				m_szFileName[_MAX_PATH];	//数据库文件名
	
		SQLHANDLE			m_hEnv;						//环境句柄
		SQLHANDLE			m_hConn;					//连接句柄

		DWORD				m_dwLoginTimeOut;			//登录超时时间

		BOOL				m_bIsOpen;					//表示连接是否打开
		BOOL				m_bCanTrans;				//是否有事务功能
		BOOL				m_bCanGetDataAnyOrder;		//能否不依据记录集中字段的顺序(从小到大)去取字段的值,而是可以以任意顺序取

		DP_DATABASE_TYPE	m_dtDatabaseType;			//数据库类型

		char				m_szGlobalID[6];			//全局ID,要于标志服务器


	public:
		BOOL ModifyDataServer(char *szServerName, char *szServerAddr, char *szServerDesc);
		BOOL DeleteDataServer (char* szServerName);
		BOOL RemoveAllDataServer ();
		BOOL CreateDataServer (char* szServerName,char* szServerAddr,char* szServerDesc);
		BOOL GetDataServer (CDpStringArray& szName,CDpStringArray& szAddr,CDpStringArray& szDesc);
		int GetDataServerCount();
		char				m_szUserID[64];				//登录系统的用户ID,
		CDpRightMgr*		m_pRightMgr;				//权限管理器

};


/************************************************************/
/*                         权限管理类                       */
/************************************************************/
class CDpRightMgr
{
	private:
		//参数权限结构
		struct _ParamRight
		{
			ULONGLONG	uWksID;						//工作区ID
			char*		szParamTableName;			//参数表的名称
			UINT		uID;						//参数ID
			DP_RIGHT	sRight;						//权限
		};

		//增加图幅的图层的权限,修改编号:NO.050220
		struct _Unit_Layer_Right
		{
			ULONGLONG	uUnitID;					//图幅ID
			ULONGLONG	ulLayerID;					//图层ID
			DP_RIGHT	sRight;						//权限
		};

		//增加影像的指定标签的权限,修改编号:NO.050220
		struct _Image_XMLTag_Right
		{
			ULONGLONG	uImageID;					//影像ID
			char*		szXMLTagName;				//影像的标签名称
			DP_RIGHT	sRight;						//权限
		};

		//是否拥有对所有影像,图幅,参数的读权限的结构
		struct _Wks_AllRead_Right
		{
			ULONGLONG	uWksID;						//工作区ID
			BOOL		m_bIsCanReadAllImage;		//是否拥有对所有影像的读权限,即拥有组"All Image Read Right"
			BOOL		m_bIsCanReadAllUnitLayer;	//是否拥有对所有图幅图层的读权限,即拥有组"All Map Read Right"
			BOOL		m_bIsCanReadAllParam;		//是否拥有对所有参数的读权限,即拥有组"All Parament Read Right"
		};


		typedef vector<char*> WKSRIGHT;						//工作区权限数组
		typedef vector<_Unit_Layer_Right*> LAYERARRAY;		//图幅的图层的数组,修改编号:NO.050220
		typedef vector<_Image_XMLTag_Right*> XMLTAGARRAY;	//影像的XML标签数组,修改编号:NO.050220

		typedef vector<_ParamRight*> PARAMARRAY;			//单个工作区的参数权限的数组

		typedef vector<PARAMARRAY*> PARAMMAP;				//所有的工作区的参数权限
		typedef vector<LAYERARRAY*> UNITMAP;			//图幅权限,MAP中的KEY为图幅的ID,second为具体的权限
		typedef vector<XMLTAGARRAY*> IMAGEMAP;			//影像权限,MAP中的KEY为影像的ID,second为具体的权限
		typedef vector<_Wks_AllRead_Right*> WKSALLREADMAP;

		DP_USER_RIGHTTYPE	m_iUserType;			//登陆用户的类型
		WKSRIGHT			m_mapWorkspace;			//允许访问的工作区的数组
		UNITMAP				m_mapUnit;				//允许访问的图幅的HASH表
		IMAGEMAP			m_mapImage;				//允许访问的影像的HASH表
		PARAMMAP			m_mapParam;				//允许访问的参数的HASH表,这些图层一定是在指定的工作区下的
		WKSALLREADMAP		m_mapWksAllRead;		//拥有所有读权限的访问组的情况


	public:
		CDpRightMgr(CDpDatabase* pDB,const char* szUserID = NULL);
		virtual ~CDpRightMgr();
	public:
		//从数据库中加载权限管理信息
		BOOL LoadRightInfo();
		//释放权限管理记录所占的内存
		void RemoveRightInfo();

		//得到当前用户的登陆类型
		DP_USER_RIGHTTYPE	GetUserType(){return m_iUserType;}
		//判断该SQL语句是否右权限进行执行,如果有就返回TRUE,否则返回FALSE;
		BOOL SqlCanExecute(const char* szSql);
		//得到对某个工作区的权限	
		BOOL GetWorkspaceRight(const char* szWksID,DP_RIGHT* piRight);
		//得到对某个图幅的权限
		BOOL GetUnitRight(const char* szUnitID,DP_RIGHT* piRight);
		//得到图幅的图层的权限
		BOOL GetUnitLayerRight(const char* szUnitID,const char* szLayerID,DP_RIGHT* piRight);
		//得到对某个影像的权限
		BOOL GetImageRight(const char* szImageID,DP_RIGHT* piRight);
		//得到影像的某个XML标签的权限
		BOOL GetImageXmlRight(const char* szImageID,const char* szXmlTagName,DP_RIGHT* piRight);
		//确认是否能访问该图层,判断的依据就是该用户是否能访问该图层的任意一个图幅即表示能访问该图层
		BOOL CanAccessLayer(const char* szLayerID);

		//得到某个工作区的某个参数的权限
		BOOL GetParamRight(const char* szWksID,const char* szParamTabName,UINT uParamID,DP_RIGHT* piRight);

		//锁定某个影像的某个XML标签
		//bLock -- TRUE:锁定,FALSE:解锁
		BOOL LockImageXml(const char* szWksID,const char* szImageID,const char* szXmlTagName,BOOL bLock = true);

		//锁定某个图幅的某个图层
		//bLock -- TRUE:锁定,FALSE:解锁
		BOOL LockUnitLayer(const char* szWksID,const char* szUnitID,const char* szLayerID,BOOL bLock = true);

		//得到锁定指定影像的指定XML标签的锁定人的用户名称
		//szRetLockerName -- 返回锁定人的名称,如果szRetLockerName为空字符串则表示该标签没有被锁定
		BOOL GetImageXmlLockerName(const char* szWksID,const char* szImageID,const char* szXmlTagName,char* szRetLockerName);

		//得到锁定指定图幅的指定图层的锁定人的用户名称
		//szRetLockerName -- 返回锁定人的名称,如果szRetLockerName为空字符串则表示该图幅的图层没有被锁定
		BOOL GetUnitLayerLockerName(const char* szWksID,const char* szUnitID,const char* szLayerID,char* szRetLockerName);

		//生成用于在打开查询时(如打开FeatureClass,GeoQuery等)所需的权限过滤语句
		//szRetClause -- 用于返回过滤语句
		BOOL MakeRightFilterClauseWhenOpenQuery(const char* szWksID,const char* szLayerID,char* szRetClause);

	private:
		//找到指定工作区的全读权限组的情况
		const _Wks_AllRead_Right* FindAllReadByWksID(const char* szWksID);

	private:
		CDpDatabase*		m_pDB;
		char				m_szUserID[51];						//用户ID
};


/************************************************************/
/*                        工作区管理类                      */
/************************************************************/   
class CDpWorkspace
{
	public:
		CDpWorkspace(CDpDatabase* pDB);
		virtual ~CDpWorkspace();

	public:
		//得到工作区的空间范围
		BOOL GetWksBound(CDpRect* pBound);
		//修改工作区的空间范围
		BOOL SetWksBound(CDpRect* pBound);
		//得到工作区的比例尺
		double GetWksScale(){return m_dblScale;}
		//设置工作区的比例尺
		BOOL SetWksScale(double dScale);

		//得到工作区的控制点XML
		BOOL GetWksCtlrPointXML(CDpDBVariant& var);
		//得到工作区的控制点XML,ppBuff的内存需要调用者释放
		BOOL GetWksCtlrPointXML(BYTE** ppBuff,int* piBuffSize);
		//更新数据库中控制点XML字段的内容
		BOOL SetWksCtrlPointXML(BYTE* pBuff,int iBuffSize);

		//得到工作区的相机文件数据
		BOOL GetWksCameraData(CDpDBVariant& var);
		//得到工作区的相机文件数据,ppBuff的内存需要调用者释放
		BOOL GetWksCameraData(BYTE** ppBuff,int* piBuffSize);
		//更新工作区的相机文件数据
		BOOL SetWksCameraData(BYTE* pBuff,int iBuffSize);


		//得到工作区的投影坐标系配置XML
		BOOL GetProjectXML(CDpDBVariant& var);
		//得到工作区的投影坐标系配置XML,ppBuff的内存需要调用者释放
		BOOL GetProjectXML(BYTE** ppBuff,int* iBuffSize);
		//更新数据库中的投影坐标系配置XML
		BOOL SetProjectXML(BYTE* pBuff,int iBuffSize);


		//得到当前工作区当前用户的任务的ID和名称的列表
		BOOL GetTasksIDAndName(CDpStringArray& szIDsArray,CDpStringArray& szNamesArray);
		//得到对应ID的任务对象,ppMgr需要调用者释放
		BOOL GetTask(const char* szID,CDpTaskMgr** ppMgr);
		//得到对应ID的任务对象,ppTaskSetArray--任务集的数组
		//szUserID--用户名,如果为空,则取所有的用户的任务,
		//pFilter - 用于查询任务的过滤条件,如果为空,则表示不需要过滤
		BOOL GetTask(CDpTaskSetArray& nTaskSetArray,const char* szUserID = NULL,const _DpTaskFilter* pFilter = NULL);
		//得到对应ID和对应用户名的任务集
		BOOL GetTask(CDpTaskSet** ppTaskSet,const char* szUserID,const _DpTaskFilter* pFilter = NULL);
		//枚举该用户所拥有的角色
		BOOL GetRoles(CDpStringArray& nRoleArray,const char* szuserID);
		


		//删除某个任务
		BOOL DeleteTask(const char* szID);
		//创建一个任务,[in]szName-为任务的名称,[in]A,B,C,D分别为WBS CODE 的四个部分,
		//[out]szNewID为创建成功后的新的任务的ID,如果不需要可以设为NULL 
		BOOL CreateTask(const char* szName,DpAPartType A,const char* B,const char* C,int D,const char* szStartTime,const char* szEndTime,char* szNewID);
		//将任务分配给某个用户,如果szUserID为NULL时,表示删除该任务的分配,让该任务还原到为被分配的状态,
		//                     但是如果该任务已经被启动(即任务完成百分比大于0),则为返回失败
		BOOL AssignTaskToUser(const char* szTaskID,const char* szUserID);
		//将一个任务组分配给某个用户,如果szUserID为NULL时,表示删除该任务的分配,让该任务还原到为被分配的状态,
		//                     但是如果该任务已经被启动(即任务完成百分比大于0),则为返回失败
		BOOL AssignTaskToUser(CDpTaskGroup* pGroup,const char* szUserID);
	
		//临时添加的一个函数,在下个版本将废弃掉
		//该接口的用处是当分配的任务是基于DLG的任务时,为自动的将该任务中的B(即任务图幅)下的所有DLG图层的
		//写权限都分配给szUserID所表示的用户	
		BOOL AssignTaskToUserTmp(const char* szTaskID,const char* szUserID,BOOL bDlgOnly = true);

				
		//创建图幅,[in]szName-图幅名称,[in]pRtBound-图幅空间范围,qqq
		//         [in]ptValiRegion-图幅有效范围,[out]szUnitID-返回创建成功的图幅的ID
		BOOL CreateUnit(const char* szName,CDpRect* pRtBound,CDpPoint* ptValiRegion,char* szUnitID = NULL,DpAPartType iType = DP_A_UNIT,char* szRefer = NULL,	char* szLocalName = NULL);
		//在工作区中创建原始影像,[in]szName-影像名称,,实际就是影像的文件名(包含扩展名)
		//                       [in]szStrip-航带名称
		//			             [out]szFilePath-影像绝对文件名    [in]pRtBound-影像空间范围 
		//                       [out]szImageID-返回创建成功的影像ID,失败则返回空字串,如果不需要可以设为NULL
		//                       [in]iType-标识是模型还是图幅
		//                       [in]szRefer-参考数据，给模型使用，用来标识左右片
		//                       [in]szLocalName-地名，标识图幅的地名
		//BOOL CreateUnit(const char* szName,CDpRect* pRtBound,CDpPoint* ptValiRegion,char* szUnitID = NULL);
		//在工作区中创建原始影像,[in]szName-影像名称,,实际就是影像的文件名(包含扩展名)
		//                       [in]szStrip-航带名称
		//			             [out]szFilePath-影像绝对文件名    [in]pRtBound-影像空间范围 
		//                       [out]szImageID-返回创建成功的影像ID,失败则返回空字串,如果不需要可以设为NULL

		BOOL CreateImage(const char* szName,const char* szStrip,UINT iIndexInStrip,char* szFilePath,CDpRect* pRtBound = NULL,char* szImageID = NULL);
		//在工作区中创建图层,[in]szName-图层名称      
		//                   [in]iLayerType-图层类型  [out]szLayerID-返回创建成功的图层ID,失败则返回空字串,如果不需要可以设为NULL
		BOOL CreateLayer(const char* szName,DP_LAYER_TYPE iLayerType,DP_LAYER_TYPE_EX iLayerTypeEx,char* szLayerID = NULL);

		//从工作区中删除图幅
		BOOL DeleteUnit(const char* szUnitID,BOOL bDelFeature = false);
		//从工作区中删除原始影像
		BOOL DeleteImage(const char* szImageID);
		//从工作区中删除图层
		BOOL DeleteLayer(const char* szLayerID);


		//得到工作区中的图幅的数目DpAPartType ApartType,
		//int	 GetUnitCount(DP_LOCK_TYPE UnitType = DP_UT_ANY);
		
		
		//得到工作区中的图幅的数目
		int	 GetUnitCount(DP_LOCK_TYPE UnitType = DP_UT_ANY,DpAPartType ApartType =DP_A_UNIT);

		//根据空间范围取符合条件的图幅的ID
		//szIDArray用来存放检索的结果
		//rtBound为要检索的范围,为空则表示不考虑空间位置
		//bIntersect表示过滤条件,
		//为TRUE表示只要是相交的图幅就得到,为FALSE则表示需要完全包含才得到
		//bIsLocked表示是否返回被锁定的图层,为TRUE表示返回
		//BOOL GetUnitsID(CDpStringArray& szIDArray,CDpRect* pRect = NULL,BOOL bIntersect = TRUE,DP_LOCK_TYPE UnitType = DP_UT_ANY);
		//根据图幅ID取得图幅,ppUnit返回图幅对象的指针,该对象在使用完后,需要手工释放
//		BOOL GetUnitsID(DpAPartType APartType,CDpStringArray& szIDArray,CDpRect* pRect = NULL,BOOL bIntersect = TRUE,DP_LOCK_TYPE UnitType = DP_UT_ANY);
		BOOL GetUnit(const char* szUnitID,CDpUnit** ppUnit);//,UINT iType,char* szRefer = NULL,	char* szLocalName = NULL		


		//得到工作区中的影像的数目
		int	 GetImageCount(DP_LOCK_TYPE UnitType = DP_UT_ANY);
		//根据空间范围取符合条件的影像的ID
		//szIDArray用来存放检索的结果
		//rtBound为要检索的范围,为空则表示不考虑空间位置
		//bIntersect表示过滤条件,
		//为TRUE表示只要是相交的图幅就得到,为FALSE则表示需要完全包含才得到
		//bIsLocked表示是否返回被锁定的图层,为TRUE表示返回
		BOOL GetImagesID(CDpStringArray& szIDArray,CDpRect* pRect = NULL,BOOL bIntersect = TRUE,DP_LOCK_TYPE UnitType = DP_UT_ANY);
		//根据航带名称得到该航带内的所有影像
		BOOL GetImagesIDInStrip(CDpStringArray& szIDArray,const char* szStrip);
		//根据影像ID取得影像对象,ppImage返回影像对象的指针,该对象在使用完后,需要手工释放
		BOOL GetImage(const char* szImageID,CDpImage** ppImage);		//houkui,06.7.3
		//得到当前工作区的影像的航带列表
		BOOL GetStrips(CDpStringArray& szStripArray);



		//得到工作区中层的数目
		int GetLayersCount();
		//根据图层索引(即放置顺序,最底层为0)得到图层ID
		BOOL GetLayerID(UINT uIndex,char** szLayerID);
		//得到当前工作区下所有的图层的ID,
		//参数说明: szIDArray -- 返回得到的图层ID的数组, szNameArray -- 返回得到的图层名称的数组
		//          nLayerType   -- 设置得到的图层的类型的过滤条件,如果为GEO_NULL,则表示不用该参数
		//          nLayerTypeEx -- 设置得到的图层的成果类型的过滤条件,如果为LAYER_TYPE_EX_NULL,则表示不用该参数
		BOOL GetLayersIDAndName(CDpStringArray& szIDArray,CDpStringArray& szNameArray,DP_LAYER_TYPE nLayerType = GEO_NULL,DP_LAYER_TYPE_EX nLayerTypeEx = LAYER_TYPE_EX_NULL);
		//得到图层的类型
		DP_LAYER_TYPE GetLayerType(const char* szLayerID,DP_LAYER_TYPE_EX* iLayerTypeEx = NULL);

		//根据图层的ID得到图层对象,得到的LAYER对象在使用完后要手工释放
		BOOL GetLayer(const char* szID,CDpLayer** ppLayer);

		//得到指定图层ID的相关表的表名
		BOOL GetLayerTableName(const char* szID,char** ppszSpatialTableName,char** ppszParamTableName,char** ppszAttributeTableName);

		BOOL GetGroupTableName(char** ppszUDLTableName,char** ppszGRPTableName);

		//取数据的生产状况,
		//szWBSB    -- 当是影像任务时,为指定影像ID,当是MAP任务时为图幅ID
		//szWBSC    -- 当是影像任务时,为指定影像XML标签名,当是MAP任务时为图层ID,当为dlg组时,可以是该图幅下的任一个图层ID
		//szTaskID	-- 返回正在操作该数据的任务ID,如果没有任何任务就返回NULL
		//D			-- 返回操作当前数据的任务的D码
		//iPercent  -- 返回操作当前数据的任务的完成百分比
		//iPass     -- 返回操作当前数据的任务的是否合格
		//szDesc	-- 返回操作当前数据的任务的备注,最大不超过200个字节,内存是否调用者分配的,最好是分配一个401个字节长度的字符数组
		BOOL GetDataStatus(const char* szWBSB,const char* szWBSC,char* szTaskID,int& D,int& iCurrentAllowFolwD,int& iPercent,int& iPass,char* szDesc);

		//设置当前数据允许的生成状况
		BOOL SetDataStatus(DpAPartType A,const char* szWBSB,const char* szWBSC,int iCurrentAllowFolwD);

		//数据传输的接口

		//开始连接到服务器
		BOOL StartLinkToServer();
		//取消服务器连接
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
		UINT					m_iMetricType;			//描述测区类型,houkui,06.6.27

		CDpDatabase*			m_pDB;					//数据库
		char*					m_szID;					//工作区ID
		char*					m_szName;				//工作区名称
		char*					m_szDesc;				//工作区的备注
		char*					m_szCreater;			//工作区创建人名称
		char*					m_szDefaultDir;			//工作区缺省的工作目录
//		char*					m_szPhysicalDir;		//工作区文件物理目录
		time_t					m_tmCreate;				//工作区创建日期
		time_t					m_tmUpdate;				//工作区更新日期
		UINT					m_iVersion;				//工作去版本号						
		DP_WORKSPACE_TYPE		m_nWksType;				//工作区类型
		CDpRect					m_rtBound;				//工作区的范围
		double					m_dblScale;				//比例尺
		UINT					m_iImageType;			//标识影像类型，决定参数处理的方法,houkui,2006.6.19
		char					m_szServerName[10];				// 数据服务器名字，用来存取成果数据,houkui,2006.6.19

	private:
		//根据SQL语句得到LAYER	
		BOOL GetLayerBySql(char* szSql,CDpLayer** ppLayer);

};


/*****************************************************************/
/*                 用于管理角色和用户权限的类                    */
/*****************************************************************/
class CDpUserMgr
{
	public:
		CDpUserMgr(CDpWorkspace* pWks);
		virtual ~CDpUserMgr();

		//给角色添加图幅的图层权限
		BOOL AddUnitLayerRightToRole(const char* szRoleID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);
		//给角色添加影像的XML标签权限
		BOOL AddImageXmlRightToRole(const char* szRoleID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//给角色添加参数权限
		BOOL AddParamRightToRole(const char* szRoleID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);

		//给用户直接分配角色
		BOOL AddRoleToUser(const char* szUserID,const char* szRoleID);
		//给用户添加图幅的图层的权限
		BOOL AddUnitLayerRightToUser(const char* szUserID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);
		//给角色添加影像的XML标签权限
		BOOL AddImageXmlRightToUser(const char* szUserID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//给角色添加参数权限
		BOOL AddParamRightToUser(const char* szUserID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);

		//回收某个角色对图幅的图层的权限
		BOOL DeleteUnitLayerRightFromRole(const char* szRoleID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);

		//回收某个角色对影像的XML标签的权限
		BOOL DeleteImageXmlRightFromRole(const char* szRoleID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//回收某个角色对参数的权限
		BOOL DeleteParamRightFromRole(const char* szRoleID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);


		//回收某个用户对图幅的图层的权限
		BOOL DeleteUnitLayerRightFromUser(const char* szUserID,const char* szUnitID,const char* szLayerID,DP_RIGHT nRight);

		//回收某个用户对影像的XML标签的权限
		BOOL DeleteImageXmlRightFromUser(const char* szUserID,const char* szImageID,const char* szXmlTagName,DP_RIGHT nRight);
		//回收某个用户对参数的权限
		BOOL DeleteParamRightFromUser(const char* szUserID,const char* szParamTabName,int iParamID,DP_RIGHT nRight);

	public:

		CDpWorkspace*		m_pWks;

	private:
		//向角色表中添加权限
		BOOL AddRightToRole(const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);
		//向用户表中添加权限
		BOOL AddRightToUser(const char* szUserID,const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);

		//将用户的某种权限回收
		BOOL DeleteRightFromUser(const char* szUserID,const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);
		//将角色的某种权限回收
		BOOL DeleteRightFromRole(const char* szRoleID,const char* sz1,const char* sz2,DP_RIGHT nRight,int iRightType);

};





/****************************************************************/
/*           针对WBS结构设计的用于维护各级代码的类              */
/****************************************************************/
class CDpWBSCodeWH
{ 
	//WBS_A，WBS_B，WBS_C，WBS_D,就是指任务的各个信息 ,A:任务类型:即是基于MAP的还是基于IMAGE的
	//B:图幅ID ,C:图层ID ,D:任务的子类型,即是生成还是检查还是接边等  

	private:

		//WBS CODE 中D部分的维护列表
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
			int		iD_ID;					//D部分的ID
			//DpDPartType	D;					//D部分的标识
			char*		szName;				//D部分的名称
			char*		szDesc;				//D部分的描述
		};
		typedef vector<CDpWBSDPartWH*> DPARTARRAY;		//D部分维护的数组

		//WBS CODE 中C部分的维护列表
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
			int					iC_ID;              //C部分的ID
			DP_LAYER_TYPE_EX	C;					//C部分的扩展属性
			char*				szName;				//C部分的名称
			char*				szDesc;				//C部分的描述			
			DPARTARRAY			dArray;				//属于该C部分的D的列表
		};
		typedef vector<CDpWBSCPartWH*> CPARTARRAY;		//C部分维护的数组


		//WBS CODE 中A部分的维护列表
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
			int		iA_ID;                 //A部分的ID
			DpAPartType	A;					//A部分的标识
			char*		szName;				//A部分的名称
			char*		szDesc;				//A部分的描述
			CPARTARRAY	cArray;				//属于该A部分的C的列表
		};
		typedef vector<CDpWBSAPartWH*> APARTARRAY;		//A部分维护的数组


		
		APARTARRAY				m_pWHData;				//维护的数据
		CDpDatabase*			m_pDB;					//数据库对象

	private:
		//得到A数组中的某个节点
		CDpWBSAPartWH* GetANode(DpAPartType A);
		//得到指定的C的节点
		CDpWBSCPartWH* GetCNode(int cID);
		//得到指定的D的节点
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
		BOOL LoadDataFromDB();							//从数据库中加载数据
		//得到A的数目
		int GetACount();
		//得到C的数目,A -- 为所属的A的标识
		int GetCCount(DpAPartType A);
		//得到D的数目,C--为所属的C的标识
		int GetDCount(int cID);


		


		//得到具体的A,iIndex -- 为序号,从0开始. szName,szDesc的内存调用者不能去释放
		BOOL GetA(int iIndex,DpAPartType* pA,const char** ppszName,const char** ppszDesc = NULL);
		//得到具体的C,A -- 为所属的A的标识,iIndex -- 为序号,从0开始.  szName,szDesc的内存调用者不能去释放
		BOOL GetC(enum DpAPartType A,int iIndex,int* cID,const char** ppszName,const char** ppszDesc=NULL);
		//得到具体的D,C -- 为所属的C的标识,iIndex-序号,从0开始. szName,szDesc的内存调用者不能去释放
		BOOL GetD(int cID,int iIndex,int* dID,const char** ppszName,const char** ppszDesc=NULL);

		//通过A的标识得到名称,szName的内存调用者不能去释放
		BOOL GetAName(DpAPartType A,const char** ppszName);
		//通过A的标识得到描述,ppszDesc的内存调用者不能去释放
		BOOL GetADesc(DpAPartType A,const char** ppszDesc);
		//通过C的标识得到名称,szName的内存调用者不能去释放
		BOOL GetCName(int cID,const char** ppszName);
		//通过C的标识得到描述,szName的内存调用者不能去释放
		BOOL GetCDesc(int cID,const char**  ppszDesc);
		//通过D的标识得到名称,szName的内存调用者不能去释放
		BOOL GetDName(int dID,const char** ppszName);
		//通过D的标识得到描述,ppszDesc的内存调用者不能去释放
		BOOL GetDDesc(int dID,const char** ppszDesc);
};


		




/********************************************************************/
/*                          任务管理对象                            */
/********************************************************************/
class CDpTaskMgr
{
	public:
		CDpTaskMgr(CDpWorkspace* pWks);
		~CDpTaskMgr();

		CDpWorkspace*		m_pWks;						//工作区ID
		char				m_szTaskID[50];				//任务ID
		char*				m_szTaskName;				//任务名称
		char*				m_szUserID;					//拥有该任务的用户ID

		struct DpWBSCodeData
		{
			DpAPartType		A;							//A部分
			char*			B;							//B部分
			char*			C;							//C部分
			int				D;							//D部分,ID
			int				TypeEx;						//任务子类型,即是DEM,DOM等,目前的定义如下:
			                                            //     
			char*			szAName;					//A部分的名称
			char*			szDName;					//D部分的名称
			char*			szTypeEx;					//任务的子类型描述,即DEM,DOM等
			char*			szStartTime;				//任务计划开始时间
			char*			szEndTime;					//任务计划结束时间
			int				iPercent;					//任务进度
			int				iPass;						//任务是否合格
			char*			szTaskDesc;					//任务的备注
		};
		DpWBSCodeData		m_WBSCode;					//WBS CODE


	public:
		//打开任务
		BOOL OpenTask(const char* szTaskID);
		//得到WBS码的各个部分
		BOOL Get(DpAPartType* A,const char** B,const char** C,int* D);
		//得到WBS码的各个部分的名称,A,D:是名称,B,C:是具体的任务的内部数据的ID
		BOOL Get(const char** A,const char** B,const char** C,const char** D);
		//设置WBS码的各个部分
		BOOL Set(DpAPartType A,const char* B,const char* C,int D);

		//设置任务的状态
		BOOL SetTaskStatus(int iPercent,int iPass,char* szDesc,int iCurrentAllowFolwD);

		//取得任务的状态
//		BOOL GetTaskStatus(int iPercent,int iPass,char* szDesc);

		//释放内存
		void FreeMem();
};


/*******************************************************************/
/*                        任务组管理对象                           */
/*******************************************************************/
class CDpTaskGroup
{
	public:
		CDpTaskGroup();
		~CDpTaskGroup();
	public:
		struct _DpTaskInfo
		{
			char*		szTaskID;					//任务ID
			char*		szTaskName;					//任务名称
			char*		szWBS_C;					//WBS C的部分
			char*		szStartTime;				//任务计划开始时间,(当组内有多个任务时,取最小时间)
			char*		szEndTime;					//任务计划结束时间,(当组内有多个任务时,取最大时间)
			int			iPercent;					//任务完成状态
			int			iPass;						//是否合格
			char*		szDesc;						//任务的备注,(当组内有多个任务时,取第一个)
		};

		/*具体任务数组*/
		typedef vector<_DpTaskInfo*> DPTASKARRAY;

	public:
		//得到该组下任务项的数目
		int GetTaskCount();

		//得到具体任务
		const _DpTaskInfo* GetTaskItem(int iIndex);

	public:
		ULONGLONG			m_uGroupID;				//组ID,(当是DLG组任务时,组ID就等于该组内第一个任务的ID)
		char*				m_szGroupName;			//组名称,(当是DLG组任务时,名称就为D的描述)
		DpAPartType			m_WBS_A;				//WBS A的部分
		char*				m_WBS_B;				//WBS B的部分
		int					m_WBS_D;				//WBS D的部分 
		DPTASKARRAY			m_TaskArray;			//任务的数组 
		int					m_iPercent;				//任务完成状态,(当组内有多个任务时,取均值)
		int					m_iPass;				//是否合格,(当组内有多个任务时,当所有任务都合格,才合格)
		char*				m_szStartTime;			//任务计划开始时间,(当组内有多个任务时,取最小时间)
		char*				m_szEndTime;			//任务计划结束时间,(当组内有多个任务时,取最大时间)
		char*				m_szDesc;				//任务的备注,(当组内有多个任务时,取第一个)
};



/*******************************************************************/
/*                        任务集管理对象                           */
/*******************************************************************/
class CDpTaskSet
{
	public:
		typedef map<ULONGLONG,CDpTaskGroup*>	DPTASKSET;

		CDpTaskSet(CDpWorkspace* pWks,const char* szUserID);
		~CDpTaskSet();
	public:
		//从数据库中取任务数据
		BOOL GetDataForDB(const _DpTaskFilter* pFilter/*const char* szFilterClause*/);

		//取组的数目
		int GetGroupCount();
		//取组,iIndex 从0开始
		const CDpTaskGroup* GetGroup(int iIndex);

		char			m_szUserID[50];
	private:
		void FreeMem();

	private:
		CDpWorkspace*	m_pWks;
		DPTASKSET		m_TaskSet;						//任务集
};



/*******************************************************************/
/*                    多用户的任务集的数组类                       */
/*******************************************************************/
class CDpTaskSetArray
{
	public:
		typedef vector<CDpTaskSet*> DPTASKSETARRAY;

		CDpTaskSetArray();
		~CDpTaskSetArray();
	public:
		//取当前数组内的任务集的数目
		int GetArraySize();
		//取指定索引的任务集
		const CDpTaskSet* GetTaskSet(int iIndex);
		//向数组内添加任务集
		void AddTaskSet(CDpTaskSet* pTaskSet);
	private:
		DPTASKSETARRAY			m_TaskSetArray;
};







/*******************************************************************/
/*        工作区内管理单元对象(即图幅,图层,原始影像)               */
/*******************************************************************/
class CDpMgrObject
{
	public:
		CDpMgrObject(CDpWorkspace* pWks);
		~CDpMgrObject();

	public:
		//创建参数表
		virtual BOOL CreateParamTable();
		//删除参数表
		virtual BOOL DeleteParamTable();
		//打开参数表,ppParamTable的内存需要调用者释放
		BOOL OpenParamTable(CDpParamTable** ppParamTable);
		
		//向参数表中添加缺省的内置参数
		BOOL AddDefaultBulitinParam(CDpParamTable* pTab = NULL);

	protected:
		//得到当前管理对象的类型,图幅/图层/原始影像
		virtual DP_OBJECT_TYPE GetObjType(){return DP_OBJ_NULL;}
		//得到当前管理对象的成果类型,只有当管理对象是图层时才有效
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx(){return LAYER_TYPE_EX_NULL;}


	public:
		CDpWorkspace*	m_pWks;
		char*			m_szID;						//ID
		char*			m_szParamTableName;			//参数表名称
};



/************************************************************/
/*                        图层访问类                        */
/************************************************************/   
class CDpLayer:public CDpMgrObject
{
	public:
		CDpLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType);
		virtual ~CDpLayer();
	public:
		//得到层的类型
		DP_LAYER_TYPE GetLayerType();
		//得到层的成果类型
		DP_LAYER_TYPE_EX GetLayerTypeEx();
		//得到层的ID
		BOOL GetLayerID(char* szLayerID,int iBuffCount);
		//得到层的名称
		BOOL GetLayerName(char* szLayerName,int iBuffCount);
		//得到参数表名称
		BOOL GetParamTableName(char* szParamTabName,int iBuffCount);
		//得到层的矢量数据表的名称
		BOOL GetSpatialTableName(char* szSpatialTabName,int iBuffCount);
		//得到属性表的名称
		BOOL GetAttrTableName(char* szAttrTabName,int iNameBuffCount);
		//得到图层对应的FeatureClass的名称
		BOOL GetGeoClassName(char* szFeaClsName,int iNameBuffCount);

		//得到属性字段的个数,不包含ATT_OID
		int GetAttrFieldCount();
		//得到属性表某个属性字段的信息
		BOOL GetAttrFieldInfo(char* szAttrFieldName,CDpCustomFieldInfo* pInfo);
		//得到属性表某个属性字段的信息,uIndex -- 表示字段的索引,字段的索引从0开始
		BOOL GetAttrFieldInfo(UINT uIndex,CDpCustomFieldInfo* pInfo);
		//得到指定的字段名的属性字段的索引,属性字段的索引从0开始
		int  GetAttrFieldIndex(char* szAttrFieldName);
		//添加属性字段
		BOOL AddAttrField(CDpCustomFieldInfo* pInfo,bool bRefreshView = true);
		//添加属性字段,一次添加多个属性字段
		BOOL AddAttrField(CDpCustomFieldInfo* pFieldInfoArray,int iFieldCount,bool bRefreshView = true);
		//删除属性字段,iIndex - 为字段的索引号,从0开始
		BOOL RemoveAttrField(int iIndex);
		//得到单个属性字段的值
		BOOL GetAttrFieldValue(GUID uOID,const char* szUnitID,const char* szFieldName,CDpDBVariant& var);
		//设置单个属性字段的值
		BOOL SetAttrFieldValue(GUID uOID,const char* szUnitID,const char* szFieldName,CDpDBVariant& var);
		
		//刷新FeatureClass的视图,主要是刷新结构.因为在属性表修改了属性项后，需要对视图进行刷新
		BOOL RefreshFeaClsView();


		//批量设置符合某种查询条件的属性值
		BOOL BatchSetAttrFieldValue(CDpUniqueFilter* nFilter,CDpUniqueFilter* pNewAttValue);

		//得到当前层所支持的层
		BOOL GetSupportClassIDs(CDpIntArray& nClassIDArray);

		//创建参数表
		virtual BOOL CreateParamTable();
		//删除参数表
		virtual BOOL DeleteParamTable();

		//修改图层名称
		BOOL ModifyLayerName(const char* szNewLayerName);

		int GetRecordCount();

		
	protected:
		//得到当前管理对象的类型,图幅/图层/原始影像
		virtual DP_OBJECT_TYPE GetObjType();
		//得到当前管理对象的成果类型,只有当管理对象是图层时才有效
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx();
		

	public:
		char*				m_szName;					//图层的名称
		char*				m_szSpatialTableName;		//空间数据表名称
		char*				m_szAttributeTableName;		//属性表名称
		int					m_iIndex;					//图层的显示顺序,0为最低层
		DP_LAYER_TYPE_EX	m_iLayerTypeEx;				//成果类型,如DEM,DLG,DOM等


	protected:
		//确定是否能编辑该图幅中的内容
		BOOL CanModifyCurrUnit(const char* szUnitID);
		//通过Feature生成向图层属性表中插入记录的SQL语句
		BOOL MakeInsertSql(CDpObject* pObject,GUID nOID,char* szSql,int iBuffCount);
		//通过Feature生成向图层属性表更新记录的SQL语句
		BOOL MakeUpdateSql(CDpObject* pObject,char* szSql,int iBuffCount);

	protected:
		DP_LAYER_TYPE		m_iLayerType;				//图层的地物类型
		CDpCustomFieldInfo*	m_pCustomFieldInfo;			//属性字段的属性数组
		int					m_iAttrFieldCount;			//属性字段的个数

	private:
		//得到所有属性字段的字段信息
		BOOL GetAllAttrFieldInfo();
		//检查该属性字段是否能够进行添加
		BOOL CanAddAttrField(CDpCustomFieldInfo* pInfo);
		//生成增加属性字段的SQL语句,可以同时添加多个字段
		//pInfo -- 要添加的字段属性的数组,iFieldCount -- 要添加的字段数
		BOOL MakeAddAttrFieldSql(CDpCustomFieldInfo* pInfo,int iFieldCount,char* szSqlOut);
		//生成向属性字典表插入记录的SQL语句
		BOOL MakeInsertAttrDircSql(CDpCustomFieldInfo* pInfo,char* szSqlOut);



};


/*******************************************************************/
/*                       矢量地物层管理类                          */
/*******************************************************************/
class CDpFeatureLayer:public CDpLayer
{
	public:
		CDpFeatureLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType);	
		virtual ~CDpFeatureLayer();
	public:
		//添加地物, 如果参数uOID不为0,则表示在添加地物的时候不需要去查找最大的地物ID,而是直接使用该参数,并且不用判断是否有编辑图幅的权限
		BOOL AddShape(const char* szGridID,CDpShape* pShape,GUID* pNewOID = NULL,BOOL bInsertEmptyAttr = true,GUID *uOID = NULL);
		//编辑地物
		BOOL SetShape(GUID OID,const char* szUnitID,CDpShape* pShape);
		//删除地物,同时删除属性数据
		BOOL DelShape(GUID OID,const char* szUnitID);

		//向当前矢量层中添加一个Feature
		BOOL AddFeature(CDpFeature* pFeature);
		//更新一个Feature
		BOOL UpdateFeature(CDpFeature* pFeature);
		//删除一个Feature
		BOOL DeleteFeature(CDpFeature* pFeature);

		BOOL FindFeature(GUID OID);
		
		//打开当前图层地物的查询
		BOOL OpenGeometryQuery(CDpSpatialQuery* pSpaFilter,CDpGeometryQuery** ppGeometryQuery);
		//打开当前图层的Feature的查询
		BOOL OpenFeatureClass(CDpStringArray* pUnitIDArray,const char* szWhereClause,CDpSpatialQuery* pSpaFilter,CDpFeatureClass** ppFeatureCls);
		/************************************************************************/
		/* SELECT[ALL|DISTINCT|DISTINCTROW|TOP] 
		{*|talbe.*|[table.]field1[AS alias1][,[table.]field2[AS alias2][,…]]} 
		FROM tableexpression[,…][IN externaldatabase] 
		[WHERE…] 
		[GROUP BY…] 
		[HAVING…] 
		[ORDER BY…] 
		[WITH OWNERACCESS OPTION]
		
		[ALL|DISTINCT|DISTINCTROW|TOP]---->pPredicate
		WHERE…------>szWhereClause*/
		/************************************************************************/
		//执行某条sql语句(此sql语句仅仅作用于本地物层表中，pSelectCol为查询列的字段名；
		BOOL QueryFeatureClass(CDpStringArray* pSelectCol,const char *pPredicate,const char* szWhereClause,CDpFeatureClass** ppFeatureCls);
};


/*******************************************************************/
/*                       用户定义层的数据对象类					   */
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

	//根据属性字段索引,得到属性值
	BOOL GetAttrValue(UINT uIndex,CDpDBVariant& var);
	//设置指定字段索引的属性值
	BOOL SetAttrValue(UINT uIndex,CDpDBVariant& var);

	//得到当前Feature内的属性字段的个数
	int	GetAttrFieldCount();

public:
	int									m_iOID;						//OID
	char								m_szUnitID[20];				//地物所属的图幅ID
	int									m_iAttrFieldCount;
	CDpDBVariant*						m_pAttrFieldValueArray;		//属性字段值的数组
};


/*******************************************************************/
/*                       用户定义层的管理类								   */
/*******************************************************************/
class CDpUserDefinedGroupMgr
{
public:
	CDpUserDefinedGroupMgr();	
	virtual ~CDpUserDefinedGroupMgr();

	BOOL Attach(CDpWorkspace* pWks, CDpStringArray &arrUnitIDs, int table);

public:
	//添加地物, 如果参数uOID不为0,则表示在添加地物的时候不需要去查找最大的地物ID,而是直接使用该参数,并且不用判断是否有编辑图幅的权限
	BOOL AddGroup(CDpUserDefinedGroupObject* pObj, int* pNewId = NULL);

	//删除地物,同时删除属性数据
	BOOL DelGroup(int nid);

	int GetGroupCount();

	//根据属性字段索引,得到属性值
	CDpUserDefinedGroupObject* GetGroup(int nid);

	//设置指定字段索引的属性值
	BOOL UpdateGroup(CDpUserDefinedGroupObject* pObj);

	int GetAttrFieldCount();	

	BOOL GetAttrFieldInfo(const char* szAttrFieldName,CDpCustomFieldInfo* pInfo);

	BOOL GetAttrFieldInfo(int idx,CDpCustomFieldInfo* pInfo);

	int  GetAttrFieldIndex(const char* szAttrFieldName);
	
	//获取一个Group的查询
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

	CDpCustomFieldInfo*	m_pCustomFieldInfo;			//属性字段的属性数组
	int					m_iAttrFieldCount;			//属性字段的个数
};

/*******************************************************************/
/*                       栅格地物层管理类                          */
/*******************************************************************/
class CDpRasterLayer:public CDpLayer
{
	public:
		CDpRasterLayer(CDpWorkspace* pWks,DP_LAYER_TYPE iLayerType = GEO_Raster);	
		virtual ~CDpRasterLayer();

	public:
		//得到要新增的栅格文件在服务器内的文件名
		//参数说明:  [in] szGridID --  栅格的图幅ID    [out] szFileName -- 返回的栅格文件名
		BOOL GetNewRasterFileNameInServer(const char* szGridID,char* szFileName);

		//添加栅格
		BOOL AddRaster(const char* szGridID,const char* pszRasterFileName,CDpRect* lpRtBound,GUID* pNewOID,BOOL bInsertEmptyAttr=true);
		//添加栅格
		BOOL AddRaster(CDpRaster* pRaster);

		//编辑栅格
		BOOL SetRaster(GUID OID,const char* szUnitID,const char* pszRasterFileName,CDpRect* lpRtBound);
		//编辑栅格
		BOOL UpdateRaster(CDpRaster* pRaster);
	
		//删除栅格
		BOOL DelRaster(GUID OID,const char* szUnitID);
		//删除栅格
		BOOL DelRaster(CDpRaster* pRaster);



	public:
		//打开当前图层地物的查询
		BOOL OpenRasterQuery(CDpSpatialQuery* pSpaFilter,CDpRasterQuery** ppRasterQuery);
		//打开当前图层的Feature的查询
		BOOL OpenRasterClass(CDpStringArray* pUnitIDArray,const char* szWhereClause,CDpSpatialQuery* pSpaFilter,CDpRasterClass** ppRasterCls);

};


/*******************************************************************/
/*                         图幅访问类
/*******************************************************************/
class CDpUnit:public CDpMgrObject
{
	public:	
		CDpUnit(CDpWorkspace* pWks);
		~CDpUnit();
	public:
//		int GetUnitCount(enum DpAPartType unitType);
		BOOL SetLocalName(const char *szLocalName);
	
		//得到图幅下指定索引的图层ID
		BOOL GetLayerID(UINT iIndex,char** ppszLayerID);
		//根据图层ID得到矢量数据,矢量记录对象在使用完后需要手工释放
		BOOL OpenGeometryQuery(const char* szLayerID,CDpGeometryQuery** ppGeometryQuery,CDpSpatialQuery* pSpaFilter = NULL);

		//根据栅格图层的ID得到栅格对象
		BOOL OpenRasterQuery(const char* szRasterLayerID,CDpRasterQuery** ppRasterQuery,CDpSpatialQuery* pSpaFilter = NULL);

		//设置图幅的空间范围
		BOOL SetUnitBound(CDpRect* pRect);

		//设置图幅的有效范围
		BOOL SetUnitVailRegion(CDpPoint* pVailReg);
		
		//创建参数表
		virtual BOOL CreateParamTable();
		//删除参数表
		virtual BOOL DeleteParamTable();

		DpAPartType			m_Type;                    //标识是模型还是图幅,modified by houkui,2006.6.19
		char*				m_szRefer;					//参考数据，给模型使用，用来标识左右片,modified by houkui,2006.6.19
		char*				m_szLocalName;				//标识图幅的地名,modified by houkui,2006.6.19

		char*				m_szUnitName;				//图幅名称
		CDpRect				m_rtBound;					//图幅范围
		CDpPoint			m_ptQuadrangle[4];			//图幅的四边形有效区域
		time_t				m_tmUpdateDate;				//更新日期,精确到秒
		DP_LOCK_TYPE		m_iUnitStatus;				//图幅的状态,1-未锁定,2-锁定
		char*				m_szLocker;					//锁定该图层的用户的用户ID
	    
		//得到当前图幅下有的图层的数目
		int	 GetLayerCount(){return m_pWks->GetLayersCount();}

	protected:
		//得到当前管理对象的类型,图幅/图层/原始影像
		virtual DP_OBJECT_TYPE GetObjType();
		//得到当前管理对象的成果类型,只有当管理对象是图层时才有效
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx();
		
};



/*******************************************************************/
/*                         影像管理类
/*******************************************************************/
class CDpImage:public CDpMgrObject
{
	public:
		CDpImage(CDpWorkspace* pWks);
		~CDpImage();
	public:
		//创建参数表
		virtual BOOL CreateParamTable();
		//删除参数表
		virtual BOOL DeleteParamTable();
		//得到当前影像内所有的标签的名称
		BOOL GetAllXmlTagName(CDpStringArray& nArray);
		//得到影像的指定名称的XML流数据
		BOOL GetImageXmlData(const char* szTagName,BYTE** buff,int* piBuffLen);
		//添加影像标签,同时添加其XML流数据
		BOOL SetImageXmlData(const char* szTagName,BYTE* buff,int iBuffLen);
		//删除影像标签
		BOOL DeleteImageXmlData(const char* szTagName);
		//设置Image的空间范围
		BOOL SetImageBound(double LTX,double LTY,double RTX,double RTY,double RBX,double RBY,double LBX,double LBY);

	protected:
		//得到当前管理对象的类型,图幅/图层/原始影像
		virtual DP_OBJECT_TYPE GetObjType();
		//得到当前管理对象的成果类型,只有当管理对象是图层时才有效
		virtual DP_LAYER_TYPE_EX GetObjLayerTypeEx();


	public:
		BOOL SetServer(const char* szServerName);
	
		char*				m_szServerName;				//记录数据服务器名字，用来存取数据,modified by houkui,2006.6.19
		
		char*				m_szImageName;				//影像名称
		char*				m_szImageDesc;				//影像描述
		char*				m_szStrip;					//航带信息
		char*				m_szFilePath;				//影像的文件路径
		char*				m_szLocker;					//锁定该图层的用户的用户ID
		UINT				m_iIndexInStrip;			//影像在航带中的索引
//		CDpRect				m_rtBound;					//影像范围
		time_t				m_tmUpdateDate;				//更新日期,精确到秒
		DP_LOCK_TYPE		m_iImageStatus;				//影像的状态,1-未锁定,2-锁定
		CDpRgn				m_nRgn;						//影像的范围,为四边行
};






/*******************************************************************/
/*                用于处理记录的类
/*******************************************************************/
class CDpRecordset
{
	public:
		CDpRecordset(CDpDatabase* pDB);
		virtual ~CDpRecordset();
	public:
		//打开查询,szSql -- 表示SQL语句
		BOOL Open(const char* szSql,DP_CURSOR_TYPE nCursorType = DP_Dynaset);
		//游标是否打开
		BOOL IsOpen();
		//关闭游标
		void Close();
		//得到记录数,如果查询到记录该函数应该返回1,否则返回0
		int  GetODBCRecordCount();
		//得到字段数目
		int GetFieldCount();
		//得到字段对应的字段索引号
		int GetFieldIndex(char* szFieldName);
		//得到字段索引对应的字段的名称
		BOOL GetFieldName(UINT uIndex,char* szFieldName);
		//得到字段的SQL数据类型
		SQLSMALLINT GetFieldSqlType(UINT uIndex);
		//得到字段的信息
		BOOL GetFieldInfo(UINT uIndex,CDpFieldInfo* pInfo);


		//向下移动一条记录
		virtual BOOL Next();

		virtual CDpObjectEx* Next(int iEmpty);

		//得到ODBC语句段句柄
		SQLHANDLE GetSTMT();

		//得到字段值,索引从1开始
		BOOL GetFieldValue(UINT uIndex,CDpDBVariant& var);
		//得到字段值
		BOOL GetFieldValue(char* szFieldName,CDpDBVariant& var);


	protected:
		CDpDatabase*		m_pDB;						//数据库对象指针
		CDpFieldInfo*		m_pFieldInfoArray;			//记录集字段信息列表
		int					m_iFieldCount;				//当前记录集的字段数
		SQLHANDLE			m_hStmt;					//用于执行查询的句柄 
		BOOL				m_bIsOpen;					//表示游标是否打开

	protected:

		//将数据源的数据类型转换成驱动程序的数据类型
		SQLSMALLINT	DSDataTypeToDrv(CDpFieldInfo* pInfo);

		BOOL GetAllFieldInfo();
		//得到记录集字段数目
		int GetColsCount();
		//得到当前二进制数据的实际长度
		long GetActualSize(UINT uIndex);
		//得到字段值的缓冲区
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
/*                用于处理记录的类
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

	//清空内存
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
/*                用于处理大的二进制字段的类
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
/*                     用于进行空间数据查询的类                    */
/*       该类对象是矢量数据和栅格数据查询的基类                    */
/*******************************************************************/
class CDpGeoQuery:public CDpRecordset
{
	public:
		CDpGeoQuery(CDpWorkspace* pWks);
		virtual ~CDpGeoQuery();
	public:
		//设置空间数据字段的名称,缺省的空间字段的名称为"Spatial_Data",
		//如果调用者要设置空间字段名,应该在执行Open之前进行设置,因为在OPEN的时候会
		//根据设置的空间字段名计算空间字段的索引号
		void SetGeoFieldName(const char* szFieldName);
		//得到当前Geo的OID
		GUID GetCurrOID(){return m_iCurOID;}
		//得到当前Geo所属的图幅ID
		BOOL GetCurrUnitID(char* szUnitID);

	public:
		//打开空间查询,szSql-是针对普通SQL的查询条件,可以包含ORDER BY,CDpSpatialQuery --空间查询条件
		virtual BOOL Open(const char* szSql,CDpSpatialQuery* pSpaFilter = NULL);
		//向下移动一条记录
		virtual BOOL Next();

	protected:
		//从数据库中的外包字段直接得到该Geo的外包
		BOOL GetGeoBound(CDpRect* pRect);
		//得到具体的地物,如果当前没有记录或以及到记录尾,则返回false
		BOOL GetGeo(CDpDBVariant& var);
		//得到当前GEO的数据类型
		DP_LAYER_TYPE GetCurGeoType();

	protected:
		CDpWorkspace*		m_pWks;						//所属的工作区
		char				m_szQueryTableName[50];		//打开的表的名称
		char				m_szGeoFieldName[50];		//空间字段的名称
		char				m_szSql[8196];				//整个查询的SQL语句
		char				m_szCurUnitID[20];			//当前记录的图幅ID
		UINT				m_iSpatialFieldIndex;		//矢量字段的索引号,在OPEN的时候从根据矢量字段名从数据库中取得
		UINT				m_iOIDIndex;				//OID的字段索引号
		UINT				m_iUnitIDIndex;				//图幅ID的字段索引号
		UINT				m_iGeoTypeIndex;			//Shape类型字段的索引号
		GUID				m_iCurOID;					//当前的OID,用于在每次移动记录时记录下当前Feature的OID

	private:
		//得到当前地物的OID
		GUID GetOID();
		//得到当前的图幅ID
		BOOL GetUnitID();
};



/*******************************************************************/
/*                     用于进行空间数据查询的类                    */
/*       该类对象只能用于对空间数据的查询,不能实现空间数据的修改   */
/*******************************************************************/
class CDpGeometryQuery:public CDpGeoQuery
{
	public:
		CDpGeometryQuery(CDpWorkspace* pWks);
		virtual ~CDpGeometryQuery();

	public:
		//得到当前GEO的数据类型
		DP_LAYER_TYPE GetCurShapeType();
		//从数据库中的外包字段直接得到该地物的外包
		BOOL GetShapeBound(CDpRect* pRect);
		//得到具体的地物,如果当前没有记录或以及到记录尾,则返回false
		BOOL GetShape(CDpShape** ppShape);

};


/*******************************************************************/
/*                     用于进行空间数据查询的类                    */
/*       该类对象只能用于对空间数据的查询,不能实现空间数据的修改   */
/*******************************************************************/
class CDpRasterQuery:public CDpGeoQuery
{
	public:
		CDpRasterQuery(CDpWorkspace* pWks);
		virtual ~CDpRasterQuery();

	public:
		//得到当前GEO的数据类型
		DP_LAYER_TYPE GetCurRasterType();
		//从数据库中的外包字段直接得到该地物的外包
		BOOL GetRasterBound(CDpRect* pRect);
		//得到具体的地物,如果当前没有记录或以及到记录尾,则返回false
		BOOL GetRasterFileName(char* pszRasterFileName);
};







/*******************************************************************/
/*                 地理数据访问类    							   */
/*   该类可以完成对空间数据和属性数据的查询,并且也能够完成         */ 
/*      空间数据和属性数据的修改                                   */ 
/*******************************************************************/
class CDpGeoClass:public CDpGeoQuery
{
	public:
		//FeaClsType -- 该FeatrueClass的地物类型
		CDpGeoClass(CDpWorkspace* pWks,DP_LAYER_TYPE GeoClsType,const char* szFeaClsName);
		virtual ~CDpGeoClass();
	public:
		//打开空间查询,szSql-是针对普通SQL的查询条件,可以包含ORDER BY,CDpSpatialQuery --空间查询条件
		//目前所有FeatureClass的表结构的前面的字段应该是(按照字段顺序):
		//OID,ST_GRID_ID,ST_MINX,ST_MINY,ST_MAXX,ST_MAXY,ST_OBJECT_TYPE,SPATIAL_DATA,ATT_OID
		//后面跟属性字段
		virtual BOOL Open(const char* szSql,CDpSpatialQuery* pSpaFilter = NULL);
		//得到属性字段的数目
		int GetAttrFieldCount();
		//得到指定属性字段的索引
		int GetAttrFieldIndex(char* szAttrFieldName);

		//按照属性字段的顺序得到指定属性字段的索引和字段信息
		int GetAttrFieldInfoByOrder(int iAttrOrder,CDpFieldInfo* pInfo);
		//取属性值,在取值的时候建议按照字段的顺序(从小到大)去取
		//iAttrFieldIndex -- 属性字段索引,从0开始
		BOOL GetAttrValue(WORD iAttrFieldIndex,CDpDBVariant& var);
		//取属性值,在取值的时候建议按照字段的顺序(从小到大)去取
		BOOL GetAttrValue(char* szFieldName,CDpDBVariant& var);
		//设置属性值,只能设置非二进制的字段
		BOOL SetAttrValue(WORD iAttrFieldIndex,CDpDBVariant& var);
		//设置属性值,只能设置非二进制的字段
		BOOL SetAttrValue(char* szFieldName,CDpDBVariant& var);

		
		
	private:
		//得到属性字段的顺序
		int GetAttrFieldStart();
		//判断当前用户是否有权限修改该FeatureClass
		BOOL CanModifyRight();


	private:
		int					m_iSpaFieldIndexStart;			//空间数据字段的开始索引
		int					m_iAttrFieldIndexStart;			//属性字段的开始索引,OID不属于属性字段,属于空间数据字段	
		DP_LAYER_TYPE		m_iGeoClsType;					//GeoClass的Geo的类型
		char				m_szFeaClsName[50];				//该FeatureClass的名称	

};



/*******************************************************************/
/*                 地物访问类									   */
/*   该类可以完成对空间数据和属性数据的查询,并且也能够完成         */ 
/*      空间数据和属性数据的修改                                   */ 
/*******************************************************************/
class CDpFeatureClass:public CDpGeoClass
{
	public:
		//FeaClsType -- 该FeatrueClass的地物类型
		CDpFeatureClass(CDpWorkspace* pWks,DP_LAYER_TYPE FeaClsType,const char* szFeaClsName);
		virtual ~CDpFeatureClass();
	public:
		//得到当前GEO的数据类型
		DP_LAYER_TYPE GetCurShapeType();
		//从数据库中的外包字段直接得到该地物的外包
		BOOL GetShapeBound(CDpRect* pRect);
		//得到具体的地物,如果当前没有记录或以及到记录尾,则返回false
		BOOL GetShape(CDpShape** ppShape);
		//得到一个Feature
		BOOL GetFeature(CDpFeature** ppFeature);
		//向下移动一条记录
		BOOL NextRecord();
};


/***********************************************************************/
/*                  用于进行批量复制Feature的时候使用的对象            */
/***********************************************************************/
class CDpFeatureCache
{

	public:
		CDpFeatureCache(CDpFeatureLayer* pFeaLayer);
		~CDpFeatureCache();

	public:
		//向Feature数组中添加Feature
		BOOL				AddFeature(CDpFeature* pFeature);
		//设置每次操作的条目数
		void				SetCountOfPerOper(int iCount);			
		//开始批处理操作
		BOOL				StartBatchOpera(DpBatchOperStatus nStatus);	
		//结束并保存批处理操作
		BOOL				EndAndSaveBatchOpera();						
		//取消批处理操作并且不保存
		BOOL				CancelBatchOpera();							

	private:
		//清空内存
		void				FreeMem();								
		//关闭SQL局柄
		void				CloseSqlHandle();
		//得到当前图层的所有图幅的最大OID以及其锁定状态
		BOOL				GetAllUnitInfo();	
		//进行数据库绑定,同时分配缓冲区,以及生成绑定信息
		BOOL				Bind();	
		//向空间数据的缓冲区填充数据
		BOOL				FillSpaCache(_DpBindFieldInfo* pInfo,CDpFeature* pFeature,int iRecNum);
		//向属性数据的缓冲去区填充数据
		BOOL				FillAttrCache(_DpBindFieldInfo* pInfo,CDpFeature* pFeature,int iRecNum);
		//将缓冲区的数据更新到数据库中
		BOOL				UpdateCacheToDB();


	private:
		CDpFeatureLayer*	m_pFeaLayer;
		DpBatchOperStatus	m_nCurOperStatus;						//当前操作的状态
		int					m_iCountOfPer;							//每次操作的条目数
		int					m_iCurItemCount;						//当前缓冲区内的条目数
		int					m_iSpatialSizeOfFtr;					//计算当前要操作的Feature的空间数据表所需的绑定内存块的大小
		int					m_iAttrSizeOfFtr;						//计算当前要操作的Feature的属性数据表所需的绑定内存块的大小
		SQLSMALLINT			m_iSpaColCount;							//空间数据表的字段数
		SQLSMALLINT			m_iAttrColCount;						//属性数据表的字段数
		BYTE*				m_pSpatialCache;						//实际的缓冲内存(空间数据)
		BYTE*				m_pAttrCache;							//实际的缓冲内存(属性数据)
		_DpBindFieldInfo*	m_pSpatialBindInfo;						//空间数据表的绑定信息
		_DpBindFieldInfo*	m_pAttrBindInfo;						//属性数据表的绑定信息
		SQLHANDLE			m_hSpaStmt;								//空间数据表的SQL局柄
		SQLHANDLE			m_hAttrStmt;							//属性数据表的SQL局柄

};



/*******************************************************************/
/*                 栅格访问类									   */
/*   该类可以完成对栅格数据和属性数据的查询,并且也能够完成         */ 
/*      栅格数据和属性数据的修改                                   */ 
/*******************************************************************/
class CDpRasterClass:public CDpGeoClass
{
	public:
		CDpRasterClass(CDpWorkspace* pWks,DP_LAYER_TYPE FeaClsType,const char* szRasterClsName);
		virtual ~CDpRasterClass();
	public:
		//得到当前GEO的数据类型
		DP_LAYER_TYPE GetCurRasterType();
		//从数据库中的外包字段直接得到该地物的外包
		BOOL GetRasterBound(CDpRect* pRect);
		//得到具体的地物,如果当前没有记录或以及到记录尾,则返回false
		BOOL GetRasterFileName(char* pszRasterFileName);

		//得到Raster
		BOOL GetRaster(CDpRaster** ppRaster);
};


/*************************************************************/
/*                  单个有属性的对象的管理类
/*************************************************************/
class CDpObject
{
	public:
		CDpObject(int iAttrFieldCount);
		virtual ~CDpObject();
	public:
		//根据属性字段索引,得到属性值
		BOOL GetAttrValue(UINT uIndex,CDpDBVariant& var);
		//设置指定字段索引的属性值
		BOOL SetAttrValue(UINT uIndex,CDpDBVariant& var);

		//得到当前Feature的OID
		GUID GetOID();
		//设置当前Feature的OID
		void SetOID(GUID uiOID);

		//得到当前Feature的所属的图幅ID
		const char* GetUnitID();
		//设置当前Feature的所属的图幅ID
		void SetUnitID(const char* szUnitID);

		//得到当前Feature内的属性字段的个数
		int	GetAttrFieldCount();

	private:
		GUID								m_iOID;						//OID
		char								m_szUnitID[20];				//地物所属的图幅ID
		int									m_iAttrFieldCount;
		CDpDBVariant*						m_pAttrFieldValueArray;		//属性字段值的数组

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
/*                 单个地物的管理类
/*          其中包含地物的矢量数据和属性数据
/*************************************************************/
class CDpFeature:public CDpObject
{
	public:
		CDpFeature(int iAttrFieldCount);
		virtual ~CDpFeature();
	public:
		//得到矢量数据,只是得到指针,ppShape的内存调用者不能释放
		BOOL GetShape(const CDpShape** ppShape);
		//设置矢量数据,只是将指针进行复制,如果当前Feature中已有Shape,则先删除原有的Shape
		BOOL SetShapeByRef(CDpShape* pShape);

	private:
		CDpShape*							m_pShape;
};


/**************************************************************/
/*                   单个栅格的管理类        
/*         其中包含栅格文件的名称,栅格的外包以及属性数据      
/**************************************************************/
class CDpRaster:public CDpObject
{
	public:
		CDpRaster(int iAttrFieldCount);
		virtual ~CDpRaster();
	public:
		//得到栅格数据的文件名
		BOOL GetRasterFileName(char* szFileName,int iBuffCount);
		//得到栅格的外包
		BOOL GetRasterBound(CDpRect* pRtBound);
		//设置栅格文件的文件名
		BOOL SetRasterFileName(const char* szFileName);
		//设置栅格的外包
		BOOL SetRasterBound(CDpRect* pRtBound);

	private:
		char*							m_szRasterFileName;
		CDpRect							m_rtBound;
};



/*******************************************************************/
/*                     参数管理对象基类
/*******************************************************************/
class CDpParamMgr
{
	public:
		CDpParamMgr(CDpDatabase* pDB);
		virtual ~CDpParamMgr();

		//打开参数表
		BOOL Open(char* szParamTableName);
		//关闭参数表
		void Close();

		//得到当前分类的数目
		int  GetCategoryCount();
		//得到参数表分类
		BOOL GetCategory(int iIndex,int* iCategoryID,const char** szCategoryName,const char** szCategoryDesc);
		//得到当前分类中参数的数目
		int  GetParamItemCount(int iCategoryID);
		//从参数结构中得到参数值
		BOOL GetParamValue(const CDpParamItem* pParamItem,CDpDBVariant& var);

		//得到参数信息
		BOOL GetParamInfoByID(int iParamID,const CDpParamItem** ppParamItem);
	
	protected:
		//得到参数信息,其中包含参数的配置信息和值,pParamItem的内存调用者不能释放
		BOOL GetParamInfo(const char* szWksID,int iCategoryID,int iParamIndex,const CDpParamItem** ppParamItem);

		//设置参数内容
		BOOL SetParamValue(const char* szWksID,int iParamID,CDpDBVariant var);
		//设置参数内容
		BOOL SetParamValue(const char* szWksID,int iParamID,DpCustomFieldType nDataType,const char* szValue);
		//添加参数项
		BOOL AddParamItem(const char* szWksID,int iCategoryID,int iParamID,const char* szParamName,const char* szParamDesc,DpCustomFieldType nDataType,int iLen,int iPrecision,int iSelID,const char* szParamValue,BOOL bIsWHField,int* iNewParamID);
		//删除一个分类项,同时该分类下的参数项也被删除
		BOOL DeleteCategory(const char* szWksID,int iCategoryID);
		//删除一个参数项,同时删除该参数项下的选择值项,bDelEmptyCategory-表示如果删除后的分组内为空的时候,是否删除空的分组项,TRUE为删除
		BOOL DeleteParamItem(const char* szWksID,int iParamID,BOOL bDelEmptyCategory);
	

	private:

	protected:
		CDpDatabase*			m_pDB;

		ParamCategoryArray		m_ParamData;				//参数数据		

		char					m_szParamTableName[50];		//参数表的名称

};






/*******************************************************************/
/*                     图层参数表访问类
/*******************************************************************/
class CDpParamTable:public CDpParamMgr
{
	public:
		CDpParamTable(CDpWorkspace* pWks)
			:CDpParamMgr(pWks->m_pDB)
		{
			m_pWks		= pWks;
		}


		//得到参数信息,其中包含参数的配置信息和值,pParamItem的内存调用者不能释放
		BOOL GetParamInfo(int iCategoryID,int iParamIndex,const CDpParamItem** ppParamItem)
		{
			return CDpParamMgr::GetParamInfo(m_pWks->m_szID,iCategoryID,iParamIndex,ppParamItem);
		}
		//设置参数内容
		BOOL SetParamValue(int iParamID,CDpDBVariant var)
		{
			return CDpParamMgr::SetParamValue(m_pWks->m_szID,iParamID,var);
		}
		//设置参数内容
		BOOL SetParamValue(int iParamID,DpCustomFieldType nDataType,const char* szValue)
		{
			return CDpParamMgr::SetParamValue(m_pWks->m_szID,iParamID,nDataType,szValue);
		}

		//添加参数项
		BOOL AddParamItem(int iCategoryID,int iParamID,const char* szParamName,const char* szParamDesc,DpCustomFieldType nDataType,int iLen,int iPrecision,int iSelID,const char* szParamValue,int* iNewParamID)
		{
			return CDpParamMgr::AddParamItem(m_pWks->m_szID,iCategoryID,iParamID,szParamName,szParamDesc,nDataType,iLen,iPrecision,iSelID,szParamValue,false,iNewParamID);
		}

		//删除一个分类项,同时该分类下的参数项也被删除
		BOOL DeleteCategory(int iCategoryID)
		{
			return CDpParamMgr::DeleteCategory(m_pWks->m_szID,iCategoryID);
		}

		//删除一个参数项,同时删除该参数项下的选择值项
		BOOL DeleteParamItem(int iParamID)
		{
			return CDpParamMgr::DeleteParamItem(m_pWks->m_szID,iParamID,true);
		}


	private:	
		CDpWorkspace*			m_pWks;						//工作区	
};


/*******************************************************************/
/*                    参数维护管理对象
/*******************************************************************/
class CDpParamWH:public CDpParamMgr
{
	public:
		CDpParamWH(CDpDatabase* pDB)
				:CDpParamMgr(pDB)
		{
		}

		//得到参数信息,其中包含参数的配置信息和值,pParamItem的内存调用者不能释放
		BOOL GetParamInfo(int iCategoryID,int iParamIndex,const CDpParamItem** ppParamItem)
		{
			return CDpParamMgr::GetParamInfo(NULL,iCategoryID,iParamIndex,ppParamItem);
		}
		//设置参数内容
		BOOL SetParamValue(int iParamID,CDpDBVariant var)
		{
			return CDpParamMgr::SetParamValue(NULL,iParamID,var);
		}
		//设置参数内容
		BOOL SetParamValue(int iParamID,DpCustomFieldType nDataType,const char* szValue)
		{
			return CDpParamMgr::SetParamValue(NULL,iParamID,nDataType,szValue);
		}

		//添加参数项
		BOOL AddParamItem(int iCategoryID,int iParamID,const char* szParamName,const char* szParamDesc,DpCustomFieldType nDataType,int iLen,int iPrecision,int iSelID,const char* szParamValue,int* iNewParamID)
		{
			return CDpParamMgr::AddParamItem(NULL,iCategoryID,iParamID,szParamName,szParamDesc,nDataType,iLen,iPrecision,iSelID,szParamValue,true,iNewParamID);
		}

		//删除一个分类项,同时该分类下的参数项也被删除
		BOOL DeleteCategory(int iCategoryID)
		{
			return CDpParamMgr::DeleteCategory(NULL,iCategoryID);
		}

		//删除一个参数项,同时删除该参数项下的选择值项
		BOOL DeleteParamItem(int iParamID)
		{
			return CDpParamMgr::DeleteParamItem(NULL,iParamID,false);
		}




};












/*******************************************************************/
/*						矢量地物类
/*	该类针对某个具体的地物进行访问
/*******************************************************************/


//矢量地物数据的结构,如果数据为点的话,则iNumParts和iNumPoints均等于1
//目前在数据库中的存贮结构为:

// -----------------------------------------------------------------------------
//     PART数目  |  Point数目 |   外包   |     PART索引表    |     Point的内容
// -----------------------------------------------------------------------------
//     4 BYTE    |   4 BYTE   | 4*8 BYTE |   PART数目*4 BYTE |  Point数目*28 BYTE

struct CDpShapeData
{
	int				iNumParts;				//PART数目
	int				iNumPoints;				//点的数目
	double			dBound[4];				//范围外包,依次为MINX,MINY,MAXX,MAXY		
	int*			pParts;					//每个PART的起始点的点号,数组的下标应等于iNumParts
	CDpPoint*		pPoints;				//具体的点,数组的下标应等于iNumPoints
};
//得到一个矢量数据的大小
int	GetShapeDataSize(CDpShapeData* pData);

class CDpShape
{
	public:
		CDpShape(DP_LAYER_TYPE iShapeType);	//构造时需要指定SHAPE的类型
		~CDpShape();
	public:
		//得到矢量类型
		DP_LAYER_TYPE GetShapeType();
		//得到点的数目
		int GetPointCount();
		//得到PART的数目
		int GetPartCount();
		//直接得到矢量数据结构的指针
		BOOL GetData(const CDpShapeData** pData);
		//直接得到矢量数据结构的连续内存,用于向数据库中放入数据
		BOOL GetData(BYTE** ppBuff,int* cbLen);

		//得到点集数据,pt-为放置点集的缓冲区,iBuffCount--为缓冲区大小(单位是BYTE)
		BOOL GetPoints(CDpPoint** pt,int* iBuffCount) const;
		//设置点集内容
		BOOL SetPoints(CDpPoint* pt,int iPointCount);
		//设置PART索引表内容
		BOOL SetParts(int* pParts,int iPartCount);
		//设置矢量数据
		BOOL SetData(CDpShapeData* pData);
		//拷贝矢量数据
		BOOL CopyData(CDpShapeData* pData);
		//拷贝矢量数据,buff - 为从数据库中取出的数据
		BOOL CopyData(BYTE* pBuff,int iBuffSize);
		//初始化矢量数据结构
		void InitData(int iPartNum,int iPointNum);
		//计算矢量数据所占的内存的大小
		int  GetDataSize();

		//清除点集
		void ClearPoints();

	private:
		DP_LAYER_TYPE	m_iShapeType;
		CDpShapeData*	m_pData;		//矢量数据
};


/*******************************************************************/
/*						空间数据查询类
/*	该类用于在进行空间查询时放入空间查询条件			
/*******************************************************************/
class CDpSpatialQuery
{
	public:
		//设置查询条件
		void SetQueryCont(DP_GETUNIT_FILTER nFilter);
		//设置查询的等级:1.简单查询,即只通过对地物的外包进行比较来完成,简单查询的速度快但精度差
		//               2.高级查询,根据地物的每个端点来判断,高级查询精度高但速度慢
		void SetQueryLevel(DP_SPATIAL_QUERY_LEVEL nLevel);
		//设置查询的方式
		void SetQueryType(DP_GETUNIT_FILTER nFilter);
		//设置查询的目标的类型,bRect -- 为true时,表示查询范围为矩形,为false时,表示查询范围为多边形
		void SetQueryBoundType(BOOL bRect = true);
		//设置查询的范围
		void SetQueryBound(RECT* rt);
		//设置查询的范围 
		void SetQueryBound(POINT* pt,int iPtCount);
};








//独立值查询中的字段过滤条件的结构
struct	_DpUniqueFilter
{
	char*			szFieldName;			//字段名
	char*			szFilter;				//过滤条件,如果过滤条件为NULL或为空字串,则表示取该字段的所有值
	DpCustomFieldType	nFilterFieldType;	//要过滤的该字段的数据类型
};


/***************************************************************************/
/*				管理独立值过滤条件的类,同时在批量更新属性值的时候也可
/*              以要来记录新的属性字段的值szFilter内保存的是新的属性值
/***************************************************************************/
class CDpUniqueFilter
{
	private:
		typedef vector<_DpUniqueFilter*> FilterArray;	//过滤条件的数组
		FilterArray		m_nFilterArray;
	public:
		CDpUniqueFilter();
		~CDpUniqueFilter();

		//添加过滤条件
		BOOL AddFilter(const char* szFieldName,const char* szFilter,DpCustomFieldType nType);
		//添加图幅过滤条件,nUnitIDArray-中为要做独立值访问的图幅的ID
		BOOL AddUnitFilter(CDpStringArray& nUnitIDArray);
		//删除过滤条件-根据索引
		BOOL RemoveFilter(int iIndex);
		//删除过滤条件-根据字段名
		BOOL RemoveFilter(const char* szFieldName);
		//删除所有的过滤条件
		BOOL RemoveAllFilter();
		//得到过滤条件-根据索引
		BOOL GetFilter(int iIndex,const _DpUniqueFilter** ppcFilter);
		//得到过滤条件--根据名称
		BOOL GetFilter(const char* szFieldName,const _DpUniqueFilter** ppcFilter);
		//得到过滤条件的数目
		int GetFilterCount(){return m_nFilterArray.size();}
		//根据现有的过滤条件得到符合过滤条件的图层
		BOOL MakeLayerFilterWhereClause(char* szLayerFilterSql,int iBuffCount);
		//根据属性表名和过滤条件生成得到该属性表符合过滤条件的独立值记录的SQL语句
		BOOL MakeGetUniqueValueRecordSql(const char* szLayerID,char* szGetRecordSql,int iBuffCount);
		//根据属性表名和过滤条件生成得到该属性表符合过滤条件的独立值记录数的SQL语句
		BOOL MakeGetUniqueValueCountSql(const char* szLayerID,char* szGetCountSql,int iBuffCount);

		//得到过滤独立值记录的WHERE子句 
		BOOL MakeUniqueFilterWhereClause(char* szRecordFilterWhereClause,int iBuffCount);

	private:
		//分析过滤条件的字串. [in]szFilterStr--过滤条件字串 [out]szResult--分析结果
		//                    [in]iBuffer    --szResult的缓冲区大小
		BOOL AnalyseFilterString(const char* szFilterStr,char* szResult,int iBuffer,DpCustomFieldType nType);

};



//查询当前工作区的所有属性表中有多少不同的属性字段的结构
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
		//添加一个属性字段到结果集中
		BOOL AddField(CDpCustomFieldInfo* pInfo);
		//删除结果集中所有的字段并释放其内存
		BOOL RemoveAllField();
		//得到结果集中指定的字段的属性
		BOOL GetField(int iIndex,const CDpCustomFieldInfo** ppInfo);
		//得到结果集中的记录数
		int  GetCount(){return m_nResultArray.size();}
	private:
		typedef vector<CDpCustomFieldInfo*> UniqueFieldResultArray;	//属性字段查询结果的数组
		UniqueFieldResultArray		m_nResultArray;
};




//独立值查询后结果的图层的结果的结构,不包含图层中符合条件的独立值组合的记录
struct _DpUniqueQueryLayersResult
{
	char*			szLayerID;				//图层ID
	char*			szLayerName;			//图层名称
	int				iCount;					//所有的符合过滤条件的独立值的个数,即不同组合的个数
};


//独立值查询后结果的图层的结果的类,不包含图层中符合条件的独立值组合的记录
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


		//向结果集中添加图层的结果
		BOOL AddLayerResult(const char* szLayerID,const char* szLayerName,int iCount);
		//删除所有的结果集并释放内存
		BOOL RemoveAllResult();
		//取得指定的图层结果
		BOOL GetResult(int iIndex,const _DpUniqueQueryLayersResult** ppcResult);
		//得到当前的记录集的记录数
		int GetCount(){return m_nResultArray.size();}

	private:
		typedef vector<_DpUniqueQueryLayersResult*> UniqueLayerResultArray;	//独立值查询结果的数组
		UniqueLayerResultArray		m_nResultArray;

};


typedef vector<CDpDBVariant*>	COLUMN_VALUE_ARRAY;


struct _DpUniqueQueryValueResult
{
	COLUMN_VALUE_ARRAY*	pColumnArray;			//字段值 
	int					iRecCount;				//记录数
};



//独立值查询后某个具体图层的独立值的组合的记录的结构
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

		//向记录集中增加记录
		BOOL AddResult(COLUMN_VALUE_ARRAY* pRst,int iRecCount);					
		//删除记录集中所有的记录
		BOOL RemoveAllResult();									
		//得到指定的记录
		BOOL GetResult(int iIndex,const _DpUniqueQueryValueResult** ppRst);
		//得到记录数
		int  GetCount(){return m_nRecordArray.size();}

	private:	
		typedef vector<_DpUniqueQueryValueResult*> UNIQUE_RECORDSET;//独立值组合的记录值
		UNIQUE_RECORDSET	m_nRecordArray;
};


/********************************************************************/
/*   
/*                            独立值管理类                     
/*   用于根据N个属性字段名去查找具有这N个属性名的层,也可以根据这N个
/*   属性字段的过滤条件去查找所有层中符合这N个条件的层以及具体的记录
/********************************************************************/
class CDpUniqueQuery
{
	
	public:
		CDpUniqueQuery(CDpWorkspace* pWks);
		~CDpUniqueQuery();
	public:
		//得到当前数据库中所有层的所有属性字段,其中包含维护的属性字段和层定义的属性字段
		BOOL GetAllAttributeFields(CDpUniqueFieldResult& nFieldsInfo);

		//根据属性字段名的列表查找具有这个属性字段的层
		//参数说明: [in]  nFilter			--  要查找的字段的过滤条件
		//          [out] nResultArray  	--  查找的结果
		//          [in]  bIsGetCountOfLayer -- 是否得到具体的每个图层内的独立值的个数
		BOOL FindLayers(CDpUniqueFilter* pFilter,CDpUniqueQueryLayerResult& nResultArray,BOOL bIsGetCountOfLayer = true);

		//打开具体的某个已经经过过滤的图层中的所有独立值的组合的结果的记录
		//参数说明: [in]  nFilterArray		-- 要查找的字段的过滤条件
		//          [in]  szLayerID			-- 所在的图层的ID
		//			[out] nRecordset		-- 返回具体的记录集
		BOOL OpenUniqueRecordInLayer(CDpUniqueFilter* nFilter,const char* szLayerID,CDpUniqueQueryValueResult& nRecordset);

	private:
		CDpWorkspace*	m_pWks;
};


/***********************************************************/
/*     用于管理要进行CHECKOUT或CHECKIN的具体的内容的列表   */
/***********************************************************/
class CDpCheckInfo
{
	public:
		//要CHECKOUT的内容列表
		struct	_DpCheckOpreaInfo
		{
			char*			szUnitID;			//图幅ID或者影像ID
			char*			szLayerID;			//图层ID或XML标签
			int				iDataType;			//操作的数据的类型,1-UNIT,2-IMAGE
			DP_LOCK_TYPE	nLockType;			//在GET的过程中要求的GET后的锁定状态
			BOOL			bSuccess;			//操作是否成功,TRUE-表示成功,FALSE-失败
			char*			szErrorMsg;			//错误信息描述
		};


	public:
		CDpCheckInfo(DpCheckType nCheckType);
		~CDpCheckInfo();

		//取当前的GET类型,即是CHECKOUT还是CHECKIN 
		DpCheckType GetCheckType(){return m_nCheckType;}

		//删除所有列表项
		void RemoveAllItem();

		//得到总共要操作的ITEM的个数
		int GetItemCount();

		//得到当前所有的要操作的不同的图层ID
		void GetAllDistinctLayer(CDpStringArray& idArray);

		//得到当前所有的要操作的不同的影像ID
		void GetAllDistinctImage(CDpStringArray& idArray);

		//得到当前所有的要操作的不同的图幅ID
		void GetAllDistinctUnit(CDpStringArray& idArray);
		
		//得到指定序号的ITEM
		BOOL GetItem(int iIndex,const _DpCheckOpreaInfo** ppInfo);

		//添加要操作的XML标签
		void AddImageXmlItem(const char* szImageID,const char* szXmlTagName,DP_LOCK_TYPE nLockType = DP_UT_UNLOCK);
		//添加要操作的图幅的图层
		void AddUnitLayer(const char* szUnitID,const char* szLayerID,DP_LOCK_TYPE nLockType = DP_UT_UNLOCK);
		//将当前图幅中的所有图层都加入到要被操作的数组中
		BOOL AddUnit(CDpWorkspace* pWks,const char* szUnitID,DP_LOCK_TYPE nLockType = DP_UT_UNLOCK,BOOL bDlgOnly = true);

	private:
		typedef vector<_DpCheckOpreaInfo*> CHECKARRAY;
		typedef map<ULONGLONG,int> DISTINCTLAYER;		
		typedef map<ULONGLONG,int> DISTINCTIMAGE;
		typedef map<ULONGLONG,int> DISTINCTUNIT;

		CHECKARRAY			m_CheckArray;		//要CHECK的信息的列表
		DISTINCTLAYER		m_DistinctLayer;	//要CHECK的不同图层的列表
		DISTINCTIMAGE		m_DistinctImage;	//要CHECK的不同的影像的列表
		DISTINCTUNIT		m_DistinctUnit;		//要CHECK的不同的图幅的列表

		DpCheckType			m_nCheckType;		//CHECK的类型

};

struct _DPWP_Param
{
	int		iPrecent;
	BOOL	bIsBreak;
	void*	pWnd;
};

typedef void (CALLBACK *PWORKPROCESS) ( _DPWP_Param* );

/*************************************************************************/
/*                          CHECK_OUT管理类                       
/* 用于管理将服务器中的数据CHECK_OUT到本地数据库文件                     
/*************************************************************************/
class CDpCheckOutMgr
{
	public:
		//pWks -- 数据库服务器上的某个已打开的工作区,用于做CheckOut
		//szTemplateFileName -- 本地文件数据库的模板数据库文件名,用于在创建空本地文件时使用
		CDpCheckOutMgr(CDpWorkspace* pWks,const char* szTemplateFileName = NULL);
		~CDpCheckOutMgr();
	public:
		//执行一个CHECKOUT操作
		//szFileName--CheckOut到本地的文件数据库的名称
		//pInfo--要GET的具体数据的ID已经标志等信息
		BOOL GetOut(const char* szFileName,CDpCheckInfo* pInfo,PWORKPROCESS pFun = NULL,_DPWP_Param* pParam = NULL);	
		
	private:
		//判断当前本地数据库文件是否存在
		BOOL DBFileIsExist(const char* szFileName);
		//生成空的本地数据库文件
		BOOL MakeEmptyDBFile(const char* szFileName);


		//向目标数据库内复制当前用户的权限数据
		BOOL CopyUserInfoToDest(SQLHANDLE hConnDest);
		//向目标数据库内复制锁定管理表的内容
		BOOL CopyLockMgrDataToDest(SQLHANDLE hConnDest);
		//向目标数据库内复制参数维护表的相关内容
		BOOL CopyParamWhDataToDest(SQLHANDLE hConnDest);	
		//向目标数据库内复制属性字段维护表的相关内容
		BOOL CopyAttrWhDataToDest(SQLHANDLE	hConnDest);
		//向目标数据库内复制WBS的维护信息
		BOOL CopyWBSWhDataToDest(SQLHANDLE hConnDest);

		//向目标本地数据库中复制工作区记录
		BOOL CopyWksDataToDest(SQLHANDLE hConnDest);
		//向目标数据库中复制图幅管理表
		BOOL CopyGridMgrDataToDest(SQLHANDLE hConnDest,CDpUnit* pUnit = NULL);
		//向目标数据库中复制影像管理表
		BOOL CopyImageMgrDataToDest(SQLHANDLE hConnDest,const char* szImageID = NULL);
		//向目标数据库中复制影像结果XML流数据表
		BOOL CopyImageXmlDataToDest(SQLHANDLE hConnDest,CDpImage* pSourceImage);
		//向目标数据库中复制指定的影像结果XML流数据
		BOOL CopyImageXmlDataToDest(SQLHANDLE hConnDest,const char* szImageID,const char* szXmlTagName);


		//向目标数据库内复制属性字典表的相关内容
		BOOL CopyAttrFieldDircToDest(SQLHANDLE hConnDest);	
		//向目标数据库复制参数表,其中包含先创建参数表后拷贝记录
		BOOL CopyParamTableToDest(SQLHANDLE hConnDest,const char* szParamTableName);
		//向目标数据库中复制图层管理表
		BOOL CopyLayerDataToDest(SQLHANDLE hConnDest);

		//在目标数据库内创建空的空间数据表
		//在创建表之前先判断该表在目标数据库中是否已经存在,如果存在就不进行创建操作
		BOOL CreateEmptySpatialTableInDest(SQLHANDLE hConnDest,const char* szSpaTableName);
		//向目标数据库中指定的空间数据表和属性数据表中复制记录
		BOOL CopyFeatureRecordToDest(SQLHANDLE hConnDest,const char* szLayerID,const char* szGridID);

		//在目标数据库内创建空的属性数据表
		//在创建表之前先判断该表在目标数据库中是否已经存在,如果存在就不进行创建操作
		BOOL CreateEmptyAttributeTableToDest(SQLHANDLE hConnDest,CDpLayer* pSourceLayer);


	private:
		CDpWorkspace*		m_pWks;
		char				m_szTemplateFileName[_MAX_PATH];
		
};


/*************************************************************************/
/*                          CHECK_IN管理类                       
/* 用于管理将源文件数据库中的数据CHECK_IN到数据库服务器中
/*************************************************************************/
class CDpCheckInMgr
{
	public:
		//pSourceWks -- 打开要CHECKIN的源数据库的工作区,目前打开的ACCESS的文件数据库
		//pDestWks   -- 打开要CHECKIN的目标数据库的工作区,目前打开的ORACLE的数据库服务器
		CDpCheckInMgr(CDpWorkspace* pSourceWks,CDpWorkspace* pDestWks);
		~CDpCheckInMgr();
	public:
		//如果源工作区在构造的时候没有传,那么就必须在此处打开
		BOOL OpenSourceWks(const char* szDBFile,const char* szWksID = NULL);

		//执行CHECK操作
		BOOL CheckIn(CDpCheckInfo* pInfo,PWORKPROCESS pFun = NULL,_DPWP_Param* pParam = NULL);

		//拷贝影像XML数据
		BOOL CopyImageXmlDataToDest(const char* szImageID,const char* szXmlTagName);
	
		//拷贝图幅图层数据
		BOOL CopyUnitLayerDataToDest(const char* szUnitID,const char* szLayerID);

	private:
		CDpWorkspace*		m_pSourceWks;
		CDpWorkspace*		m_pDestWks;
		CDpDatabase*		m_pSourceDB;

};



/********************************************************/
/*                类的固有属性管理对象                  */
/********************************************************/
class CDpFixedClassAttrMgr
{
public:
	CDpFixedClassAttrMgr(CDpDatabase* pDB);
	~CDpFixedClassAttrMgr();

	typedef vector<CDpCustomFieldInfo*> DP_CLASSFIELDARRAY;

	//类的结构
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

		int					iClassID;				//类ID
		char*				szClassName;			//类名
		char*				szDescName;				//类描述
		int					iClassType;				//类的类型 
		int					iParentClassID;			//类的父类ID
		DP_CLASSFIELDARRAY	nFieldArray;			//该类内的字段数组
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
/*                属性字段维护管理对象                  */
/********************************************************/
class CDpAttrWHMgr
{
	public:
		CDpAttrWHMgr(CDpDatabase* pDB);
		~CDpAttrWHMgr();

		typedef vector<CDpCustomFieldInfo*>	DP_WHFIELDARRAY;

		//维护属性组结构
		struct _DpAttrWHGroup
		{
			char*				szAttrGroupName;		//组名称
			char*				szAttrGroupDesc;		//组描述	
			DP_WHFIELDARRAY		nFieldArray;			//组内字段信息数组
		};

		typedef vector<_DpAttrWHGroup*>	DP_ATTRWHGROUP_ARRAY;

		DP_ATTRWHGROUP_ARRAY	m_GroupArray;

		//从数据库中加载数据,如果需要加载工作区已有的不同字段信息,则pWks不能为空
		//如果不需要加载工作区内已有字段信息,则pWks可以为NULL
		BOOL LoadDataFromDB(CDpWorkspace* pWks = NULL);

		//释放内存
		void FreeMem();

		//取组的个数
		int	 GetGroupCount();

		//取具体的组
		const _DpAttrWHGroup* GetGruop(int iIndex);

		//取组内的字段数目
		int	 GetFieldItemCount(const _DpAttrWHGroup* pGroup); 

		//取组内具体的字段信息
		const CDpCustomFieldInfo* GetFieldInfo(const _DpAttrWHGroup* pGroup,int iIndex);


	private:
		CDpDatabase*			m_pDB;
};



#endif //_DBVISIT712304823904802398490238409238





