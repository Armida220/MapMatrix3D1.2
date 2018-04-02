#include "StdAfx.h"
#include "SelfDefTerrainManipulator.h"
#include <osg/Vec3d>
#include <osg/CoordinateSystemNode>
#include "qcomm.h"
#include "ICallBack.h"
#include "ThreadSafeUpdateCallback.h"
#include "osg/LineSegment"
#include <osgUtil/PlaneIntersector>

using namespace osgGA;
using namespace osg;
using namespace mm3dView;
using namespace osgCall;
using namespace std;
using namespace osgUtil;
using namespace chrono;
using namespace std::chrono;



CSelfDefTerrainManipulator::CSelfDefTerrainManipulator(ref_ptr<Camera> camera, shared_ptr<ICallBack> pCall)
	:mCamera(camera),
	bMidButDown(false),
	bShitRotate(false),
	iCall(pCall)
{
}

void CSelfDefTerrainManipulator::topView()
{
	Vec3d eye, center, up;
	mCamera->getViewMatrixAsLookAt(eye, center, up);
	center = getCenter();

	eye.x() = center.x();
	eye.y() = center.y();
	setTransformation(eye, center, up);
}

void CSelfDefTerrainManipulator::leftView()
{
	Vec3d eye, center, up;
	mCamera->getViewMatrixAsLookAt(eye, center, up);
	center = getCenter();

	//setLookAt(eye, center, localUp);
	Quat quat;
	quat.makeRotate(-PI / 2, Vec3d(0, 0, 1));
	Matrixd mat;
	mat.setRotate(quat);
	mat.setTrans(Vec3(center.x(), center.y(), 0));

	Matrixd rot;
	rot.setRotate(_rotation);

	rot = rot * mat;
	_rotation = rot.getRotate();
}


void CSelfDefTerrainManipulator::rightView()
{
	Vec3d eye, center, up;
	mCamera->getViewMatrixAsLookAt(eye, center, up);
	center = getCenter();

	//setLookAt(eye, center, localUp);
	Quat quat;
	quat.makeRotate(PI / 2, Vec3d(0, 0, 1));
	Matrixd mat;
	mat.setRotate(quat);
	mat.setTrans(Vec3(center.x(), center.y(), 0));

	Matrixd rot;
	rot.setRotate(_rotation);

	rot = rot * mat;
	_rotation = rot.getRotate();
}


Vec3d CSelfDefTerrainManipulator::getSide(const Matrix& mat)
{
	return getSideVector(mat);
}


Vec3d CSelfDefTerrainManipulator::getFront(const Matrix& mat)
{
	return getFrontVector(mat);
}

double CSelfDefTerrainManipulator::getScale()
{
	Vec3d eye, center, up;
	getTransformation(eye, center, up);
	double dist = abs(eye.z() - center.z());
	Viewport *vp = mCamera->getViewport();

	if (vp) 
	{
		int width = vp->width();
		int height = vp->height();

		double sceneWidth = 2 * dist * tan(PI / 12);
		double zoomRate = (double)width / sceneWidth;
		return zoomRate;
	}
	else
	{
		return 1;
	}
}

double CSelfDefTerrainManipulator::getScaleFromDist()
{
	return abs(0.006 * 0.3 * _distance);
}

Matrix CSelfDefTerrainManipulator::getScreen2WorldMat()
{
	Matrix viewMat = mCamera->getViewMatrix();
	Matrix prjMat = mCamera->getProjectionMatrix();
	Matrix windowsMat = mCamera->getViewport()->computeWindowMatrix();

	Matrix mVPW = mCamera->getViewMatrix() * mCamera->getProjectionMatrix() * mCamera->getViewport()->computeWindowMatrix();
	Matrix invertVPW;
	invertVPW.invert(mVPW);
	return mVPW;
}

