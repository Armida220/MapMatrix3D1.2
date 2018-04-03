#pragma once
#include<iostream>

#include<string>
#include<memory>

#ifdef OSGDOMLIBDLL  
#define OSGDOMAPI _declspec(dllexport)  
#else  
#define OSGDOMAPI  _declspec(dllimport)  
#endif  


/*
** brief 生成dom真正射影像的接口
*/

class OSGDOMAPI IOsgDomGenerator
{
public:
	/*
	** brief generateDOM 生成真正射影像
	** param sceneRootFile 场景顶层节点文件路径
	** param outFileName 输出场景文件
	** param res 真正射影像分辨率大小
	*/
	virtual bool generateDOM(std::string sceneFileName, std::string outFileName, double res) = 0;
};

/*
** brief 生成dom真正射影像的接口工厂
*/
class OSGDOMAPI IOsgDomGeneratorFactory
{
public:
	/*
	** brief create 生成接口
	*/
	static std::shared_ptr<IOsgDomGenerator> create();
};