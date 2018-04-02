#include "stdafx.h"
#include "Smart3DImportor.h"
#include "..\tinyxml2\tinyxml2.h"
#include "EXIF.H"
#include <sstream>
#include <osg/Vec3d>
#include <osg/Matrix>
#include <exception>
#include "..\tinyxml2\tinyxml2.cpp"

#include "resource.h"

using namespace std;
using namespace tinyxml2;
using namespace osg;

#define _PI 3.14159265358979323846

int g_imgIndex;
//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}
void TransposedMatrix(int w, double * rot){
	for (int i = 0; i < w; i ++)
	{
		for (int j = i+1; j < w; j ++)
		{
			int i0 = i + j * w;
			int i1 = j + i * w;
			std::swap(rot[i0], rot[i1]);
		}
	}
}

void MatrixToAngle(double *lfMarix, double *lfAngle, int nRotSystem, int iAngleSystem)
{
	double angle[3];//phi,omega,kapa/a alpha kapa
	switch (nRotSystem)
	{
	case 0:
	{
		angle[0] = atan(-lfMarix[2] / lfMarix[8]);
		angle[1] = asin(-lfMarix[5]);
		if (lfMarix[4] > 0)angle[2] = atan(lfMarix[3] / lfMarix[4]);
		else if (lfMarix[4] < 0)angle[2] = atan(lfMarix[3] / lfMarix[4]) + _PI;
		else if (lfMarix[3] >= 0)angle[2] = _PI*0.5;
		else angle[2] = _PI*1.5;
	}
	break;

	case 1:
	{
		angle[0] = asin(-lfMarix[2]);
		angle[1] = atan(-lfMarix[5] / lfMarix[8]);
		if (lfMarix[0] > 0)angle[2] = atan(-lfMarix[1] / lfMarix[0]);
		else if (lfMarix[0] < 0)angle[2] = angle[2] = atan(-lfMarix[1] / lfMarix[0]) + _PI;
		else if (lfMarix[1] >= 0)angle[2] = _PI*1.5;
		else angle[2] = _PI*0.5;
	}
	break;

	case 2:// 暂缺
	{
		angle[1] = acos(lfMarix[8]);
		if (0 != angle[1])angle[0] = atan(lfMarix[2] / lfMarix[5]);
	}

	case 3:// Matrix, what do you want?
		return;

	default:// Not support
		break;
	}

	// 转换角度系统
	int i;
	for (i = 0; i < 3; i++)
	{
		switch (iAngleSystem)
		{
		case 0://360
			lfAngle[i] = angle[i] * 180 / _PI;	break;
		case 1://400
			lfAngle[i] = angle[i] * 200 / _PI;	break;
			break;
		case 2://arc
			lfAngle[i] = angle[i];			break;
		default:
			break;
		}
	}
}


CSmart3DImportor::CSmart3DImportor()
{
}


CSmart3DImportor::~CSmart3DImportor()
{
}

void CSmart3DImportor::start(vector<string>& vecImageFileName, string xmlSmart3DPath, string xmlOriPrjPath, 
	string outXmlPath)
{
	m_message.RemoveAll();
	//读取焦距
	std::vector<float> vecfLength;
	for (int i = 0;i<vecImageFileName.size();++i)
	{
		FILE* hFile = NULL;
		fopen_s(&hFile, vecImageFileName[i].c_str(), "rb");
		float fLength = 0;

		if (hFile) {
			Cexif decoder;
			decoder.DecodeExif(hFile);
			fLength = decoder.m_exifinfo->FocalLength;
		}
		vecfLength.push_back(fLength);
		fclose(hFile);
	}
	

	g_imgIndex = 0;

	//读取smart3d空三成果
	parseSmart3DAerialExport(xmlSmart3DPath.c_str());

	//输出mapmatrix成果
	outMapMatrixResult(xmlOriPrjPath.c_str(), outXmlPath.c_str(), vecImageFileName, vecfLength);

}

void CSmart3DImportor::extract(string str, double &x, double &y, double &z)
{
	regex reg1("(\\s+),(\\s+),(\\s+)");
	smatch r1;
	regex_match(str, r1, reg1);
	x = stringToNum<int>(r1[1]);
	y = stringToNum<int>(r1[2]);
	z = stringToNum<int>(r1[3]);
}

void AssertAndThrowException(XMLElement * p, const char * elementName){
	if (!p){
		std::string str = ("Can't find element:");
		str += elementName;
		throw std::exception(str.c_str());
	}
}

