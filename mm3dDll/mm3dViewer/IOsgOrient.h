#pragma once
#include <string>
#include <osg/Node>



class IOsgOrient
{
public:																					//���ؿ��Ƶ�

	//�ӿ�
	virtual std::string absOrient(osg::ref_ptr<osg::Node> oriNode, std::string oriInfo) = 0;										

	virtual std::string predict(std::string oriInfo) = 0;
};

class IOsgOrientFactory
{
public:
	static IOsgOrient* create();
};