#pragma once
#include <memory>
#include <string>

#ifdef OSGDSMLIBDLL  
#define OSGDSMAPI _declspec(dllexport)  
#else  
#define OSGDSMAPI  _declspec(dllimport)  
#endif  

/*
** brief dsm�������ӿ�
*/
class OSGDSMAPI IOsgDsmGene
{
public:	
	/*
	** brief dsm�������ӿ�
	** param sceneFileName �����ļ�
	** param outDemFileName ���dsm�����ļ�
	** param res x�ֱ���
	*/
	virtual void generateDSM(std::string sceneFileName, std::string outDemFileName, double res) = 0;	//����DSM
};

/*
** brief dsm�������ӿڹ���
*/
class OSGDSMAPI IOsgDsmGeneFactory
{
public:
	/*
	** brief ����dsm�������ӿ�
	*/
	static std::shared_ptr<IOsgDsmGene> create();
};
