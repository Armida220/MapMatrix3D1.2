#include "stdafx.h"
#include "IsInAreaJudger.h"
#include "ObtainSelTileFileName.h"
#include "Geometry.h"
#include "OsgVertextExtract.h"
#include <osgDB/WriteFile>
#include <osg/PagedLOD>
#include <osg/MatrixTransform>
#include "ScriptExecutor.h"

using namespace osg;
using namespace osgDB;
using namespace std;

CObtainSelTileFileName::CObtainSelTileFileName(vector<Vec3d> area) 
{
	vector<CL::Vec2d> pol;

	for (int i = 0; i < area.size(); i++)
	{
		pol.push_back(CL::Vec2d(area[i].x(), area[i].y()));
	}
	
	pol.push_back(CL::Vec2d(area[0].x(), area[0].y()));
	shared_ptr<CIsInAreaJudger> isInAreaJudger(new CIsInAreaJudger(pol));
	mIsInAreaJudger = isInAreaJudger;
}

void CObtainSelTileFileName::obtainSelTileName(ref_ptr<Group> mGroup, Matrix matrix)
{
	//根据范围获取所有的tile目录
	int groupNum = mGroup->getNumChildren();

	//根据多边形范围筛选
	int pos = 0;

	for (int i = groupNum - 1; i >= 0; i--)
	{
		ref_ptr<PagedLOD> pagedLod = dynamic_cast<PagedLOD*>(mGroup->getChild(i));

		if (!pagedLod)
		{
			continue;
		}

		//获取所有的节点

		COsgVertextExtract ive;
		pagedLod->accept(ive);
		ref_ptr<Vec3Array> extracted_verts = ive.extracted_verts;

		if (extracted_verts->size() == 0)
		{
			continue;
		}

		string fileName = pagedLod->getFileName(1);
		int innerCnt = 0;

		for (int j = 0; j < extracted_verts->size(); j++)
		{
			Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
			Vec3d pt1 = pt * matrix;

			bool bIsInPolygon = mIsInAreaJudger->isPtInArea(pt1);

			if (bIsInPolygon)
			{
				innerCnt++;
			}
		}

		if (innerCnt > 0 && innerCnt < extracted_verts->size())
		{
			vecHalfInFileName.push_back(fileName);
		}
		else if (innerCnt == extracted_verts->size())
		{
			vecTotalInFileName.push_back(fileName);
		}

		int percent = (int)((double)(pos + 1) / (double)groupNum * 15);

		string script = "setProgressVal(" + to_string(percent) + ")";
		CScriptExecutor::getInstance()->excuteScript(script);
	}

	return;
}
