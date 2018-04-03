#include "stdafx.h"

#include "redirect.h"
#include <io.h>
#pragma region redirect_streambuf
template <class E, class T = std::char_traits<E>, int BUF_SIZE = 512 >
class redirect_streambuf : public std::basic_streambuf< E, T >
{
public:
	redirect_streambuf(redirect* Log);
	virtual ~redirect_streambuf();
protected:
	virtual int_type overflow(int_type c = T::eof());
	virtual int sync();
	virtual std::streamsize xsputn(const char_type* pch, std::streamsize n);

	char_type*	psz;
	redirect*   m_Log;
	CRITICAL_SECTION	m_csLock;
};

// Ansi version
typedef redirect_streambuf< char >		redirectstreambuf;
// Unicode version
typedef redirect_streambuf< wchar_t>	wredirectstreambuf;

// TCHAR version
#ifdef _UNICODE
#define _teditstreambuf weditstreambuf
#else
#define _teditstreambuf editstreambuf
#endif

template <class E, class T, int BUF_SIZE >
redirect_streambuf<E, T, BUF_SIZE>::redirect_streambuf(
	redirect* Log
	)
	: std::basic_streambuf<E, T>(), m_Log(Log)
{
		psz = new char_type[BUF_SIZE];
		setbuf(psz, BUF_SIZE);
		// leave place for single char + 0 terminator
		setp(psz, psz + BUF_SIZE - 2);
		::InitializeCriticalSection(&m_csLock);
	}

template <class E, class T, int BUF_SIZE >
redirect_streambuf<E, T, BUF_SIZE>::~redirect_streambuf()
{
	::DeleteCriticalSection(&m_csLock);
	delete psz;
}

template <class E, class T, int BUF_SIZE >
int redirect_streambuf<E, T, BUF_SIZE>::sync(
	)
{
	overflow();
	return 0;
}

template <class E, class T, int BUF_SIZE >
typename std::streamsize redirect_streambuf<E, T, BUF_SIZE>::xsputn(
	const char_type* pch,
	std::streamsize n
	)
{
	std::streamsize nMax, nPut;
	::EnterCriticalSection(&m_csLock);
	for (nPut = 0; 0 < n;) {
		if (pptr() != 0 && 0 < (nMax = epptr() - pptr())) {
			if (n < nMax)
				nMax = n;
			traits_type::copy(pptr(), pch, nMax);

			// Sync if string contains LF
			bool bSync = traits_type::find(pch, nMax, traits_type::to_char_type('\n')) != NULL;
			pch += nMax, nPut += nMax, n -= nMax, pbump(nMax);
			if (bSync)
				sync();


		}
		else if (traits_type::eq_int_type(traits_type::eof(),
			overflow(traits_type::to_int_type(*pch))))
			break;
		else
			++pch, ++nPut, --n;
	}
	::LeaveCriticalSection(&m_csLock);
	return (nPut);
}

template <class E, class T, int BUF_SIZE >
typename redirect_streambuf< E, T, BUF_SIZE >::int_type 
redirect_streambuf< E, T, BUF_SIZE >::overflow(
	int_type c		// = T::eof() 
	)
{
	::EnterCriticalSection(&m_csLock);
	char_type* plast = pptr();
	if (c != T::eof())
		// add c to buffer
		*plast++ = c;
	*plast = char_type();

	// Pass text to the edit control
	m_Log->textout(pbase());
	setp(pbase(), epptr());

	::LeaveCriticalSection(&m_csLock);
	return c != T::eof() ? T::not_eof(c) : T::eof();
}
#pragma endregion


redirect::redirect() :cbf(nullptr), user_define_ptr(nullptr),
stop(false), m_pOldBuf(nullptr),  pth(nullptr)
{
	::InitializeCriticalSection(&m_csLock);
}


redirect::~redirect()
{
	cbf = nullptr;
	user_define_ptr = nullptr;
	if (m_pOldBuf)
		delete std::cout.rdbuf(m_pOldBuf);
	stop = true;
	pth->join();
	::DeleteCriticalSection(&m_csLock);
}

void redirect::set_callback_fun(stdout_redirect_callback_fun fun, void * udp)
{
	cbf = fun;
	user_define_ptr = udp;
	init();
}

void redirect::init()
{
	//重定向cout
	m_pOldBuf = std::cout.rdbuf(new redirectstreambuf(this));
	//重定向printf
	if (!CreatePipe(&m_hRead, &m_hWrite, NULL, 0))
	{
		m_hRead = NULL;
		m_hWrite = NULL;
		return;
	}
	int hCrt = _open_osfhandle((long)m_hWrite, 0x4000);
	FILE *hf = _fdopen(hCrt, "w");
	*stdout = *hf;
	setvbuf(stdout, NULL, _IONBF, 0);
	pth = new std::thread(&redirect::listen_thread, this);
}

// void start_sub_thread(redirect * p)
// {
// 	p->listen_thread();
// }

void redirect::listen_thread()
{
	const UINT nBuffLen = 1024 * 1; //1k
	char chBuf[nBuffLen + 1];
	UINT ttlRead = 0;
	memset(chBuf, 0, nBuffLen + 1);
	DWORD dwRead;
	HANDLE hRead = m_hRead;
	while (1)
	{
		if (stop)
			break;
		DWORD bytes = 0;
		if (!PeekNamedPipe(hRead, NULL, 0, NULL, &bytes, NULL)) break;
		if (ttlRead > 0 && (bytes == 0 || (ttlRead + bytes) > nBuffLen))
		{   // 管道中没有数据了  // 后面的数据将使缓冲区溢出，则先刷新缓冲
			// 如果已经读了一些数据，则输出之
			textout(chBuf);
			memset(chBuf, 0, nBuffLen + 1);
			ttlRead = 0;
		}
		// 没有数据可读，延时后继续测试
		if (bytes == 0)
		{
			Sleep(200);
			continue;
		}
		dwRead = 0;
		if (bytes > (nBuffLen - ttlRead))
			bytes = nBuffLen - ttlRead;
		BOOL bOk = ReadFile(hRead, chBuf + ttlRead, bytes, &dwRead, NULL);
		ttlRead += dwRead;
		chBuf[ttlRead] = '\0';

		if (stop || dwRead == 0)
		{
			break;
		}
	}
}