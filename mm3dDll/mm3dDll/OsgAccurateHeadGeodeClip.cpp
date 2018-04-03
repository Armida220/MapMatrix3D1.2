#include "stdafx.h"
#include "OsgAccurateHeadGeodeClip.h"
#include <osg/Geometry>
#include "Geometry.h"
#include <osg/Geode>
#include "VertsTriangle.h"
#include <osg/Texture2D>

using namespace std;
using namespace osg;

COsgAccurateHeadGeodeClip::COsgAccurateHeadGeodeClip(vector<Vec3d> area, Matrix mat)
	: NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN),
	mArea(area),
	mMat(mat)
{
	vector<CL::Vec2d> pol;
	createClipPolygon(pol);

	shared_ptr<CIsInAreaJudger> judger(new CIsInAreaJudger(pol));
	mJudger = judger;
}

void COsgAccurateHeadGeodeClip::apply(Geode& geode)
{
	int num = geode.getNumDrawables();

	for (int j = num - 1; j >= 0; --j)
	{
		ApplyWithGeode(&geode, j);
	}
}


bool COsgAccurateHeadGeodeClip::ApplyWithGeode(Geode* geode, int j)
{
	Geometry* geom = dynamic_cast<Geometry*>(geode->getDrawable(j));

	if (!geom)
		return false; //得到可绘制结点的顶点序列 

	bool isEmpty = ApplyWithTriangle(geom);

	if (isEmpty == true)
	{
		geode->removeDrawable(geom);
	}

	return true;
}

bool COsgAccurateHeadGeodeClip::ApplyWithTriangle(Geometry* geom)
{
	bool isEmpty = true;

	Vec3Array* verts = dynamic_cast<Vec3Array*>(geom->getVertexArray());
	//查找纹理
	ref_ptr<Vec2Array> textureArray = FindTextureArray(geom);
	ref_ptr<Image> image = FindTextureImage(geom);

	if (image == NULL || textureArray == NULL)
	{
		return isEmpty;
	}

	isEmpty = ApplyWithGeometry(geom, textureArray, verts, image);

	return isEmpty;
}

bool COsgAccurateHeadGeodeClip::ApplyWithGeometry(Geometry* geom, ref_ptr<Vec2Array> textureArray, Vec3Array* verts, ref_ptr<Image> image)
{
	bool isEmpty = true;
	int imageWidth = image->s();
	int imageHeight = image->t();

	for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
	{
		PrimitiveSet* priSet = geom->getPrimitiveSet(k);
		ref_ptr<DrawElements> drawElementTriangle = priSet->getDrawElements();
		ref_ptr<DrawElementsUInt> newDrawElementsTriangle = new DrawElementsUInt(PrimitiveSet::TRIANGLES);

		int cnt = drawElementTriangle->getNumIndices();
		int triCnt = cnt / 3;

		for (int w = 0; w < triCnt; w++)
		{
			int index1 = drawElementTriangle->index(w * 3);
			int index2 = drawElementTriangle->index(w * 3 + 1);
			int index3 = drawElementTriangle->index(w * 3 + 2);
			vector<Vec3d> vecWorldPt;
			ObtainWorldPt(verts, index1, index2, index3, vecWorldPt);
			Vec3d pt1 = vecWorldPt[0];
			Vec3d pt2 = vecWorldPt[1];
			Vec3d pt3 = vecWorldPt[2];

			//创建范围
			vector<CL::Vec2d> pol;
			createClipPolygon(pol);

			vector<Vec2d> vecIntersectPtTexture;
			vector<Vec3d> vecIntersectPt;

			Vec2d texture1, texture2, texture3;
			texture1.x() = textureArray->at(index1).x() * imageWidth;
			texture1.y() = textureArray->at(index1).y() * imageHeight;
			texture2.x() = textureArray->at(index2).x() * imageWidth;
			texture2.y() = textureArray->at(index2).y() * imageHeight;
			texture3.x() = textureArray->at(index3).x() * imageWidth;
			texture3.y() = textureArray->at(index3).y() * imageHeight;

			bool isCorner = GetTriangleIntersectPolygon(pt1, pt2, pt3, pol, texture1, texture2, texture3, vecIntersectPt, vecIntersectPtTexture);

			if (vecIntersectPt.size() == 2 && isCorner == false)
			{
				vecIntersectPtTexture[0].x() = vecIntersectPtTexture[0].x() / imageWidth;
				vecIntersectPtTexture[0].y() = vecIntersectPtTexture[0].y() / imageHeight;

				vecIntersectPtTexture[1].x() = vecIntersectPtTexture[1].x() / imageWidth;
				vecIntersectPtTexture[1].y() = vecIntersectPtTexture[1].y() / imageHeight;

				ApplyWithEdgeTriangle(vecIntersectPtTexture, pt1, pt2, pt3, pol, vecIntersectPt, verts, index1, index2, index3, newDrawElementsTriangle, textureArray);
				isEmpty = false;

				vecInsideVerts.push_back(vecIntersectPt[0]);
				vecIntersectPt.push_back(vecIntersectPt[1]);
			}
			else if (vecIntersectPt.size() > 0 || isCorner == true)
			{
				//认为是边角位置的三角形
				newDrawElementsTriangle->push_back(index1);
				newDrawElementsTriangle->push_back(index2);
				newDrawElementsTriangle->push_back(index3);

				isEmpty = false;
			}
			else 
			{
				if(ApplyWithInnerTriangle(pt1, pt2, pt3))
				{
					newDrawElementsTriangle->push_back(index1);
					newDrawElementsTriangle->push_back(index2);
					newDrawElementsTriangle->push_back(index3);
					isEmpty = false;
				}
			}
			
		}

		geom->setPrimitiveSet(k, newDrawElementsTriangle);
	}

	return isEmpty;
}

