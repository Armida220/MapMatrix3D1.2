#pragma once
#include <osg/NodeVisitor>
#include <osg/PagedLOD>
#include <vector>
#include <string>

class COsgPagedLodBottomLevelFinder : public osg::NodeVisitor
{
public:
	COsgPagedLodBottomLevelFinder(std::vector<std::string> *VecBottomLevelPagedLodFileName);
	~COsgPagedLodBottomLevelFinder(void);

protected:
	virtual void apply(osg::PagedLOD& pagedLod);

public:
	std::vector<std::string>* vecBottomLevelPagedLodFileName;
};

	