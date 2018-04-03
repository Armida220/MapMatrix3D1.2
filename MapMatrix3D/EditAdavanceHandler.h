#pragma once
#include <include/cef_v8.h>
/*
** brief �߼��༭�ص���
*/
class CEditAdavanceHandler
{
public:
	CEditAdavanceHandler();
	virtual ~CEditAdavanceHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CEditAdavanceHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