bool COsgAccurateHeadGeodeClip::ApplyWithInnerTriangle(Vec3d pt1, Vec3d pt2, Vec3d pt3)
{
	bool isInPolygon = false;

	if (mJudger->isTriangleInPolygon(pt1, pt2, pt3))
	{
		isInPolygon = true;
	}

	return isInPolygon;
	
}

void COsgAccurateHeadGeodeClip::ApplyWithEdgeTriangle(vector<Vec2d> vecIntersectPtTexture, Vec3d pt1, Vec3d pt2, Vec3d pt3,
	vector<CL::Vec2d> pol, vector<Vec3d> vecIntersectPt, Vec3Array* verts, int index1, int index2, int index3, 
	ref_ptr<DrawElementsUInt> newDrawElementsTriangle, ref_ptr<Vec2Array> textureArray)
{
	Matrix iMatrix = mMat.inverse(mMat);
	Vec3d ori1 = vecIntersectPt[0] * iMatrix;
	Vec3d ori2 = vecIntersectPt[1] * iMatrix;

	//插入坐标
	verts->push_back(ori1);
	verts->push_back(ori2);

	textureArray->push_back(vecIntersectPtTexture[0]);
	textureArray->push_back(vecIntersectPtTexture[1]);

	vector<int> vecInsidePtIndex;
	int numIndex = verts->size();
	bool isIndex1 = false;
	bool isIndex2 = false;
	bool isIndex3 = false;

	if ((CL::Point_In_Polygon_2D(pt1.x(), pt1.y(), pol)))
	{
		vecInsidePtIndex.push_back(index1);
		isIndex1 = true;
	}

	if ((CL::Point_In_Polygon_2D(pt2.x(), pt2.y(), pol)))
	{
		vecInsidePtIndex.push_back(index2);
		isIndex2 = true;
	}

	if ((CL::Point_In_Polygon_2D(pt3.x(), pt3.y(), pol)))
	{
		vecInsidePtIndex.push_back(index3);
		isIndex3 = true;
	}

	if (vecInsidePtIndex.size() == 1)
	{
		newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
		newDrawElementsTriangle->push_back(numIndex - 2);
		newDrawElementsTriangle->push_back(numIndex - 1);
	}
	else if (vecInsidePtIndex.size() == 2)											//两个顶点在里面
	{
		if (isIndex1 == false)
		{
			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 1);

			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(numIndex - 2);
			newDrawElementsTriangle->push_back(numIndex - 1);
		}
		else if (isIndex2 == false)
		{
			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 1);

			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(numIndex - 2);
			newDrawElementsTriangle->push_back(numIndex - 1);
		}
		else if (isIndex3 == false)
		{
			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 1);

			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 2);
			newDrawElementsTriangle->push_back(numIndex - 1);
		}
	}
}

