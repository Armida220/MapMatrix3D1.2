#pragma once
#include <string>

/**
* @brief ����Ӱ��Ŀ������,���,���
*/
class COri
{
public:
	COri();
	~COri();

	/**
	* @brief ��д��ȷ���
	* @param rhs �Ⱥ��ұ�
	*/
	bool operator ==(const COri& rhs);

	/**
	* @brief getFileName ��ȫ·�����л�ȡ�ļ���
	* @param imgName ȫ·����
	*/
	std::string getFileName(std::string imgName);

	/**
	* @brief Ӱ��·��
	*/
	std::string imgName = "";

	/**
	* @brief imgX ���x
	*/
	double imgX = 0;

	/**
	* @brief imgY ���y
	*/
	double imgY = 0;
};

