#pragma once
#include<string>
#include <regex>
#include "..\tinyxml2\tinyxml2.h"


/**
* @brief C3D
*/
struct C3D
{
	C3D(double x, double y, double z) :
		mX(x), mY(y), mZ(z)
	{
	}

	double mX;
	double mY;
	double mZ;
};


/**
* @brief C9D
*/
struct C9D
{
	double m00 = 0;
	double m01 = 0;
	double m02 = 0;

	double m10 = 0;
	double m11 = 0;
	double m12 = 0;

	double m20 = 0;
	double m21 = 0;
	double m22 = 0;
};

C9D C9DFromOPK(	double Omega,double Phi, double Kappa);

/**
* @brief C9D
*/
struct CDISTORTION
{
	double k1 = 0;
	double k2 = 0;
	double k3 = 0;
};

/**
* @brief CSmart3DOrientation smart3d
* @每张影像的内参 外参 主点主距
*/
struct CSmart3DOrientation
{
	CSmart3DOrientation(double f,
		double px, double py,double fc,
		std::string imgName, C9D rot, C3D center,CDISTORTION dist, double imgWidth,
		double imgHeight) :
		mf(f), mpX(px), mpY(py),mfc(fc),
		mImgName(imgName), mRot(rot), mCenter(center),mDistortion(dist),
		mImgWidth(imgWidth), mImgHeight(imgHeight)
	{
	}

	/**
	* @brief f 内参
	*/
	double mf = 0;

	/**
	* @brief pX 主点x
	*/
	double mpX = 0;

	/**
	* @brief pY 主点y
	*/
	double mpY = 0;

	/**
	* @brief pfc 焦距
	*/
	double mfc = 0;

	/**
	* @brief 影像名
	*/
	std::string mImgName = "";

	/**
	* @brief 旋转矩阵
	*/
	C9D mRot;

	/**
	* @brief 外方位元素中心
	*/
	C3D mCenter;

	/**
	* @brief 畸变参数
	*/
	CDISTORTION mDistortion;
	/**
	* @brief 影像宽
	*/

	double mImgWidth;

	/**
	* @brief 影像高
	*/
	double mImgHeight;
};

typedef CArray<CString> StringArray;
/**
* @brief CSmart3DImportor 抓取smart3d工程中的所有数据
*/
class CSmart3DImportor
{
public:
	CSmart3DImportor();
	~CSmart3DImportor();

	/**
	* @brief start 开始导入smart3d空三成果，并试图转换成mapmatrix的支持的导入格式
	* @para  inImageFileName 影像文件名
	* @para  xmlSmart3DPath smart3d 空三成果路径
	* @para  xmlOriPrjPath 作为模板的mapMatrix工程路径
	* @para  outXmlPath 输出mapmatrix路径
	*/
	virtual void start(std::vector<std::string>& inImageFileName, std::string xmlSmart3DPath, std::string xmlOriPrjPath, 
		std::string outXmlPath);

	/**
	* @brief extract 从字符串中利用正则表达式提取x y z坐标
	* @para  str 影像文件名
	* @para  x y z 三维坐标
	*/
	virtual void extract(std::string str, double &x, double &y, double &z);

	/**
	* @brief parseSmart3DAerialExport 解析smart3d空三成果
	* @para  xmlInputFileName xml空三成果输入路径
	*/
	virtual int parseSmart3DAerialExport(const char* xmlInputFileName);

	/**
	* @brief outMapMatrixResult 输出mapmatrix成果
	* @para  oriXmlFileName 原始xml成果
	* @para  inImageFileName 影像路径
	* @para  outXmlMatrixFileName 输出MapMatrix成果
	*/
	virtual int outMapMatrixResult(const char* oriXmlFileName, const char* outXmlMatrixFileName, std::vector<std::string>& inImageFileName, std::vector<float>& fLength);


	const StringArray & getMessage() const { return m_message; }

protected:
	/**
	* @brief insertStripLine 插入航带id
	* @para  head  头元素
	* @para  inImageFileName 影像图片路径名
	*/
	virtual void insertStripLine(tinyxml2::XMLElement* stripList, std::vector<float>& fLength, std::vector<std::string>& inImageFileName);

	/**
	* @brief insertImageID 插入imageID
	* @para  head  头元素
	*/
	virtual void insertImageID(tinyxml2::XMLElement* head, const CSmart3DOrientation& ori);

	/**
	* @brief insertImagePara 插入影像参数
	* @para  image  影像
	* @para  fLength 焦距
	* @para  ori    影像的外内方位元素
	*/
	virtual void insertImagePara(tinyxml2::XMLElement* image, double fLength, const CSmart3DOrientation& ori);

	/**
	* @brief insertCamera 插入相机参数
	* @para  imagePara  影像参数
	* @para  ori    影像的外内方位元素
	* @para  fLength 焦距
	*/
	virtual void insertCamera(tinyxml2::XMLElement* imagePara, const CSmart3DOrientation& ori, double fLength);

	/**
	* @brief insertImagePara 插入影像校正系数
	* @para  image  影像
	*/
	virtual void insertImageCorr(tinyxml2::XMLElement* image);

	/**
	* @brief insertExOri 插入外方位元素
	* @para  image  影像
	* @para  ori    影像的外内方位元素
	*/
	virtual void insertExOri(tinyxml2::XMLElement* image, const CSmart3DOrientation& ori);

	/**
	* @brief insertInnerOri 插入内方位元素
	* @para  image  影像
	* @para  ori    影像的外内方位元素
	*/
	virtual void insertInnerOri(tinyxml2::XMLElement* image, const CSmart3DOrientation& ori, double fLength);

	/**
	* @brief insertImageEntity 插入影像路径
	* @para  strImagePath  影像路径
	*/
	virtual void insertImageEntity(tinyxml2::XMLElement* image, std::string strImagePath);

	/**
	* @brief insertFocus 插入焦点
	* @para  image  影像xml元素
	* @para  fLength 焦距
	*/
	virtual void insertFocus(tinyxml2::XMLElement* cameraPara, double fLength);

	/**
	* @brief insertFidMask 插入fidMask
	* @para  cameraPara  影像参数
	*/
	virtual void insertFidMask(tinyxml2::XMLElement* cameraPara);

	/**
	* @brief insertDistortion 插入畸变
	* @para  cameraPara  影像参数
	*/
	virtual void insertDistortion(tinyxml2::XMLElement* cameraPara,const CSmart3DOrientation & ori);

	bool compareLastFolder(std::string newImgPath, std::string newImgPath1);
protected:
	/**
	* @brief vecOri smart3D空三成果
	*/
	std::vector<CSmart3DOrientation> vecOri;

	/**
	* @brief smart3Ddoc smart3D xml文档
	*/
	tinyxml2::XMLDocument smart3Ddoc;

	/**
	* @brief smart3Ddoc 作为模板的xml文档
	*/
	tinyxml2::XMLDocument oriDoc;

	StringArray m_message;
};

