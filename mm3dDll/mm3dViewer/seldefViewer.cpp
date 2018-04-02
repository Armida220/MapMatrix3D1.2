#include "stdafx.h"
#include "seldefViewer.h"
#include <osgGA/TrackballManipulator>
#include <osgDB/Registry>
#include "qcomm.h"
#include <mutex>

using namespace osgGA;
using namespace osg;
using namespace mm3dView;

CSelDefViewer::CSelDefViewer()
{
}


CSelDefViewer::~CSelDefViewer()
{
}




void CSelDefViewer::frame(double simulationTime)
{
	if (_done) return;

	// OSG_NOTICE<<std::endl<<"CompositeViewer::frame()"<<std::endl<<std::endl;

	if (_firstFrame)
	{
		viewerInit();

		if (!isRealized())
		{
			realize();
		}

		_firstFrame = false;
	}
	advance(simulationTime);

	eventTraversal();

	updateTraversal();
	renderingTraversals();
}