/** Handles events. Returns true if handled, false otherwise.*/
bool CSelfDefTerrainManipulator::handle(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	switch (ea.getEventType())
	{
	case GUIEventAdapter::FRAME:
		return handleFrame(ea, us);

	case GUIEventAdapter::RESIZE:
		return handleResize(ea, us);

	default:
		break;
	}

	if (ea.getHandled())
		return false;

	switch (ea.getEventType())
	{
		case GUIEventAdapter::MOVE:
		{				
			return handleMouseMove(ea, us);	
		}
		case GUIEventAdapter::DRAG:
		{
			return handleMouseDrag(ea, us);
		}
		case GUIEventAdapter::PUSH:
		{
			return handleMousePush(ea, us);
		}
		case GUIEventAdapter::RELEASE:
		{
			return handleMouseRelease(ea, us);
		}
		case GUIEventAdapter::KEYDOWN:
			return handleKeyDown(ea, us);

		case GUIEventAdapter::KEYUP:
			return handleKeyUp(ea, us);

		case GUIEventAdapter::SCROLL:
			if (_flags & PROCESS_MOUSE_WHEEL)
				return handleMouseWheel(ea, us);
			else
				return false;

		default:
			return false;
	}
}

/// Handles GUIEventAdapter::KEYDOWN event.
bool CSelfDefTerrainManipulator::handleKeyDown(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	return false;
}

bool CSelfDefTerrainManipulator::handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
	{
		//前进
		double dx = 0;
		double dy = 1;
		changePosition(dx, dy);
	}
	else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
	{
		double dx = 0;
		double dy = -1;
		changePosition(dx, dy);
	}
	else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
	{
		double dx = -1;
		double dy = 0;
		changePosition(dx, dy);
	}
	else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
	{
		double dx = 1;
		double dy = 0;
		changePosition(1.0, 0);
	}
// 	else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_6)
// 	{
// 		shiftOperation();
// 	}

	return false;
}



// doc in parent
bool CSelfDefTerrainManipulator::handleMouseWheel(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	//进行平移时候不允许旋转
	if (bMidButDown)
	{
		return false;
	}	

	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

	// handle centering
	if (_flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT)
	{

		if (((sm == GUIEventAdapter::SCROLL_DOWN && _wheelZoomFactor > 0.)) ||
			((sm == GUIEventAdapter::SCROLL_UP   && _wheelZoomFactor < 0.)))
		{

			if (getAnimationTime() <= 0.)
			{
				// center by mouse intersection (no animation)
				setCenterByMousePointerIntersection(ea, us);
			}
			else
			{
				// start new animation only if there is no animation in progress
				if (!isAnimating())
					startAnimationByMousePointerIntersection(ea, us);

			}

		}
	}

	switch (sm)
	{
		// mouse scroll up event
	case GUIEventAdapter::SCROLL_UP:
	{
		// perform zoom
		subZoomModel(-_wheelZoomFactor, ea, us, true);
		us.requestRedraw();
		us.requestContinuousUpdate(isAnimating() || _thrown);
		return true;
	}

	// mouse scroll down event
	case GUIEventAdapter::SCROLL_DOWN:
	{
		// perform zoom
		subZoomModel(_wheelZoomFactor, ea, us, true);
		us.requestRedraw();
		us.requestContinuousUpdate(isAnimating() || _thrown);

		return true;
	}

	// unhandled mouse scrolling motion
	default:
		return false;
	}
}

bool CSelfDefTerrainManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	if (bShitRotate)
	{
		//处于按下状态才可以
		if (GetKeyState(VK_SHIFT) >= 0)
		{
			return false;
		}
	}

	//设置已经处理了
	// rotate camera
	if (getVerticalAxisFixed())
		rotateWithFixedVertical(dx, dy);
	else
		rotateTrackball(_ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
		_ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
		getThrowScale(eventTimeDelta));
	return true;
}


bool CSelfDefTerrainManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	// pan model.
	double scale = -0.3f * _distance * getThrowScale(eventTimeDelta);

	Matrixd rotation_matrix;
	rotation_matrix.makeRotate(_rotation);


	// compute look vector.
	Vec3d sideVector = getSideVector(rotation_matrix);

	// CoordinateFrame coordinateFrame = getCoordinateFrame(_center);
	// Vec3d localUp = getUpVector(coordinateFrame);
	Vec3d localUp = _previousUp;

	Vec3d forwardVector = localUp^sideVector;
	sideVector = forwardVector^localUp;

	forwardVector.normalize();
	sideVector.normalize();

	Vec3d dv = forwardVector * (dy*scale) + sideVector * (dx*scale);

	_center += dv;

	//回调中键移动事件
	iCall->callPan(_center.x(), _center.y(), _center.z());

	return true;
}

