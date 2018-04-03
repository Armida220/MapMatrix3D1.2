#pragma once
#include <include/cef_v8.h>

/*
** brief �����༭�ص���
*/
class CEditBaseHandler
{
public:
	CEditBaseHandler();
	virtual ~CEditBaseHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CEditBaseHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

