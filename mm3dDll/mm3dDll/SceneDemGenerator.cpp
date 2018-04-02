#include "StdAfx.h"
#include "SceneDemGenerator.h"
#include <osg/Geometry>
#include "gdal.h"
#include "gdal_priv.h"
#include <osgDB/ReadFile>
#include <stdlib.h>
#include "OsgVertextExtract.h"
#include "OsgTransFolderFileNameFinder.h"
#include "OsgPagedLodFinder.h"
#include "OsgPagedLodBottomLevelFinder.h"
#include <osg/MatrixTransform>
#include "OsgRepairTransNodeFinder.h"
#include "Geometry.h"
#include "ScriptExecutor.h"

using namespace std;
using namespace osg;
using namespace osgDB;

void CNodeDemCreator::InterpolateDem(Geode& geode, Matrix matrix, CvMat* demMat, GridDem gDem, bool isRepair)
{
	int childGeomNum = geode.getNumDrawables();

	for (int i = 0; i < childGeomNum; i++)
	{	
		ref_ptr<Geometry> geom = dynamic_cast<Geometry*> (geode.getDrawable(i));
		int iAverageSize = 0;

		if (geom)
		{
			ref_ptr<Vec3Array> verts = dynamic_cast<Vec3Array*>(geom->getVertexArray()); 
			//获取三角形序号
			PrimitiveSet* priSet = geom->getPrimitiveSet(0);
			ref_ptr<DrawElements> drawElementTriangle = priSet->getDrawElements();
			int cnt = drawElementTriangle->getNumIndices();
			int triCnt = cnt / 3;

			for (int w = 0; w < triCnt; w++)
			{
				int index1 = drawElementTriangle->index(w * 3);
				int index2 = drawElementTriangle->index(w * 3 + 1);
				int index3 = drawElementTriangle->index(w * 3 + 2);

				//顶点坐标对应新影像坐标
				vector<Vec3d> pt;
				double vertX1 = verts->at(index1).x();
				double vertY1 = verts->at(index1).y();
				double vertZ1 = verts->at(index1).z();
				Vec3d tempPt1(vertX1, vertY1, vertZ1);
				tempPt1 = tempPt1 * matrix;
				pt.push_back(Vec3d(tempPt1.x(), tempPt1.y(), tempPt1.z()));
				vertX1 = tempPt1.x();
				vertY1 = tempPt1.y();
				vertZ1 = tempPt1.z();

				double vertX2 = verts->at(index2).x();
				double vertY2 = verts->at(index2).y();
				double vertZ2 = verts->at(index2).z();
				Vec3d tempPt2(vertX2, vertY2, vertZ2);
				tempPt2 = tempPt2 * matrix;
				pt.push_back(Vec3d(tempPt2.x(), tempPt2.y(), tempPt2.z()));
				vertX2 = tempPt2.x();
				vertY2 = tempPt2.y();
				vertZ2 = tempPt2.z();

				double vertX3 = verts->at(index3).x();
				double vertY3 = verts->at(index3).y();
				double vertZ3 = verts->at(index3).z();
				Vec3d tempPt3(vertX3, vertY3, vertZ3);
				tempPt3 = tempPt3 * matrix;
				pt.push_back(Vec3d(tempPt3.x(), tempPt3.y(), tempPt3.z()));
				vertX3 = tempPt3.x();
				vertY3 = tempPt3.y();
				vertZ3 = tempPt3.z();

				double lfTmp=max(pt[0].x(), max(pt[1].x(), pt[2].x())) - gDem.x0;
				int iCol_max = ceil(lfTmp / gDem.dx);

				lfTmp = max(pt[0].y(), max(pt[1].y(), pt[2].y())) - gDem.y0;
				int iRow_max = ceil(lfTmp / gDem.dy);

				lfTmp = min(pt[0].x(), min(pt[1].x(), pt[2].x())) - gDem.x0;
				int iCol_min = floor(lfTmp / gDem.dx);

				lfTmp = min(pt[0].y(), min(pt[1].y(), pt[2].y())) - gDem.y0;
				int iRow_min = floor(lfTmp / gDem.dy);

				iRow_min--;
				iRow_max++;
				iCol_min--;
				iCol_max++;

				if(w == 0)
				{
					iAverageSize = max((iRow_max - iRow_min), (iCol_max - iCol_min)); 
				}
				else
				{
					if((iRow_max - iRow_min) > iAverageSize * 10||(iCol_max - iCol_min) > iAverageSize * 10)
					{
						continue; //非正常三角形。。。不处理
					}
				}

				vector<CL::Vec2d> pol;
				pol.push_back(CL::Vec2d(vertX1, vertY1));
				pol.push_back(CL::Vec2d(vertX2, vertY2));
				pol.push_back(CL::Vec2d(vertX3, vertY3));
				pol.push_back(CL::Vec2d(vertX1, vertY1));

				for(int k = iRow_min; k <= iRow_max; k++)
				{
					for(int s = iCol_min; s <= iCol_max; s++)
					{
						if(k < 0)
							k = 0;
						if(s < 0)
							s = 0;
						if(k > (gDem.rows - 1))
							continue;
						if(s > (gDem.cols - 1))
							continue;

						double x = gDem.x0 + gDem.dx * s;
						double y = gDem.y0 + gDem.dy * k;
						//利用反距离权重进行插值
						double d1 = sqrt((vertX1 - x) * (vertX1 - x) + (vertY1 - y) * (vertY1 - y));
						double d2 = sqrt((vertX2 - x) * (vertX2 - x) + (vertY2 - y) * (vertY2 - y));
						double d3 = sqrt((vertX3 - x) * (vertX3 - x) + (vertY3 - y) * (vertY3 - y));
						double z = (1 / d1) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ1 + (1 / d2) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ2 + (1 / d3) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ3;

						if (Point_In_Polygon_2D(x, y, pol))
						{
							double oriZ = CV_MAT_ELEM(*demMat, double, k, s);

							if (oriZ == 0)
							{
								CV_MAT_ELEM(*demMat, double, k, s) = z;
							}
							else if (!isRepair)
							{
								if (oriZ < z)
								{
									CV_MAT_ELEM(*demMat, double, k, s) = z;
								}
							}
						}

					}
				}
			}
		}
	}
}

