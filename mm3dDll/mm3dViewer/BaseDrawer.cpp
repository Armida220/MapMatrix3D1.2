// BaseDrawer.cpp : 定义 DLL 应用程序的导出函数。
//
#include "StdAfx.h"
#include "BaseDrawer.h"
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/Point>
#include <osg/LineStipple>
#include <osg/StateSet>
#include "qcomm.h"
#include <osgFX/Outline>
#include <osg/Camera>
#include <osgText/text>
#include <osgUtil/Tessellator>
#include "ThreadSafeUpdateCallback.h"

using namespace osgDraw;
using namespace osg;
using namespace std;
using namespace osgFX;
using namespace osgText;
using namespace mm3dView;

const double ARROWLENGTH = 3.0f;
const double ARROWSIDE = 1.0f;

CSceneDrawer::CSceneDrawer(Vec4d color, double size, Group *drawGroup, Group* root)
	: mClr(color),
	mSize(size),
	mLineSize(size - 2),
	mDrawGroup(drawGroup),
	mRoot(root)
{
}

Node* CSceneDrawer::drawPt(const Vec3d &worldPt, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();
	Vec3d drawPt(worldPt.x() - xyzTrans.x(), worldPt.y() - xyzTrans.y(), worldPt.z() - xyzTrans.z());
	ref_ptr<Geode> geode = drawBasePoint(drawPt);

	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	
	return threadSafeAddNode(geode, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawHightPt(const Vec3d &worldPt, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();
	Vec3d drawPt(worldPt.x() - xyzTrans.x(), worldPt.y() - xyzTrans.y(), worldPt.z() - xyzTrans.z());
	ref_ptr<Geode> geode = drawBasePoint(drawPt);

	//高亮几何点
	highLightGeode(geode);

	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	
	return threadSafeAddNode(geode, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawLine(const Vec3d &worldPt1, const Vec3d &worldPt2, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();
	Vec3d drawPt1(worldPt1.x() - xyzTrans.x(), worldPt1.y() - xyzTrans.y(), worldPt1.z() - xyzTrans.z());

	Vec3d drawPt2(worldPt2.x() - xyzTrans.x(), worldPt2.y() - xyzTrans.y(), worldPt2.z() - xyzTrans.z());
	ref_ptr<Geode> geode = drawBaseLine(drawPt1, drawPt2);

	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);

	return threadSafeAddNode(geode, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawSolid(const vector<Vec3d> &worldCoord, const double height, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();
	vector<Vec3d> coord;

	for (int i = 0; i < worldCoord.size(); i++)
	{
		coord.push_back(Vec3d(worldCoord[i].x() - xyzTrans.x(), worldCoord[i].y() - xyzTrans.y(), worldCoord[i].z() - xyzTrans.z()));
	}

	ref_ptr<Group> groupSolid = drawBaseSolid(coord, height);

	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	
	return threadSafeAddNode(groupSolid, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawFlatPlane(const Vec3d &center, const double radius, const Matrix &worldMatrix)
{
	ref_ptr<Geode> geoFlatPlane = drawBaseFlatPlane(center, radius);
	Vec3d xyzTrans = worldMatrix.getTrans();
	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	
	return threadSafeAddNode(geoFlatPlane, mDrawGroup, drawMatrix);
}


Node* CSceneDrawer::drawLineLoop(vector<Vec3d> &vecWorldCoord, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();

	for (int i = 0; i < vecWorldCoord.size(); i++)
	{
		vecWorldCoord[i] -= xyzTrans;
	}

	ref_ptr<Geode> geodeLineLoop = drawBaseLineLoop(vecWorldCoord);
	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	
	return threadSafeAddNode(geodeLineLoop, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawHighLineLoop(vector<Vec3d> &vecWorldCoord, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();

	for (int i = 0; i < vecWorldCoord.size(); i++)
	{
		vecWorldCoord[i] -= xyzTrans;
	}

	ref_ptr<Geode> geodeLineLoop = drawBaseLineLoop(vecWorldCoord);
	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);

	highLightGeode(geodeLineLoop);

	return threadSafeAddNode(geodeLineLoop, mDrawGroup, drawMatrix);
}


Node* CSceneDrawer::drawCurLine(vector<Vec3d> &vecWorldCoord, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();

	for (int i = 0; i < vecWorldCoord.size(); i++)
	{
		vecWorldCoord[i] -= xyzTrans;
	}

	ref_ptr<Geode> geodeCurLine = drawBaseCurLine(vecWorldCoord);
	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);

	return threadSafeAddNode(geodeCurLine, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawHightCurLine(vector<Vec3d> &vecWorldCoord, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();

	for (int i = 0; i < vecWorldCoord.size(); i++)
	{
		vecWorldCoord[i] -= xyzTrans;
	}

	ref_ptr<Geode> geodeCurLine = drawBaseCurLine(vecWorldCoord);
	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);

	highLightGeode(geodeCurLine);

	return threadSafeAddNode(geodeCurLine, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawCircleLoop(Vec3d &center, const double radius, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();
	center -= xyzTrans;

	ref_ptr<Geode> geodeLineLoop = drawBaseCircleLoop(center, radius);
	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);

	return threadSafeAddNode(geodeLineLoop, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawPolygon(const vector<Vec3d> &vecWorldCoord, const Matrix &worldMatrix)
{
	Vec3d xyzTrans = worldMatrix.getTrans();
	vector<Vec3d> coord;

	for (int i = 0; i < vecWorldCoord.size() - 1; i++)
	{
		coord.push_back(Vec3d(vecWorldCoord[i].x() - xyzTrans.x(), vecWorldCoord[i].y() - xyzTrans.y(), vecWorldCoord[i].z() - xyzTrans.z()));
	}

	ref_ptr<Geode> geode = drawBasePolygon(coord);
	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	
	return threadSafeAddNode(geode, mDrawGroup, drawMatrix);
}

Node* CSceneDrawer::drawArrow(const Vec3 &xyz, double tangent, const Matrix &mat)
{
	Vec3 xyzTrans = mat.getTrans();
	Vec3 xyz1 = xyz - xyzTrans;
	Group* group = drawBaseArrow(xyz1, tangent, mat);

	Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	
	return threadSafeAddNode(group, mDrawGroup, drawMatrix);
}
#define TAH_LFT 0x01
#define TAH_MID	0x02
#define TAH_RGT 0x04

#define TAV_TOP 0x08
#define TAV_MID	0x10
#define TAV_BOT 0x20
const int LFT_TOP = TAH_LFT | TAV_TOP;
const int MID_TOP = TAH_MID | TAV_TOP;
const int RGT_TOP = TAH_RGT | TAV_TOP;
const int LFT_MID = TAH_LFT | TAV_MID;
const int MID_MID = TAH_MID | TAV_MID;
const int RGT_MID = TAH_RGT | TAV_MID;
const int LFT_BOT = TAH_LFT | TAV_BOT;
const int MID_BOT = TAH_MID | TAV_BOT;
const int RGT_BOT = TAH_RGT | TAV_BOT;
Node* CSceneDrawer::drawText(const string &text, const Vec3 &position, double fontH, int nAlign,const Matrix &mat)
{

	ref_ptr<Text> posText = new Text;
	string caiyun("fonts/simhei.ttf");								//此处设置的是汉字字体
	posText->setFont(caiyun);

	posText->setText(AnsiToUnicode(text.c_str()));				//设置显示的文字
	posText->setCharacterSize(fontH);
	ref_ptr<Geode> textGeode = new Geode();
	textGeode->addDrawable(posText);

	Vec3 xyzTrans = mat.getTrans();
	Vec3 pos1 = position - xyzTrans;
	posText->setPosition(pos1);
	posText->setColor(mClr);
	TextBase::AlignmentType ta = TextBase::RIGHT_BOTTOM;

	switch (nAlign)
	{
	case LFT_TOP:ta = TextBase::LEFT_TOP; break;
	case MID_TOP:ta = TextBase::CENTER_TOP; break;
	case RGT_TOP:ta = TextBase::RIGHT_TOP; break;
	case LFT_MID:ta = TextBase::LEFT_CENTER; break;
	case MID_MID:ta = TextBase::CENTER_CENTER; break;
	case RGT_MID:ta = TextBase::RIGHT_CENTER; break;
	case LFT_BOT:ta = TextBase::LEFT_BOTTOM; break;
	case MID_BOT:ta = TextBase::CENTER_BOTTOM; break;
	case RGT_BOT:ta = TextBase::RIGHT_BOTTOM; break;
	default:
		break;
	}

	posText->setAlignment(ta);
	
	//posText->setAutoRotateToScreen(true);
	ref_ptr<StateSet> stateset = posText->getOrCreateStateSet();
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	
	Matrix drawMatrix = mat;
	//drawMatrix.setTrans(xyzTrans);

	return threadSafeAddNode(textGeode, mDrawGroup, drawMatrix);
}

void CSceneDrawer::deleteVecChild(int childIndex, vector<Vec3d> &vecGroup)
{
	int childNum = vecGroup.size();

	if (childNum == 0)
	{
		return;
	}

	vector<Vec3d>::iterator iter = vecGroup.begin() + childNum - 1;
	vecGroup.erase(iter);
}


void CSceneDrawer::clearGroupChild(Group* group)
{
	while (group->getNumChildren() > 0)
	{
		ref_ptr<Group> childGroup = group->getChild(0)->asGroup();
		//如果是Group，先删除其子节点，再删除自身
		if (childGroup)
		{
			while (childGroup->getNumChildren() > 0)
				childGroup->removeChildren(0, 1);
		}

		group->removeChildren(0, 1);
	}
}


void CSceneDrawer::addDraggerToScene(Group* mRoot, Group* tempGroup, Group* scopeGroup)
{
	//Geode* pGeoSphereDragger= new Geode();
	//ref_ptr<osgManipulator::CommandManager> cmdMgr = new osgManipulator::CommandManager;
	//Node* scene = pGeoSphereDragger;
	//scene->getOrCreateStateSet()->setMode(GL_NORMALIZE, StateAttribute::ON);
	//osgManipulator::Selection* selection = new osgManipulator::Selection;
	//selection->addChild(scene);
	//string name =" ";
	//osgManipulator::Dragger* dragger = createDragger(scopeGroup);
	//dragger->setName( name);
	//Group* root = new Group;
	//root->addChild(dragger);
	//root->addChild(selection);
	//cmdMgr->connect(*dragger, *selection);
	//ref_ptr<CThreadSafeUpdateCallback> spCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
	//// Add the model to the scene
	//if (spCallback != NULL)
	//{
	//    spCallback->AddChild(tempGroup, root);
	//    spCallback->AddChild(mRoot, tempGroup);
	//}
}

void CSceneDrawer::resetColor(Vec4d clr)
{
	mClr = clr;
}

void CSceneDrawer::resetSize(double size)
{
	mSize = size;
}

Vec4d CSceneDrawer::getColor()
{
	return mClr;
}

//protect:

Geode* CSceneDrawer::drawBasePoint(Vec3d pt)
{
	//绘制点
	Geode* geode = new Geode();
	ref_ptr<Geometry> geomPt = new Geometry();
	//设定点
	ref_ptr<Vec3Array> ptArray = new Vec3Array();
	ptArray->push_back(pt);
	geomPt->setVertexArray(ptArray);
	//设定颜色
	ref_ptr<Vec4Array> colorArray = new Vec4Array();
	colorArray->push_back(mClr);
	geomPt->setColorArray(colorArray);
	geomPt->setColorBinding(Geometry::BIND_PER_VERTEX);

	//设定点样式
	ref_ptr<StateSet> stateSet = makePtState(mSize);
	geomPt->setStateSet(stateSet);

	ref_ptr<DrawArrays> drawArray = new DrawArrays(PrimitiveSet::POINTS, 0, ptArray->size());
	//添加几何节点
	geomPt->addPrimitiveSet(drawArray);
	geode->addDrawable(geomPt);

	//设置类型
	geode->setName("point");
	return geode;
}

Geode* CSceneDrawer::drawBaseLine(Vec3d pt1, Vec3d pt2)
{
	//绘制线
	Geode* geoLine = new Geode();
	ref_ptr<Geometry> geomLine = new Geometry();
	ref_ptr<Vec4Array> lineColor = new Vec4Array();
	ref_ptr<Vec3Array> lineArray = new Vec3Array();

	lineArray->push_back(pt1);
	lineArray->push_back(pt2);
	lineColor->push_back(mClr);
	geomLine->setVertexArray(lineArray);
	geomLine->setColorArray(lineColor);
	geomLine->setColorBinding(Geometry::BIND_OVERALL);
	geomLine->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINES, 0, 2));
	geoLine->addDrawable(geomLine);

	//设置线样式
	geoLine->setStateSet(makeLineState());

	//设置线宽
	ref_ptr <LineWidth> LineSize = new LineWidth;
	LineSize->setWidth(mLineSize);
	geomLine->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(), StateAttribute::ON);
	return geoLine;
}

Geode* CSceneDrawer::drawBasePolygon(vector<Vec3d> _coord)
{
	Geode* geode = new Geode();
	ref_ptr<Geometry> geom = new Geometry();
	ref_ptr<Vec4Array> plygonColor = new Vec4Array();
	ref_ptr<Vec3Array> plygonArray = new Vec3Array();
	vector<Vec3d> coord;
	coord.reserve(_coord.size());
	coord.push_back(_coord[0]);
	for (int i = 1; i < _coord.size(); i++)
	{
		if (coord.back() == _coord[i]) continue; 
		coord.push_back(_coord[i]);
	}


	struct _se
	{
		int s;
		int e;
	};
	vector<_se> poly_part;
	poly_part.push_back({ 0, -1 });
	for (int i = 0; i < coord.size(); i++)
	{
		plygonArray->push_back(coord[i]);
		if (i > 0)
		{
			if (poly_part.back().e != -1)
			{
				poly_part.push_back({ i, -1 });
			}
			else if (coord[poly_part.back().s] == coord[i])
			{
				poly_part.back().e = i;
			}
		}
	}
	if (poly_part.back().e == -1) poly_part.back().e = coord.size();

	plygonColor->push_back(mClr);
	geom->setVertexArray(plygonArray);
	geom->setColorArray(plygonColor);
	geom->setColorBinding(Geometry::BIND_OVERALL);
	
	for (_se & se : poly_part)
	{


// 		DrawElementsUInt* geomBase = new DrawElementsUInt(PrimitiveSet::POLYGON, 1);
// 
// 		for (int i = se.s; i <se.e; i++)
// 		{
// 			geomBase->push_back(i);
// 		}

		//geom->addPrimitiveSet(geomBase);
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, se.s, se.e - se.s));
	}
	{
		osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator();
		tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
		tscx->setBoundaryOnly(false);
		tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_NONZERO);
		tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_POSITIVE);
		tscx->retessellatePolygons(*geom);
	}
	geode->addDrawable(geom);
	ref_ptr<StateSet> stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, StateAttribute::ON);
	stateset->setRenderingHint(StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);
	geode->setStateSet(stateset);

	return geode;
}

Geode* CSceneDrawer::drawBaseLineLoop(vector<Vec3d> coord)
{
	//绘制线
	Geode* geoLineLoop = new Geode();
	ref_ptr<Geometry> geomLineLoop = new Geometry();
	ref_ptr<Vec4Array> lineLoopColor = new Vec4Array();
	ref_ptr<Vec3Array> lineLoopArray = new Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		lineLoopArray->push_back(coord[i]);
	}

	lineLoopColor->push_back(mClr);
	geomLineLoop->setVertexArray(lineLoopArray);
	geomLineLoop->setColorArray(lineLoopColor);
	geomLineLoop->setColorBinding(Geometry::BIND_OVERALL);
	geomLineLoop->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINE_LOOP, 0, lineLoopArray->size()));
	geoLineLoop->addDrawable(geomLineLoop);

	//设置线样式
	geoLineLoop->setStateSet(makeLineState());

	//设置线宽
	ref_ptr<LineWidth> LineSize = new LineWidth;
	LineSize->setWidth(mLineSize);
	geomLineLoop->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(), StateAttribute::ON);
	return geoLineLoop;
}

