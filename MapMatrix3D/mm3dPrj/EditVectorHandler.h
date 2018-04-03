#pragma once
#include <include/cef_v8.h>
/*
** brief �༭ʸ���ص���
*/

class CEditVectorHandler
{
public:
	CEditVectorHandler();
	virtual ~CEditVectorHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CEditVectorHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