CvMat* CSceneDemGenerator::GenerateDem(string rootModelPath, ref_ptr<MatrixTransform> transNode, double Xinterval, double Yinterval)	//生成DEM
{
	//查找pagedlod并赋值
	COsgPagedLodFinder ive;
	transNode->accept(ive);
	vector<PagedLOD*> vecPagedLod = ive.vecPagedLod;

	if (vecPagedLod.size() == 0)
	{
		return NULL;
	}

	vector<string> vecGroupHighLevelChildName;

	for (int i = 0; i < vecPagedLod.size(); i++)
	{
		vector<string> vecTileHighLevelChildName;
		COsgPagedLodBottomLevelFinder ive(&vecTileHighLevelChildName);
		vecPagedLod[i]->accept(ive);

		if (vecTileHighLevelChildName.size() > 0)
		{
			vecGroupHighLevelChildName.insert(vecGroupHighLevelChildName.end(), vecTileHighLevelChildName.begin(), vecTileHighLevelChildName.end());

			int pos = (int)((double)(i + 1) / (double)vecPagedLod.size() * 150);

			string script = "setProgressVal(" + to_string(pos) + ")";
			CScriptExecutor::getInstance()->excuteScript(script);
		}
	}

	vector<Vec3d> minMaxNode;

	Matrix matrix = transNode->getMatrix();

	for (int i = 0; i < vecGroupHighLevelChildName.size(); i++)
	{
		if (i == 0 || minMaxNode.size() == 0)
		{
			ref_ptr<Node> node = readNodeFile(vecGroupHighLevelChildName[i]);

			if (!node)
			{
				continue;
			}

			ObtainGeoRect(node, matrix, minMaxNode);
		}
		else
		{
			ref_ptr<Node> node = readNodeFile(vecGroupHighLevelChildName[i]);

			if (!node)
			{
				continue;
			}

			vector<Vec3d> minMaxNode1;
			bool isSuccess = ObtainGeoRect(node, matrix, minMaxNode1);

			if (isSuccess)
			{
				minMaxNode = findMinMaxNode(minMaxNode, minMaxNode1);
			}
		}

		int pos = (int)((double)(i + 1) / (double)vecGroupHighLevelChildName.size() * 300) + 150;
		string script = "setProgressVal(" + to_string(pos) + ")";
		CScriptExecutor::getInstance()->excuteScript(script);
	}

	gDem.dx = Xinterval;
	gDem.dy = Yinterval;
	gDem.x0 = minMaxNode[0].x();
	gDem.y0 = minMaxNode[0].y();
	gDem.x1 = minMaxNode[1].x();
	gDem.y1 = minMaxNode[1].y();
	gDem.rows = (gDem.y1 - gDem.y0) / gDem.dy;
	gDem.cols = (gDem.x1 - gDem.x0) / gDem.dx;

	CvMat* demMat = cvCreateMat(gDem.rows, gDem.cols, CV_64FC1);
	cvSetZero(demMat);


	for (int i = 0; i < vecGroupHighLevelChildName.size(); i++)
	{
		string fileName = vecGroupHighLevelChildName[i];
		ref_ptr<Node> node = readNodeFile(fileName);

		if (!node)
		{
			continue;
		}

		CNodeDemCreator ive(matrix, demMat, gDem, false);
		node->accept(ive);
	}

	//修补空洞dem生成
	vector<MatrixTransform*> mRepairMTrans;
	COsgRepairTransNodeFinder iveRe;
	iveRe.FindRepairTransNode(transNode, mRepairMTrans);

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

		CNodeDemCreator ive(worldMatrix, demMat, gDem, true);
		repairNode->accept(ive);
	}

	return demMat;
}

