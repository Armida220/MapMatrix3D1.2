#pragma once
#include <include/cef_v8.h>



/*
** brief ͼ�α༭�ص���
*/
class CGraphicEditHandler
{
public:
	CGraphicEditHandler();
	virtual ~CGraphicEditHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CGraphicEditHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);
};

