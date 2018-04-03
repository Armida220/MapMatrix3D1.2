#include "StdAfx.h"
#include "AbsoluteOrientationor.h"
#include "projection_matrix_transform.hpp"
#include "SevenParameter.h"
#include "osgTransNodeFinder.h"
#include "reader.h"
#include <sstream> 
#include <math.h> 

using namespace osg;
using namespace std;

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}


CAbsoluteOrientationor::CAbsoluteOrientationor(void)
{
}


CAbsoluteOrientationor::~CAbsoluteOrientationor(void)
{
}

string CAbsoluteOrientationor::absOrient(ref_ptr<Node> oriNode, string oriInfo)
{
	Json::Reader reader;
	Json::Value root;

	vector<CoordDouble3D> vecAbsXYZ;
	vector<CoordDouble3D> vecRelaXYZ;
	vector<std::pair<string, string>> vecCtrlPt;

	if (reader.parse(oriInfo, root))
	{
		Json::Value children = root["children"];
		int size = children.size();
		
		for (int i = 0; i < size; ++i)
		{
			Json::Value child = children[i];
			string strAbsX = child["absX"].asString();
			string strAbsY = child["absY"].asString();
			string strAbsZ = child["absZ"].asString();

			string strRelaX = child["relaX"].asString();
			string strRelaY = child["relaY"].asString();
			string strRelaZ = child["relaZ"].asString();

			string strCtrlPt = child["ctrlPtNo"].asString();
			string strCheck = child["bCheck"].asString();

			double absX = stringToNum<double>(strAbsX);
			double absY = stringToNum<double>(strAbsY);
			double absZ = stringToNum<double>(strAbsZ);

			double relaX = stringToNum<double>(strRelaX);
			double relaY = stringToNum<double>(strRelaY);
			double relaZ = stringToNum<double>(strRelaZ);

			if (relaX == 0.0 && relaY == 0.0 && relaZ == 0.0)
			{
				continue;
			}

			if (strCheck == "true")
			{
				continue;
			}

			CoordDouble3D relaPt(relaX, relaY, relaZ);
			CoordDouble3D absPt(absX, absY, absZ);

			vecAbsXYZ.push_back(absPt);
			vecRelaXYZ.push_back(relaPt);
			vecCtrlPt.push_back(std::make_pair(strCtrlPt, strCheck));
		}
	}


	// TODO: 在此添加控件通知处理程序代码
	if (vecRelaXYZ.size() < 3)
	{
		return false;
	}

	CSevenParameter sevenParamCalulator;
	sevenParamCalulator.LoadControlPoints(vecAbsXYZ, vecRelaXYZ);
	sevenParamCalulator.CalculateSevenParameter();

	if (sevenParamCalulator.s_ < 0.5)
	{
		return FALSE;
	}

	//绝对定向
	modelOrientation(sevenParamCalulator, oriNode);

	vecRelaXYZ.clear();
	vecAbsXYZ.clear();
	vecCtrlPt.clear();

	if (reader.parse(oriInfo, root))
	{
		Json::Value children = root["children"];
		int size = children.size();

		for (int i = 0; i < size; ++i)
		{
			Json::Value child = children[i];
			string strAbsX = child["absX"].asString();
			string strAbsY = child["absY"].asString();
			string strAbsZ = child["absZ"].asString();

			string strRelaX = child["relaX"].asString();
			string strRelaY = child["relaY"].asString();
			string strRelaZ = child["relaZ"].asString();

			string strCtrlPt = child["ctrlPtNo"].asString();
			string strCheck = child["bCheck"].asString();

			double absX = stringToNum<double>(strAbsX);
			double absY = stringToNum<double>(strAbsY);
			double absZ = stringToNum<double>(strAbsZ);

			double relaX = stringToNum<double>(strRelaX);
			double relaY = stringToNum<double>(strRelaY);
			double relaZ = stringToNum<double>(strRelaZ);

			if (relaX == 0.0 && relaY == 0.0 && relaZ == 0.0)
			{
				continue;
			}

			CoordDouble3D relaPt(relaX, relaY, relaZ);
			CoordDouble3D absPt(absX, absY, absZ);

			vecAbsXYZ.push_back(absPt);
			vecRelaXYZ.push_back(relaPt);
			vecCtrlPt.push_back(std::make_pair(strCtrlPt, strCheck));
		}
	}

	//计算误差
	vector<CoordDouble3D> diffXYZ;

	Json::Value children;
	
	for (int i = 0; i < vecRelaXYZ.size(); i++)
	{
		CoordDouble3D relaXYZ = vecRelaXYZ[i];

		Json::Value child;
		//根据矩阵反算的绝对坐标
		CoordDouble3D calAbsXYZ;
		sevenParamCalulator.CalculateRelativeToAbsolute(vecRelaXYZ[i], calAbsXYZ);

		diffXYZ.push_back(vecAbsXYZ[i] - calAbsXYZ);

		child["ctrlPtNo"] = vecCtrlPt[i].first;
		child["bCheck"] = vecCtrlPt[i].second;

		child["calAbsX"] = calAbsXYZ.x;
		child["calAbsY"] = calAbsXYZ.y;
		child["calAbsZ"] = calAbsXYZ.z;
		
		child["errX"] = calAbsXYZ.x - vecAbsXYZ[i].x;
		child["errY"] = calAbsXYZ.y - vecAbsXYZ[i].y;
		child["errZ"] = calAbsXYZ.z - vecAbsXYZ[i].z;

		children.append(child);
	}

	Json::Value res;
	res["children"] = children;

	string str = res.toStyledString();
	
	return str;
}