Geode* CSceneDrawer::drawBaseCurLine(vector<Vec3d> coord)
{
	//绘制线
	Geode* geoCurLine = new Geode();
	ref_ptr<Geometry> geomCurLine = new Geometry();
	ref_ptr<Vec4Array> curLineColor = new Vec4Array();
	ref_ptr<Vec3Array> curLineArray = new Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		curLineArray->push_back(coord[i]);
	}

	curLineColor->push_back(mClr);
	geomCurLine->setVertexArray(curLineArray);
	geomCurLine->setColorArray(curLineColor);
	geomCurLine->setColorBinding(Geometry::BIND_OVERALL);
	geomCurLine->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINE_STRIP, 0, curLineArray->size()));
	geoCurLine->addDrawable(geomCurLine);

	//设置线样式
	geoCurLine->setStateSet(makeLineState());

	//设置线宽
	ref_ptr <LineWidth> LineSize = new LineWidth;
	LineSize->setWidth(mLineSize);
	geomCurLine->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(), StateAttribute::ON);
	return geoCurLine;
}

Group* CSceneDrawer::drawBaseSolid(vector<Vec3d> coord, double height)
{
	//画第一个面
	Group* group = new Group;
	ref_ptr<Geode> geode1 = drawBasePolygon(coord);
	group->addChild(geode1);
	vector<Vec3d> coordUp;

	for (int i = 0; i < coord.size(); i++)
	{
		coordUp.push_back(Vec3d(coord[i].x(), coord[i].y(), coord[i].z() + height));
	}

	//画第二个面
	ref_ptr<Geode> geode2 = drawBasePolygon(coordUp);
	group->addChild(geode2);

	//画第三个面 到最后
	for (int i = 0; i < coord.size(); i++)
	{
		int index1 = i;
		int index2 = i + 1;

		if (index1 == coord.size() - 1)
		{
			index2 = 0;
		}

		vector<Vec3d> coordSideFace;
		coordSideFace.push_back(coord[index1]);
		coordSideFace.push_back(coord[index2]);
		coordSideFace.push_back(coordUp[index2]);
		coordSideFace.push_back(coordUp[index1]);
		ref_ptr<Geode> geode3 = drawBasePolygon(coordSideFace);
		group->addChild(geode3);
	}

	return group;
}

