#pragma once
#include <string>


class CMFCExcute
{
public:
	LPWSTR ConvertCharToLPWSTR(const char *szString);

	bool createFolder(std::string createFilePath);

	bool IsFileExists(std::string fileName);

	void RemoveFile(std::string fileName);

	void FileCopyTo(std::string source, std::string destination, std::string searchStr, BOOL cover = TRUE);
};
	

