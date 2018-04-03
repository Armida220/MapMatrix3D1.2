

#include "stdafx.h"
#include "OsgSyn.h"
#include "IBaseDrawer.h"
#include "ICallBack.h"
#include <numeric>
#include "OsgModel.h"
#include "ThreadSafeUpdateCallback.h"
#include "qcomm.h"
#include "OsgUserData.h"
#include <set>
using namespace mm3dView;
using namespace std;
using namespace osgCall;
using namespace osgDraw;
using namespace osg;



#define CLS_NULL					-1
#define CLS_PERMANENT				0
#define CLS_FEATURE					1
#define CLS_GEOMETRY				2
#define CLS_GEOPOINT				3
#define CLS_GEOCURVE				4
#define CLS_GEOSURFACE				5
#define CLS_GEOTEXT					6
#define CLS_GEODIRPOINT				7
#define CLS_DLGFEATURE				8
#define CLS_GEOPARALLEL				9
#define CLS_GEODCURVE				10
#define CLS_GEOMULTISURFACE		    11
#define CLS_GEOMULTIPOINT			12
#define CLS_GEODEMPOINT				13
#define CLS_GEOSURFACEPOINT			14		


COsgSyn::COsgSyn(COsgModel* model)
	: osgbModel(model)
{
}


COsgSyn::~COsgSyn()
{
}

void removeGroupChild(ref_ptr<Group> group, ref_ptr<CThreadSafeUpdateCallback> spChildCallback)
{
	if (!group)
	{
		return;
	}

	int numChild = group->getNumChildren();

	if (numChild == 0)
	{
		return;
	}
	std::list<ref_ptr<Node>>childers;
	for (int idx = 0; idx < numChild; idx++)
	{
		childers.push_back(group->getChild(idx));
	}

	for (ref_ptr<Node> spChild : childers)
	{

		if (!spChild)
		{
			continue;
		}

		int numParent = spChild->getNumParents();

		if (numParent == 0)
		{
			continue;
		}
		std::vector<ref_ptr<Node>> parents;

		for (int j = 0; j < numParent; j++)
		{
			ref_ptr<Node> parent = spChild->getParent(j);
			parents.push_back(parent);
		}

		for (ref_ptr<Node> parent : parents)
		{
			spChildCallback->RemoveChild(parent, spChild);
		}

	}
}

void COsgSyn::synFtr(osg::Group * root, shared_ptr<IBaseDrawer> IDrawer, const vector<osgFtr> &vecFtr, bool bIsMap)
{
	bool bIsHighLight = false;
	Vec4d DrawerClr = IDrawer->getColor();
	if (DrawerClr.x() == 0.f && DrawerClr.y() == 1.f && DrawerClr.z() == 1.f)
	{
		bIsHighLight = true;
	}
	set<string> ftrIDSet;
	//绘制
	for (auto ftr : vecFtr)
	{
		if (ftr.isVisible == false)
		{
			continue;
		}

	//	if (!bIsHighLight)
		{
			Coord rgb = ftr.rgb;
			double r = rgb.x;
			double g = rgb.y;
			double b = rgb.z;
			IDrawer->resetColor(Vec4d(r, g, b, ftr.alpha));
		}

		string idStr = ftr.mId;
		ref_ptr<Node> node = drawBaseGeom(IDrawer, ftr);

		if (!bIsMap)
		{
			continue;
		}
		
		//说明没有被插入过
		MAP_ITER iter = mapNode.find(idStr);
		bool findI = (ftrIDSet.find(idStr) != ftrIDSet.end());
		ftrIDSet.insert(idStr);
		if (iter == mapNode.end())
		{
			ref_ptr<Group> group = new Group;
			group->addChild(node);
			mapNode.insert(make_pair(idStr, group));
			iter = mapNode.find(idStr);
			//group->setNodeMask(ftr.isVisible ? 1 : 0);
		}
		else
		{
			ref_ptr<Group> group = iter->second;
			if (!findI)
			{
				ref_ptr<CThreadSafeUpdateCallback> spgroupcb = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());
				if (!spgroupcb)
				{
					group->removeChildren(0, group->getNumChildren());
				}
				else
				{
					removeGroupChild(group, spgroupcb);
				}
			}
			group->addChild(node);
			//group->setNodeMask(ftr.isVisible ? 1 : 0);
		}
	}
}

