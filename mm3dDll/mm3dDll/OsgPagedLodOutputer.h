#pragma once
#include <osg/PagedLOD>
#include <string>

class COsgPagedLodOutputer
{
public:
	COsgPagedLodOutputer(std::string OutFilePath)
	{
		outFilePath = OutFilePath;
	}

	virtual bool outputPagedLodNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node);

	virtual void setOutFilePath(std::string OutFilePath);

	virtual void CheckClipFolder(std::string filePath, std::string fileTitle, std::string &outFileName);

	virtual void OutputTotalInScopeOsgb(std::string inFilePath, std::vector<std::string> &vecTotalInPolygonFileName);

protected:
	virtual std::string ObtainTileFileName(std::string fileName);

	virtual void WriteNodeToFile(osg::ref_ptr<osg::Node> node, std::string outFileNameInPiece);

private:
	std::string outFilePath;
};

