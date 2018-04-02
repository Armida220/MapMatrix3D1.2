#pragma once
#include <memory>
#include <string>

#ifdef OSGDSMLIBDLL  
#define OSGDSMAPI _declspec(dllexport)  
#else  
#define OSGDSMAPI  _declspec(dllimport)  
#endif  

/*
** brief dsm生成器接口
*/
class OSGDSMAPI IOsgDsmGene
{
public:	
	/*
	** brief dsm生成器接口
	** param sceneFileName 场景文件
	** param outDemFileName 输出dsm场景文件
	** param res x分辨率
	*/
	virtual void generateDSM(std::string sceneFileName, std::string outDemFileName, double res) = 0;	//产生DSM
};

/*
** brief dsm生成器接口工厂
*/
class OSGDSMAPI IOsgDsmGeneFactory
{
public:
	/*
	** brief 生产dsm生成器接口
	*/
	static std::shared_ptr<IOsgDsmGene> create();
};