double MATH_POW2(double x) { return x * x; }

template <class Type>
Type * newArray(int size, const Type & init)
{
	Type * res = new Type[size];
	for (int i = 0; i < size; i++)
	{
		res[i] = init;
	}
	return res;
}

double * matrix_rotation_from_axis_angle(double axis[3], double angle)
{
	double ca = cos(angle);
	double sa = sin(angle);
	double omca = 1.0 - ca;
	
	double * rot = newArray<double>(9,0.0);
		
	rot[0] = ca + MATH_POW2(axis[0]) * omca;
	rot[1] = axis[0] * axis[1] * omca - axis[2] * sa;
	rot[2] = axis[0] * axis[2] * omca + axis[1] * sa;

	rot[3] = axis[1] * axis[0] * omca + axis[2] * sa;
	rot[4] = ca + MATH_POW2(axis[1]) * omca;
	rot[5] = axis[1] * axis[2] * omca - axis[0] * sa;

	rot[6] = axis[2] * axis[0] * omca - axis[1] * sa;
	rot[7] = axis[2] * axis[1] * omca + axis[0] * sa;
	rot[8] = ca + MATH_POW2(axis[2]) * omca;

	return rot;
}

double * matrix_multiply(double *  mat_a,int rows_a,int cols_a,double * mat_b, int cols_b)
{
	double * mat_res = newArray<double>(rows_a * cols_b, 0.0);
	for (int j = 0; j < (cols_b); j++)
	{
		for (int i = 0; i < rows_a; i++)
		{
			int ica = i * cols_a;
			int icb = i * cols_b;
			for (int k = 0; k < (cols_a); k++)
			{
				mat_res[icb + j] = mat_res[icb + j] + \
					mat_a[ica + k] * mat_b[k * cols_b + j];
			}
		}
	}
	return mat_res;
}


void RotationMatrix(double omega, double phi, double kappa, double *matrix)
{
	double a0[3] = { 1, 0, 0 };
	double a1[3] = { 0, 1, 0 };
	double a2[3] = { 0, 0, 1 };
	double * r0 = matrix_rotation_from_axis_angle(a0, omega);
	double * r1 = matrix_rotation_from_axis_angle(a1, phi);
	double * r2 = matrix_rotation_from_axis_angle(a2, kappa);
	double * r3 = matrix_multiply(r0, 3, 3, r1, 3);
	double * r4 = matrix_multiply(r3, 3, 3, r2, 3);
	delete r0;
	delete r1;
	delete r2;
	delete r3;

	memcpy(matrix, r4, sizeof(double) * 9);
	delete r4;
	TransposedMatrix(3, matrix);

}

C9D C9DFromOPK(double Omega, double Phi, double Kappa)
{
	C9D r;
	RotationMatrix(
		Omega * PI / 180.,
		Phi   * PI / 180.,
		Kappa * PI / 180.,
		(double*)&r);
	return r;
	
}