Geode* CSceneDrawer::drawBaseFlatPlane(Vec3d center, double radius)
{
	double angle = 0;
	double PI = 3.14159f;
	vector<Vec3d> coordCircle;

	for (int i = 0; i < 72; i++)
	{
		double circle[72][2] = { 0 };
		circle[i][0] = radius * cos(angle) + center.x();
		circle[i][1] = radius * sin(angle) + center.y();
		angle += 2.0 * PI / 72.0f;
		coordCircle.push_back(Vec3d(circle[i][0], circle[i][1], center.z()));
	}

	Geode* geoPolygon = drawBasePolygon(coordCircle);
	//设置半透明
	ref_ptr<BlendFunc> blendFunc = new BlendFunc();  // blend func    
	blendFunc->setSource(BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(BlendFunc::ONE_MINUS_SRC_ALPHA);
	ref_ptr<StateSet> stateset = geoPolygon->getOrCreateStateSet();
	stateset->setAttributeAndModes(blendFunc);
	stateset->setRenderingHint(StateSet::TRANSPARENT_BIN);
	return geoPolygon;
}

Geode* CSceneDrawer::drawBaseCircleLoop(Vec3d center, double radius)
{
	double angle = 0;
	double PI = 3.14159f;
	vector<Vec3d> coordCircle;

	for (int i = 0; i < 72; i++)
	{
		double circle[72][2] = { 0 };
		circle[i][0] = radius * cos(angle) + center.x();
		circle[i][1] = radius * sin(angle) + center.y();
		angle += 2.0 * PI / 72.0f;
		coordCircle.push_back(Vec3d(circle[i][0], circle[i][1], center.z()));
	}

	Geode* geoCircleLoop = drawBaseLineLoop(coordCircle);
	//设置半透明
	ref_ptr<BlendFunc> blendFunc = new BlendFunc();  // blend func    
	blendFunc->setSource(BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(BlendFunc::ONE_MINUS_SRC_ALPHA);
	ref_ptr<StateSet> stateset = geoCircleLoop->getOrCreateStateSet();
	stateset->setAttributeAndModes(blendFunc);
	stateset->setRenderingHint(StateSet::TRANSPARENT_BIN);
	return geoCircleLoop;
}

Group* CSceneDrawer::drawBaseArrow(const Vec3 &xyz, double tangent, const Matrix &mat)
{
	Group* group = new Group;
	double ang = tangent;

	double cosa = cos(ang), sina = sin(ang);

	Vec3d xyz1;
	xyz1.x() = xyz.x() + cosa * ARROWLENGTH;
	xyz1.y() = xyz.y() + sina * ARROWLENGTH;
	xyz1.z() = xyz.z();

	Geode* geoLine = drawBaseLine(xyz, xyz1);
	ang -= PI;
	group->addChild(geoLine);

	{
		cosa = cos(ang - PI / 6);
		sina = sin(ang - PI / 6);
		Vec3d xyz2;
		xyz2.x() = xyz1.x() + cosa * ARROWSIDE;
		xyz2.y() = xyz1.y() + sina * ARROWSIDE;
		xyz2.z() = xyz1.z();
		Geode* geoLine1 = drawBaseLine(xyz1, xyz2);
		group->addChild(geoLine1);
	}

	{
		cosa = cos(ang + PI / 6);
		sina = sin(ang + PI / 6);
		Vec3d xyz3;
		xyz3.x() = xyz1.x() + cosa * ARROWSIDE;
		xyz3.y() = xyz1.y() + sina * ARROWSIDE;
		xyz3.z() = xyz1.z();
		Geode* geoLine2 = drawBaseLine(xyz1, xyz3);
		group->addChild(geoLine2);
	}

	return group;
}

Node* CSceneDrawer::addNodeChildToGroup(Node* nodeChild, Group* parent, Matrix matrix)
{
	ref_ptr<MatrixTransform> trans = new MatrixTransform();
	trans->setMatrix(matrix);
	trans->addChild(nodeChild);
	parent->addChild(trans);

	return nodeChild;
}

Node* CSceneDrawer::threadSafeAddNode(Node* nodeChild, Group* parent, Matrix matrix)
{
	ref_ptr<MatrixTransform> trans = new MatrixTransform();
	trans->setMatrix(matrix);
	trans->addChild(nodeChild);

	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spChildCallback)
	{
		spChildCallback->AddChild(parent, trans);
	}

	return trans;
}

StateSet* CSceneDrawer::makePtState(int size)
{
	//设置样式
	StateSet* set = new StateSet();
	ref_ptr<Point> point = new Point();
	point->setSize(size);
	set->setAttribute(point);
	set->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	set->setMode(GL_LIGHTING, StateAttribute::OFF);
	return set;
}

StateSet* CSceneDrawer::makeLineState()
{
	//设置样式
	StateSet* stateset = new StateSet();
	ref_ptr<LineStipple> linestipple = new LineStipple;
	linestipple->setFactor(1);
	stateset->setAttributeAndModes(linestipple, StateAttribute::OVERRIDE | StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);
	return stateset;
}

StateSet* CSceneDrawer::makeLineStateNT()
{
	//设置样式
	StateSet* stateset = new StateSet();
	ref_ptr<LineStipple> linestipple = new LineStipple;
	linestipple->setFactor(1);
	stateset->setAttributeAndModes(linestipple, StateAttribute::OVERRIDE | StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);
	return stateset;
}

Geode* CSceneDrawer::drawBasePolygonNT(vector<Vec3d> coord)
{
	Geode* geode = new Geode();
	ref_ptr<Geometry> geom = new Geometry();
	ref_ptr<Vec4Array> plygonColor = new Vec4Array();
	ref_ptr<Vec3Array> plygonArray = new Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		plygonArray->push_back(coord[i]);
	}

	plygonColor->push_back(mClr);
	geom->setVertexArray(plygonArray);
	geom->setColorArray(plygonColor);
	geom->setColorBinding(Geometry::BIND_OVERALL);

	DrawElementsUInt* geomBase = new DrawElementsUInt(PrimitiveSet::POLYGON, 1);

	for (int i = 0; i < coord.size(); i++)
	{
		geomBase->push_back(i);
	}

	geom->addPrimitiveSet(geomBase);
	geode->addDrawable(geom);
	geode->setStateSet(makeLineStateNT());

	return geode;
}

