#include "stdafx.h"
//#include "..\include\cv.h"
//#include "..\include\highgui.h"

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

/* any depth, ? */ 
#define CV_LOAD_IMAGE_ANYDEPTH    2
/* ?, any color */
#define CV_LOAD_IMAGE_ANYCOLOR    4

BOOL SM_VerifyFolderExist(LPCTSTR lpszFolderPath, BOOL bCreate/*=TRUE*/)
{
	if (bCreate && GetFileAttributes(lpszFolderPath) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectory(lpszFolderPath, NULL);
	}

	return (GetFileAttributes(lpszFolderPath) != INVALID_FILE_ATTRIBUTES);
}

BOOL SM_DeleteDirectory(LPCTSTR lpszDirName)
{
	CFileFind tempFind;			//声明一个CFileFind类变量，以用来搜索
	TCHAR tempFileFind[200];	//用于定义搜索格式
	_stprintf(tempFileFind, _T("%s\\*.*"), lpszDirName);

	//匹配格式为*.*,即该目录下的所有文件 
	BOOL IsFinded = (BOOL)tempFind.FindFile(tempFileFind);

	//查找第一个文件
	while (IsFinded)
	{
		IsFinded = (BOOL)tempFind.FindNextFile(); //递归搜索其他的文件
		if (!tempFind.IsDots()) //如果不是"."目录
		{
			char foundFileName[200];
			strcpy(foundFileName, tempFind.GetFileName().GetBuffer(200));
			if (tempFind.IsDirectory()) //如果是目录，则递归地调用
			{
				//DeleteDirectory
				TCHAR tempDir[200];
				_stprintf(tempDir, _T("%s\\%s"), lpszDirName, foundFileName);
				SM_DeleteDirectory(tempDir);
			}
			else
			{
				//如果是文件则直接删除之
				char tempFileName[200];
				_stprintf(tempFileName, _T("%s\\%s"), lpszDirName, foundFileName);
				DeleteFile(tempFileName);
			}
		}
	}
	tempFind.Close();

//	if(!RemoveDirectory(lpszDirName)) //删除目录
//	{
//		AfxMessageBox("删除目录失败！",MB_OK);
//		return FALSE;
//	}
	return TRUE;
}

//返回值： 
//		0	-> 文件不存在  
//		1	-> 正常TIF
//		2   -> 解密成功
//		-2	-> 拷贝TIF到临时解压目录失败
//		-3	-> 打开临时目录的TIF失败
int SM_DomDecryption(LPCTSTR lpszDom, TCHAR szNewDomPath[_MAX_PATH])
{
	return 1;
#if 0
	FILE *pFile = _tfopen(lpszDom, _T("rb"));

	char pch;
	if (pFile == NULL)
	{
		return 0;
	}
	
	fscanf(pFile, "%c", &pch);
	fclose(pFile);

	if (pch == 'I' || pch == 'M')
	{
		//未加密 
		_tcscpy(szNewDomPath, lpszDom);
		return 1;
	}

	TCHAR szPath[_MAX_PATH];
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_INTERNET_CACHE, FALSE);

	CString szCachePath;
	szCachePath.Format("%s\\SM_TEP", szPath);

	SM_VerifyFolderExist(szCachePath, TRUE);
	SM_DeleteDirectory(szCachePath);

	//DOM解密
	CString szNewDom, szNewDom_2;
	CString szOldTfw, szNewTfw;

	DWORD dwTickCount = GetTickCount();

	szNewDom.Format("%s\\temp.tif", szCachePath);
	szNewDom_2.Format("%s\\%d.tif", szCachePath, dwTickCount);

	szOldTfw = lpszDom;
	szOldTfw.Replace(_T(".tif"), _T(".tfw"));
	szNewTfw.Format("%s\\%d.tfw", szCachePath, dwTickCount);

	if (!CopyFile(szOldTfw, szNewTfw, FALSE) || !CopyFile(lpszDom, szNewDom, FALSE))
	{
		return -1;
	}

	pFile = _tfopen(szNewDom, _T("rb+"));
	if (pFile == NULL)
	{
		return -3;
	}

	fseek(pFile, 0, SEEK_SET);

	pch -= 2;
	if (pFile != NULL)
	{
		fwrite(&pch, 1, 1, pFile);
		fwrite(&pch, 1, 1, pFile);
	}
	fclose(pFile);

	IplImage *pImg = cvLoadImage(szNewDom, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if (pImg == NULL)
	{
		return -3;
	}

	//the encryption color map
	BYTE pLut[256];
	int i = 0;
	for ( i = 0; i < 256; i++)
	{
		pLut[i] = i;
	}

	for ( i = 1; i <= 49; i++)
	{
		pLut[i] = 100 + i; // 1 -- 49 > 101 -- 149
	}

	for ( i = 101; i <= 149; i++)
	{
		pLut[i] = i - 100; //101 --- 149 > 1--49
	}

	for ( i = 51; i < 75; i++)
	{
		pLut[i] = 25 + i;  // 76 --- 99
	}
	for ( i = 76; i < 100; i++)
	{
		pLut[i] = i - 25; // 51 -- 74
	}

	for ( i = 151; i < 200; i++) // 201 -- 249
	{
		pLut[i] = i + 50;
	}
	for ( i = 201; i <= 249; i++) // 151 -- 199
	{
		pLut[i] = i - 50;
	}

	PBYTE pDataLine = (PBYTE)pImg->imageData;

	for (int h = 0; h < pImg->height; h++)
	{
		PBYTE pLine = pDataLine + h*pImg->widthStep;
		for (int w = 0; w < pImg->widthStep; w++)
		{
			*pLine = pLut[*pLine];
			pLine++;
		}
	}

	cvSaveImage(szNewDom_2, pImg);
	cvReleaseImage(&pImg);

	remove(szNewDom);
	_tcscpy(szNewDomPath, szNewDom_2);

	//生成金字塔ovr 文件
#ifdef BUILD_OVR_FILE
#	ifndef UNICODE
	GDALDatasetEx *pDataset = GDALOpenEx1(szNewDomPath, GA_ReadOnly);

	int anPyramidLevels[4] = { 2, 4, 6, 8 };
	pDataset->BuildOverviews("nearest", 4, anPyramidLevels, 0, 0, nullptr, nullptr);
	pDataset->Close();
	GDALReleaseEx(pDataset);
#	endif
#endif
#endif
	return 2;
}