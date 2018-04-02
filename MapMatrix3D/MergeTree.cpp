// MergeTree.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "MergeTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeTree

CMergeTree::CMergeTree()
{
}

CMergeTree::~CMergeTree()
{
}


BEGIN_MESSAGE_MAP(CMergeTree, CTreeCtrlEx)
	//{{AFX_MSG_MAP(CMergeTree)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMergeTree::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.style |= TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_TRACKSELECT|TVS_EDITLABELS;
	return CTreeCtrlEx::PreCreateWindow(cs);
}



/////////////////////////////////////////////////////////////////////////////
// CMergeTree message handlers
