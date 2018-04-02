// GrElementList.h: interface for the GrElementList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRELEMENTLIST_H__9E257EA8_4167_46AC_B960_EFEDC3AB86F8__INCLUDED_)
#define AFX_GRELEMENTLIST_H__9E257EA8_4167_46AC_B960_EFEDC3AB86F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemoryPool.h"


MyNameSpaceBegin


// GrElement ��� CFArray<GrElement*>�����԰������ͱ�ſ��ټ���
struct EXPORT_SMARTVIEW GrElement
{
	GrElement(){
		no = 0;
		bvisible = 1; bmark1 = 0; bmark2 = 0;
		pBuf = NULL;
		prev = NULL; next = NULL;
	}
	LONG_PTR no;
	DWORD bvisible:1;
	DWORD bmark1:1;
	DWORD bmark2:1;
	
	void *pBuf;
	
	//����˫��������֧��Ԫ�ش���(��Ӧ����ʾ˳��)����������ٵص���
	GrElement *prev;
	GrElement *next;

	DECLARE_MEMPOOL(GrElement);
};


class EXPORT_SMARTVIEW GrElementList
{
public:
	GrElementList();
	~GrElementList();

	//�Դ���GrElement��������ʱ���õ�����������
	GrElementList(void *pBuf, BOOL bmark1=FALSE, BOOL bmark2=FALSE);
	void addBuffer(void *pBuf, BOOL bmark1=FALSE, BOOL bmark2=FALSE);
	
	void delAll();
	
	void del(GrElement *ele);
	void add(GrElement *ele);
	void moveToAfter(GrElement *moveEle, GrElement *posEle);
	void moveToBefore(GrElement *moveEle, GrElement *posEle);
	void moveToHead(GrElement *moveEle);
	void moveToTail(GrElement *moveEle);
	void reverse();
	
	GrElement *head, *tail;
	int which_mark;
private:
	BOOL bCreateElement;
};


MyNameSpaceEnd


#endif // !defined(AFX_GRELEMENTLIST_H__9E257EA8_4167_46AC_B960_EFEDC3AB86F8__INCLUDED_)
