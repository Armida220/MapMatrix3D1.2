#pragma once
#include <vector>
#include <osg/Vec3d>
#include <osg/Matrix>
#include <string>
#include <memory>

//#ifdef REPAIRLIBDLL  
//#define REPAIRAPI _declspec(dllexport)  
//#else  
//#define REPAIRAPI  _declspec(dllimport)  
//#endif  


class /*REPAIRAPI*/ IOsgRepair
{
public:
	virtual osg::ref_ptr<osg::Node> createRepairPatch(std::vector<osg::Vec3d> vecWorldCoord, osg::Matrix worldMatrix, std::string imgFileName) = 0;  //´´½¨¿Õ¶´²¹¶¡
};

class /*REPAIRAPI*/ IOsgRepairFactory
{
public:
	static std::shared_ptr<IOsgRepair> create();
};

