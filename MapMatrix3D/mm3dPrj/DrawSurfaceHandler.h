#pragma once
#include <include/cef_v8.h>
/*
** brief ����ص���
*/
class CDrawSurfaceHandler
{
public:
	CDrawSurfaceHandler();
	virtual ~CDrawSurfaceHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CDrawSurfaceHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);

};

