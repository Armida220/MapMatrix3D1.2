#pragma once
#include <vector>
#include <memory>
#include <osg/Vec3d>
#include <osg/Node>
#include <osg/MatrixTransform>

class CIsInAreaJudger;

class CObtainSelTileFileName
{
public:
	CObtainSelTileFileName(std::vector<osg::Vec3d> area);

	virtual void obtainSelTileName(osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix);


	std::vector<std::string> getHalfInFileName() const { return vecHalfInFileName; }

	std::vector<std::string> getTotalInFileName() const { return vecTotalInFileName; }
 
	std::vector<std::string> getFlatPolygonFileName() const { return vecFlatPolygonFileName; } 

private:
	std::vector<osg::Vec3d> mArea;
	std::vector<std::string> vecHalfInFileName;
	std::vector<std::string> vecTotalInFileName;
	std::vector<std::string> vecFlatPolygonFileName;

protected:
	std::shared_ptr<CIsInAreaJudger> mIsInAreaJudger;
};

