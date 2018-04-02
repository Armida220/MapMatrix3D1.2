#pragma once
#include <include/cef_v8.h>

/*
** brief ���Զ���Ի���ص���
*/
class CAbsOriDlgHandler
{
public:
	CAbsOriDlgHandler();
	virtual ~CAbsOriDlgHandler();

	/*
	** brief getInstance ���ؾ��Զ���Ի���ص���ĵ���
	*/
	static CAbsOriDlgHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);

};