//osgManipulator::Dragger* CSceneDrawer::createDragger(Group* group)
//{
//	osgManipulator::Dragger* dragger = 0;
//	CDraggerPoint* d = new CDraggerPoint(group);
//	d->setScence();
//	dragger = d;
//	return dragger;
//}

void CSceneDrawer::highLightGeode(Geode *geode)
{
	int numChild = geode->getNumChildren();

	for (int i = 0; i < numChild; i++)
	{
		ref_ptr<Geometry> geom = geode->getChild(i)->asGeometry();

		if (geom)
		{
			Vec4dArray* colorArray = new Vec4dArray;
			colorArray->push_back(Vec4d(0.f, 1.f, 1.f, 0.f));
			geom->setColorArray(colorArray);
			geom->setColorBinding(Geometry::BIND_OVERALL);
		}
	}
}

void CSceneDrawer::unicodeToUTF8(const wstring &src, string& result)
{
	int n = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, 0, 0, 0, 0);
	result.resize(n);
	::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, (char*)result.c_str(), result.length(), 0, 0);
}


void CSceneDrawer::gb2312ToUnicode(const string& src, wstring& result)
{
	int n = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
	result.resize(n);
	::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, (LPWSTR)result.c_str(), result.length());
}

void CSceneDrawer::gb2312ToUtf8(const string& src, string& result)
{
	wstring strWideChar;
	gb2312ToUnicode(src, strWideChar);
	unicodeToUTF8(strWideChar, result);
}


wchar_t* CSceneDrawer::AnsiToUnicode(const char* szStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)   { return NULL; }
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}