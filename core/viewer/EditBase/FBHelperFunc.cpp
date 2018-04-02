#include "stdafx.h"
#include "FBHelperFunc.h"
#include "ExMessage.h"


//优化滚动条，将 AfxGetMainWnd()->SendMessage 执行次数控制为 100 以内
static int g_ProgressSum = 0;
static int g_ProgressOld = 0;
static int g_ProgressCur = 0;
static HWND g_hWndProgress = NULL;


void GSetProgressHwnd(HWND hWnd)
{
	g_hWndProgress = hWnd;
}

void GProgressStart(int sum)
{
	if( g_hWndProgress==NULL )
		return;

	g_ProgressSum = sum;
	if( g_ProgressSum<=0 )
		return;

	g_ProgressCur = 0;
	g_ProgressOld = 0;

	if( g_ProgressSum>100 )
		::SendMessage(g_hWndProgress,FCCM_PROGRESS,1, 100);
	else
		::SendMessage(g_hWndProgress,FCCM_PROGRESS,1, sum);
}

void GProgressStep(int step)
{
	if( g_hWndProgress==NULL )
		return;

	if( g_ProgressSum>100 )
	{
		g_ProgressCur += step;

		int dx = g_ProgressCur*100/g_ProgressSum - g_ProgressOld*100/g_ProgressSum;

		if( dx>=1 )
		{
			::SendMessage(g_hWndProgress,FCCM_PROGRESS,2, dx);
			g_ProgressOld = g_ProgressCur;
		}
	}
	else if( g_ProgressSum>0 && g_ProgressSum<100 )
	{
		::SendMessage(g_hWndProgress,FCCM_PROGRESS,2, step);
	}
	
}

void GProgressEnd()
{
	if( g_hWndProgress==NULL )
		return;

	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,1, -1);
}

void GOutPut(LPCTSTR str, BOOL bNewLine)
{
	if( g_hWndProgress==NULL )
		return;

	::SendMessage(g_hWndProgress,FCCM_PRINTSTR,(WPARAM)bNewLine,(LPARAM)str);
}
