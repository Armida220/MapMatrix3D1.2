// MruFile.cpp: implementation of the CMruFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MruFile.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMruFile::CMruFile()
{
	m_IniFileName = _T("");
	for( int i=0; i<MAXNUM; i++ )
	{
		m_PathName[i] = _T("");
		m_nIndex[i] = _T("");
	}
		m_CurNum = 0;
}

CMruFile::~CMruFile()
{

}

void CMruFile::SetMenuPtr(CMenu *pMruMenu)
{
	m_pMruMenu = pMruMenu;
}

CMenu * CMruFile::GetMenuPtr()
{
	return m_pMruMenu;
}

//��ȡini�ļ��е�����ļ�
void CMruFile::ReadMru()
{
	m_CurNum = ::GetPrivateProfileInt(_T("Mru File"), _T("FileNum"), 0, m_IniFileName);  //��ȡ��ǰ�ļ���
	CString no;
	for( int i=0; i<m_CurNum; i++)
	{
		no.Format( "%d", i+1 );   //������
		::GetPrivateProfileString(_T("Mru File"), no, "", m_PathName[i].GetBuffer(MAX_PATH),
			MAX_PATH, m_IniFileName);   //��ȡ·����
		m_PathName[i].ReleaseBuffer();

		no.Format("%d_index",i+1);	//������
		::GetPrivateProfileString(_T("Mru File"), no, "", m_nIndex[i].GetBuffer(MAX_PATH),
			MAX_PATH, m_IniFileName);   //��ȡ·����
		m_nIndex[i].ReleaseBuffer();
		
	}
	SetMruMenu();   //�޸�MRU�˵�
}

//����ļ�д��ini�ļ�
void CMruFile::WriteMru()
{
	CString no;
	no.Format( "%d", m_CurNum );
	::WritePrivateProfileString( _T("Mru File"), _T("FileNum"), no, m_IniFileName );  //д��ǰ�ļ���
	for( int i=0; i<MAXNUM; i++)
	{
		no.Format( "%d", i+1 );
		::WritePrivateProfileString(_T("Mru File"), no, m_PathName[i], m_IniFileName); //д·����
		no.Format("%d_index",i+1);
		::WritePrivateProfileString(_T("Mru File"), no, m_nIndex[i], m_IniFileName); //д����
	}
}

//�������ļ���nPathName-��ӵ��ļ�·������
//���˼·����nPathName��m_PathName[]�б����Ѵ��ڣ��������������ͷ�����������ڣ�����ӵ���ͷ�������������ɾ����β��Ԫ�ء�
void CMruFile::AddMru(CString nPathName,CString nIndex)
{
	int i;
	CString str1, str2;
	CString str3,str4;
	if( m_CurNum )
	{
		if( nPathName.CompareNoCase(m_PathName[0])==0 )  //���nPathName���ڱ�ͷ������
			return;
	}
	str1 = nPathName;
	str3 = nIndex;
	i = 0;
	while( i<m_CurNum && nPathName.CompareNoCase(m_PathName[i])!=0 )  //�ڱ��в��ң�����ȵ�Ԫ������
	{
		str2 = m_PathName[i];
		m_PathName[i] = str1;
		str1 = str2;
		str4 = m_nIndex[i];
		m_nIndex[i] = str3;
		str3 = str4;
		i++;
	}
	if( i<m_CurNum )
	{
		m_PathName[i] = str1;  //nPathName�Ѵ���
		m_nIndex[i] = str3;
	}
	else if( m_CurNum<MAXNUM )
	{
		m_PathName[m_CurNum] =str1;  //��δ��
		m_nIndex[m_CurNum] = str3;
		m_CurNum++;
	}

	SetMruMenu();   //�޸�MRU�˵�
	WriteMru();     //����ļ�д��ini�ļ�
}

void CMruFile::DelMru(CString nPathName,int nindex)
{
	if (!m_pMruMenu||nindex<0||nindex>MAXNUM||nindex>m_CurNum)
	{
		return;
	}
	int i = 0;
	if (0 != nPathName.Compare(m_PathName[nindex]))
	{
		return;
	}
	m_PathName[nindex].Empty();
	m_nIndex[nindex].Empty();
	for (i = nindex;i<m_CurNum-1;i++)
	{
		m_PathName[i] = m_PathName[i+1];
		m_nIndex[i] = m_nIndex[i+1];
	}
	m_CurNum = m_CurNum-1;
	SetMruMenu();   //�޸�MRU�˵�
	WriteMru();     //����ļ�д��ini�ļ�
}


//�������ļ�
void CMruFile::ClearMru()
{
	m_CurNum = 0;
	for( int i=0; i<MAXNUM; i++)
	{	
		m_PathName[i].Empty();
		m_nIndex[i].Empty();
	}	
	SetMruMenu();   //�޸�MRU�˵�
	WriteMru();
}

//�޸�����ļ��˵�
void CMruFile::SetMruMenu()
{
// 	CMenu *pMenu = AfxGetMainWnd()->GetMenu();		//���˵�ָ��
// 	CMenu *pFileMenu = pMenu->GetSubMenu(0);		//���ļ����˵�ָ��
// 	CMenu *pMruMenu = pFileMenu->GetSubMenu(5);		//������ļ����˵�ָ��
// 	SetMenuPtr(pMruMenu);
	if (!m_pMruMenu)
	{
		return;
	}
	m_pMruMenu->RemoveMenu( ID_MRU1, MF_BYCOMMAND );	//ɾ�����˵���
	m_pMruMenu->RemoveMenu( ID_MRU2, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU3, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU4, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU5, MF_BYCOMMAND );	//ɾ�����˵���
	m_pMruMenu->RemoveMenu( ID_MRU6, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU7, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU8, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU9, MF_BYCOMMAND );	//ɾ�����˵���
	m_pMruMenu->RemoveMenu( ID_MRU10, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU11, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU12, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU13, MF_BYCOMMAND );	//ɾ�����˵���
	m_pMruMenu->RemoveMenu( ID_MRU14, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU15, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU16, MF_BYCOMMAND );
	if( m_CurNum>0 )								//���²�����˵���
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU1, _T("&1 ")+m_PathName[0] );
	if( m_CurNum>1 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU2, _T("&2 ")+m_PathName[1] );
	if( m_CurNum>2 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU3,_T("&3 ")+ m_PathName[2] );
	if( m_CurNum>3 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU4, _T("&4 ")+m_PathName[3] );
	if( m_CurNum>4 )								//���²�����˵���
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU5, _T("&5 ")+m_PathName[4] );
	if( m_CurNum>5 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU6, _T("&6 ")+m_PathName[5] );
	if( m_CurNum>6 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU7, _T("&7 ")+m_PathName[6] );
	if( m_CurNum>7 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU8, _T("&8 ")+m_PathName[7] );
	if( m_CurNum>8 )								//���²�����˵���
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU9, _T("&9 ")+m_PathName[8] );
	if( m_CurNum>9 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU10, _T("1&0 ")+m_PathName[9] );
	if( m_CurNum>10 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU11, _T("1&1 ")+m_PathName[10] );
	if( m_CurNum>11 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU12, _T("1&2 ")+m_PathName[11] );
	if( m_CurNum>12 )								//���²�����˵���
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU13, _T("1&3 ")+m_PathName[12] );
	if( m_CurNum>13 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU14, _T("1&4 ")+m_PathName[13] );
	if( m_CurNum>14 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU15,_T("1&5 ")+m_PathName[14] );
	if( m_CurNum>15)
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU16, _T("1&6 ")+m_PathName[15] );
}