// doc in parent
bool CSelfDefTerrainManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double /*dx*/, const double dy)
{
	// zoom model
	/*zoomModel(dy * getThrowScale(eventTimeDelta), true);*/
	return true;
}

/// Handles GUIEventAdapter::RELEASE event.
bool CSelfDefTerrainManipulator::handleMouseRelease(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	bMidButDown = false;

	if (ea.getButtonMask() == 0)
	{
		double timeSinceLastRecordEvent = _ga_t0.valid() ? (ea.getTime() - _ga_t0->getTime()) : DBL_MAX;
		if (timeSinceLastRecordEvent > 0.02)
			flushMouseEventStack();

		if (isMouseMoving())
		{
			//去掉防止拖动滑动的操作
			/*if (performMovement() && _allowThrow)
			{
			us.requestRedraw();
			us.requestContinuousUpdate(true);
			_thrown = true;
			}
			*/
			return true;
		}
	}

	flushMouseEventStack();
	addMouseEvent(ea);
	if (performMovement())
		us.requestRedraw();
	us.requestContinuousUpdate(false);
	/*_thrown = false;*/

	return true;
}



/// Make movement step of manipulator. Returns true if any movement was made.
bool CSelfDefTerrainManipulator::performMovement()
{
	// return if less then two events have been added
	if (_ga_t0.get() == NULL || _ga_t1.get() == NULL)
		return false;

	// get delta time
	double eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();
	if (eventTimeDelta < 0.)
	{
		OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
		eventTimeDelta = 0.;
	}

	// get deltaX and deltaY
	float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

	// return if there is no movement.
	if (dx == 0. && dy == 0.)
		return false;


	// call appropriate methods
	unsigned int buttonMask = _ga_t1->getButtonMask();
	if (buttonMask == GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		return performMovementLeftMouseButton(eventTimeDelta, dx, dy);
	}
	else if (buttonMask == GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
		buttonMask == (GUIEventAdapter::LEFT_MOUSE_BUTTON | GUIEventAdapter::RIGHT_MOUSE_BUTTON))
	{
		return performMovementMiddleMouseButton(eventTimeDelta, dx, dy);
	}
	else if (buttonMask == GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		return performMovementRightMouseButton(eventTimeDelta, dx, dy);
	}

	return false;
}


void CSelfDefTerrainManipulator::changePosition(double dx, double dy)
{
	double scale = 0.3f * _distance;
	// pan model.

	Matrixd rotation_matrix;
	rotation_matrix.makeRotate(_rotation);

	// compute look vector.
	Vec3d sideVector = getSideVector(rotation_matrix);

	// CoordinateFrame coordinateFrame = getCoordinateFrame(_center);
	// Vec3d localUp = getUpVector(coordinateFrame);
	Vec3d localUp = _previousUp;

	Vec3d forwardVector = localUp^sideVector;
	sideVector = forwardVector^localUp;

	forwardVector.normalize();
	sideVector.normalize();

	Vec3d dv = forwardVector * (dy*scale) + sideVector * (dx*scale);

	_center += dv;
	iCall->callPan(_center.x(), _center.y(), _center.z());

}

void CSelfDefTerrainManipulator::panToXY(double x, double y)
{
	_center.x() = x;
	_center.y() = y;
}

void CSelfDefTerrainManipulator::zoomTo(double scale)
{
	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "%lf\n", scale);
	_logop(strMsg);

	double scale3D = getScale();

	//如果相等则返回
	if (scale3D == scale)
	{
		return;
	}

	//获得三参数
	Viewport *vp = mCamera->getViewport();

	if (vp)
	{
		int width = vp->width();
		int height = vp->height();

		char strMsg[MAX_PATH];
		sprintf_s(strMsg, "%lf\n", scale);
		_logop(strMsg);

		double sceneWidth = (double)width / scale;
		double dist = sceneWidth / 2.0 / tan(PI / 12);
		_distance = dist;
	}
}


