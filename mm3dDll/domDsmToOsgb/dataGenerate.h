#pragma once
#include "stdafx.h"
#include <vector>
#include <osg/Vec3d>
#include "domDsmToOsgbDlg.h"

class CDataConvertor
{
public:
	void startProcess();

	void startProduce();
	int dsmDemToOSGB(std::string strDomPath, std::string strDsmPath, std::string strOutPath, int lod);
	static void produce(void* ptr);

	//std::string Tiff2Jpg(std::string inputFile, std::string tifName);

	int produceDsmDomOsgb(/*std::string inputFilePath, std::string strDomTitle, std::string strDsmTitle, int xPart, int yPart*/);

	CdomDsmToOsgbDlg* produceDlg;
};