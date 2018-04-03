#pragma once
#include <include/cef_v8.h>

/*
** brief �༭�ȸ��߻ص���
*/
class CEditContourHandler
{
public:
	CEditContourHandler();
	virtual ~CEditContourHandler();

	/*
	** brief getInstance ���ػص�������ĵ���
	*/
	static CEditContourHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

