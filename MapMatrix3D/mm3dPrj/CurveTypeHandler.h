#pragma once
#include <include/cef_v8.h>

/*
** brief �����༭�ص���
*/
class CCurveTypeHandler
{
public:
	CCurveTypeHandler();
	virtual ~CCurveTypeHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CCurveTypeHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);

};

