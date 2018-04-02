#include "StdAfx.h"
#include "SceneDomGenerator.h"
#include "osgTransNodeFinder.h"
#include <osg/ComputeBoundsVisitor>
#include "OsgPagedLodFinder.h"
#include "osgDB/ReadFile"
#include "OsgPagedLodBottomLevelFinder.h"
#include "OsgGeodeFinder.h"

#include "OsgVertextExtract.h"
#include "OsgRepairTransNodeFinder.h"
#include "OsgTransFolderFileNameFinder.h"
#include "ScriptExecutor.h"
#include "mm3dDll.h"
#include "Executor.h"
#include "resource.h"
#include <osg/StateAttribute>

using namespace std;
using namespace osg;

extern Cmm3dDllApp theApp;

CSceneDomGenerator::CSceneDomGenerator(string sceneFileName, 
	string outFileName, double mRes, int xTileNum, int yTileNum)
	: mSceneFileName(sceneFileName),
	mRes(mRes),
	mOutputFileName(outFileName),
	mXTileNum(1),
	mYTileNum(1)
{

}

bool CSceneDomGenerator::startProducingDOM()
{
	mSceneNode = osgDB::readNodeFile(mSceneFileName);
	bool isSuccess = false;
	vector<MatrixTransform*> vecTransNode;
	COsgTransNodeFinder ive;
	ive.findTransNode(mSceneNode, vecTransNode);

	if (vecTransNode.size() == 1)
	{
		isSuccess = generateSingleDOM(vecTransNode[0], mOutputFileName, false);
	}
	else if (vecTransNode.size() >= 2)
	{
		isSuccess = generateMultiDom(vecTransNode);
	}
	return isSuccess;
}

bool CSceneDomGenerator::generate(string outputDomFileName, string outputCoordFileName, ref_ptr<MatrixTransform> mTrans, 
	bool isCombine)
{
	int rPos = mSceneFileName.rfind('\\');
	string sceneFilepath = mSceneFileName.substr(0, rPos + 1);

	int pos = 0;
	Matrix matrix = mTrans->getMatrix();
	vector<string> vecImgFile;
	vector<string> vecCoordTxtFileName;
	
	vector<PagedLOD*> vecPagedLod;
	COsgPagedLodFinder ive;
	ive.FindPagedLod(mSceneNode, vecPagedLod);

	if (vecPagedLod.size() == 0)
	{
		return false;
	}

	for (int i = 0; i < vecPagedLod.size(); i++)											
	{
		ref_ptr<Node> node = vecPagedLod[i];
		string fileName = vecPagedLod[i]->getFileName(1);

		vector<string> vecChildFileName;
		COsgPagedLodBottomLevelFinder ive(&vecChildFileName);
		node->accept(ive);

		if (vecChildFileName.size() == 0)
		{
			continue;
		}
		
		ref_ptr<Group> group = new Group;

		for (int j = 0; j < vecChildFileName.size(); j++)									 //生成block影像
		{
			if (vecChildFileName[j].size() <= 0)
			{
				continue;
			}

			ref_ptr<Node> node = osgDB::readNodeFile(vecChildFileName[j]);

			if (!node)
			{
				continue;
			}

			COsgGeodeFinder ive;
			node->accept(ive);

			if (ive.bFindGeode == false)
			{
				continue;
			}

			node->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::ON);
			group->addChild(node);
		}

		if (group->getNumChildren() == 0)
		{
			continue;
		}

		int pos = vecChildFileName[0].rfind('.') + 1;
		string outRectDomFileName = vecChildFileName[0].substr(0, pos) + "jpg";

		if (produceNodeDOM(group, outRectDomFileName, matrix))
		{
			vecImgFile.push_back(outRectDomFileName);
			vecCoordTxtFileName.push_back(vecChildFileName[0].substr(0, pos) + "txt");
		}
		
		double totalNum = vecPagedLod.size();
		double blkPercent = i * 100 / totalNum;
		string script = "setProgressVal(" + to_string(blkPercent) + ")";
		CScriptExecutor::getInstance()->excuteScript(script);
	}

	vector<Vec4d> vecRect = findImageRect(vecCoordTxtFileName);			   //获取image范围
	Vec4d maxRect = findMaxRect(vecRect);										   //找到最大范围

	//计算范围创建影像
	int row = (maxRect.w() - maxRect.z()) / mRes;
	int col = (maxRect.y() - maxRect.x()) / mRes;
	//创建
	GDALAllRegister();          //GDAL所有操作都需要先注册格式
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF"); //图像驱动
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");  
	char** ppszOptions = CSLSetNameValue(NULL, "BIGTIFF", "IF_NEEDED"); //配置图像信息	
	int nBandCount = 3;
	GDALDataset* poDataset = pDriver->Create(outputDomFileName.c_str(), col, row, nBandCount, GDT_Byte, ppszOptions);

	for (int i = 0; i < vecRect.size(); i++)
	{
		Vec4d rect;

		if (findTileImageRect(vecCoordTxtFileName[i], rect))
		{
			combineImage(poDataset, vecImgFile[i], vecRect[i], maxRect);
			CMFCExcute excutor;
			excutor.RemoveFile(vecImgFile[i]);
			excutor.RemoveFile(vecCoordTxtFileName[i]);
		}
	}

	if (!isCombine)
	{
		startRepairDom(mTrans, poDataset, sceneFilepath, maxRect);
	}

	//设置仿射变换参数
	double  adfGeoTransform[6] = {0};
	adfGeoTransform[0] = maxRect.x();
	adfGeoTransform[3] = maxRect.w();
	adfGeoTransform[1] = mRes;
	adfGeoTransform[5] = -mRes;
	poDataset->SetGeoTransform(adfGeoTransform);
	GDALClose(poDataset);
	poDataset = NULL;

	double xRot = 0;
	double yRot = 0;
	FILE* fpCoord = fopen(outputCoordFileName.c_str(), "w");
	fprintf(fpCoord, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", mRes, xRot, yRot, -mRes, maxRect.x(), maxRect.w());
	fclose(fpCoord);

	string script = "setProgressVal(100)";
	CScriptExecutor::getInstance()->excuteScript(script);
}

