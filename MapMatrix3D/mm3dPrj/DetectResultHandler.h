#pragma once
#include <include/cef_v8.h>

/*
** brief �۲����ص���
*/
class CDetectResultHandler
{
public:
	CDetectResultHandler();
	virtual ~CDetectResultHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CDetectResultHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

