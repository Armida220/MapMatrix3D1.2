
// osgMultiFileLinkCreator.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CosgMultiFileLinkCreatorApp: 
// �йش����ʵ�֣������ osgMultiFileLinkCreator.cpp
//

class CosgMultiFileLinkCreatorApp : public CWinApp
{
public:
	CosgMultiFileLinkCreatorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CosgMultiFileLinkCreatorApp theApp;