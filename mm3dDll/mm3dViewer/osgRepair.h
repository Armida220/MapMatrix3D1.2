#pragma once

#include "IOsgRepair.h"

class osgRepair : public IOsgRepair
{
public:
	osg::ref_ptr<osg::Node> createRepairPatch(std::vector<osg::Vec3d> vecWorldCoord, osg::Matrix worldMatrix, std::string imgFileName);  //´´½¨¿Õ¶´²¹¶¡
};