string CAbsoluteOrientationor::predict(string oriInfo)
{
	Json::Reader reader;
	Json::Value root;

	vector<CoordDouble3D> vecAbsXYZ;
	vector<CoordDouble3D> vecRelaXYZ;
	vector<string> vecCtrlPt;

	if (reader.parse(oriInfo, root))
	{
		Json::Value children = root["children"];
		int size = children.size();

		for (int i = 0; i < size; ++i)
		{
			Json::Value child = children[i];
			string strAbsX = child["absX"].asString();
			string strAbsY = child["absY"].asString();
			string strAbsZ = child["absZ"].asString();

			string strRelaX = child["relaX"].asString();
			string strRelaY = child["relaY"].asString();
			string strRelaZ = child["relaZ"].asString();

			string strCtrlPt = child["ctrlPtNo"].asString();

			double absX = stringToNum<double>(strAbsX);
			double absY = stringToNum<double>(strAbsY);
			double absZ = stringToNum<double>(strAbsZ);

			double relaX = stringToNum<double>(strRelaX);
			double relaY = stringToNum<double>(strRelaY);
			double relaZ = stringToNum<double>(strRelaZ);

			if (relaX == 0.0 && relaY == 0.0 && relaZ == 0.0)
			{
				continue;
			}

			CoordDouble3D relaPt(relaX, relaY, relaZ);
			CoordDouble3D absPt(absX, absY, absZ);

			vecAbsXYZ.push_back(absPt);
			vecRelaXYZ.push_back(relaPt);
			vecCtrlPt.push_back(strCtrlPt);
		}

		// TODO: 在此添加控件通知处理程序代码
		if (vecRelaXYZ.size() < 3)
		{
			return "";
		}

		CSevenParameter sevenParamCalulator;
		sevenParamCalulator.LoadControlPoints(vecAbsXYZ, vecRelaXYZ);
		sevenParamCalulator.CalculateSevenParameter();

		if (sevenParamCalulator.s_ < 0.5)
		{
			return "";
		}

		for (int i = 0; i < size; ++i)
		{
			string strAbsX = root["children"][i]["absX"].asString();
			string strAbsY = root["children"][i]["absY"].asString();
			string strAbsZ = root["children"][i]["absZ"].asString();

			string strRelaX = root["children"][i]["relaX"].asString();
			string strRelaY = root["children"][i]["relaY"].asString();
			string strRelaZ = root["children"][i]["relaZ"].asString();

			string strCtrlPt = root["children"][i]["ctrlPtNo"].asString();

			double absX = stringToNum<double>(strAbsX);
			double absY = stringToNum<double>(strAbsY);
			double absZ = stringToNum<double>(strAbsZ);

			double relaX = stringToNum<double>(strRelaX);
			double relaY = stringToNum<double>(strRelaY);
			double relaZ = stringToNum<double>(strRelaZ);

			if (relaX == 0.0 && relaY == 0.0 && relaZ == 0.0)
			{
				CoordDouble3D r_pt;
				CoordDouble3D a_pt;
				a_pt.x = absX;
				a_pt.y = absY;
				a_pt.z = absZ;

				sevenParamCalulator.CalculateAbsoluteToRelative(a_pt, r_pt);
				root["children"][i]["relaX"] = to_string(r_pt.x);
				root["children"][i]["relaY"] = to_string(r_pt.y);
				root["children"][i]["relaZ"] = to_string(r_pt.z);
			}
		}

	}

	return root.toStyledString();

}


void CAbsoluteOrientationor::modelOrientation(CSevenParameter &sevenParamCalulator, osg::ref_ptr<osg::Node> oriNode)
{
	// TODO: 在此添加控件通知处理程序代码

	double matDB[16] = { 0 };
	int cnt = 0;
	//设置变换矩阵，R是转置输入，T除以scale，scale：1/scale
	matDB[0] = sevenParamCalulator.R_[0]; matDB[1] = sevenParamCalulator.R_[3]; matDB[2] = sevenParamCalulator.R_[6];
	matDB[4] = sevenParamCalulator.R_[1]; matDB[5] = sevenParamCalulator.R_[4]; matDB[6] = sevenParamCalulator.R_[7];
	matDB[8] = sevenParamCalulator.R_[2]; matDB[9] = sevenParamCalulator.R_[5]; matDB[10] = sevenParamCalulator.R_[8];
	matDB[12] = sevenParamCalulator.t_[0] / sevenParamCalulator.s_; matDB[13] = sevenParamCalulator.t_[1] / sevenParamCalulator.s_; matDB[14] = sevenParamCalulator.t_[2] / sevenParamCalulator.s_; matDB[15] = 1 / sevenParamCalulator.s_;

	osg::Matrix absMatrix;
	absMatrix.set(matDB);
	double* newDB = (double*)absMatrix.ptr();

	for (int i = 0; i < 16; i++)
	{
		newDB[i] *= sevenParamCalulator.s_;
	}

	vector<MatrixTransform*> vecTrans;
	COsgTransNodeFinder ive;
	ive.findTransNode(oriNode, vecTrans);

	vecTrans[0]->setMatrix(absMatrix);

}