bool CSceneDomGenerator::generateSingleDOM(ref_ptr<MatrixTransform> mTrans, string outputDomFileName, bool isCombine)
{
	int pos = outputDomFileName.rfind('.');
	string outputCoordFileName = outputDomFileName.substr(0, pos) + ".tfw";
	bool isSuccess = generate(outputDomFileName, outputCoordFileName, mTrans,isCombine);

	return isSuccess;
}

bool CSceneDomGenerator::generateMultiDom(vector<MatrixTransform*> vecTransNode)
{
	vector<string> vecImageFileName;
	vector<string> vecCoordFileName;
	bool isSuccess = false;

	//获取block目录
	int pos = mSceneFileName.rfind('\\');
	string sceneFilePath = mSceneFileName.substr(0, pos);

	vector<string> vecSceneFilePath;

	//生成多个block的DOM
	for (int i = 0; i < vecTransNode.size(); i++)
	{
		
		COsgVertextExtract ive;
		vecTransNode[i]->accept(ive);
		ref_ptr<Vec3Array> extracted_verts = ive.extracted_verts;

		if (extracted_verts->size() > 0)
		{
			ref_ptr<MatrixTransform> mTrans = vecTransNode[i];
			COsgTransFolderNameFinder ive;
			mTrans->accept(ive);
			string folderName = ive.FolderFileName;

			if (folderName.empty())
			{
				continue;
			}

			string blockSceneFilePath = sceneFilePath + "\\" + folderName + "\\";

			string sceneImageFileName = blockSceneFilePath + "TDOM.tif";
			string sceneCoordFileName = blockSceneFilePath + "TDOM.tfw";
			//对单个block生成DOM
			if (generateSingleDOM(mTrans, sceneImageFileName, true))
			{
				//保存影像和范围
				vecImageFileName.push_back(sceneImageFileName);
				vecCoordFileName.push_back(sceneCoordFileName);
				isSuccess = true;
			}
		}

		vecSceneFilePath.push_back(sceneFilePath);
	}

	if (isSuccess)
	{
		isSuccess = outputImageAndCoords(vecTransNode, vecSceneFilePath, vecImageFileName, vecCoordFileName);
	}

	return isSuccess;
}


bool CSceneDomGenerator::produceNodeDOM(ref_ptr<Node> loadedModel, string &outRectDomFileName, Matrix matrix)
{
	loadedModel->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::ON);		//设置全光源
	loadedModel->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, StateAttribute::ON);

	ref_ptr<MatrixTransform> mTrans = new MatrixTransform;
	mTrans->setMatrix(matrix);
	mTrans->addChild(loadedModel);

	//计算分块大小
	vector<Vec3d> rectNode;

	if (findMinMaxNode(loadedModel, matrix, rectNode) == false)
	{
		return false;
	}

	bool isSuccess = produceRectDom(mTrans, rectNode, outRectDomFileName);

	return true;
}


Vec4d CSceneDomGenerator::findMaxRect(vector<Vec4d> vecRect)
{
	double xMin = vecRect[0].x();
	double xMax = vecRect[0].y();
	double yMin = vecRect[0].z();
	double yMax = vecRect[0].w();

	for (int i = 1; i < vecRect.size(); i++)
	{
		if (xMin > vecRect[i].x())
		{
			xMin = vecRect[i].x();
		}

		if (xMax < vecRect[i].y())
		{
			xMax = vecRect[i].y();
		}

		if (yMin > vecRect[i].z())
		{
			yMin = vecRect[i].z();
		}

		if (yMax < vecRect[i].w())
		{
			yMax = vecRect[i].w();
		}
	}

	return Vec4d(xMin, xMax, yMin, yMax);
}


vector<Vec4d> CSceneDomGenerator::findImageRect(vector<string> vecCoordTxtFileName)
{
	//寻找所有影像范围
	vector<Vec4d> vecRect;

	for (int i = 0; i < vecCoordTxtFileName.size(); i++)
	{
		Vec4d rect;

		if (findTileImageRect(vecCoordTxtFileName[i], rect))
		{
			vecRect.push_back(rect);
		}
	}

	return vecRect;
}

