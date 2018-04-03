#include "stdafx.h"
#include "SelfStateManipulator.h"
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>

using namespace osgGA;
using namespace mm3dView;

CSelfStateManipulator::CSelfStateManipulator(osg::StateSet* stateset)
	: StateSetManipulator(stateset)
{
}


CSelfStateManipulator::~CSelfStateManipulator()
{
}

bool CSelfStateManipulator::handle(const GUIEventAdapter& ea, GUIActionAdapter& aa)
{
	if (!_stateset.valid()) return false;

	if (!_initialized)
	{
		_initialized = true;
		_backface = (_stateset->getMode(GL_CULL_FACE)&osg::StateAttribute::ON);
		_lighting = (_stateset->getMode(GL_LIGHTING)&osg::StateAttribute::ON);

		unsigned int mode = osg::StateAttribute::INHERIT | osg::StateAttribute::ON;

		_texture = (_stateset->getTextureMode(0, GL_TEXTURE_2D)&mode) != 0 ||
			(_stateset->getTextureMode(0, GL_TEXTURE_3D)&mode) != 0 ||
			(_stateset->getTextureMode(0, GL_TEXTURE_RECTANGLE)&mode) != 0 ||
			(_stateset->getTextureMode(0, GL_TEXTURE_CUBE_MAP)&mode) != 0;

#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
		_texture |= ((_stateset->getTextureMode(0, GL_TEXTURE_1D)&mode) != 0);
#endif
	}

	if (ea.getHandled()) return false;

	return false;
}


void CSelfStateManipulator::toggleTexture()
{
	setTextureEnabled(!getTextureEnabled());
}

void CSelfStateManipulator::light()
{
	//setLightingEnabled(!getLightingEnabled());
	//防止多次调用打开光照
	setLightingEnabled(false);
}