#define GETCHILD(C,P,CI) XMLElement * C = P->FirstChildElement(CI);AssertAndThrowException(C,CI);
#define GETCHILD_WITH_OUT_ASSERT(C,P,CI) XMLElement * C = P->FirstChildElement(CI);
int CSmart3DImportor::parseSmart3DAerialExport(const char* xmlInputFileName)
{
	int res = smart3Ddoc.LoadFile(xmlInputFileName);

	if (res != 0)
	{
		return res;
	}

	XMLElement* root = smart3Ddoc.RootElement();

	if (!root)
	{
		return 0;
	}

	GETCHILD(blockNode,root,"Block")
	while (blockNode != NULL)
	{
		//继续查找photogroups
		GETCHILD(photoGroupsNode, blockNode      ,"Photogroups");
		GETCHILD(photoGroupNode , photoGroupsNode,"Photogroup" );

		while (photoGroupNode != NULL)
		{
			GETCHILD(photoNode,photoGroupNode,"Photo");
			
			string strFocal;
			double f;
			double fc = 0;
			GETCHILD_WITH_OUT_ASSERT(pFocalLengthPixels, photoGroupNode, "FocalLengthPixels");
			GETCHILD_WITH_OUT_ASSERT(pFocalLength, photoGroupNode, "FocalLength");
			
			
			GETCHILD(pImageDimensions,photoGroupNode,"ImageDimensions");
			GETCHILD(pImageWidth,pImageDimensions,"Width");
			GETCHILD(pImageHeight,pImageDimensions,"Height");
			double imgWidth = stringToNum<double>(pImageWidth->GetText());
			double imgHeight = stringToNum<double>(pImageHeight->GetText());

			if (pFocalLength){
				fc = stringToNum<double>(pFocalLength->GetText());
			}
			if (pFocalLengthPixels){
				f = stringToNum<double>(pFocalLengthPixels->GetText());
			}
			else
			{
				GETCHILD(sensor, photoGroupNode, "SensorSize");
				double d1 = fc;
				double d2 = stringToNum<double>(sensor->GetText());
				//f = d1 * d2;
				//f = d1/0.004683;
				//f = d1 / 0.003896;
				//f = d1 / 0.00391667;
				f = d1 / (d2 / imgWidth);
			}

			GETCHILD(pPt,photoGroupNode,"PrincipalPoint");
			GETCHILD(pX,pPt,"x");
			GETCHILD(pY,pPt,"y");
			string strX = pX->GetText();
			string strY = pY->GetText();
			double px = stringToNum<double>(strX);
			double py = stringToNum<double>(strY);

			double pk1 = 0;
			double pk2 = 0;
			double pk3 = 0;
			try{
				GETCHILD(pDist, photoGroupNode, "Distortion");
				GETCHILD_WITH_OUT_ASSERT(pK1, pDist, "K1");
				if(pK1)
					pk1 = stringToNum<double>(pK1->GetText());
				GETCHILD_WITH_OUT_ASSERT(pK2, pDist, "K2");
				if (pK2)
					pk2 = stringToNum<double>(pK2->GetText());
				GETCHILD_WITH_OUT_ASSERT(pK3, pDist, "K3");
				if (pK3)
					pk3 = stringToNum<double>(pK3->GetText());
			}
			catch (std::exception & e){
				m_message.Add(CString(e.what()) + ". K1=K2=K3=0");
			}
			
			double k1 = 0;
			double k2 = 0;
			double k3 = 0;
			if (fc != 0.0){
				k1 = -1 * pk1 / (fc * fc);
				k2 = -1 * pk2 / (fc * fc * fc * fc);
				k3 = -1 * pk3 / (fc * fc * fc * fc * fc * fc);
			}
			CDISTORTION dist1;
			dist1.k1 = k1;
			dist1.k2 = k2;
			dist1.k3 = k3;

			while (photoNode != NULL)
			{
				string imgText;
				try{
					GETCHILD(imgPath, photoNode, "ImagePath");
					imgText = imgPath->GetText();
					GETCHILD(pos, photoNode, "Pose");
					GETCHILD(rot, pos, "Rotation");

					C9D rot1;
					try{
						GETCHILD(m0, rot, "M_00");
						GETCHILD(m1, rot, "M_01");
						GETCHILD(m2, rot, "M_02");
						GETCHILD(m10, rot, "M_10");
						GETCHILD(m11, rot, "M_11");
						GETCHILD(m12, rot, "M_12");
						GETCHILD(m20, rot, "M_20");
						GETCHILD(m21, rot, "M_21");
						GETCHILD(m22, rot, "M_22");
						double nm00 = stringToNum<double>(m0->GetText());
						double nm01 = stringToNum<double>(m1->GetText());
						double nm02 = stringToNum<double>(m2->GetText());
						double nm10 = stringToNum<double>(m10->GetText());
						double nm11 = stringToNum<double>(m11->GetText());
						double nm12 = stringToNum<double>(m12->GetText());
						double nm20 = stringToNum<double>(m20->GetText());
						double nm21 = stringToNum<double>(m21->GetText());
						double nm22 = stringToNum<double>(m22->GetText());
						rot1.m00 = nm00;
						rot1.m01 = nm01;
						rot1.m02 = nm02;
						rot1.m10 = nm10;
						rot1.m11 = nm11;
						rot1.m12 = nm12;
						rot1.m20 = nm20;
						rot1.m21 = nm21;
						rot1.m22 = nm22;
					}
					catch (std::exception){
						GETCHILD(pOmega, rot, "Omega");
						GETCHILD(pPhi  , rot, "Phi");
						GETCHILD(pKappa, rot, "Kappa");
						double Omega = stringToNum<double>(pOmega->GetText());
						double Phi   = stringToNum<double>(pPhi->GetText());
						double Kappa = stringToNum<double>(pKappa->GetText());
 						rot1 = C9DFromOPK(Omega, Phi, Kappa);
					}

					GETCHILD(center, pos, "Center");

					GETCHILD(x, center, "x");
					GETCHILD(y, center, "y");
					GETCHILD(z, center, "z");
					double mx = stringToNum<double>(x->GetText());
					double my = stringToNum<double>(y->GetText());
					double mz = stringToNum<double>(z->GetText());
					C3D trans(mx, my, mz);
					double cfc = fc;
					if (cfc == 0.0)
					{
						GETCHILD_WITH_OUT_ASSERT(pExifData, photoNode, "ExifData");
						if (pExifData)
						{
							GETCHILD_WITH_OUT_ASSERT(pFocalLength_Exif, pExifData, "FocalLength");
							if (pFocalLength_Exif)
								cfc = stringToNum<double>(pFocalLength_Exif->GetText());
						}
					}
					CSmart3DOrientation ori(f, px, imgHeight - py, cfc, imgText, rot1, trans, dist1, imgWidth, imgHeight);

					vecOri.push_back(ori);

					photoNode = photoNode->NextSiblingElement();
				}
				catch (std::exception & e){
					m_message.Add(CString(e.what()) + " in " + imgText.c_str());
					photoNode = photoNode->NextSiblingElement();
					continue;
				}
				
			}

			photoGroupNode = photoGroupNode->NextSiblingElement();
		}

		blockNode = blockNode->NextSiblingElement();//下一个兄弟节点
	}
}

