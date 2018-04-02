// PlugBase.h: interface for the CPlugBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGBASE_H__EA4B3EF4_EA2D_4291_A89C_8B2FFBD1AFD3__INCLUDED_)
#define AFX_PLUGBASE_H__EA4B3EF4_EA2D_4291_A89C_8B2FFBD1AFD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Command.h "
#include "PluginAPI.h "
#include "AccessPlugin.h "
#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#define  NULL_ID					-1  //�ڹ�������ť����Դ�ַ���ID������˵����ַ���ID��Ϊ��ֵ����ȥ����Ӧ������
#define  PLUG_TYPE_INVALID			-1
#define  PLUG_TYPE_CMD				0	//��Ӧ�˵��򹤾����Ĳ��
#define  PLUG_TYPE_DOC				1	//�����ĵ����͵Ĳ��
#define  PLUG_TYPE_FILE				2   //�ļ����뵼��


#define  PLUGFUNC_TYPE_CREATE			0x01   //Command���������Create����
#define  PLUGFUNC_TYPE_CREATE_COLLECT   0x03   // �ɼ�����
#define  PLUGFUNC_TYPE_CREATE_EDIT      0x05   // �༭����
#define  PLUGFUNC_TYPE_SELECT		    0x0D   // ѡ������
#define  PLUGFUNC_TYPE_PROCESS		    0x10   //һ��������Ӧ����



MyNameSpaceBegin


typedef void (*LPPROC_DOCMAP)();

typedef LONG_PTR FUNC_PTR;

class EXPORT_EDITBASE CPlugBase 
{
public:
	CPlugBase();	
	virtual ~CPlugBase();
	//�����������֮��ĳ�ʼ������
	virtual void Init(){};
	//���ز�������ƣ�����������˵����������������Ӧ��popup�˵��ַ���
	virtual CString GetPlugName(){return CString();};
	//���ز������PLUG_TYPE_CMD��PLUG_TYPE_DOC
	inline int GetPlugType(){return m_nType;}
	//������ĸ���
	virtual int GetItemCount() = 0;
	//�������ı���
	virtual CString GetToolBarTitle() = 0; 
	//�������˵�����λͼ��ID��λͼ��Դ��
	virtual UINT GetToolBarBmpResID() = 0;
	// ���������ID
	virtual const UINT * GetItemCommandIDs() = 0;
	//��Ӧ��������Ĳ˵�����ַ���
	virtual const UINT * GetItemMenuStrResIDs() = 0;
	//���������������Ӧ����ʾ�ַ���ID
	virtual const UINT * GetItemNamesResIDs() = 0;
	//���������Ӧ�ļ��ټ�
	virtual const char ** GetItemShortCuts() = 0;
	//���������Ӧ��������
	virtual const char ** GetItemCmdLines() = 0;
	//���������Ӧ�ĺ���ָ��
	virtual const FUNC_PTR* GetFuncPtr() = 0;
	//���������Ӧ�ĺ���������
	virtual const int * GetFuncTypes() = 0;
	//���ڴ˴��ͷŲ����ռ�õ���Դ
	virtual void Release() = 0;

	// ����ͣ������
	virtual CDockablePane* GetDockingControlBar() { return NULL; };
	// ͣ�����ڱ���
	virtual CString GetDockingControlBarTitle() { return CString();};
	// ͣ���������ͣ�����(0)������(1)������(2)������(3)���ײ�(4)���ֱ����������Ӧλ�ô��ںϲ�
	virtual int GetDockingControlBarType() { return -1; };

	//���ز������
	virtual void LoadPluginData(DocId docId) {};
protected:
	int m_nType;
	
};

//��������
class EXPORT_EDITBASE CPlugCmds: public  CPlugBase
{	
public:
	CPlugCmds();	
	virtual ~CPlugCmds();

protected:	
};
 
// �ĵ���������
class EXPORT_EDITBASE CPlugDocs:public CPlugBase
{
public:
	CPlugDocs();
	virtual~CPlugDocs();
	//�ĵ��洢�ļ��ĺ�׺��
	virtual CString GetExtString()=0;
	//��Χ����־û���Access����
	virtual CAccessPlugin *GetAccessObj()=0;

protected:
private:
};

//���뵼��
class EXPORT_EDITBASE CPlugFiles: public  CPlugBase
{	
public:
	CPlugFiles();	
	virtual ~CPlugFiles();
//	virtual void ProcessData() = 0;
	
protected:	
};

MyNameSpaceEnd
#endif // !defined(AFX_PLUGBASE_H__EA4B3EF4_EA2D_4291_A89C_8B2FFBD1AFD3__INCLUDED_)