void COsgSyn::removeFtr(const vector<osgFtr> &vecFtr, ref_ptr<Group> root)
{
	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());

	if (!spChildCallback)
	{
		return;
	}

	for (auto ftr : vecFtr)
	{
		string id = ftr.mId;
		MAP_ITER iter = mapNode.find(id);

		if (iter == mapNode.end())
		{
			continue;
		}
	
		ref_ptr<Group> spParent = iter->second;

		removeGroupChild(spParent, spChildCallback);

		mapNode.erase(iter);
	}
}

void COsgSyn::synFtrVerts(const shared_ptr<IBaseDrawer> &IDrawer, const vector<osgFtr> &vecFtr)
{
	for (auto ftr : vecFtr)
	{
		if (ftr.type != CLS_GEOPOINT) continue;
		IDrawer->resetColor(Vec4d(1, 0, 1, 1));

		vector<Coord> coords = ftr.vecCoord;
		Coord ave = calAve(coords);
		Matrix mat;
		mat.setTrans(Vec3(ave.x, ave.y, ave.z));

		for (auto coord : coords)
		{
			coord -= ave;
			IDrawer->drawPt(Vec3d(coord.x, coord.y, coord.z), mat);
		}
	}
}

void COsgSyn::synFtrArrow(const shared_ptr<IBaseDrawer> &IDrawer, const vector<osgFtr> &vecFtr)
{
	for (auto ftr : vecFtr)
	{
		vector<Coord> coords = ftr.vecCoord;

		//计算平均值，作为偏移量
		Coord ave = calAve(coords);
		Matrix mat;
		mat.setTrans(Vec3d(ave.x, ave.y, ave.z));

		double tangent = ftr.tangent;
		Coord coord = coords[0] - ave;

		IDrawer->drawArrow(Vec3(coord.x, coord.y, coord.z), tangent, mat);
	}
}

void COsgSyn::synAuxGraph(const shared_ptr<IBaseDrawer> &IDrawer, const vector<osgFtr> &vecFtr)
{
	//绘制点
	for (auto ftr : vecFtr)
	{
		Coord rgb = ftr.rgb;
		double r = rgb.x;
		double g = rgb.y;
		double b = rgb.z;
		IDrawer->resetColor(Vec4d(r, g, b, 1));

		drawBaseGeom(IDrawer, ftr);
	}
}

void COsgSyn::showAllFtr(const vector<osgFtr> &vecFtr, vector<osgFtr>& vecFtrNotIn)
{
	for (auto ftr : vecFtr)
	{
		string id = ftr.mId;
		MAP_ITER iter = mapNode.find(id);

		if (iter == mapNode.end())
		{
			vecFtrNotIn.push_back(ftr);
			continue;
		}

		ref_ptr<Group> spParent = iter->second;

		for (int i = 0; i < spParent->getNumChildren(); i++)
		{
			ref_ptr<Node> child = spParent->getChild(i);
			if (child)
				child->setNodeMask(1);
		}
	}
}

void COsgSyn::hideAllFtr(const vector<osgFtr> &vecFtr)
{
	for (auto ftr : vecFtr)
	{
		string id = ftr.mId;
		MAP_ITER iter = mapNode.find(id);

		if (iter == mapNode.end())
		{
			continue;
		}

		ref_ptr<Group> spParent = iter->second;

		for (int i = 0; i < spParent->getNumChildren(); i++)
		{
			ref_ptr<Node> child = spParent->getChild(i);
			if (child)
				child->setNodeMask(0x0);
		}
	}
}

void COsgSyn::showLocalFtr(const vector<osgFtr> &vecFtr)
{
	for (auto ftr : vecFtr)
	{
		string id = ftr.mId;
		MAP_ITER iter = mapNode.find(id);

		if (iter == mapNode.end())
		{
			continue;
		}

		if (ftr.isLocal == true)
		{
			ref_ptr<Group> spParent = iter->second;

			for (int i = 0; i < spParent->getNumChildren(); i++)
			{
				ref_ptr<Node> child = spParent->getChild(i);
				if (child)
					child->setNodeMask(1);
			}
		}
		else
		{
			ref_ptr<Group> spParent = iter->second;

			for (int i = 0; i < spParent->getNumChildren(); i++)
			{
				ref_ptr<Node> child = spParent->getChild(i);
				if (child)
					child->setNodeMask(0x0);
			}
		}
	}
}

