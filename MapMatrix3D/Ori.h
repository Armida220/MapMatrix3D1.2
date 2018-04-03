#pragma once
#include <string>

/**
* @brief 保存影像的空三结果,物点,像点
*/
class COri
{
public:
	COri();
	~COri();

	/**
	* @brief 重写相等符号
	* @param rhs 等号右边
	*/
	bool operator ==(const COri& rhs);

	/**
	* @brief getFileName 从全路径名中获取文件名
	* @param imgName 全路径名
	*/
	std::string getFileName(std::string imgName);

	/**
	* @brief 影像路径
	*/
	std::string imgName = "";

	/**
	* @brief imgX 像点x
	*/
	double imgX = 0;

	/**
	* @brief imgY 像点y
	*/
	double imgY = 0;
};