int CSmart3DImportor::outMapMatrixResult(const char* oriXmlFileName, const char* outXmlMatrixFileName, std::vector<std::string>& vecImageFileName, std::vector<float>& vecfLength)
{
	int res1 = oriDoc.LoadFile(oriXmlFileName);

	if (res1 != 0)
	{
		return res1;
	}

	XMLElement* head = oriDoc.RootElement();

	if (!head)
	{
		return 0;
	}

	//插入文件头
	XMLElement* firstChild = head->FirstChildElement("Stereo_Number");
	firstChild->SetText("1");
	XMLElement* stripNum = oriDoc.NewElement("Strip_Number");
	stripNum->SetText("1");
	head->InsertAfterChild(firstChild, stripNum);

	//插入航带信息，这里使用单航带进行
	XMLElement* stripList = oriDoc.NewElement("Strip_List");
	insertStripLine(stripList, vecfLength, vecImageFileName);
	head->InsertAfterChild(stripNum, stripList);

	oriDoc.SaveFile(outXmlMatrixFileName);

	CString outMsg;
	outMsg.LoadString(IDS_SUCCESS);
	AfxMessageBox(outMsg);
}

bool CSmart3DImportor::compareLastFolder(std::string newImgPath, std::string newImgPath1)
{
	int pos1 = newImgPath.rfind("\\");

	if (pos1 == -1)
	{
		pos1 = newImgPath.rfind("/");
	}
	std::string s1 = newImgPath.substr(pos1 + 1);

	pos1 = newImgPath1.rfind("\\");

	if (pos1 == -1)
	{
		pos1 = newImgPath1.rfind("/");
	}
	std::string s2 = newImgPath1.substr(pos1 + 1);

	return s1 == s2;
}

