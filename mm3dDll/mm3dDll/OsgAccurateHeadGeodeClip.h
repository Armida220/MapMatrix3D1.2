#pragma once
#include <osg/NodeVisitor>
#include <vector>
#include <osg/Matrix>
#include "Geometry.h"
#include "IsInAreaJudger.h"
#include <memory>
#include <osg/PrimitiveSet>

class COsgAccurateHeadGeodeClip : public osg::NodeVisitor
{
public:
	COsgAccurateHeadGeodeClip(std::vector<osg::Vec3d> area, osg::Matrix mat);



public:
	std::vector<osg::Vec3d> vecInsideVerts;

protected:
	virtual void apply(osg::Geode& geode);

	virtual bool ApplyWithGeode(osg::Geode* geode, int j);

	virtual bool ApplyWithTriangle(osg::Geometry* geom);

	virtual bool ApplyWithGeometry(osg::Geometry* geom, osg::ref_ptr<osg::Vec2Array> textureArray, osg::Vec3Array* verts, osg::ref_ptr<osg::Image> image);

	virtual void ApplyWithEdgeTriangle(std::vector<osg::Vec2d> vecIntersectPtTexture, osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, 
		std::vector<CL::Vec2d> pol, std::vector<osg::Vec3d> vecIntersectPt, osg::Vec3Array* verts,
		int index1, int index2, int index3, osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle, osg::ref_ptr<osg::Vec2Array> textureArray);

	virtual bool ApplyWithInnerTriangle(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3);

	bool GetTriangleIntersectPolygon(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, std::vector<CL::Vec2d> pol, osg::Vec2d texture1, osg::Vec2d texture2, osg::Vec2d texture3, std::vector<osg::Vec3d> &vecIntersectPt, std::vector<osg::Vec2d> &vecIntersectPtTexture);
	
	double CalculateIntersectPtHeight(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double  y3);

	void CalculateIntersectPtTextureXY(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d intersectPt, osg::Vec2d texture1, osg::Vec2d texture2, osg::Vec2d &interTexture);

	osg::ref_ptr<osg::Image> FindTextureImage(osg::ref_ptr<osg::Geometry> geom);

	osg::ref_ptr<osg::Vec2Array> FindTextureArray(osg::ref_ptr<osg::Geometry> geom);

	void ObtainWorldPt(osg::Vec3Array* verts, int index1, int index2, int index3, std::vector<osg::Vec3d> &vecPt);

	void createClipPolygon(std::vector<CL::Vec2d> &pol);

private:
	std::vector<osg::Vec3d> mArea;

	osg::Matrix mMat;

	std::shared_ptr<CIsInAreaJudger> mJudger;

};