void CSceneDomGenerator::combineImage(GDALDataset* poDataset, string imgFileName, Vec4d rect, Vec4d maxRect)
{
	GDALAllRegister();
	GDALDataset* poBlockDataset = (GDALDataset*)GDALOpen(imgFileName.c_str(), GA_ReadOnly);

	if (poBlockDataset == NULL)
	{
		GDALClose(poBlockDataset);
		poBlockDataset = NULL;
		return;
	}

	int rowDiff = (maxRect.w() - rect.w()) / mRes;
	int colDiff = (rect.x() - maxRect.x()) / mRes;

	int totalCol = poDataset->GetRasterXSize();
	int totalRow = poDataset->GetRasterYSize();
	int totalBandCount = poDataset->GetRasterCount();

	int blockCol = poBlockDataset->GetRasterXSize();
	int blockRow = poBlockDataset->GetRasterYSize();

	if (blockCol <= 0 || blockRow <= 0)
	{
		return;
	}

	int bandCount = poBlockDataset->GetRasterCount();

	int newBlockCol = 0;
	int newBlockRow = 0;

	if (rowDiff + blockRow > totalRow)
	{
		newBlockRow = totalRow - rowDiff;
	}
	else
	{
		newBlockRow = blockRow;
	}

	if (colDiff + blockCol > totalCol)
	{
		newBlockCol = totalCol - colDiff;
	}
	else
	{
		newBlockCol = blockCol;
	}

	if (newBlockCol <= 0 || newBlockRow <= 0)
	{
		return;
	}

	unsigned char* pBlockData = new unsigned char[blockRow * blockCol * bandCount];
	poBlockDataset->RasterIO(GF_Read, 0, 0, blockCol, blockRow, pBlockData, blockCol, blockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	unsigned char* pTotalBlockData = new unsigned char[newBlockRow * newBlockCol * bandCount];
	poDataset->RasterIO(GF_Read, colDiff, rowDiff, newBlockCol, newBlockRow, pTotalBlockData, newBlockCol, newBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	for (int i = 0; i < newBlockRow; i++)
	{
		for (int j = 0; j < newBlockCol; j++)
		{
			int originR = pTotalBlockData[0 * newBlockCol * newBlockRow + i * newBlockCol + j];
			int originG = pTotalBlockData[1 * newBlockCol * newBlockRow + i * newBlockCol + j];
			int originB = pTotalBlockData[2 * newBlockCol * newBlockRow + i * newBlockCol + j];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = pBlockData[0 * blockCol * blockRow + i * blockCol + j];
			int G = pBlockData[1 * blockCol * blockRow + i * blockCol + j];
			int B = pBlockData[2 * blockCol * blockRow + i * blockCol + j];

			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB < aveRGB)													//用周边的高亮像素覆盖
			{
				pTotalBlockData[0 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[0 * blockCol * blockRow + i * blockCol + j];
				pTotalBlockData[1 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[1 * blockCol * blockRow + i * blockCol + j];
				pTotalBlockData[2 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[2 * blockCol * blockRow + i * blockCol + j];
			}
		}
	}

	poDataset->RasterIO(GF_Write, colDiff, rowDiff, newBlockCol, newBlockRow, pTotalBlockData, newBlockCol, newBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	delete[] pBlockData;
	delete[] pTotalBlockData;

	GDALClose(poBlockDataset);
	poBlockDataset = NULL;
}


bool CSceneDomGenerator::findTileImageRect(string txtFileName, Vec4d &rect)
{
	cout << txtFileName << endl;
	FILE* fpRect = fopen(txtFileName.c_str(), "r");

	if (fpRect == NULL)
	{
		return false;
	}

	double left = 0;
	double top = 0;
	double right = 0;
	double bottom = 0;
	double xmRes = 0;
	double ymRes = 0;
	fscanf(fpRect, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &left, &top, &xmRes, &ymRes, &right, &bottom);
	rect = Vec4d(left, right, bottom, top);
	fclose(fpRect);

	return true;
}

void CSceneDomGenerator::combineBigRepairImage(GDALDataset* poDataset, GDALDataset* poBigRepairImageDataset, Vec4d rect, Vec4d bigRepairImageMaxRect,
	Vec4d maxRect)
{
	int rowFinalDiff = (maxRect.w() - rect.w()) / mRes;																			//该范围在最终影像的位置
	int colFinalDiff = (rect.x() - maxRect.x()) / mRes;

	int RowBigRepairImageDiff = (bigRepairImageMaxRect.w() - rect.w()) / mRes;													//该范围在大影像的位置
	int ColBigRepairImageDiff = (rect.x() - bigRepairImageMaxRect.x()) / mRes;

	int totalCol = poDataset->GetRasterXSize();																					//总的大小
	int totalRow = poDataset->GetRasterYSize();
	int totalBandCount = poDataset->GetRasterCount();

	int bigImageCol = poBigRepairImageDataset->GetRasterXSize();																//大影影像的大小
	int bigImageRow = poBigRepairImageDataset->GetRasterYSize();

	int blockImageCol = (rect.y() - rect.x()) / mRes;
	int blockImageRow = (rect.w() - rect.z()) / mRes;

	if (bigImageCol <= 0 || bigImageRow <= 0)
	{
		return;
	}

	if (blockImageCol <= 0 || blockImageRow <= 0)
	{
		return;
	}

	int bandCount = poBigRepairImageDataset->GetRasterCount();

	//计算该块在最后影像的大小
	int newFinalBlockCol = 0;
	int newFinalBlockRow = 0;

	if (rowFinalDiff + blockImageRow > totalRow)
	{
		newFinalBlockRow = totalRow - rowFinalDiff;
	}
	else
	{
		newFinalBlockRow = blockImageRow;
	}

	if (colFinalDiff + blockImageCol > totalCol)
	{
		newFinalBlockCol = totalCol - colFinalDiff;
	}
	else
	{
		newFinalBlockCol = blockImageCol;
	}

	//计算该块在大影像的大小
	int newBigRepairImageBlockCol = 0;
	int newBigRepairImageBlockRow = 0;

	if (RowBigRepairImageDiff + blockImageRow > bigImageRow)
	{
		newBigRepairImageBlockRow = bigImageRow - RowBigRepairImageDiff;
	}
	else
	{
		newBigRepairImageBlockRow = blockImageRow;
	}

	if (ColBigRepairImageDiff + blockImageCol > bigImageCol)
	{
		newBigRepairImageBlockCol = bigImageCol - ColBigRepairImageDiff;
	}
	else
	{
		newBigRepairImageBlockCol = blockImageCol;
	}

	if (newFinalBlockCol <= 0 || newFinalBlockRow <= 0)
	{
		return;
	}

	if (newBigRepairImageBlockCol <= 0 || newBigRepairImageBlockRow <= 0)
	{
		return;
	}

	unsigned char* pBigRepairImageData = new unsigned char[newBigRepairImageBlockCol * newBigRepairImageBlockRow * bandCount];
	poBigRepairImageDataset->RasterIO(GF_Read, ColBigRepairImageDiff, RowBigRepairImageDiff, newBigRepairImageBlockCol, newBigRepairImageBlockRow, pBigRepairImageData, newBigRepairImageBlockCol, newBigRepairImageBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	unsigned char* pTotalBlockData = new unsigned char[newFinalBlockRow * newFinalBlockCol * bandCount];
	poDataset->RasterIO(GF_Read, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	for (int i = 0; i < newFinalBlockRow; i++)
	{
		for (int j = 0; j < newFinalBlockCol; j++)
		{
			int originR = pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originG = pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originB = pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = pBigRepairImageData[0 * newBigRepairImageBlockCol * newBigRepairImageBlockRow + i * newBigRepairImageBlockCol + j];
			int G = pBigRepairImageData[1 * newBigRepairImageBlockCol * newBigRepairImageBlockRow + i * newBigRepairImageBlockCol + j];
			int B = pBigRepairImageData[2 * newBigRepairImageBlockCol * newBigRepairImageBlockRow + i * newBigRepairImageBlockCol + j];

			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB <= 0 && aveRGB > 10)												//用周边的高亮像素覆盖
			{
				pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = R;
				pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = G;
				pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = B;
			}
			else if (aveOriginRGB > 0 && aveOriginRGB <= 30 && aveRGB > 30)
			{
				if (i - 1 < 0 || j - 1 < 0 || i + 1 > newFinalBlockRow - 1 || j + 1 > newFinalBlockCol - 1)
				{
					continue;
				}

				Vec3d SumPixel = Vec3d(0, 0, 0);
				int countPixel = 0;

				for (int k = 0; k < 3; k++)
				{
					for (int l = 0; l < 3; l++)
					{
						int pixelR = pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + (i + k - 1) * newFinalBlockCol + j + l - 1];
						int pixelG = pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + (i + k - 1) * newFinalBlockCol + j + l - 1];
						int pixelB = pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + (i + k - 1) * newFinalBlockCol + j + l - 1];
						double avePixelRGB = (pixelR + pixelG + pixelB) / 3;

						if (avePixelRGB > 30)
						{
							SumPixel.x() += pixelR;
							SumPixel.y() += pixelG;
							SumPixel.z() += pixelB;
							countPixel++;
						}
					}
				}

				if (countPixel > 0)
				{
					double AvePixelR = SumPixel.x() / countPixel;
					double AvePixelG = SumPixel.y() / countPixel;
					double AvePixelB = SumPixel.z() / countPixel;

					if ((AvePixelB + AvePixelG + AvePixelR) > 90)
					{
						pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = AvePixelR;
						pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = AvePixelG;
						pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = AvePixelB;
					}
				}
			}

		}
	}

	poDataset->RasterIO(GF_Write, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	delete[] pBigRepairImageData;
	delete[] pTotalBlockData;
}


bool CSceneDomGenerator::produceRectDom(ref_ptr<MatrixTransform> mTrans, vector<Vec3d> vecCoord, string outRectDomFileName)
{
	mTrans->getOrCreateStateSet()->setMode(GL_LIGHTING, 0x2);		//设置全光源
	mTrans->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, StateAttribute::ON);

	vector<Vec3d> rectNode;
	bool isSuccess = findMinMax(vecCoord, rectNode);

	if (!isSuccess)
	{
		return false;
	}

	Matrix matrix = mTrans->getMatrix();
	Vec3d xyzMove = matrix.getTrans();
	double rectLeft = rectNode[0].x();
	double rectRight = rectNode[1].x();
	double rectBottom = rectNode[0].y();
	double rectTop = rectNode[1].y();
	double znear = rectNode[0].z();
	double zfar = rectNode[1].z();

	int width = (rectRight - rectLeft) / mRes + 1;								//加1 保证边缘没问题				
	int height = (rectTop - rectBottom) / mRes + 1;

	if (width <= 0 || height <= 0)
	{
		return false;
	}

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (width > ScreenWidth || height > ScreenHeight)				//超过了屏幕的分辨率了，需要分块进行截屏
	{
		int xNum = width / ScreenWidth;
		int yNum = height / ScreenHeight;

		//拍照
		for (int i = 0; i <= yNum; i++)
		{
			for (int j = 0; j <= xNum; j++)
			{
				//计算细分块的范围
				int newWidth, newHeight;
				double newRectLeft, newRectRight, newRectBottom, newRectTop;
				double newRectZnear = znear;
				double newRectZfar = zfar;

				if (j == xNum)
				{
					newRectLeft = rectLeft + xNum * ScreenWidth * mRes;
					newRectRight = rectRight;
					newWidth = (newRectRight - newRectLeft) / mRes;
				}
				else
				{
					newRectLeft = rectLeft + j * ScreenWidth * mRes;
					newRectRight = rectLeft + (j + 1) * ScreenWidth * mRes;
					newWidth = ScreenWidth;
				}

				if (i == yNum)
				{
					newRectBottom = rectBottom + yNum * ScreenHeight * mRes;
					newRectTop = rectTop;
					newHeight = (newRectTop - newRectBottom) / mRes;
				}
				else
				{
					newRectBottom = rectBottom + i * ScreenHeight * mRes;
					newRectTop = rectBottom + (i + 1) * ScreenHeight * mRes;
					newHeight = ScreenHeight;
				}

				bool isSuccess = produceSplitedDOM(i, j, mTrans, newWidth, newHeight, newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar, outRectDomFileName);

				if (isSuccess == false)
				{
					return false;
				}
			}
		}

		CvMat* imgMat = cvCreateMat(height, width, CV_8UC3);
		//合并影像
		vector<Vec4d> vecCoordRect = findImageRect(vecSplitCoordFileName);

		for (int i = 0; i < vecSplitImgFileName.size(); i++)
		{
			combineImage(imgMat, vecSplitImgFileName[i], vecCoordRect[i], Vec4d(rectLeft, rectRight, rectBottom, rectTop));

			CMFCExcute excutor;
			excutor.RemoveFile(vecSplitImgFileName[i]);
			excutor.RemoveFile(vecSplitCoordFileName[i]);
		}

		cvSaveImage(outRectDomFileName.c_str(), imgMat);
		cvReleaseMat(&imgMat);
		imgMat = NULL;
	}
	else
	{
		bool isSuccess = produceScreenDOM(mTrans, width, height, rectLeft, rectRight, rectBottom, rectTop, znear,
			zfar, outRectDomFileName);

		if (!isSuccess)
		{
			return false;
		}
	}

	int posEnd = outRectDomFileName.rfind('.');
	string outFileTxt = outRectDomFileName.substr(0, posEnd) + ".txt";

	//输出坐标
	FILE* fpOut = fopen(outFileTxt.c_str(), "w");
	fprintf(fpOut, "%lf\n%lf\n%lf\n%lf%\n%lf\n%lf\n", rectLeft, rectTop, mRes, mRes, rectRight, rectBottom);
	fclose(fpOut);

	return true;
};


bool CSceneDomGenerator::findMinMaxNode(ref_ptr<Node> node, Matrix matrix, vector<Vec3d> &minMaxCoord)
{
	COsgVertextExtract ive;
	node->accept(ive);
	ref_ptr<Vec3Array> extracted_verts = ive.extracted_verts;

	vector <Vec3>::iterator iter = extracted_verts.get()->begin();
	vector<Vec3d> coord;

	for (int i = 0; i < extracted_verts->size(); i++)
	{
		double x = iter->x();
		double y = iter->y();
		double z = iter->z();
		Vec3d pt(x, y, z);
		pt = pt * matrix;
		coord.push_back(pt);
		iter++;
	}

	bool isSuccess = findMinMax(coord, minMaxCoord);

	return isSuccess;
}


bool CSceneDomGenerator::findMinMax(vector<Vec3d> vecMinMax, vector<Vec3d> &coord)
{
	int size = vecMinMax.size();

	if (size <= 0)
	{
		return false;
	}

	double xMin = vecMinMax[0].x();
	double yMin = vecMinMax[0].y();
	double zMin = vecMinMax[0].z();
	double xMax = vecMinMax[0].x();
	double yMax = vecMinMax[0].y();
	double zMax = vecMinMax[0].z();

	for (int i = 0; i < vecMinMax.size(); i++)
	{
		if (xMin > vecMinMax[i].x())
		{
			xMin = vecMinMax[i].x();
		}

		if (yMin > vecMinMax[i].y())
		{
			yMin = vecMinMax[i].y();
		}

		if (zMin > vecMinMax[i].z())
		{
			zMin = vecMinMax[i].z();
		}

		if (xMax < vecMinMax[i].x())
		{
			xMax = vecMinMax[i].x();
		}

		if (yMax < vecMinMax[i].y())
		{
			yMax = vecMinMax[i].y();
		}

		if (zMax < vecMinMax[i].z())
		{
			zMax = vecMinMax[i].z();
		}
	}

	coord.push_back(Vec3d(xMin, yMin, zMin));
	coord.push_back(Vec3d(xMax, yMax, zMax));

	return TRUE;
}


bool CSceneDomGenerator::produceScreenDOM(ref_ptr<MatrixTransform> node, int newWidth, int newHeight, double newRectLeft, double newRectRight,
	double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, string newOutFileName)
{
	osgOffScreenViewer viewer;
	ref_ptr<Camera> camera = new Camera;
	viewer.setCamera(camera.get());
	// Set the final SceneData to show
	viewer.setSceneData(node.get());
	// Realize GUI
	viewer.realize();

	// Build matrix for computing target vector
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setViewport(0, 0, newWidth, newHeight);

	Vec3d eye(0, 0, newRectZfar + 1);
	Vec3d up(0, 1, 0);
	Vec3d center(0, 0, newRectZnear);
	camera->setViewMatrixAsLookAt(eye, center, up);

	camera->setProjectionMatrixAsOrtho(newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar);			//计算正射投影范围

	camera->setFinalDrawCallback(new WindowCaptureCallback(GL_BACK, newOutFileName, 1, newWidth, newHeight));						//获取分块影像	
	viewer.renderingTraversals();
	return true;
}


bool CSceneDomGenerator::produceSplitedDOM(int i, int j, ref_ptr<MatrixTransform> mTrans, int newWidth, int newHeight, double newRectLeft,
	double newRectRight, double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, string outRectDomFileName)
{
	stringstream ss;
	string rowStr;
	ss << i;
	ss >> rowStr;

	stringstream ss1;
	string colStr;
	ss1 << j;
	ss1 >> colStr;
	 
	int pos = outRectDomFileName.rfind('\\');
	string newOutFileName = outRectDomFileName.substr(0, pos) + "\\" + "_" + rowStr + "_" + colStr + ".jpg";

	//截图
	ref_ptr<MatrixTransform> node = dynamic_cast<MatrixTransform*>(mTrans->clone(CopyOp::DEEP_COPY_ALL));

	if (produceScreenDOM(node, newWidth, newHeight, newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar, newOutFileName))
	{
		//输出坐标
		string outFileTxt = outRectDomFileName.substr(0, pos) + "\\" + "_" + rowStr + "_" + colStr + ".txt";
		FILE* fpOut = fopen(outFileTxt.c_str(), "w");
		fprintf(fpOut, "%lf\n%lf\n%lf\n%lf%\n%lf\n%lf\n", newRectLeft, newRectTop, mRes, mRes, newRectRight, newRectBottom);
		fclose(fpOut);

		vecSplitImgFileName.push_back(newOutFileName);
		vecSplitCoordFileName.push_back(outFileTxt);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



void CSceneDomGenerator::combineImage(CvMat* imgMat, string imgFileName, Vec4d rect, Vec4d maxRect)
{
	IplImage* img = cvLoadImage(imgFileName.c_str());

	if (img == NULL)
	{
		return;
	}

	uchar* data = (uchar*)img->imageData;
	uchar* combineData = imgMat->data.ptr;
	int widthStep = img->widthStep;
	int rowDiff = (maxRect.w() - rect.w()) / mRes;
	int colDiff = (rect.x() - maxRect.x()) / mRes;

	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			int rowImg = i + rowDiff;
			int colImg = j + colDiff;

			if (imgMat->width - 1 < colImg)
			{
				continue;
			}

			if (imgMat->height - 1 < rowImg)
			{
				continue;
			}

			int originR = combineData[rowImg * imgMat->width * 3 + colImg * 3 + 0];
			int originG = combineData[rowImg * imgMat->width * 3 + colImg * 3 + 1];
			int originB = combineData[rowImg * imgMat->width * 3 + colImg * 3 + 2];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = data[i * widthStep + j * 3 + 0];
			int G = data[i * widthStep + j * 3 + 1];
			int B = data[i * widthStep + j * 3 + 2];
			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB < aveRGB)													//用周边的高亮像素覆盖
			{
				combineData[rowImg * imgMat->width * 3 + colImg * 3 + 0] = data[i * widthStep + j * 3 + 0];
				combineData[rowImg * imgMat->width * 3 + colImg * 3 + 1] = data[i * widthStep + j * 3 + 1];
				combineData[rowImg * imgMat->width * 3 + colImg * 3 + 2] = data[i * widthStep + j * 3 + 2];
			}
		}
	}

	cvReleaseImage(&img);
}


bool CSceneDomGenerator::outputImageAndCoords(vector<MatrixTransform*> vecTransNode, vector<string> vecSceneFilePath,
	vector<string> vecImageFileName, vector<string> vecCoordFileName)
{
	if (vecImageFileName.size() == 0 || vecCoordFileName.size() == 0)
	{
		return false;
	}

	vector<Vec4d> vecRect = findDOMRect(vecCoordFileName, vecImageFileName);	//获取image范围

	if (vecRect.size() == 0)
	{
		return false;
	}

	Vec4d maxRect = findMaxRect(vecRect);										    //找到最大范围

	//计算范围创建影像
	int row = (maxRect.w() - maxRect.z()) / mRes;
	int col = (maxRect.y() - maxRect.x()) / mRes;

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	char** ppszOptions = CSLSetNameValue(NULL, "BIGTIFF", "IF_NEEDED"); //配置图像信息	
	int nBandCount = 3;
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF"); //图像驱动
	GDALDataset* poDataset = pDriver->Create(mOutputFileName.c_str(), col, row, nBandCount, GDT_Byte, ppszOptions);

	double blockXSize = 10000 * mRes;
	double blockYSize = 10000 * mRes;

	for (int i = 0; i < vecRect.size(); i++)
	{
		GDALDataset* poBlockDataset = (GDALDataset*)GDALOpen(vecImageFileName[i].c_str(), GA_ReadOnly);

		if (poBlockDataset == NULL)
		{
			GDALClose(poBlockDataset);
			poBlockDataset = NULL;
			continue;
		}

		double left = vecRect[i].x(); double right = vecRect[i].y(); double bottom = vecRect[i].z(); double top = vecRect[i].w();
		int blockXNum = (right - left) / blockXSize + 1;
		int blockYNum = (top - bottom) / blockYSize + 1;

		for (int j = 0; j < blockXNum; j++)
		{
			for (int k = 0; k < blockYNum; k++)
			{
				double blockLeft = j * blockXSize + left;
				double blockRight = 0;
				double blockBottom = k * blockYSize + bottom;
				double blockTop = 0;

				if (j != blockXNum - 1)
				{
					blockRight = blockLeft + blockXSize;
				}
				else
				{
					blockRight = right;
				}

				if (k != blockYNum - 1)
				{
					blockTop = blockBottom + blockYSize;
				}
				else
				{
					blockTop = top;
				}

				Vec4d blockRect(blockLeft, blockRight, blockBottom, blockTop);
				combineBigImage(poDataset, poBlockDataset, blockRect, vecRect[i], maxRect);
			}
		}

		GDALClose(poBlockDataset);
		poBlockDataset = NULL;
	}

	//修补空洞

	for (int i = 0; i < vecTransNode.size(); i++)
	{
		COsgVertextExtract ive;
		vecTransNode[i]->accept(ive);
		ref_ptr<Vec3Array> extracted_verts = ive.extracted_verts;

		if (extracted_verts->size() == 0)
		{
			continue;
		}

		startRepairDom(vecTransNode[i], poDataset, vecSceneFilePath[i], maxRect);
	}

	double  adfGeoTransform[6] = { 0 };
	adfGeoTransform[0] = maxRect.x();
	adfGeoTransform[3] = maxRect.w();
	adfGeoTransform[1] = mRes;
	adfGeoTransform[5] = -mRes;

	poDataset->SetGeoTransform(adfGeoTransform);
	GDALClose(poDataset);
	poDataset = NULL;

	int pos = mOutputFileName.rfind('.');
	string coordFileName = mOutputFileName.substr(0, pos) + ".tfw";
	double xRot = 0;
	double yRot = 0;
	FILE* fpCoord = fopen(coordFileName.c_str(), "w");
	fprintf(fpCoord, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", mRes, xRot, yRot, -mRes, maxRect.x(), maxRect.w());
	fclose(fpCoord);

	return true;
}


bool CSceneDomGenerator::startRepairDom(ref_ptr<MatrixTransform> mTrans, GDALDataset* poDataset, string sceneFilepath, Vec4d maxRect)
{
	vector<string> vecRepairImgFile;
	vector<string> vecRepairCoordTxtFileName;
	//修补空洞DOM生成
	COsgRepairTransNodeFinder ive;
	mTrans->accept(ive);
	vector<MatrixTransform*> mRepairMTrans = ive.vecMatTrans;

	for (int i = 0; i < mRepairMTrans.size(); i++)
	{
		ref_ptr<Node> repairNode = mRepairMTrans[i]->getChild(0);
		Matrix worldMatrix;
		MatrixList matrixList = repairNode->getWorldMatrices();

		if (matrixList.size() != 0)
		{
			Matrix newMatrix;

			for (int i = 0; i < matrixList.size(); i++)
			{
				newMatrix.postMult(matrixList[i]);
			}

			worldMatrix = newMatrix;
		}

		char strMsg[99];
		sprintf_s(strMsg, "repair%d.jpg", i);
		string repairTitle = strMsg;
		string repairDomFileName = sceneFilepath + repairTitle;
		int pos = repairDomFileName.rfind('.') + 1;

		ref_ptr<Node> node = mRepairMTrans[i]->getChild(0);

		if (produceNodeDOM(node, repairDomFileName, worldMatrix))
		{
			vecRepairImgFile.push_back(repairDomFileName);
			vecRepairCoordTxtFileName.push_back(repairDomFileName.substr(0, pos) + "txt");
		}
	}

	//进行修补图像的填补
	vector<Vec4d> vecRepairRect = findImageRect(vecRepairCoordTxtFileName);			   //获取image范围

	for (int i = 0; i < vecRepairRect.size(); i++)
	{
		Vec4d rect;

		if (findTileImageRect(vecRepairCoordTxtFileName[i], rect))
		{
			GDALDataset* poRepairBlockDataset = (GDALDataset*)GDALOpen(vecRepairImgFile[i].c_str(), GA_ReadOnly);

			if (poRepairBlockDataset == NULL)
			{
				GDALClose(poRepairBlockDataset);
				poRepairBlockDataset = NULL;
				return FALSE;
			}

			double blockXSize = 10000 * mRes;
			double blockYSize = 10000 * mRes;

			double left = rect.x(); double right = rect.y(); double bottom = rect.z(); double top = rect.w();
			int blockXNum = (right - left) / blockXSize + 1;
			int blockYNum = (top - bottom) / blockYSize + 1;

			for (int j = 0; j < blockXNum; j++)
			{
				for (int k = 0; k < blockYNum; k++)
				{
					double blockLeft = j * blockXSize + left;
					double blockRight = 0;
					double blockBottom = k * blockYSize + bottom;
					double blockTop = 0;

					if (j != blockXNum - 1)
					{
						blockRight = blockLeft + blockXSize;
					}
					else
					{
						blockRight = right;
					}

					if (k != blockYNum - 1)
					{
						blockTop = blockBottom + blockYSize;
					}
					else
					{
						blockTop = top;
					}

					Vec4d blockRect(blockLeft, blockRight, blockBottom, blockTop);
					combineBigRepairImage(poDataset, poRepairBlockDataset, blockRect, rect, maxRect);
				}
			}

			GDALClose(poRepairBlockDataset);
		}

		CMFCExcute excutor;
		excutor.RemoveFile(vecRepairImgFile[i]);
		excutor.RemoveFile(vecRepairCoordTxtFileName[i]);

	}
}


void CSceneDomGenerator::combineBigImage(GDALDataset* poDataset, GDALDataset* poBigImageDataset, Vec4d rect, Vec4d bigImageMaxRect, Vec4d maxRect)
{
	int rowFinalDiff = (maxRect.w() - rect.w()) / mRes;																			//该范围在最终影像的位置
	int colFinalDiff = (rect.x() - maxRect.x()) / mRes;

	int RowBigImageDiff = (bigImageMaxRect.w() - rect.w()) / mRes;																//该范围在大影像的位置
	int ColBigImageDiff = (rect.x() - bigImageMaxRect.x()) / mRes;

	int totalCol = poDataset->GetRasterXSize();																					//总的大小
	int totalRow = poDataset->GetRasterYSize();
	int totalBandCount = poDataset->GetRasterCount();

	int bigImageCol = poBigImageDataset->GetRasterXSize();																		//大影影像的大小
	int bigImageRow = poBigImageDataset->GetRasterYSize();

	int blockImageCol = (rect.y() - rect.x()) / mRes;
	int blockImageRow = (rect.w() - rect.z()) / mRes;

	if (bigImageCol <= 0 || bigImageRow <= 0)
	{
		return;
	}

	if (blockImageCol <= 0 || blockImageRow <= 0)
	{
		return;
	}

	int bandCount = poBigImageDataset->GetRasterCount();

	//计算该块在最后影像的大小
	int newFinalBlockCol = 0;
	int newFinalBlockRow = 0;

	if (rowFinalDiff + blockImageRow > totalRow)
	{
		newFinalBlockRow = totalRow - rowFinalDiff;
	}
	else
	{
		newFinalBlockRow = blockImageRow;
	}

	if (colFinalDiff + blockImageCol > totalCol)
	{
		newFinalBlockCol = totalCol - colFinalDiff;
	}
	else
	{
		newFinalBlockCol = blockImageCol;
	}

	//计算该块在大影像的大小
	int newBigImageBlockCol = 0;
	int newBigImageBlockRow = 0;

	if (RowBigImageDiff + blockImageRow > bigImageRow)
	{
		newBigImageBlockRow = bigImageRow - RowBigImageDiff;
	}
	else
	{
		newBigImageBlockRow = blockImageRow;
	}

	if (ColBigImageDiff + blockImageCol > bigImageCol)
	{
		newBigImageBlockCol = bigImageCol - ColBigImageDiff;
	}
	else
	{
		newBigImageBlockCol = blockImageCol;
	}

	if (newFinalBlockCol <= 0 || newFinalBlockRow <= 0)
	{
		return;
	}

	if (newBigImageBlockCol <= 0 || newBigImageBlockRow <= 0)
	{
		return;
	}

	uchar* pBigImageData = new uchar[newBigImageBlockCol * newBigImageBlockRow * bandCount];
	poBigImageDataset->RasterIO(GF_Read, ColBigImageDiff, RowBigImageDiff, newBigImageBlockCol, newBigImageBlockRow, pBigImageData, newBigImageBlockCol, newBigImageBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	uchar* pTotalBlockData = new uchar[newFinalBlockRow * newFinalBlockCol * bandCount];
	poDataset->RasterIO(GF_Read, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	for (int i = 0; i < newFinalBlockRow; i++)
	{
		for (int j = 0; j < newFinalBlockCol; j++)
		{
			int originR = pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originG = pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originB = pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = pBigImageData[0 * newBigImageBlockCol * newBigImageBlockRow + i * newBigImageBlockCol + j];
			int G = pBigImageData[1 * newBigImageBlockCol * newBigImageBlockRow + i * newBigImageBlockCol + j];
			int B = pBigImageData[2 * newBigImageBlockCol * newBigImageBlockRow + i * newBigImageBlockCol + j];

			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB < aveRGB)													//用周边的高亮像素覆盖
			{
				pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = R;
				pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = G;
				pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = B;
			}
		}
	}

	poDataset->RasterIO(GF_Write, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	delete[] pBigImageData;
	delete[] pTotalBlockData;
}


vector<Vec4d> CSceneDomGenerator::findDOMRect(vector<string> vecCoordFileName, vector<string> vecImageFileName)
{
	//寻找所有影像范围
	GDALAllRegister();
	vector<Vec4d> vecRect;

	for (int i = 0; i < vecCoordFileName.size(); i++)
	{
		GDALDataset* poDomDataset = (GDALDataset*)GDALOpen(vecImageFileName[i].c_str(), GA_ReadOnly);

		if (poDomDataset == NULL)
		{
			GDALClose(poDomDataset);
			poDomDataset = NULL;
			vector<Vec4d> vecRec;
			return vecRec;
		}

		int imgHeight = poDomDataset->GetRasterYSize();
		int imgWidth = poDomDataset->GetRasterXSize();

		GDALClose(poDomDataset);
		poDomDataset = NULL;

		FILE* fpRect = fopen(vecCoordFileName[i].c_str(), "r");

		if (fpRect == NULL)
		{
			continue;
		}

		double left = 0;
		double top = 0;
		double xRot = 0;
		double yRot = 0;
		double xRes = 0;
		double yRes = 0;
		fscanf(fpRect, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &xRes, &xRot, &yRot, &yRes, &left, &top);
		double bottom = top + imgHeight * yRes;
		double right = left + imgWidth * xRes;
		vecRect.push_back(Vec4d(left, right, bottom, top));
		fclose(fpRect);
	}

	return vecRect;
}

