#pragma once
#include <osg/Vec3d>
#include <osg/Geometry>
#include <osg/Geode>
#include "osg/NodeVisitor"
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>
#include <osgDB/Options>
#include <osg/PagedLOD>
#include <string>
#include <vector>
#include "OsgPageLodCenterReCalculator.h"
#include <time.h>
#include <io.h>
#include <Sys/stat.h>
#include <Sys/timeb.h>
#include "mm3dConvertSmart3DDlg.h"
#include <process.h>
using namespace std;

inline CString  LoadString(UINT uid)
{
	CString str;
	str.LoadString(uid);
	return str;
}

class CPageLodSetter : public osg::NodeVisitor
{
public:
	CPageLodSetter(std::string FileName);
	~CPageLodSetter(void);

	//接口
	void SetLodFileName(osg::ref_ptr<osg::Node> node, std::string fileName);		//设置lod文件名

protected:
	virtual void apply(osg::PagedLOD& pagedLod);
private:
	std::string fileName;
};

class CTransNodeFinder : public osg::NodeVisitor
{
public:
	std::vector<osg::MatrixTransform*> vecTrans;
public:
	CTransNodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{

	}

	//接口
	void FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans);		//查找trans节点

protected:
	virtual void apply(osg::MatrixTransform& node);

private:

};

//////////////////////////////////////////////////////

class CChildPagedLodTraveller : public osg::NodeVisitor
{
public:
	CChildPagedLodTraveller(std::string InFilePath, std::string OutFilePath,osg::Matrix& m) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		outFilePath = OutFilePath;
		inFilePath = InFilePath;
		matrix = m;
	}

protected:
	virtual void apply(osg::PagedLOD& pagedLod);
	virtual void apply(osg::Geode& geode);
	osg::ref_ptr<osg::Node> readChildPagedLodNode(osg::PagedLOD& pagedLod);
	bool outputChildPagedLodNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node);
	void travelChildNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node);

	void CheckClipFolder(std::string filePath, std::string fileTitle, std::string &outFileName);

	std::string ObtainTileFileName(std::string fileName);

private:
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
};
///////////////////////////////////////

class CMultiChildScopeGeoClipper : public osg::NodeVisitor
{
public:
	CMultiChildScopeGeoClipper( osg::Matrix MATRIX) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		
		matrix = MATRIX;
		
	}

	virtual void apply(osg::Geode& geode)
	{
		osg::Matrix mat;
		mat.setRotate(matrix.getRotate());
		int num = geode.getNumDrawables();

		for(int j = num  - 1; j >= 0; --j) 
		{
			
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(j));
			osg::Geometry* newGeom = new osg::Geometry;
			if( !geom ) 
				continue; 
			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			osg::Vec3Array* newVerts = new osg::Vec3Array(verts->size());

			for (int i = 0;i<verts->size();++i)
			{
				double x1 = verts->at(i).x();
				double y1 = verts->at(i).y();
				double z1 = verts->at(i).z();
				osg::Vec3d pt1(x1, y1, z1);
				pt1 = pt1 * mat; 

				float* pIndex1 = verts->at(i).ptr();

				pIndex1[0] = pt1.x();
				pIndex1[1] = pt1.y();
				pIndex1[2] = pt1.z();
			}
		}
	}

private:	
	osg::Matrix matrix;

};

//////////////////////////////////////


//////////////////////////




class CMultiScopePagedLodClipper : public osg::NodeVisitor
{
public:
	CMultiScopePagedLodClipper(Cmm3dConvertSmart3DDlg* toolDlg,std::string InFilePath, std::string OutFilePath, osg::Matrix Matrix) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		outFilePath = OutFilePath;
		inFilePath = InFilePath;
		matrix = Matrix;
		ctoolDlg = toolDlg;
	}

private:
	virtual void apply(osg::PagedLOD& pagedLod);

	void travelChildNode(osg::PagedLOD& pagedLod);

	osg::ref_ptr<osg::Node> readPagedLodNode(osg::PagedLOD& pagedLod)
	{
		std::string fileName = pagedLod.getFileName(1);

		int posEnd = fileName.find_last_of('\\');  
		int posStart = fileName.find_first_of('\\');
		std::string outFolder = fileName.substr(posStart, posEnd - posStart + 1);				//输出目录
		std::string outFileTitle(fileName.substr(posEnd + 1));					//输出文件名

		std::string inHeadFileName = inFilePath + outFolder + outFileTitle;
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(inHeadFileName);
		//设置相对路径
		CPageLodSetter ive("");
		node->accept(ive);
		return node;
	}

	bool outputPagedLodNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node);
protected:
	Cmm3dConvertSmart3DDlg* ctoolDlg;
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
};

class CMultiScopeCHildPagedLodClipper : public CChildPagedLodTraveller
{
public:
	CMultiScopeCHildPagedLodClipper( std::string InFilePath, std::string OutFilePath, osg::Matrix Matrix) : CChildPagedLodTraveller(InFilePath, OutFilePath,Matrix)
	{
		
		outFilePath = OutFilePath;
		inFilePath = InFilePath;
		matrix = Matrix;
		
	}

private:
	virtual void apply(osg::PagedLOD& pagedLod)	;
	

	void travelChildNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node)
	{
		//继续访问
		std::string childFileTitle = pagedLod.getFileName(1);
		std::string childFileName = inFilePath + childFileTitle;

		int pos = childFileName.rfind('\\') + 1;
		std::string inChildFilePath = childFileName.substr(0, pos);

		CMultiScopeCHildPagedLodClipper ive(inChildFilePath, outFilePath, matrix);
		node->accept(ive);
	}

private:
	
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
	
};

class CMultiScopeHeadGeodeClipper : public osg::NodeVisitor
{
public:
	CMultiScopeHeadGeodeClipper(osg::Matrix MATRIX) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		
		matrix = MATRIX;
		
	}

	virtual void apply(osg::Geode& geode);

private:
	
	osg::Matrix matrix;
	
};


class CConvertTool
{
public:
	static void doConvert(void* ptr);

	void startProcess();

	void startConvert();

	bool convert();

	void FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans);

	bool ClipSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<std::string> &vecTotalInPolygonFileName);

	void ObtainSelTileName(osg::ref_ptr<osg::Group> mGroup, std::vector<std::string> &vecTotalInPagedLodFileName);

	void ObtainSelFileNameInPolygon(osg::ref_ptr<osg::Group> group,std::vector<std::string> &vecTotalInFileName);

	std::string ObtainTileFileName(std::string fileName);

	void CheckClipFolder(std::string filePath, std::string fileTitle, std::string &outFileName);

	bool CreateFolder(std::string createFilePath);

	BOOL MakeDirectory(CString dd);

	void FileCopyTo(std::string source, std::string destination, std::string searchStr, BOOL cover = TRUE);

	osg::ref_ptr<osg::Group> CombinePagedLod(std::vector<std::string> vecPagedLodFileName, std::string inFilePath);

	bool ClipOsgHeader(osg::ref_ptr<osg::Node> sceneNode);

	Cmm3dConvertSmart3DDlg* convertDlg;

protected:

	
};