void CSelfDefTerrainManipulator::subZoomModel(const float dy, const GUIEventAdapter& ea, GUIActionAdapter& us, bool pushForwardIfNeeded)
{
	// scale
	float scale = 1.0f + dy;

	// minimum distance
	float minDist = _minimumDistance;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;

	//限制缩放大小
	float maxDist = 10 * _modelSize;
	if (scale > 1 &&( _distance*scale > maxDist))
		return;

	if (_distance*scale > minDist)
	{
		// regular zoom
		_distance *= scale;

		if (dy < 0)
		{
			subSetCenterByMousePointerIntersection(ea, us);
		}
	}
	else
	{
		if (pushForwardIfNeeded)
		{
			// push the camera forward
//			float scale = -_distance;
			Matrixd rotation_matrix(_rotation);
			Vec3d dv = (Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (dy * scale);
			_center += dv;

			// center by mouse intersection (no animation)
			if (dy < 0)
			{
				subSetCenterByMousePointerIntersection(ea, us);
			}
		}
		else
		{
			// set distance on its minimum value
			_distance = minDist;
		}
	}

	double zoomRate = getScale();
	iCall->callZoom(_center.x(), _center.y(), zoomRate);
	
}


bool CSelfDefTerrainManipulator::subSetCenterByMousePointerIntersection(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	View* view = us.asView();
	if (!view)
		return false;

	Camera *camera = view->getCamera();
	if (!camera)
		return false;
	

	// prepare variables
	float x = (ea.getX() - ea.getXmin()) / (ea.getXmax() - ea.getXmin());
	float y = (ea.getY() - ea.getYmin()) / (ea.getYmax() - ea.getYmin());
	LineSegmentIntersector::CoordinateFrame cf;
	Viewport *vp = camera->getViewport();
	if (vp) {
		cf = Intersector::WINDOW;
		x *= vp->width();
		y *= vp->height();

		//根据屏幕坐标设置碰撞的视点区域
		x = vp->width() / 2;
		y = vp->height() / 2;
	}
	else
		cf = Intersector::PROJECTION;

	// perform intersection computation
	ref_ptr< LineSegmentIntersector > picker = new LineSegmentIntersector(cf, x, y);
	IntersectionVisitor iv(picker.get());
	camera->accept(iv);

	// return on no intersections
	if (!picker->containsIntersections())
		return false;

	// get all intersections
	LineSegmentIntersector::Intersections& intersections = picker->getIntersections();

	// get current transformation
	Vec3d eye, oldCenter, up;
	getTransformation(eye, oldCenter, up);

	// new center
	Vec3d newCenter = (*intersections.begin()).getWorldIntersectPoint();

	// make vertical axis correction
	if (getVerticalAxisFixed())
	{

		CoordinateFrame coordinateFrame = getCoordinateFrame(newCenter);
		Vec3d localUp = getUpVector(coordinateFrame);

		fixVerticalAxis(newCenter - eye, up, up, localUp, true);

	}

	// set the new center
	setTransformation(eye, newCenter, up);


	// warp pointer
	// note: this works for me on standard camera on GraphicsWindowEmbedded and Qt,
	//       while it was necessary to implement requestWarpPointer like follows:
	//
	// void QOSGWidget::requestWarpPointer( float x, float y )
	// {
	//    osgViewer::Viewer::requestWarpPointer( x, y );
	//    QCursor::setPos( this->mapToGlobal( QPoint( int( x+.5f ), int( y+.5f ) ) ) );
	// }
	//
	// Additions of .5f are just for the purpose of rounding.
	/*centerMousePointer(ea, us);*/

	return true;
}


bool CSelfDefTerrainManipulator::handleMousePush(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		bMidButDown = true;
	}

	flushMouseEventStack();
	addMouseEvent(ea);

	if (performMovement())
		us.requestRedraw();

	us.requestContinuousUpdate(false);
	_thrown = false;

	return true;
}

bool CSelfDefTerrainManipulator::shiftOperation()
{
	bShitRotate = !bShitRotate;
	return bShitRotate;
}

