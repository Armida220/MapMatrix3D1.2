#pragma once
typedef void (*stdout_redirect_callback_fun)(const char *,void *);
#include "stdafx.h"
#include <iostream>
#include <thread>
class  redirect
{
public:
	redirect();
	~redirect();
	void set_callback_fun(stdout_redirect_callback_fun fun, void * udp = nullptr);
	friend void start_sub_thread(redirect * p);
	void textout(const char * text)
	{
		if (cbf)
		{
			::EnterCriticalSection(&m_csLock);
			cbf(text, user_define_ptr);
			::LeaveCriticalSection(&m_csLock);
		}
	}
private:
	HANDLE m_hRead;
	HANDLE m_hWrite;
	CRITICAL_SECTION	m_csLock;
	stdout_redirect_callback_fun cbf;
	void * user_define_ptr;
	
	std::basic_streambuf<char>*  m_pOldBuf;
	bool stop;
	void init();
	void listen_thread();
	std::thread *pth;
};

