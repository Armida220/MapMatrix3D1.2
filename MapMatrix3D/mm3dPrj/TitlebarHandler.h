#pragma once
#include <include/cef_v8.h>

/*
** brief titlebar �ص���
*/
class CTitlebarHandler
{
public:
	CTitlebarHandler();
	virtual ~CTitlebarHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CTitlebarHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

