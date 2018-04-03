#pragma once
#include "IOsgClip.h"
#include <osg/Node>
#include <osg/Vec3d>

class COsgAccurateClip : public IOsgClip
{
public:
	virtual void accurateClip(std::string inFileName, std::string outputFileName, std::vector<osg::Vec3d> area);

protected:
	void clipHalfInPolygonTile(osg::ref_ptr<osg::Node> node, std::string inFilePath, std::string outFilePath, 
		std::vector<osg::Vec3d> area, osg::Matrix matrix);

	void clip(std::string inFileName, std::string outputFileName, std::vector<osg::Vec3d> area, int xTileNum, int yTileNum);

	void startClipScene();

protected:
	std::string mInFileName;

	std::string mOutputFileName;
	
	std::vector<osg::Vec3d> mArea;
};