bool CSceneDemGenerator::ObtainGeoRect(ref_ptr<Node> node, Matrix matrix, vector<Vec3d> &minMaxGeode)    //获取osgb节点
{
	COsgVertextExtract ive;
	node->accept(ive);
	ref_ptr<Vec3Array> extracted_verts = ive.extracted_verts;
	int size_t = extracted_verts.get()->size() ; 

	if (size_t == 0)
	{
		return false;
	}

	vector <Vec3>::iterator iter = extracted_verts.get()->begin();
	double x = iter->x();
	double y = iter->y();
	double z = iter->z();
	Vec3 pt(x, y, z);
	pt = pt * matrix;
	double xMin = pt.x();
	double yMin = pt.y(); 
	double zMin = pt.z();
	double xMax = pt.x();
	double yMax = pt.y();
	double zMax = pt.z();

	for (int i = 0 ; i < size_t; i++)
	{
		double x = iter->x();
		double y = iter->y();
		double z = iter->z();
		Vec3 pt(x, y, z);
		pt = pt * matrix;

		if (xMin > pt.x())
		{
			xMin = pt.x();
		}

		if (yMin > pt.y())
		{
			yMin = pt.y();
		}

		if (zMin > pt.z())
		{
			zMin = pt.z();
		}

		if (xMax < pt.x())
		{
			xMax = pt.x();
		}

		if (yMax < pt.y())
		{
			yMax = pt.y();
		}

		if (zMax < pt.z())
		{
			zMax = pt.z();
		}

		iter++ ; 
	}

	minMaxGeode.push_back(Vec3d(xMin, yMin, zMin));
	minMaxGeode.push_back(Vec3d(xMax, yMax, zMax));
	return true;
}				

vector<Vec3d> CSceneDemGenerator::findMinMaxNode(vector<Vec3d> minMaxNode, vector<Vec3d> minMaxNode1)  //找到两个范围的合并集合
{
	vector<Vec3d> minMaxNode2;
	double minX = min(minMaxNode[0].x(), minMaxNode1[0].x());
	double minY = min(minMaxNode[0].y(), minMaxNode1[0].y());
	double minZ = min(minMaxNode[0].z(), minMaxNode1[0].z());
	double maxX = max(minMaxNode[1].x(), minMaxNode1[1].x());
	double maxY = max(minMaxNode[1].y(), minMaxNode1[1].y());
	double maxZ = max(minMaxNode[1].z(), minMaxNode1[1].z());
	minMaxNode2.push_back(Vec3d(minX, minY, minZ));
	minMaxNode2.push_back(Vec3d(maxX, maxY, maxZ));
	return minMaxNode2;
}


