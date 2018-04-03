#include "stdafx.h"
#include <stdio.h>  
#include <osg/Geode>  
#include <osg/Geometry>  
#include <osg/MatrixTransform>  
#include <osg/LineWidth>  
#include <osgText/Text>  
#include <osgUtil/CullVisitor>  
#include <osg/ClipPlane>  
#include "Compass.h"

osg::Vec4 color(0.8, 0.8, 0, 1.0);

Compass::Compass(osg::Viewport *vp)
{
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;

	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	stateSet->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OFF);
	stateSet->setRenderBinDetails(110, "RenderBin");

	stateSet->setAttribute(vp);

	setMatrix(osg::Matrix::frustum(-1, 1, -1, 1, 1.0, 10000.0));

	stateSet->setAttributeAndModes(new osg::LineWidth(1.5));

	setStateSet(stateSet.get());
	setCullingActive(false);

	_tx = new osg::MatrixTransform;
	_tx->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	_tx->addChild(_makeGeode().get());
	addChild(_tx.get());

}

osg::ref_ptr<osg::Geode> Compass::_makeGeode()
{
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	for (int i = 0; i < 360; i += 45)
	{
		double a = osg::DegreesToRadians(double(i-180));
		double sa = sin(-a);
		double ca = cos(-a);
		coords->push_back(osg::Vec3(0.05 * sa, 0.05 * ca, 0.0));
		if (!(i % 10))
			coords->push_back(osg::Vec3(sa * 0.4, ca * 0.4, 0));
		else
			coords->push_back(osg::Vec3(sa * 0.3, ca * 0.3, 0));

		if (!(i % 90))
		{
			osg::ref_ptr<osgText::Text> text = new osgText::Text;
			text->setFont("fonts/simhei.ttf");
			text->setAlignment(osgText::Text::CENTER_CENTER);
			text->setFontResolution(100, 100);
			text->setCharacterSize(0.3);
			text->setColor(color);

			osg::Matrix m = //osg::Matrix::rotate(osg::PI*0.5, 1, 0, 0) *
				osg::Matrix::rotate(a, 0, 0, 1);
			osg::Quat q;
			q.set(m);
			text->setRotation(q);

			char buff[8];

			    (i == 0) ?   sprintf(buff, "S") :
				(i == 90) ?  sprintf(buff, "E") :
				(i == 180) ? sprintf(buff, "N") :
				(i == 270) ? sprintf(buff, "W") : sprintf(buff, "%2d", (((360 - i) % 360) / 10));

			text->setText(buff);
			text->setPosition(osg::Vec3(0.5 * sa, 0.5 * ca, 0));

			geode->addDrawable(text.get());
		}
	}

	colors->push_back(color);

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geometry->setVertexArray(coords.get());
	geometry->setColorArray(colors.get());
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, coords->size()));

	//geometry->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( 4.0 ));  
	geode->addDrawable(geometry.get());

	return geode;
}


void Compass::traverse(osg::NodeVisitor&nv)
{
	if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
	{
		osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor *>(&nv);
		osg::Matrix ivm = cv->getState()->getInitialViewMatrix();
		ivm(3, 0) = ivm(3, 1) = ivm(3, 2) = 0.0;
		osg::Vec3 v = osg::Vec3(0, 1, 0) * ivm;// *osg::Matrix::rotate(osg::PI*0.5, 1, 0, 0);
		double a = atan2(v[0], v[1]);
		double b = atan2(hypot(v[0],v[1]), v[2]);
		_tx->setMatrix(ivm *
			osg::Matrix::lookAt(osg::Vec3(0, 0, 1), osg::Vec3(0, 0, 0), osg::Vec3(0, 1, 0))
			);

		//osg::Vec4d cp = osg::Vec4d(0.0, -1.0, 0.0, -0.75) * osg::Matrix::rotate(a, 0, 0, 1);
		//_clipPlane->setClipPlane(cp);

		//_ltx->setMatrix(osg::Matrix::rotate(a, 0, 0, 1));


	}
	osg::Projection::traverse(nv);
}