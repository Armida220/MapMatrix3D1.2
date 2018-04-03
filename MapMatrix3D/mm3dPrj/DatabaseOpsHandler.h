#pragma once
#include <include/cef_v8.h>

/*
** brief ���ݿ⹤���ļ��Ĺ����� �ص���
*/
class CDatabaseOpsHandler
{
public:
	CDatabaseOpsHandler();
	virtual ~CDatabaseOpsHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CDatabaseOpsHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

