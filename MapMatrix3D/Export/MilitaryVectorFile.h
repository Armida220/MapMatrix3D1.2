// MilitaryVectorFile.h: interface for the CMilitaryVectorFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MILITARYVECTORFILE_H__6CF14D2F_2218_4060_99F4_6C296C88954C__INCLUDED_)
#define AFX_MILITARYVECTORFILE_H__6CF14D2F_2218_4060_99F4_6C296C88954C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "valueTable.h"
#include "JBFormatData.h"


#define JBGEOTYPE_POINT			1
#define JBGEOTYPE_LINE			2
#define JBGEOTYPE_AREA			3
#define JBGEOTYPE_COMPLICATED		4
#define JBGEOTYPE_ANNOT			5


#define	JBFIELD_CODE			"编码"
#define JBFIELD_GEOTYPE			"GEOTYPE"
#define JBFIELD_SHAPE			"SHAPE"
#define JBFIELD_INDEX			"要素编号"
#define JBFIELD_GRAPH			"图形特征"


#define JBTOPO_NODE				1
#define JBTOPO_POINT			2
#define JBTOPO_AREAPOINT		3
#define JBTOPO_LINE				4
#define JBTOPO_AREA				5

//拓扑数据
struct TopoDataItem
{
	int type;
};

struct TopoPoint : public TopoDataItem
{
	TopoPoint(){
		type = JBTOPO_NODE;
	}

	CArray<int,int> arcList;
};

struct TopoLine : public TopoDataItem
{
	TopoLine(){
		type = JBTOPO_LINE;
		nPt0 = nPt1 = nArea0 = nArea1 = 0;
	}
	int nPt0, nPt1;
	int nArea0, nArea1;
};

struct TopoArea : public TopoDataItem
{
	TopoArea(){
		type = JBTOPO_AREA;
	}
	CArray<int,int> arcList;
	CArray<int,int> areaList;
};



//大层
struct GroupItem
{
public:
	GroupItem(){
		m_nType = m_nPoint = m_nLine = m_nArea = 0;
	}
	int m_nType;
	CString m_strID;
	CString m_strName;

	int m_nPoint, m_nLine, m_nArea;

	CValueTable m_ObjData;
	CArray<TopoDataItem*,TopoDataItem*> m_arrTopoData;
};

struct FileHead
{
	char fileName[64];
	char secret[64];
	char level[64];
	char layer[64];
	char standard[64];
	char system[128];
};

class CMilitaryVectorFile  
{
public:
	CMilitaryVectorFile();
	virtual ~CMilitaryVectorFile();

	BOOL Open(LPCTSTR path);
	BOOL Save(LPCTSTR path, LPCTSTR mapName, BOOL b2D);
	void Clear();

	// read
	int  GetGroupCount();
	LPCTSTR GetGroupID(int idx);
	LPCTSTR GetNameOfGroup(LPCTSTR group);
	int  GetObjectNumOfGroup(LPCTSTR group);
	BOOL GetObjectData(LPCTSTR group, int i, CValueTable& tab);
	BOOL GetObjectTopoData(LPCTSTR group, int i);

	void CreateDefaultObject(LPCTSTR group, CValueTable& tab);

	// write
	void PrepareSave();
	BOOL CreateDefaultGroups();
	BOOL AddObject(LPCTSTR group, CValueTable& tab);

	//要素层的数据
	CArray<GroupItem*,GroupItem*> m_arrGroups;

	//元数据
	CJBMetaData m_metaData;

	//图名和路径
	CString m_strMapName;
	CString m_strPath;

	//属性数据的格式
	CArray<CJBFormatData*, CJBFormatData*> m_arrPFormat;

	BOOL m_b2D;

	//用于写数据的信息
	FileHead m_fileHead;

protected:
	BOOL LoadFormats();
	CJBFormatData *GetFormat(LPCTSTR name);

	//读取坐标
	void ReadCoordinates(FILE *fp, CValueTable& tab);
	void ReadAnnotCoordinates(FILE *fp, CValueTable& tab);

	void WriteCoordinates(FILE *fp, CValueTable& tab);
	void WriteAnnotCoordinates(FILE *fp, CValueTable& tab);

	void GetObjectCounts(CValueTable& tab, int *nPoint, int *nLine, int *nArea, int *nOther, int *nAnnot);

	//读取要素层数据
	BOOL ReadGroup(int type, LPCTSTR group);
	BOOL CreateGroup(int type, LPCTSTR group);
	BOOL WriteGroup(int type, LPCTSTR group);

	//元数据
	BOOL ReadYSJ();
	BOOL WriteYSJ();
	//区域描述数据
	BOOL ReadQYMSSJ();
	BOOL WriteQYMSSJ();
	//要素层描述数据
	BOOL ReadYSCMSSJ();
	BOOL WriteYSCMSSJ();

	//测量控制点
	BOOL ReadCLKZD();
	BOOL WriteCLKZD();
	//工农业社会文化设施
	BOOL ReadGNY();
	BOOL WriteGNY();
	//居民地及附属设施
	BOOL ReadJMD();
	BOOL WriteJMD();
	//陆地交通
	BOOL ReadLDJT();
	BOOL WriteLDJT();
	//管线
	BOOL ReadGX();
	BOOL WriteGX();
	//水域/陆地
	BOOL ReadSYLD();
	BOOL WriteSYLD();
	//海底地貌及底质
	BOOL ReadHDDM();
	BOOL WriteHDDM();
	//礁石、沉船、障碍物
	BOOL ReadJS();
	BOOL WriteJS();
	//水文
	BOOL ReadSW();
	BOOL WriteSW();
	//陆地地貌及土质
	BOOL ReadLDDM();
	BOOL WriteLDDM();
	//境界与政区
	BOOL ReadJJ();
	BOOL WriteJJ();
	//植被
	BOOL ReadZB();
	BOOL WriteZB();
	//地磁要素
	BOOL ReadDC();
	BOOL WriteDC();
	//助航设备及航道
	BOOL ReadZHSB();
	BOOL WriteZHSB();
	//海上区域界线
	BOOL ReadHSQYJX();
	BOOL WriteHSQYJX();
	//航空要素
	BOOL ReadHKYS();
	BOOL WriteHKYS();
	//军事区域
	BOOL ReadJSQY();
	BOOL WriteJSQY();
	//注记
	BOOL ReadZJ();
	BOOL WriteZJ();
};

#endif // !defined(AFX_MILITARYVECTORFILE_H__6CF14D2F_2218_4060_99F4_6C296C88954C__INCLUDED_)
