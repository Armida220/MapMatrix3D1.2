#include "StdAfx.h"
#include "IOsgEvent.h"
#include "osgEvent.h"
#include "ICallBack.h"

using namespace osgEvent;
using namespace osgCall;
using namespace osg;
using namespace std;
using namespace osgViewer;
using namespace mm3dView;


shared_ptr<IOsgEvent> IOsgEventFactory::create(Viewer *view, COsgModel* pModel,
	shared_ptr<ICallBack> ic)
{
	ptrDrawEvent ptr(new COsgEvent(view, pModel, ic));
	return ptr;
}

