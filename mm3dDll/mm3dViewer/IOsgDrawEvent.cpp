#include "StdAfx.h"
#include "IOsgDrawEvent.h"
#include "osgDrawEvent.h"
#include "ICallBack.h"

using namespace osgEvent;
using namespace osgCall;
using namespace osg;
using namespace std;
using namespace osgViewer;
using namespace mm3dView;


shared_ptr<IOsgDrawEvent> IOsgDrawEventFactory::create(Viewer *view, COsgModel* pModel,
	shared_ptr<ICallBack> ic)
{
	ptrDrawEvent ptr(new COsgDrawEvent(view, pModel, ic));
	return ptr;
}

