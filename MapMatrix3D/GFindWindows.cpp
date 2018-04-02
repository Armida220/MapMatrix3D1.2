#include"stdafx.h"
#include "tlhelp32.h"

//����һ������ID�ʹ��ھ���Ľṹ��
typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

//ö�ٻص�����
BOOL CALLBACK EnumWindsProc(HWND hWnd,LPARAM lParam)
{ 
	DWORD dwProcessId;
	//ͨ�����ھ����ȡ����ID
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	
	LPWNDINFO pInfo = (LPWNDINFO)lParam;
	//ƥ��������ڽ��̺���ͨ���������õ��Ľ��̺�
	if(dwProcessId == pInfo->dwProcessId)
	{

	pInfo->hWnd = hWnd;//�����ҵ��Ĵ��ھ��
	
	return FALSE;
	}
	
	return TRUE;
}



int GFindWindows(LPCTSTR proc_name,HWND *bufs, int buf_len)
{

	int num=0;//���صĴ��ھ����Ŀ
	
	
	DWORD dwPID = 0;  //һ����ʱPID
	
	int a[MAX_PATH];//��Ž���PID������
	DWORD Proc_num=0;//��������
	CString procname(proc_name);
	//ƥ��������Ƿ� exe
	
	if (strcmp(procname.Right(procname.GetLength() - procname.ReverseFind('.') - 1),_T("exe")))
	{	
		return -1;//�������󷵻�-1
	}
	
	//************************���ݽ������ƻ�ȡ����ID***********//
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
		TRACE(("��ǰ���ҽ��̵�������: %s\n"),pe.szExeFile);
		TRACE(("��ǰ����PID�� %d \n"),pe.th32ProcessID);
		if (!_tcscmp(pe.szExeFile, proc_name))
		{
			//CloseHandle(hSnapshot);
			
			temp.Format(("��ǰ����12PID�� %d \n"),pe.th32ProcessID);
			
			TRACE(("��ǰƥ��Ľ���PID�� %d \n"),pe.th32ProcessID);
			a[Proc_num]=	pe.th32ProcessID;
			Proc_num++;
			
		}
		
	}
	CString temp1;
	//��������Ϊ0 ��ʾ�Ҳ������� ����Ϊ0��
	if (Proc_num==0)
	{
		bufs=NULL;
		buf_len=0;
		return 0;
		
	}else//�ҵ�һ��ƥ�����
	{
		//��һ���������ж����ͬ����ID��ȫ��������д���
		for (int j=0;j<Proc_num;j++)
		{
			
			WNDINFO wi;
			wi.dwProcessId =a[j];//����ȡ���Ľ���ID�����ṹ��
			wi.hWnd = NULL;
			//�������㴰�ڻ�ȡ���ھ��
			EnumWindows(EnumWindsProc,(LPARAM)&wi);
			
			
			HWND re_hwnd = wi.hWnd;
			while( GetParent(re_hwnd) != NULL )
				//ѭ�����Ҹ����ڣ��Ա㱣֤���صľ�������Ĵ��ھ��
			{
				re_hwnd = GetParent( re_hwnd );
			}
			
			//�Ƚ��趨�Ĵ�С����ƴ�С
			if (num<buf_len)
			{
				//�жϵ�ǰ�����Ƿ��޴��� �޴��ھ���򲻱���
				if (wi.hWnd!=NULL)
				{
					bufs[num]=re_hwnd;
					num++;
				}
			}
			
		}
		
		return num;//���ؾ������
		
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
