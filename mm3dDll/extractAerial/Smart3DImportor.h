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
* @ÿ��Ӱ����ڲ� ��� ��������
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
	* @brief f �ڲ�
	*/
	double mf = 0;

	/**
	* @brief pX ����x
	*/
	double mpX = 0;

	/**
	* @brief pY ����y
	*/
	double mpY = 0;

	/**
	* @brief pfc ����
	*/
	double mfc = 0;

	/**
	* @brief Ӱ����
	*/
	std::string mImgName = "";

	/**
	* @brief ��ת����
	*/
	C9D mRot;

	/**
	* @brief �ⷽλԪ������
	*/
	C3D mCenter;

	/**
	* @brief �������
	*/
	CDISTORTION mDistortion;
	/**
	* @brief Ӱ���
	*/

	double mImgWidth;

	/**
	* @brief Ӱ���
	*/
	double mImgHeight;
};

typedef CArray<CString> StringArray;
/**
* @brief CSmart3DImportor ץȡsmart3d�����е���������
*/
class CSmart3DImportor
{
public:
	CSmart3DImportor();
	~CSmart3DImportor();

	/**
	* @brief start ��ʼ����smart3d�����ɹ�������ͼת����mapmatrix��֧�ֵĵ����ʽ
	* @para  inImageFileName Ӱ���ļ���
	* @para  xmlSmart3DPath smart3d �����ɹ�·��
	* @para  xmlOriPrjPath ��Ϊģ���mapMatrix����·��
	* @para  outXmlPath ���mapmatrix·��
	*/
	virtual void start(std::vector<std::string>& inImageFileName, std::string xmlSmart3DPath, std::string xmlOriPrjPath, 
		std::string outXmlPath);

	/**
	* @brief extract ���ַ���������������ʽ��ȡx y z����
	* @para  str Ӱ���ļ���
	* @para  x y z ��ά����
	*/
	virtual void extract(std::string str, double &x, double &y, double &z);

	/**
	* @brief parseSmart3DAerialExport ����smart3d�����ɹ�
	* @para  xmlInputFileName xml�����ɹ�����·��
	*/
	virtual int parseSmart3DAerialExport(const char* xmlInputFileName);

	/**
	* @brief outMapMatrixResult ���mapmatrix�ɹ�
	* @para  oriXmlFileName ԭʼxml�ɹ�
	* @para  inImageFileName Ӱ��·��
	* @para  outXmlMatrixFileName ���MapMatrix�ɹ�
	*/
	virtual int outMapMatrixResult(const char* oriXmlFileName, const char* outXmlMatrixFileName, std::vector<std::string>& inImageFileName, std::vector<float>& fLength);


	const StringArray & getMessage() const { return m_message; }

protected:
	/**
	* @brief insertStripLine ���뺽��id
	* @para  head  ͷԪ��
	* @para  inImageFileName Ӱ��ͼƬ·����
	*/
	virtual void insertStripLine(tinyxml2::XMLElement* stripList, std::vector<float>& fLength, std::vector<std::string>& inImageFileName);

	/**
	* @brief insertImageID ����imageID
	* @para  head  ͷԪ��
	*/
	virtual void insertImageID(tinyxml2::XMLElement* head, const CSmart3DOrientation& ori);

	/**
	* @brief insertImagePara ����Ӱ�����
	* @para  image  Ӱ��
	* @para  fLength ����
	* @para  ori    Ӱ������ڷ�λԪ��
	*/
	virtual void insertImagePara(tinyxml2::XMLElement* image, double fLength, const CSmart3DOrientation& ori);

	/**
	* @brief insertCamera �����������
	* @para  imagePara  Ӱ�����
	* @para  ori    Ӱ������ڷ�λԪ��
	* @para  fLength ����
	*/
	virtual void insertCamera(tinyxml2::XMLElement* imagePara, const CSmart3DOrientation& ori, double fLength);

	/**
	* @brief insertImagePara ����Ӱ��У��ϵ��
	* @para  image  Ӱ��
	*/
	virtual void insertImageCorr(tinyxml2::XMLElement* image);

	/**
	* @brief insertExOri �����ⷽλԪ��
	* @para  image  Ӱ��
	* @para  ori    Ӱ������ڷ�λԪ��
	*/
	virtual void insertExOri(tinyxml2::XMLElement* image, const CSmart3DOrientation& ori);

	/**
	* @brief insertInnerOri �����ڷ�λԪ��
	* @para  image  Ӱ��
	* @para  ori    Ӱ������ڷ�λԪ��
	*/
	virtual void insertInnerOri(tinyxml2::XMLElement* image, const CSmart3DOrientation& ori, double fLength);

	/**
	* @brief insertImageEntity ����Ӱ��·��
	* @para  strImagePath  Ӱ��·��
	*/
	virtual void insertImageEntity(tinyxml2::XMLElement* image, std::string strImagePath);

	/**
	* @brief insertFocus ���뽹��
	* @para  image  Ӱ��xmlԪ��
	* @para  fLength ����
	*/
	virtual void insertFocus(tinyxml2::XMLElement* cameraPara, double fLength);

	/**
	* @brief insertFidMask ����fidMask
	* @para  cameraPara  Ӱ�����
	*/
	virtual void insertFidMask(tinyxml2::XMLElement* cameraPara);

	/**
	* @brief insertDistortion �������
	* @para  cameraPara  Ӱ�����
	*/
	virtual void insertDistortion(tinyxml2::XMLElement* cameraPara,const CSmart3DOrientation & ori);

	bool compareLastFolder(std::string newImgPath, std::string newImgPath1);
protected:
	/**
	* @brief vecOri smart3D�����ɹ�
	*/
	std::vector<CSmart3DOrientation> vecOri;

	/**
	* @brief smart3Ddoc smart3D xml�ĵ�
	*/
	tinyxml2::XMLDocument smart3Ddoc;

	/**
	* @brief smart3Ddoc ��Ϊģ���xml�ĵ�
	*/
	tinyxml2::XMLDocument oriDoc;

	StringArray m_message;
};

