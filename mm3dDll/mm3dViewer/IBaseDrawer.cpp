#include "StdAfx.h"
#include "IBaseDrawer.h"
#include "BaseDrawer.h"

using namespace osgDraw;
using namespace std;
using namespace osg;

shared_ptr<IBaseDrawer> IBaseDrawerFactory::create(Vec4d color, double size, Group *drawGroup, Group* root)
{
	shared_ptr<IBaseDrawer> ptrDrawer(new CSceneDrawer(color, size, drawGroup, root));
	return ptrDrawer;
}

