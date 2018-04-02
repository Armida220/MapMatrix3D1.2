#pragma once
#include <include/cef_v8.h>
#include "include/internal/cef_ptr.h"

class CDrawPointHandler;

class COsgbView;
/**
* @brief Execute v8����ص��࣬���е���ҳ�˻ص�����������
*/
class CV8JsHandler : public CefV8Handler
{
public:
	CV8JsHandler();
	virtual ~CV8JsHandler(void);

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

	IMPLEMENT_REFCOUNTING(CV8JsHandler);


protected:

};