void COsgAccurateHeadGeodeClip::ObtainWorldPt(Vec3Array* verts, int index1, int index2, int index3, vector<Vec3d> &vecPt)
{
	double x1 = verts->at(index1).x();
	double y1 = verts->at(index1).y();
	double z1 = verts->at(index1).z();
	Vec3d pt1(x1, y1, z1);
	pt1 = pt1 * mMat;

	double x2 = verts->at(index2).x();
	double y2 = verts->at(index2).y();
	double z2 = verts->at(index2).z();
	Vec3d pt2(x2, y2, z2);
	pt2 = pt2 * mMat;

	double x3 = verts->at(index3).x();
	double y3 = verts->at(index3).y();
	double z3 = verts->at(index3).z();
	Vec3d pt3(x3, y3, z3);
	pt3 = pt3 * mMat;

	vecPt.push_back(pt1);
	vecPt.push_back(pt2);
	vecPt.push_back(pt3);
}

void COsgAccurateHeadGeodeClip::createClipPolygon(vector<CL::Vec2d> &pol)
{
	for (int t = 0; t < mArea.size(); t++)
	{
		double x = mArea[t].x();
		double y = mArea[t].y();
		double z = mArea[t].z();
		pol.push_back(CL::Vec2d(x, y));
	}

	double x = mArea[0].x();
	double y = mArea[0].y();
	pol.push_back(CL::Vec2d(x, y));
}

ref_ptr<Image> COsgAccurateHeadGeodeClip::FindTextureImage(ref_ptr<Geometry> geom)
{
	ref_ptr<Texture2D> texture = dynamic_cast<Texture2D*> (geom->getOrCreateStateSet()->getTextureAttribute(0, StateAttribute::TEXTURE));

	if (!texture)
	{
		return NULL;
	}

	ref_ptr<Image> image = texture->getImage(0);
	return image.get();
}

ref_ptr<Vec2Array> COsgAccurateHeadGeodeClip::FindTextureArray(ref_ptr<Geometry> geom)
{
	ref_ptr<Vec2Array> textureArray = dynamic_cast<Vec2Array*> (geom->getTexCoordArray(0));

	if (!textureArray)
	{
		return NULL;
	}

	return textureArray.get();
}