void CSmart3DImportor::insertStripLine(XMLElement* stripList, std::vector<float>& vecfLength, std::vector<string>& vecinImageFileName)
{
	//设置航带
	XMLElement* strip0 = oriDoc.NewElement("STRIP_0");

	//设置航带中影像
	XMLElement* Strip_ID = oriDoc.NewElement("Strip_ID");
	Strip_ID->SetText("NULL");
	strip0->InsertEndChild(Strip_ID);

	int num = vecOri.size();
	char str[MAX_PATH];
	sprintf_s(str, "%d\0", num);

	XMLElement* ImageNum = oriDoc.NewElement("Image_Number");
	ImageNum->SetText(str);
	strip0->InsertEndChild(ImageNum);

	XMLElement* expandStrip = oriDoc.NewElement("Expand_Strip");
	expandStrip->SetText("1");
	strip0->InsertEndChild(expandStrip);
	string newImgPath1;
	for (auto ori : vecOri)
	{
		XMLElement* image = oriDoc.NewElement("Image");
		strip0->InsertEndChild(image);

		//
		string newImgFileName = ori.mImgName;
		int pos1 = newImgFileName.rfind("\\");

		if (pos1 == -1)
		{
			pos1 = newImgFileName.rfind("/");
		}

		pos1 += 1;
		int length = newImgFileName.length() - 1;
		string newImgTitle = newImgFileName.substr(pos1, length - pos1 + 1);
		double fLength = 0.0;
		string newImgPath = newImgFileName.substr(0, pos1 - 1);

		for (int i = 0; i < vecinImageFileName.size(); ++i)
		{
			string ImgFileName = vecinImageFileName[i];
			int pos1 = ImgFileName.rfind("\\");

			if (pos1 == -1)
			{
				pos1 = ImgFileName.rfind("/");
			}
			newImgPath1 = ImgFileName.substr(0, pos1);

			if (compareLastFolder(newImgPath, newImgPath1))
			{
				fLength = vecfLength[i];
				break;
			}
		}
		if (ori.mfc != 0)
		{
			fLength = ori.mfc;
		}
		//插入影像id
		insertImageID(image, ori);

		//插入影像参数		
		insertImagePara(image, fLength, ori);

		//影像校正系数
		insertImageCorr(image);

		//插入外方位元素
		insertExOri(image, ori);

		//插入内方位元素
		insertInnerOri(image, ori, fLength);

		/*int pos = inImageFileName.rfind("\\") + 1;
		string inImageFilePath = inImageFileName.substr(0, pos);*/
		string newImgFile = newImgPath1 + "\\" + newImgTitle;

		//插入影像entity
		insertImageEntity(image, newImgFile);
	}
	

	stripList->InsertEndChild(strip0);
}

void CSmart3DImportor::insertImageID(XMLElement* image, const CSmart3DOrientation& ori)
{
	XMLElement* imageID = oriDoc.NewElement("Image_ID");
	
	stringstream ss;
	ss << g_imgIndex;
	string imgName = ss.str();
	imageID->SetText(imgName.c_str());
	image->InsertEndChild(imageID);
	g_imgIndex++;
}

void CSmart3DImportor::insertImagePara(XMLElement* image, double fLength, const CSmart3DOrientation& ori)
{
	XMLElement* imagePara = oriDoc.NewElement("Image_Para");
	image->InsertEndChild(imagePara);

	XMLElement* imageHeight = oriDoc.NewElement("Image_Height");
	imagePara->InsertEndChild(imageHeight);
	double height = ori.mImgHeight;
	double width = ori.mImgWidth;
	char str[MAX_PATH];
	sprintf_s(str, "%lf", height);
	imageHeight->SetText(str);

	XMLElement* imageWidth = oriDoc.NewElement("Image_Width");
	imagePara->InsertEndChild(imageWidth);
	char str1[MAX_PATH];
	sprintf_s(str1, "%lf", width);
	imageWidth->SetText(str1);

	XMLElement* imageColor = oriDoc.NewElement("Image_Color");
	imagePara->InsertEndChild(imageColor);
	imageColor->SetText("3");

	double f = ori.mf;
	double unit = fLength / f;
	XMLElement* scanSize = oriDoc.NewElement("Scan_Size");
	imagePara->InsertEndChild(scanSize);

	char str2[MAX_PATH];
	sprintf_s(str2, "%lf", unit);
	scanSize->SetText(str2);

	XMLElement* cameraReverse = oriDoc.NewElement("Camera_Reverse");
	imagePara->InsertEndChild(cameraReverse);
	cameraReverse->SetText("0");

	XMLElement* imageRot = oriDoc.NewElement("Image_Rotated");
	imagePara->InsertEndChild(imageRot);
	imageRot->SetText("0");

	insertCamera(imagePara, ori, fLength);
}

void CSmart3DImportor::insertCamera(XMLElement* imagePara, const CSmart3DOrientation& ori, double fLength)
{
	XMLElement* cameraPara = oriDoc.NewElement("CAMERA");
	double f = ori.mf;
	double unit = fLength / f;

	//相机内部参数设置
	{
		double cx = ori.mImgWidth / 2.0;
		double cy = ori.mImgHeight / 2.0;

		double pX = (ori.mpX - cx) * unit;
		double pY = (ori.mpY - cy) * unit;

		XMLElement* principleX = oriDoc.NewElement("Principle_X");
		XMLElement* principleY = oriDoc.NewElement("Principle_Y");

		principleX->SetText(pX);
		principleY->SetText(pY);
		cameraPara->InsertEndChild(principleX);
		cameraPara->InsertEndChild(principleY);

		//插入焦点
		insertFocus(cameraPara, fLength);

		//插入fidMask
		insertFidMask(cameraPara);

		//插入畸变参数
		insertDistortion(cameraPara,ori);
		
	}

	imagePara->InsertEndChild(cameraPara);
}