vector<vector<string>> CSceneDemGenerator::SeparateScenenTile(vector<string> vecChildFilePath, double Xinterval, double Yinterval)
{
	int rowMin = 999999, rowMax = -9999, colMin = 999999, colMax = -99999;

	for (int i = 0; i < vecChildFilePath.size(); i++)
	{
		int rPos = vecChildFilePath[i].rfind('_+');
		int pos = vecChildFilePath[i].find('_+');
		int length = vecChildFilePath[i].length() - 1;
		string rowStr = vecChildFilePath[i].substr(pos + 2, rPos);
		string colStr = vecChildFilePath[i].substr(rPos + 2, length);
		int row = atoi(rowStr.c_str());
		int col = atoi(colStr.c_str());

		if (rowMin > row)
		{
			rowMin = row;
		}

		if (rowMax < row)
		{
			rowMax = row;
		}

		if (colMin > col)
		{
			colMin = col;
		}

		if (colMax < col)
		{
			colMax = col;
		}
	}

	int rowNum = (int) ceil((double)(rowMax - rowMin + 1) / Yinterval); 
	int colNum = (int) ceil((double)(colMax - colMin + 1) / Xinterval);
	int groupNum = rowNum * colNum;

	vector<vector<string>> vecGroupFilePath;

	for (int i = 0; i < groupNum; i++)
	{
		vector<string> groupFilePath;
		vecGroupFilePath.push_back(groupFilePath);
	}

	for (int i = 0; i < vecChildFilePath.size(); i++)
	{
		int rPos = vecChildFilePath[i].rfind('_+');
		int pos = vecChildFilePath[i].find('_+');
		int length = vecChildFilePath[i].length() - 1;
		string rowStr = vecChildFilePath[i].substr(pos + 2, rPos);
		string colStr = vecChildFilePath[i].substr(rPos + 2, length);
		int row = atoi(rowStr.c_str());
		int col = atoi(colStr.c_str());
		int rowIndex = (row - rowMin) / Xinterval;
		int colIndex = (col - colMin) / Yinterval;
		vecGroupFilePath[rowIndex * colNum + colIndex].push_back(vecChildFilePath[i]);
	}

	return vecGroupFilePath;
}

void CSceneDemGenerator::SelectHighLevelLodFileName(vector<vector<string>> vecGroupChildName, vector<vector<string>> &vecGroupHighLevelChildName)
{
	for (int i = 0; i < vecGroupChildName.size(); i++)		
	{
		int maxLevelNum = 0;

		for(int j = 0; j < vecGroupChildName[i].size(); j++)
		{
			int pos = vecGroupChildName[i][j].find('L');
			int rPos = vecGroupChildName[i][j].rfind('_');

			if (pos == -1)
			{
				continue;
			}

			if (rPos == -1)
			{
				continue;
			}

			string levelStr = vecGroupChildName[i][j].substr(pos + 1, rPos - pos - 1);

			int levelNum = atoi(levelStr.c_str());

			if (levelNum > maxLevelNum)
			{
				maxLevelNum = levelNum;
			}
		}

		if (maxLevelNum == 0)
		{
			continue;
		}

		vector<string> vecHighLevelChildName;

		for(int j = 0; j < vecGroupChildName[i].size(); j++)
		{
			int pos = vecGroupChildName[i][j].find('L');
			int rPos = vecGroupChildName[i][j].rfind('_');

			if (pos == -1)
			{
				continue;
			}

			if (rPos == -1)
			{
				continue;
			}

			if (vecGroupChildName[i][j] == "")
			{
				continue;
			}

			string levelStr = vecGroupChildName[i][j].substr(pos + 1, rPos - pos - 1);
			int levelNum = atoi(levelStr.c_str());

			if (levelNum == maxLevelNum)
			{
				vecHighLevelChildName.push_back(vecGroupChildName[i][j]);

			}
		}

		vecGroupHighLevelChildName.push_back(vecHighLevelChildName);
	}
}

