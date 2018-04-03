
#include "StdAfx.h"

#include "Executor.h"
#include  <io.h>
#include <shlwapi.h>

using namespace std;

BOOL MakeDirectory(CString dd)
{
	HANDLE fFile; // File Handle
	WIN32_FIND_DATA fileinfo; // File Information Structure
	CStringArray m_arr; // CString Array to hold Directory Structures
	BOOL tt; // BOOL used to test if create Directory was successful
	int x1 = 0; // Counter
	CString tem = ""; // Temporary CString Object

	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return FALSE;
	}
	m_arr.RemoveAll();

	for(x1 = 0; x1 < dd.GetLength(); x1++ ) // Parse the supplied CString Directory String
	{ 
		if(dd.GetAt(x1) != '\\') // if the Charachter is not a \ 
			tem += dd.GetAt(x1); // add the character to the Temp String
		else
		{
			m_arr.Add(tem); // if the Character is a \ 
			tem += "\\"; // Now add the \ to the temp string
		}
		if(x1 == dd.GetLength()-1) // If we reached the end of the String
			m_arr.Add(tem);
	}

	// Close the file
	FindClose(fFile);

	// Now lets cycle through the String Array and create each directory in turn
	for(x1 = 1; x1 < m_arr.GetSize(); x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem,NULL);

		// If the Directory exists it will return a false
		if(tt)
			SetFileAttributes(tem,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	}
	//  Now lets see if the directory was successfully created
	fFile = FindFirstFile(dd,&fileinfo);

	m_arr.RemoveAll();
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		// For Some reason the Function Failed  Return FALSE
		FindClose(fFile);
		return FALSE;
	}
}


LPWSTR CMFCExcute::ConvertCharToLPWSTR(const char *szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);

	return lpszPath;
};

bool CMFCExcute::createFolder(string createFilePath)
{
	if (!IsFileExists(createFilePath))
	{
		char strMsg[MAX_PATH];
		sprintf_s(strMsg, "%s", createFilePath.c_str());
		CString str = strMsg;
		bool isSuccess = MakeDirectory(str);
		return isSuccess;
	}

	return false;
}


bool CMFCExcute::IsFileExists(string fileName)
{
	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "%s", fileName.c_str());
	CString str = strMsg;

	if (PathFileExists(str))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CMFCExcute::RemoveFile(string fileName)
{
	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "%s", fileName.c_str());
	CString str = strMsg;
	DeleteFile(str);
}

void CMFCExcute::FileCopyTo(string source, string destination, string searchStr, BOOL cover)
{
	char sourceStr[MAX_PATH];
	sprintf_s(sourceStr, "%s", source.c_str());
	CString strSourcePath = sourceStr;

	char destStr[MAX_PATH];
	sprintf_s(destStr, "%s", destination.c_str());
	CString strDesPath = destStr;

	char StrSearch[MAX_PATH];
	sprintf_s(StrSearch, "%s", searchStr.c_str());
	CString strFileName = StrSearch;
	CFileFind filefinder;
	CString strSearchPath = strSourcePath + "\\" + strFileName;
	CString filename;
	BOOL bfind = filefinder.FindFile(strSearchPath);
	CString SourcePath, DisPath;

	while (bfind)
	{
		bfind = filefinder.FindNextFile();
		filename = filefinder.GetFileName();
		SourcePath = strSourcePath + "\\" + filename;
		DisPath = strDesPath + "\\" + filename;
		CopyFile(SourcePath, DisPath, cover);
	}

	filefinder.Close();
}