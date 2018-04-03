#include"stdafx.h"
#include "tlhelp32.h"

//定义一个进程ID和窗口句柄的结构体
typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

//枚举回调函数
BOOL CALLBACK EnumWindsProc(HWND hWnd,LPARAM lParam)
{ 
	DWORD dwProcessId;
	//通过窗口句柄获取进程ID
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	
	LPWNDINFO pInfo = (LPWNDINFO)lParam;
	//匹配遍历窗口进程号与通过进程名得到的进程号
	if(dwProcessId == pInfo->dwProcessId)
	{

	pInfo->hWnd = hWnd;//返回找到的窗口句柄
	
	return FALSE;
	}
	
	return TRUE;
}



int GFindWindows(LPCTSTR proc_name,HWND *bufs, int buf_len)
{

	int num=0;//返回的窗口句柄数目
	
	
	DWORD dwPID = 0;  //一个临时PID
	
	int a[MAX_PATH];//存放进程PID的数组
	DWORD Proc_num=0;//进程数量
	CString procname(proc_name);
	//匹配进程名是否含 exe
	
	if (strcmp(procname.Right(procname.GetLength() - procname.ReverseFind('.') - 1),_T("exe")))
	{	
		return -1;//参数错误返回-1
	}
	
	//************************根据进程名称获取进程ID***********//
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return 0;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		CString temp;
		TRACE(("当前查找进程的名称是: %s\n"),pe.szExeFile);
		TRACE(("当前进程PID： %d \n"),pe.th32ProcessID);
		if (!_tcscmp(pe.szExeFile, proc_name))
		{
			//CloseHandle(hSnapshot);
			
			temp.Format(("当前进程12PID： %d \n"),pe.th32ProcessID);
			
			TRACE(("当前匹配的进程PID： %d \n"),pe.th32ProcessID);
			a[Proc_num]=	pe.th32ProcessID;
			Proc_num++;
			
		}
		
	}
	CString temp1;
	//进程数量为0 提示找不到进程 返回为0；
	if (Proc_num==0)
	{
		bufs=NULL;
		buf_len=0;
		return 0;
		
	}else//找到一个匹配进程
	{
		//对一个进程名有多个相同进程ID的全部数组进行处理
		for (int j=0;j<Proc_num;j++)
		{
			
			WNDINFO wi;
			wi.dwProcessId =a[j];//将获取到的进程ID传给结构体
			wi.hWnd = NULL;
			//遍历顶层窗口获取窗口句柄
			EnumWindows(EnumWindsProc,(LPARAM)&wi);
			
			
			HWND re_hwnd = wi.hWnd;
			while( GetParent(re_hwnd) != NULL )
				//循环查找父窗口，以便保证返回的句柄是最顶层的窗口句柄
			{
				re_hwnd = GetParent( re_hwnd );
			}
			
			//比较设定的大小与设计大小
			if (num<buf_len)
			{
				//判断当前进程是否无窗口 无窗口句柄则不保存
				if (wi.hWnd!=NULL)
				{
					bufs[num]=re_hwnd;
					num++;
				}
			}
			
		}
		
		return num;//返回句柄个数
		
	}
	
}


HWND GetXHandle(LPCWSTR procName)
{
    DWORD iProcID=0;
	
    PROCESSENTRY32 pEntry={sizeof(PROCESSENTRY32)};
	
    HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);
    if(hSnapshot!=INVALID_HANDLE_VALUE)
    {
        if(Process32First(hSnapshot,&pEntry))
        {
            while (Process32Next(hSnapshot, &pEntry))
            {
                if(lstrcmpiW(procName, (LPCWSTR)pEntry.szExeFile) == 0)
                {
                    iProcID=pEntry.th32ProcessID;
                    CloseHandle(hSnapshot);
                    break;
                }
            }
        }
    }    
	
    HWND hwnd=::GetTopWindow(NULL);
    while(hwnd)
    {
        DWORD pid=0;
        DWORD dwProcessId=GetWindowThreadProcessId(hwnd,&pid);
		
        if(dwProcessId!=0)
        {
            if(pid==iProcID)
            {
                return hwnd;
            }
        }
		
        hwnd=::GetNextWindow(hwnd,GW_HWNDNEXT);
    }
    return NULL;
}