void CSceneDemGenerator::ObtainImageGeoRect(string twfFileName, Vec4d &rect, string imageFileName)
{
	//获取范围
	double bottom = 0;
	double left = 0;
	double xRes = 0;
	double yRes = 0;
	double xRotation = 0;
	double yRotation = 0;
	FILE* fp = fopen(twfFileName.c_str(), "r");
	fscanf(fp, "%lf %lf %lf %lf %lf %lf", &xRes, &xRotation, &yRotation, &yRes, &left, &bottom);
	fclose(fp);

	//获取影像
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	GDALDataset* poDataSet = (GDALDataset *)GDALOpen(imageFileName.c_str(), GA_ReadOnly);
	
	int width = poDataSet->GetRasterXSize();
	int height = poDataSet->GetRasterYSize();

	double right = left + width * xRes;
	double top = bottom + height * yRes;

	rect.x() = left;
	rect.y() = right;
	rect.z() = bottom;
	rect.w() = top;
	GDALClose(poDataSet);
}

bool CSceneDemGenerator::GenerateDemForSingleModel(string rootModelPath, ref_ptr<MatrixTransform> transNode, string outDemFileName, double xRes, double yRes)
{
	CvMat* demMat = GenerateDem(rootModelPath, transNode, xRes, yRes);

	if (demMat == NULL)
	{
		return false;
	}

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	int height = demMat->height;
	int width = demMat->width;
	int bandCount = 1;
	double *data = new double[height * width * bandCount];
	memset(data, 0, sizeof(double) * height * width * bandCount);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			data[(height - 1 - i) * width + j] = CV_MAT_ELEM(*demMat, double, i, j);
		}
	}

	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff"); 
	GDALDataset* poDataset = poDriver->Create(outDemFileName.c_str(), width, height, bandCount, GDT_Float64, NULL);

	poDataset->RasterIO(GF_Write, 0, 0, width, height, data, width, height, GDT_Float64, bandCount, NULL, 0, 0, 0);
	GDALClose(poDataset); 
	poDataset = NULL;
	delete []data;  
	data = NULL;
	cvReleaseMat(&demMat);

	//输出范围
	int pos = outDemFileName.rfind('.');
	string twfFileName = outDemFileName.substr(0, pos) + ".tfw";
	FILE* fp = fopen(twfFileName.c_str(), "w");
	fprintf(fp, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", gDem.dx, 0, 0, -gDem.dy, gDem.x0, gDem.y1);
	fclose(fp);

	return true;
}

