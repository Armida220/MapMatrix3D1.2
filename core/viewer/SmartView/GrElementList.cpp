// GrElementList.cpp: implementation of the GrElementList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GrElementList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

IMPLEMENT_MEMPOOL(GrElement);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


GrElementList::GrElementList()
{
	head = tail = NULL;
	bCreateElement = FALSE;

	which_mark = 1;
}


GrElementList::~GrElementList()
{
	delAll();
}


GrElementList::GrElementList(void *pBuf, BOOL bmark1, BOOL bmark2)
{
	which_mark = 1;
	head = tail = NULL;
	bCreateElement = TRUE;
	GrElement *p = new GrElement;
	if( p!=NULL )
	{
		p->bmark1 = (bmark1?1:0);
		p->bmark2 = (bmark2?1:0);

		p->pBuf = pBuf;

		add(p);
	}
}


void GrElementList::addBuffer(void *pBuf, BOOL bmark1, BOOL bmark2)
{
	if( head==NULL )
	{
		bCreateElement = TRUE;
	}
	if( bCreateElement )
	{
		GrElement *p = new GrElement;
		if( p!=NULL )
		{
			p->bmark1 = (bmark1?1:0);
			p->bmark2 = (bmark2?1:0);
			
			p->pBuf = pBuf;
			
			add(p);
		}
	}
}



void GrElementList::delAll()
{
	if( bCreateElement )
	{
		GrElement *p1 = head, *p2;
		while( p1!=NULL )
		{
			p2 = p1;
			p1 = p1->next;
			delete p2;			
		}
	}
	head = tail = NULL;
}

void GrElementList::del(GrElement *ele)
{
	if( head==ele )
	{
		if( tail==head )
		{
			head = NULL;
			tail = NULL;

			if( bCreateElement )delete ele;
		}
		else
		{
			head = ele->next;
			head->prev = NULL;

			if( bCreateElement )delete ele;
		}
	}
	else if( tail==ele )
	{
		tail = ele->prev;
		tail->next = NULL;

		if( bCreateElement )delete ele;
	}
	else
	{
		GrElement *prev = ele->prev;
		GrElement *next = ele->next;
		prev->next = next;
		next->prev = prev;

		if( bCreateElement )delete ele;
	}
}


void GrElementList::add(GrElement *ele)
{
	if( head==NULL )
		head = ele;
	if( tail==NULL )
		tail = ele;
	else
	{
		tail->next = ele;
		ele->next = NULL;
		ele->prev = tail;
		tail = ele;
	}
}


void GrElementList::moveToAfter(GrElement *moveNode, GrElement *posNode)
{
	if( moveNode==NULL || posNode==NULL || moveNode==posNode )
		return;

	del(moveNode);

	moveNode->next = posNode->next;
	moveNode->prev = posNode;

	if( posNode->next )posNode->next->prev = moveNode;
	posNode->next = moveNode;
	
}


void GrElementList::moveToBefore(GrElement *moveNode, GrElement *posNode)
{
	if( moveNode==NULL || posNode==NULL || moveNode==posNode )
		return;
	
	del(moveNode);
	
	moveNode->next = posNode;
	moveNode->prev = posNode->prev;
	
	if( posNode->prev )posNode->prev->next = moveNode;
	posNode->prev = moveNode;
}


void GrElementList::moveToHead(GrElement *moveNode)
{
	if( moveNode==NULL || moveNode==head )
		return;

	moveToBefore(moveNode,head);
	head = moveNode;
}


void GrElementList::moveToTail(GrElement *moveNode)
{
	if( moveNode==NULL || moveNode==tail )
		return;

	moveToAfter(moveNode,tail);
	tail = moveNode;
}	

void GrElementList::reverse()
{
	
	GrElement *prev = NULL;
    GrElement *cur = head;
    GrElement *temp;

	// Í·Î²½»»»
	temp = head;
	head = tail;
	tail = temp;

    while (cur != NULL)
    {
        temp = cur->next;
        
        cur->next = prev;
        cur->prev = temp;
        
        prev = cur;
        cur = temp;
    }

}

MyNameSpaceEnd