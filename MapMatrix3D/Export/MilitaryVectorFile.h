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


#define	JBFIELD_CODE			"����"
#define JBFIELD_GEOTYPE			"GEOTYPE"
#define JBFIELD_SHAPE			"SHAPE"
#define JBFIELD_INDEX			"Ҫ�ر��"
#define JBFIELD_GRAPH			"ͼ������"


#define JBTOPO_NODE				1
#define JBTOPO_POINT			2
#define JBTOPO_AREAPOINT		3
#define JBTOPO_LINE				4
#define JBTOPO_AREA				5

//��������
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



//���
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

	//Ҫ�ز������
	CArray<GroupItem*,GroupItem*> m_arrGroups;

	//Ԫ����
	CJBMetaData m_metaData;

	//ͼ����·��
	CString m_strMapName;
	CString m_strPath;

	//�������ݵĸ�ʽ
	CArray<CJBFormatData*, CJBFormatData*> m_arrPFormat;

	BOOL m_b2D;

	//����д���ݵ���Ϣ
	FileHead m_fileHead;

protected:
	BOOL LoadFormats();
	CJBFormatData *GetFormat(LPCTSTR name);

	//��ȡ����
	void ReadCoordinates(FILE *fp, CValueTable& tab);
	void ReadAnnotCoordinates(FILE *fp, CValueTable& tab);

	void WriteCoordinates(FILE *fp, CValueTable& tab);
	void WriteAnnotCoordinates(FILE *fp, CValueTable& tab);

	void GetObjectCounts(CValueTable& tab, int *nPoint, int *nLine, int *nArea, int *nOther, int *nAnnot);

	//��ȡҪ�ز�����
	BOOL ReadGroup(int type, LPCTSTR group);
	BOOL CreateGroup(int type, LPCTSTR group);
	BOOL WriteGroup(int type, LPCTSTR group);

	//Ԫ����
	BOOL ReadYSJ();
	BOOL WriteYSJ();
	//������������
	BOOL ReadQYMSSJ();
	BOOL WriteQYMSSJ();
	//Ҫ�ز���������
	BOOL ReadYSCMSSJ();
	BOOL WriteYSCMSSJ();

	//�������Ƶ�
	BOOL ReadCLKZD();
	BOOL WriteCLKZD();
	//��ũҵ����Ļ���ʩ
	BOOL ReadGNY();
	BOOL WriteGNY();
	//����ؼ�������ʩ
	BOOL ReadJMD();
	BOOL WriteJMD();
	//½�ؽ�ͨ
	BOOL ReadLDJT();
	BOOL WriteLDJT();
	//����
	BOOL ReadGX();
	BOOL WriteGX();
	//ˮ��/½��
	BOOL ReadSYLD();
	BOOL WriteSYLD();
	//���׵�ò������
	BOOL ReadHDDM();
	BOOL WriteHDDM();
	//��ʯ���������ϰ���
	BOOL ReadJS();
	BOOL WriteJS();
	//ˮ��
	BOOL ReadSW();
	BOOL WriteSW();
	//½�ص�ò������
	BOOL ReadLDDM();
	BOOL WriteLDDM();
	//����������
	BOOL ReadJJ();
	BOOL WriteJJ();
	//ֲ��
	BOOL ReadZB();
	BOOL WriteZB();
	//�ش�Ҫ��
	BOOL ReadDC();
	BOOL WriteDC();
	//�����豸������
	BOOL ReadZHSB();
	BOOL WriteZHSB();
	//�����������
	BOOL ReadHSQYJX();
	BOOL WriteHSQYJX();
	//����Ҫ��
	BOOL ReadHKYS();
	BOOL WriteHKYS();
	//��������
	BOOL ReadJSQY();
	BOOL WriteJSQY();
	//ע��
	BOOL ReadZJ();
	BOOL WriteZJ();
};

#endif // !defined(AFX_MILITARYVECTORFILE_H__6CF14D2F_2218_4060_99F4_6C296C88954C__INCLUDED_)
