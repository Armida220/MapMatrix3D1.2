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
** brief ����dom������Ӱ��Ľӿ�
*/

class OSGDOMAPI IOsgDomGenerator
{
public:
	/*
	** brief generateDOM ����������Ӱ��
	** param sceneRootFile ��������ڵ��ļ�·��
	** param outFileName ��������ļ�
	** param res ������Ӱ��ֱ��ʴ�С
	*/
	virtual bool generateDOM(std::string sceneFileName, std::string outFileName, double res) = 0;
};

/*
** brief ����dom������Ӱ��Ľӿڹ���
*/
class OSGDOMAPI IOsgDomGeneratorFactory
{
public:
	/*
	** brief create ���ɽӿ�
	*/
	static std::shared_ptr<IOsgDomGenerator> create();
};