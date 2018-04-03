#pragma once
#include <vector>
#include <string>
#include <osgViewer/Renderer>
#include <osgDB/WriteFile>
#include "osgOffScreenViewer.h"
#include "gdal.h"
#include "gdal_priv.h"
//#include "Executor.h"
#include "highgui.h"
#include "cv.h"


/**
** brief Capture the frame buffer and write image to disk
**/
class WindowCaptureCallback : public osg::Camera::DrawCallback
{
public:
	bool isOutputImage;
public:    
	WindowCaptureCallback(GLenum readBuffer, const std::string& name, int frameNum, int Width, int Height):
	  _readBuffer(readBuffer),
		  _fileName(name)
	  {
		  _image = new osg::Image;
		  isOutputImage = false;
		  _frameNum = frameNum;
		  width = Width;
		  height = Height;
		  outputFileName = name;
	  }

	  virtual void operator () (osg::RenderInfo& renderInfo) const
	  {
		  glReadBuffer(_readBuffer);
		  osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();

		  if (gc->getTraits())
		  {
			  GLenum pixelFormat;

			  if (gc->getTraits()->alpha)

				  pixelFormat = GL_RGBA;
			  else 
				  pixelFormat = GL_RGB;

			  _image->readPixels(0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE); 
		  }

		  --_frameNum;

		  if (!outputFileName.empty())
		  {
			  osgDB::writeImageFile(*_image, outputFileName);
		  }
	  }

protected:    
	GLenum                      _readBuffer;
	std::string                 _fileName;
	osg::ref_ptr<osg::Image>    _image;
	mutable OpenThreads::Mutex  _mutex;
	mutable int _frameNum;
	int width;
	int height;
	std::string outputFileName;
};

/** 
** brief Do Culling only while loading PagedLODs
**/
class CustomRenderer : public osgViewer::Renderer
{
public:
	CustomRenderer(osg::Camera* camera) 
		: osgViewer::Renderer(camera),
		_cullOnly(true)
	{
		/*setTargetFrameRate(1);
		setMinimumTimeAvailableForGLCompileAndDeletePerFrame(1);*/
	}

	/** Set flag to omit drawing in renderingTraversals */
	void setCullOnly(bool on) { _cullOnly = on; }

	virtual void operator () (osg::GraphicsContext* /*context*/)
	{
		if (_graphicsThreadDoesCull)
		{
			if (_cullOnly)
				cull();
			else
				cull_draw();
		}
	}

	virtual void cull()
	{
		osgUtil::SceneView* sceneView = _sceneView[0].get();
		if (!sceneView || _done ) return;

		updateSceneView(sceneView);

		osgViewer::View* view = dynamic_cast<osgViewer::View*>(_camera->getView());
		if (view) sceneView->setFusionDistance(view->getFusionDistanceMode(), view->getFusionDistanceValue());

		sceneView->inheritCullSettings(*(sceneView->getCamera()));
		sceneView->cull();
	}

	bool _cullOnly;
};

/**
** brief dom������
**/
class CSceneDomGenerator
{
public:
	CSceneDomGenerator(std::string sceneFileName,
		std::string outFileName, double res, int xTileNum, int yTileNum);

	/**
	** brief ��ʼ����dom
	**/
	virtual bool startProducingDOM();

protected:

	/**
	** brief generateSingleDOM��ʼ��������dom
	** param mTrans �����ڵ�
	** param outputDomFileName ���dom�ļ�
	** param �Ƿ��Ƕ��
	**/
	virtual bool generateSingleDOM(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string outputDomFileName, bool isCombine);

	/**
	** brief generateSingleDOM��ʼ�������dom�������Ǻϲ����������
	** param vecTransNode ��������ڵ�
	**/
	virtual bool generateMultiDom(std::vector<osg::MatrixTransform*> vecTransNode);
	
	/**
	** brief generateDOM��ʼ����dom
	** param outputDomFileName ���dom�ļ�
	** param outputCoordFileName �����������
	** param mTrans �����ڵ�
	** param isCombine �Ƿ��Ƕ��
	**/
	virtual bool generate(std::string outputDomFileName, std::string outputCoordFileName, osg::ref_ptr<osg::MatrixTransform> mTrans, bool isCombine);

	virtual bool produceNodeDOM(osg::ref_ptr<osg::Node> loadedModel, std::string &outRectDomFileName, osg::Matrix matrix);

	virtual osg::Vec4d findMaxRect(std::vector<osg::Vec4d> vecRect);

	virtual std::vector<osg::Vec4d> findImageRect(std::vector<std::string> vecCoordTxtFileName);

	virtual void combineImage(GDALDataset* poDataset, std::string imgFileName, osg::Vec4d rect, osg::Vec4d maxRect);

	virtual void combineBigRepairImage(GDALDataset* poDataset, GDALDataset* poBigRepairImageDataset, osg::Vec4d rect, osg::Vec4d bigRepairImageMaxRect,
		osg::Vec4d maxRect);

	virtual bool produceRectDom(osg::ref_ptr<osg::MatrixTransform> mTrans, std::vector<osg::Vec3d> vecCoord, std::string outRectDomFileName);

	virtual bool findMinMaxNode(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxCoord);

	virtual bool findMinMax(std::vector<osg::Vec3d> vecMinMax, std::vector<osg::Vec3d> &coord);

	virtual bool produceScreenDOM(osg::ref_ptr<osg::MatrixTransform> node, int newWidth, int newHeight, double newRectLeft, double newRectRight,
		double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, std::string newOutFileName);

	virtual void combineImage(CvMat* imgMat, std::string imgFileName, osg::Vec4d rect, osg::Vec4d maxRect);

	virtual bool findTileImageRect(std::string txtFileName, osg::Vec4d &rect);

	virtual bool outputImageAndCoords(std::vector<osg::MatrixTransform*> vecTransNode, std::vector<std::string> vecSceneFilePath,
		std::vector<std::string> vecImageFileName, std::vector<std::string> vecCoordFileName);

	virtual bool startRepairDom(osg::ref_ptr<osg::MatrixTransform> mTrans, GDALDataset* poDataset, std::string sceneFilepath, osg::Vec4d maxRect);

	virtual bool produceSplitedDOM(int i, int j, osg::ref_ptr<osg::MatrixTransform> mTrans, int newWidth, int newHeight, double newRectLeft,
		double newRectRight, double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, std::string outRectDomFileName);

	virtual void combineBigImage(GDALDataset* poDataset, GDALDataset* poBigImageDataset, osg::Vec4d rect, osg::Vec4d bigImageMaxRect, osg::Vec4d maxRect);

	virtual std::vector<osg::Vec4d> findDOMRect(std::vector<std::string> vecCoordFileName, std::vector<std::string> vecImageFileName);

protected:
	/*
	** brief mSceneNode �����ڵ�
	*/
	osg::ref_ptr<osg::Node> mSceneNode;

	/*
	** brief sceneFileName �����ڵ�
	*/
	std::string mSceneFileName;

	/*
	** brief mRes ������ֱ��ʴ�С
	*/
	double mRes;

	/*
	** brief mXTileNum x����tile��С
	*/
	int mXTileNum;

	/*
	** brief mYTileNum y����tile��С
	*/
	int mYTileNum;

	/*
	** brief mOutputFileName ���dom�ļ�
	*/
	std::string mOutputFileName;

	std::vector<std::string> vecSplitImgFileName;

	std::vector<std::string> vecSplitCoordFileName;
};