bool CSceneDemGenerator::StartGeneratingDem(vector<MatrixTransform*> vecTransNode, string rootModelPath, string outfileName, double xRes, double yRes)
{
	bool isSuccess = false;

	if (vecTransNode.size() == 1)
	{
		isSuccess = GenerateDemForSingleModel(rootModelPath, vecTransNode[0], outfileName, xRes, yRes);
	}
	else if (vecTransNode.size() > 1)
	{
		vector<string> vecDemTiffFileName;
		vector<string> vecDemTwfFileName;

		//生成多个block的Dem
		for (int i = 0; i < vecTransNode.size(); i++)
		{
			char strMsg[MAX_PATH];
			sprintf_s(strMsg, "正在生成DSM...当前数据为第%d块，共%d块", i+1,vecTransNode.size());

			ref_ptr<MatrixTransform> matrixTrans = vecTransNode[i];

			if (!matrixTrans)
			{
				continue;
			}

			//查找trans目录名
			string blockFolderName;
			COsgTransFolderNameFinder ive;
			ive.FindTransFolderName(matrixTrans, blockFolderName);
			
			string blockDemFilePath = rootModelPath + blockFolderName + "\\";
			string outBlockDemFileName = blockDemFilePath + "dem.tif";
			Matrix matrix = matrixTrans->getMatrix();			

			bool isBlockSuccess = GenerateDemForSingleModel(blockDemFilePath, vecTransNode[i], outBlockDemFileName, xRes, yRes);

			int pos = outBlockDemFileName.rfind('.') + 1;
			string twfBlockFileName = outBlockDemFileName.substr(0, pos) + "tfw";

			if (isBlockSuccess)
			{
				vecDemTiffFileName.push_back(outBlockDemFileName);
				vecDemTwfFileName.push_back(twfBlockFileName);
				isSuccess = true;
			}

		}

		if (isSuccess)
		{
			vector<Vec4d> vecRect;

			//计算出合并后的范围
			for (int i = 0; i < vecDemTwfFileName.size(); i++)
			{
				Vec4d rect;
				ObtainImageGeoRect(vecDemTwfFileName[i], rect, vecDemTiffFileName[i]);
				vecRect.push_back(rect);
			}

			Vec4d maxRect = findMaxRect(vecRect);

			//合并tif
			CombineImage(maxRect, outfileName, vecRect, vecDemTiffFileName, vecDemTwfFileName, xRes, yRes);

			//输出twf
			int pos = outfileName.rfind('.');
			string outTwfFileName = outfileName.substr(0, pos) + ".tfw";

			double xRot = 0; double yRot = 0;
			FILE* fpOut = fopen(outTwfFileName.c_str(), "w");
			fprintf(fpOut, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", xRes, xRot, yRot, yRes, maxRect.x(), maxRect.z());
			fclose(fpOut);
		}
	}

	return isSuccess;
}

Vec4d CSceneDemGenerator::findMaxRect(vector<Vec4d> vecRect)
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

void CSceneDemGenerator::CombineImage(Vec4d maxRect, string outfileName, vector<Vec4d> vecRect, vector<string> vecDemTiffFileName, vector<string> vecDemTwfFileName, double xRes, double yRes)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");

	int width = (int) ((maxRect.y() - maxRect.x()) / xRes);
	int height = (int) ((maxRect.w() - maxRect.z()) / yRes);
	int bandCount = 1;

	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff"); 
	GDALDataset* poDataset = poDriver->Create(outfileName.c_str(), width, height, bandCount, GDT_Float64, NULL);
	double* pCombineBuffer =  new double[width * height * bandCount];
	memset(pCombineBuffer, 0, sizeof(double) * width * height * bandCount);

	for (int i = 0; i < vecDemTiffFileName.size(); i++)
	{
		GDALDataset* po = (GDALDataset*) GDALOpen(vecDemTiffFileName[i].c_str(), GA_ReadOnly);
		int xRasterSize = po->GetRasterXSize();
		int yRasterSize = po->GetRasterYSize();
		int bandCount = po->GetRasterCount();
		double* pBuffer = new double[xRasterSize * yRasterSize * bandCount];
		po->RasterIO(GF_Read, 0, 0, xRasterSize, yRasterSize, pBuffer, xRasterSize, yRasterSize, GDT_Float64, bandCount, NULL, 0, 0, 0);
	
		//计算出起始位置
		int rowDiff = (vecRect[i].w() - maxRect.w()) / yRes;
		int colDiff = (vecRect[i].x() - maxRect.x()) / xRes;

		for (int j = 0; j < yRasterSize; j++)
		{
			for (int k = 0; k < xRasterSize; k++)
			{
				int rowImg = j + rowDiff;
				int colImg = k + colDiff;

				if (width - 1< colImg)
				{
					continue;
				}

				if (height - 1< rowImg)
				{
					continue;
				}

				double Height = pBuffer[j * xRasterSize + k];

				if (Height != 0)
				{
					pCombineBuffer[rowImg * width + colImg] = Height;
				}
				
			}
		}
		delete[] pBuffer;
		GDALClose(po);
		po = NULL;
	}

	poDataset->RasterIO(GF_Write, 0, 0, width, height, pCombineBuffer, width, height, GDT_Float64, bandCount, NULL, 0, 0, 0);
	GDALClose(poDataset);
	poDataset = NULL;
	delete[] pCombineBuffer;
}