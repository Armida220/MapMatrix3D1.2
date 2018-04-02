// XmlAccess.h: interface for the CXmlAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLACCESS_H__23800AF7_7A33_449D_802B_56227F38CA98__INCLUDED_)
#define AFX_XMLACCESS_H__23800AF7_7A33_449D_802B_56227F38CA98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Access.h"
#include "Markup.h"
#include "ValueTable.h"
#include "ObjectXmlIo.h"
#include "DataSourceEx.h"

#define XMLTAG_FILE			_T("FeatureDataFile")

#define XMLTAG_DATASOURCE	_T("WorkSpace")

#define XMLTAG_LAYER		_T("Layer")
#define XMLTAG_GROUP		_T("Group")

#define XMLTAG_LAYERID		_T("LayerID")
#define XMLTAG_GROUPID		_T("GROUPID")

// ����������Ϣ
#define XMLTAG_FEATURE		_T("Feature")
#define XMLTAG_GEOMETRY		_T("Geometry")


class CXmlAccess : public CAccessRead, public CAccessWrite
{
public:
	enum
	{
		modeRead = 0,
		modeWrite = 1
	};
	CXmlAccess();
	virtual ~CXmlAccess();

	virtual BOOL OpenRead(LPCTSTR filename);
	virtual BOOL OpenWrite(LPCTSTR filename);
	virtual BOOL CloseRead();
	virtual BOOL CloseWrite();

	void Close();

	//��ȡ����Դ��Ϣ
	virtual BOOL ReadDataSourceInfo(CDataSourceEx *pDS);

	//��ȡ��
	virtual CFtrLayer *GetFirstFtrLayer(long &idx);
	virtual CFtrLayer *GetNextFtrLayer(long &idx);
	
	//��ȡ����
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual int GetCurFtrLayID();
	virtual int GetCurFtrGrpID();

	//��ȡ��չ������Ϣ
	virtual BOOL ReadXAttribute(CFeature *pFtr, CValueTable& tab);
	
	//��������Դ��Ϣ
	virtual void WriteDataSourceInfo(CDataSourceEx *pDS);

	virtual BOOL DelXAttributes(CFeature *pFtr);
	virtual BOOL RestoreXAttributes(CFeature *pFtr);
	
	//���²�
	virtual BOOL SaveFtrLayer(CFtrLayer *pLayer);
	
	//���µ���
	virtual BOOL SetCurFtrLayID(int id);
	virtual BOOL SetCurFtrGrpID(int id);
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL);

	//������չ������Ϣ
	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0);
protected:
	CMarkup m_xmlfile;
	int m_nOpenMode;
	int m_nCurFtrLayID;
	int m_nCurFtrGrpID;
};

#endif // !defined(AFX_XMLACCESS_H__23800AF7_7A33_449D_802B_56227F38CA98__INCLUDED_)
