#pragma once
#include <include/cef_v8.h>

class CMyAbsOriDlg;
/*
** brief mm3d�ص���
*/
class CMm3dHandler
{
public:
	CMm3dHandler();
	virtual ~CMm3dHandler();

	/*
	** brief getInstance ���ػص������ĵ���
	*/
	static CMm3dHandler* getInstance();

	/*
	** brief handle �ص��ദ����
	** param funcName �ص�������
	*/
	virtual void handle(const CefString& funcName, const CefV8ValueList& arguments);

protected:
	
};

