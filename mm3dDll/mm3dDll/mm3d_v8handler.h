#pragma once
#include <include/cef_v8.h>
#include "include/internal/cef_ptr.h"

/**
* @brief Execute v8����ص��࣬���е���ҳ�˻ص�����������
*/
class CMM3DV8Handler : public CefV8Handler
{
public:
	CMM3DV8Handler(CefRefPtr<CefBrowser> browser);
	virtual ~CMM3DV8Handler(void);

public:
	/**
	* @brief Execute javaScript �ص����������е���ҳ�˻ص�����������
	* @param func_name ��ҳ�лص��ĺ�������
	* @param arguments ��ҳ�лص������Ĳ���
	*/
	virtual bool Execute(const CefString& func_name,
						  CefRefPtr<CefV8Value> object,
						  const CefV8ValueList& arguments,
						  CefRefPtr<CefV8Value>& retval,
						  CefString& exception) OVERRIDE;

	IMPLEMENT_REFCOUNTING(CMM3DV8Handler);

protected:
	CefRefPtr<CefBrowser> mBrowser;
};