void CSmart3DImportor::insertImageCorr(XMLElement* image)
{
	XMLElement* imageCorrect = oriDoc.NewElement("IMAGE_CORRECTION");
	image->InsertEndChild(imageCorrect);

	for (int i = 0; i < 12; i++)
	{
		XMLElement* data = oriDoc.NewElement("DATA");
		imageCorrect->InsertEndChild(data);
		data->SetText("0");
	}
}

void CSmart3DImportor::insertExOri(XMLElement* image, const CSmart3DOrientation& ori)
{
	XMLElement* exOri = oriDoc.NewElement("Exterior_Orientation_Parameter");
	image->InsertEndChild(exOri);

	//外方位元素
	{
		double x = ori.mCenter.mX;
		double y = ori.mCenter.mY;
		double z = ori.mCenter.mZ;
		Vec3d p(x, y, z);

		XMLElement* lineElementX = oriDoc.NewElement("Line_Elements");
		lineElementX->SetText(x);

		XMLElement* lineElementY = oriDoc.NewElement("Line_Elements");
		lineElementY->SetText(y);

		XMLElement* lineElementZ = oriDoc.NewElement("Line_Elements");
		lineElementZ->SetText(z);

		exOri->InsertEndChild(lineElementX);
		exOri->InsertEndChild(lineElementY);
		exOri->InsertEndChild(lineElementZ);

		double rot[9] = { 0 };
		rot[0] = -1 * ori.mRot.m00; rot[1] = -1 * ori.mRot.m01; rot[2] = -1 * ori.mRot.m02;
		rot[3] = ori.mRot.m10; rot[4] = ori.mRot.m11; rot[5] = ori.mRot.m12;
		rot[6] = ori.mRot.m20; rot[7] = ori.mRot.m21; rot[8] = ori.mRot.m22;
	
// 		Matrix mat;
// 		mat.set(1, 0, 0, 0, 
// 				0, rot[0], rot[1], rot[2],
// 				0, rot[3], rot[4], rot[5],
// 				0, rot[6], rot[7], rot[8]);
// 

		//转置
		{
			double temp = rot[3];
			rot[3] = rot[1];
			rot[1] = temp;

			temp = rot[2];
			rot[2] = rot[6];
			rot[6] = temp;

			temp = rot[5];
			rot[5] = rot[7];
			rot[7] = temp;
		}

		double angle[3] = { 0 };
		MatrixToAngle(rot, angle, 0, 0);

		XMLElement* angleElementX = oriDoc.NewElement("Angle_Elements");
		angleElementX->SetText(angle[0]);
		exOri->InsertEndChild(angleElementX);

		XMLElement* angleElementY = oriDoc.NewElement("Angle_Elements");
		angleElementY->SetText(angle[1]);
		exOri->InsertEndChild(angleElementY);

		XMLElement* angleElementZ = oriDoc.NewElement("Angle_Elements");
		angleElementZ->SetText(angle[2]);
		exOri->InsertEndChild(angleElementZ);

		XMLElement* matSizeElement = oriDoc.NewElement("Matrix_Size");
		matSizeElement->SetText("9");
		exOri->InsertEndChild(matSizeElement);

		for (int i = 0; i < 9; i++)
		{
			XMLElement* matDataElement = oriDoc.NewElement("Matrix_Data");
			matDataElement->SetText(rot[i]);
			exOri->InsertEndChild(matDataElement);
		}

		XMLElement* rotSysElement = oriDoc.NewElement("Rotation_System");
		rotSysElement->SetText("0");
		exOri->InsertEndChild(rotSysElement);

		XMLElement* nodeStatusElement = oriDoc.NewElement("Node_Status");
		nodeStatusElement->SetText("0");
		exOri->InsertEndChild(nodeStatusElement);
	}
}