void COsgSyn::showExternalFtr(const vector<osgFtr> &vecFtr)
{
	for (auto ftr : vecFtr)
	{
		string id = ftr.mId;
		MAP_ITER iter = mapNode.find(id);

		if (iter == mapNode.end())
		{
			continue;
		}

		if (ftr.isLocal == true)
		{
			ref_ptr<Group> spParent = iter->second;

			for (int i = 0; i < spParent->getNumChildren(); i++)
			{
				ref_ptr<Node> child = spParent->getChild(i);
				if (child)
					child->setNodeMask(0x0);
			}
		}
		else
		{
			ref_ptr<Group> spParent = iter->second;

			for (int i = 0; i < spParent->getNumChildren(); i++)
			{
				ref_ptr<Node> child = spParent->getChild(i);
				if (child)
					child->setNodeMask(1);
			}
		}
	}
}

ref_ptr<Node> COsgSyn::drawBaseGeom(const shared_ptr<IBaseDrawer> &IDrawer, const osgFtr &ftr)
{
	const vector<Coord> & coords = ftr.vecCoord;

	//计算平均值，作为偏移量
	Coord ave = calAve(coords);

	Matrix mat;
	mat.setTrans(Vec3(ave.x, ave.y, ave.z));

	ref_ptr<Node> node = nullptr;

	if (ftr.type == CLS_GEOPOINT)
	{
		//只会出现单点
		if (coords.size() == 1)
		{
			Coord coord = coords[0];
			coord -= ave;
			node = IDrawer->drawPt(Vec3d(coord.x, coord.y, coord.z), mat);
		}
	}
	else if (ftr.type == CLS_GEOCURVE)
	{
		vector<Coord> lineCoord = ftr.vecCoord;
		vector<Vec3d> coords;

		for (auto coord : lineCoord)
		{
			coord -= ave;
			coords.push_back(Vec3d(coord.x, coord.y, coord.z));
		}

		node = IDrawer->drawCurLine(coords, mat);
	}
	else if (ftr.type == CLS_GEOSURFACE)
	{
		vector<Vec3d> coords;
		vector<Coord> polyCoord = ftr.vecCoord;

		for (auto coord : polyCoord)
		{
			coord -= ave;
			coords.push_back(Vec3d(coord.x, coord.y, coord.z));
		}
		
		node = IDrawer->drawPolygon(coords, mat);
		//node = IDrawer->drawLineLoop(coords, mat);
	}
	else if (ftr.type == CLS_GEOTEXT)
	{
		vector<Vec3d> coords;
		vector<Coord> textCoord = ftr.vecCoord;
		string text = ftr.str;

		for (auto coord : textCoord)
		{
			coord -= ave;
			coords.push_back(Vec3d(coord.x, coord.y, coord.z));
		}

		if (coords.size() > 0)
		{
			Vec3d pos = coords[0];
			double fontH = ftr.textSize;
			if (ftr.fTextAngle != 0)
			{
				Matrix mat0 = Matrix::identity();
				Matrix mat1 = Matrix::identity();
				Matrix mat2 = Matrix::identity();
				mat0.setTrans(-mat.getTrans());
				mat1.setRotate(Quat(ftr.fTextAngle * PI / 180.f, Vec3d(0, 0, 1)));
				mat2.setTrans(mat.getTrans());
				mat = mat * mat0 * mat1 * mat2;

			}
			node = IDrawer->drawText(text, pos, fontH, ftr.nAlign, mat);
		}
	}

	return node;
}


Coord COsgSyn::calAve(const vector<Coord> & coords)
{
	Coord sum = accumulate(coords.begin(), coords.end(), Coord(0, 0, 0));
	return Coord(sum / coords.size());
}

void mm3dView::COsgSyn::removeFtrByID(const std::vector<std::string> &vecID, osg::ref_ptr<osg::Group> root)
{
	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());

	if (!spChildCallback)
	{
		return;
	}

	for (const string & id : vecID)
	{
		MAP_ITER iter = mapNode.find(id);

		if (iter == mapNode.end())
		{
			continue;
		}

		ref_ptr<Group> spParent = iter->second;

		removeGroupChild(spParent, spChildCallback);

		mapNode.erase(iter);
	}
}