bool COsgAccurateHeadGeodeClip::GetTriangleIntersectPolygon(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, vector<CL::Vec2d> pol, osg::Vec2d texture1, osg::Vec2d texture2, osg::Vec2d texture3, vector<osg::Vec3d> &vecIntersectPt, vector<osg::Vec2d> &vecIntersectPtTexture)
{
	int intersectPolNum = 0;

	Matrix iMatrix = mMat.inverse(mMat);

	for (int i = 0; i < pol.size() - 1; i++)
	{
		bool isIntersect = false;

		CL::Vec2d pt2D_1, pt2D_2, pt2D_3;
		pt2D_1.x = pt1.x();
		pt2D_1.y = pt1.y();

		pt2D_2.x = pt2.x();
		pt2D_2.y = pt2.y();

		pt2D_3.x = pt3.x();
		pt2D_3.y = pt3.y();

		if (CL::IsIntersect(pt2D_1.x, pt2D_1.y, pt2D_2.x, pt2D_2.y, pol[i].x, pol[i].y, pol[i + 1].x, pol[i + 1].y))
		{
			CL::Vec2d intersetPt = CL::GetLineSegmentIntersection(pt2D_1, pt2D_2, pol[i], pol[i + 1]);

			double z = CalculateIntersectPtHeight(pt1.x(), pt1.y(), pt1.z(), pt2.x(), pt2.y(), pt2.z(), intersetPt.x, intersetPt.y);
			Vec3d intersectPt3D(intersetPt.x, intersetPt.y, z);
			vecIntersectPt.push_back(intersectPt3D);

			//计算原始的坐标
			Vec3d oriPt1 = pt1 * iMatrix;
			Vec3d oriPt2 = pt2 * iMatrix;
			Vec3d oriIntersectPt3D = intersectPt3D * iMatrix;

			//插值纹理坐标
			Vec2d intersectTexture;
			CalculateIntersectPtTextureXY(oriPt1, oriPt2, oriIntersectPt3D, texture1, texture2, intersectTexture);
			vecIntersectPtTexture.push_back(intersectTexture);

			isIntersect = true;
		}

		if (CL::IsIntersect(pt2D_2.x, pt2D_2.y, pt2D_3.x, pt2D_3.y, pol[i].x, pol[i].y, pol[i + 1].x, pol[i + 1].y))
		{
			CL::Vec2d intersetPt = CL::GetLineSegmentIntersection(pt2D_2, pt2D_3, pol[i], pol[i + 1]);

			double z = CalculateIntersectPtHeight(pt2.x(), pt2.y(), pt2.z(), pt3.x(), pt3.y(), pt3.z(), intersetPt.x, intersetPt.y);
			Vec3d intersectPt3D(intersetPt.x, intersetPt.y, z);
			vecIntersectPt.push_back(intersectPt3D);

			//计算原始的坐标
			Vec3d oriPt1 = pt1 * iMatrix;
			Vec3d oriPt2 = pt2 * iMatrix;
			Vec3d oriIntersectPt3D = intersectPt3D * iMatrix;

			//插值纹理坐标
			Vec2d intersectTexture;
			CalculateIntersectPtTextureXY(pt2, pt3, intersectPt3D, texture2, texture3, intersectTexture);
			vecIntersectPtTexture.push_back(intersectTexture);
		}

		if (CL::IsIntersect(pt2D_1.x, pt2D_1.y, pt2D_3.x, pt2D_3.y, pol[i].x, pol[i].y, pol[i + 1].x, pol[i + 1].y))
		{
			CL::Vec2d intersetPt = CL::GetLineSegmentIntersection(pt2D_1, pt2D_3, pol[i], pol[i + 1]);

			double z = CalculateIntersectPtHeight(pt1.x(), pt1.y(), pt1.z(), pt3.x(), pt3.y(), pt3.z(), intersetPt.x, intersetPt.y);
			Vec3d intersectPt3D(intersetPt.x, intersetPt.y, z);
			vecIntersectPt.push_back(intersectPt3D);

			//计算原始的坐标
			Vec3d oriPt1 = pt1 * iMatrix;
			Vec3d oriPt2 = pt2 * iMatrix;
			Vec3d oriIntersectPt3D = intersectPt3D * iMatrix;

			//插值纹理坐标
			Vec2d intersectTexture;
			CalculateIntersectPtTextureXY(pt1, pt3, intersectPt3D, texture1, texture3, intersectTexture);
			vecIntersectPtTexture.push_back(intersectTexture);

			isIntersect = true;
		}

		if (isIntersect)
		{
			intersectPolNum++;
		}

	}

	if (intersectPolNum >= 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

double COsgAccurateHeadGeodeClip::CalculateIntersectPtHeight(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double  y3)
{
	return (x3 - x1) / (x2 - x1) * (z2 - z1) + z1;
}


void COsgAccurateHeadGeodeClip::CalculateIntersectPtTextureXY(Vec3d pt1, Vec3d pt2, Vec3d intersectPt, Vec2d texture1, Vec2d texture2, Vec2d &interTexture)
{
	interTexture.x() = texture1.x() - (texture1.x() - texture2.x()) * (pt1.x() - intersectPt.x()) / (pt1.x() - pt2.x());
	interTexture.y() = texture1.y() - (texture1.y() - texture2.y()) * (pt1.y() - intersectPt.y()) / (pt1.y() - pt2.y());
}