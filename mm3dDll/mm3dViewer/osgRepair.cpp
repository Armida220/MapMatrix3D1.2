#include "StdAfx.h"
#include "osgRepair.h"
#include "SceneRepairor.h"

using namespace osg;
using namespace std;

ref_ptr<Node> osgRepair::createRepairPatch(vector<Vec3d> vecWorldCoord, Matrix worldMatrix, string imgFileName)
{
	CSceneRepairor repairor;
	ref_ptr<Node> node = repairor.createInterpolateRepairPatch(vecWorldCoord, worldMatrix, imgFileName);



	return node;
}