void CSmart3DImportor::insertInnerOri(XMLElement* image, const CSmart3DOrientation& ori, double fLength)
{
	XMLElement* innerOri = oriDoc.NewElement("Interior_Orientation_Parameter");
	image->InsertEndChild(innerOri);

	double unit = fLength / ori.mf;

	{
		XMLElement* prjCenterX = oriDoc.NewElement("Project_Center_X");
		double pX = ori.mpX;
		double pY = ori.mpY;
		prjCenterX->SetText(pX);
		innerOri->InsertEndChild(prjCenterX);

		XMLElement* prjCenterY = oriDoc.NewElement("Project_Center_Y");
		prjCenterY->SetText(pY);

		innerOri->InsertEndChild(prjCenterY);
	}
}

void CSmart3DImportor::insertImageEntity(XMLElement* image, string strImagePath)
{
	XMLElement* ImageEntity = oriDoc.NewElement("Image_Entity");
	image->InsertEndChild(ImageEntity);

	XMLElement* ImagePath = oriDoc.NewElement("Image_Path");
	ImagePath->SetText(strImagePath.c_str());
	ImageEntity->InsertEndChild(ImagePath);

}

void CSmart3DImportor::insertFocus(XMLElement* cameraPara, double fLength)
{
	XMLElement* focus = oriDoc.NewElement("Focus");
	focus->SetText(fLength);
	cameraPara->InsertEndChild(focus);
}

void CSmart3DImportor::insertFidMask(XMLElement* cameraPara)
{
	XMLElement* fidMask = oriDoc.NewElement("Fiducial_Mask");
	XMLElement* fidNum = oriDoc.NewElement("Fiducial_Number");
	XMLElement* rmk = oriDoc.NewElement("RMK");
	rmk->SetText("0");
	fidNum->SetText("0");

	fidMask->InsertEndChild(fidNum);
	fidMask->InsertEndChild(rmk);
	cameraPara->InsertEndChild(fidMask);
}

void CSmart3DImportor::insertDistortion(XMLElement* cameraPara, const CSmart3DOrientation & ori)
{
	//插入畸变参数
	XMLElement* dist = oriDoc.NewElement("Distortion");
	XMLElement* distCof0 = oriDoc.NewElement("Distortion_coff");
	distCof0->SetText(0);
	dist->InsertEndChild(distCof0);

	XMLElement* distCof1 = oriDoc.NewElement("Distortion_coff");
	distCof1->SetText(0);
	dist->InsertEndChild(distCof1);

	XMLElement* distCof2 = oriDoc.NewElement("Distortion_coff");
	distCof2->SetText(0);
	dist->InsertEndChild(distCof2);

	XMLElement* distCof3 = oriDoc.NewElement("Distortion_coff");
	distCof3->SetText(ori.mDistortion.k1);
	dist->InsertEndChild(distCof3);

	XMLElement* distCof4 = oriDoc.NewElement("Distortion_coff");
	distCof4->SetText(0);
	dist->InsertEndChild(distCof4);

	XMLElement* distCof5 = oriDoc.NewElement("Distortion_coff");
	distCof5->SetText(ori.mDistortion.k2);
	dist->InsertEndChild(distCof5);

	XMLElement* distCof6 = oriDoc.NewElement("Distortion_coff");
	distCof6->SetText(0);
	dist->InsertEndChild(distCof6);

	XMLElement* distCof7 = oriDoc.NewElement("Distortion_coff");
	distCof7->SetText(ori.mDistortion.k3);
	dist->InsertEndChild(distCof7);

	XMLElement* distP1 = oriDoc.NewElement("Distortion_P1");
	distP1->SetText(0);
	dist->InsertEndChild(distP1);

	XMLElement* distP2 = oriDoc.NewElement("Distortion_P2");
	distP2->SetText(0);
	dist->InsertEndChild(distP2);

	XMLElement* distB1 = oriDoc.NewElement("Distortion_B1");
	distB1->SetText(0);
	dist->InsertEndChild(distB1);

	XMLElement* distB2 = oriDoc.NewElement("Distortion_B2");
	distB2->SetText(0);
	dist->InsertEndChild(distB2);

	XMLElement* pixelGrid = oriDoc.NewElement("Distortion_PixelGrid");
	pixelGrid->SetText(0);
	dist->InsertEndChild(pixelGrid);

	XMLElement* distOrigin = oriDoc.NewElement("Distortion_Origin");
	distOrigin->SetText(0);
	dist->InsertEndChild(distOrigin);

	XMLElement* forType = oriDoc.NewElement("Formula_Type");
	forType->SetText(0);
	dist->InsertEndChild(forType);

	cameraPara->InsertEndChild(dist);
}