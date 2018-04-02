// MruFile.h: interface for the CMruFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MRUFILE_H__671CC55F_A571_4732_B0E9_8E33F33E3AFC__INCLUDED_)
#define AFX_MRUFILE_H__671CC55F_A571_4732_B0E9_8E33F33E3AFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXNUM 16     //���MRU�ļ���

class CMruFile  
{
public:
	CMruFile();
	virtual ~CMruFile();

	CString	m_IniFileName;		//ini�ļ���
	CString	m_PathName[MAXNUM];	//MRU�ļ�·����
	CString	m_nIndex[MAXNUM];	//·���°󶨵�����
	int		m_CurNum;			//��ǰ����ļ���

	void ReadMru();			//��ȡini�ļ��е�����ļ�
	void WriteMru();		//����ļ�д��ini�ļ�
	void AddMru(CString nPathName,CString nIndex = _T(""));		//�������ļ�
	void ClearMru();		//�������ļ�
	void SetMruMenu();		//�޸�����ļ��˵�
	void SetMenuPtr(CMenu *pMruMenu);		//������Ҫ�޸ĵĲ˵�ָ��
	void DelMru(CString nPathName,int nindex);
	CMenu *GetMenuPtr();
	CMenu *m_pMruMenu;
};

#endif // !defined(AFX_MRUFILE_H__671CC55F_A571_4732_B0E9_8E33F33E3AFC__INCLUDED_)
