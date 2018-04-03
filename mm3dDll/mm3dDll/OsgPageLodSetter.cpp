#include "StdAfx.h"
#include <osg/LOD>
#include <osg/PagedLOD>
#include "OsgPageLodSetter.h"


using namespace std;
using namespace osg;

COsgPageLodSetter::COsgPageLodSetter(string FileName) : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	fileName = FileName;
}


COsgPageLodSetter::~COsgPageLodSetter(void)
{
}

void COsgPageLodSetter::SetLodFileName(ref_ptr<Node> node, string fileName)
{
	COsgPageLodSetter ive(fileName);
	node->accept(ive);
}

void COsgPageLodSetter::apply(PagedLOD& pagedLod)
{
	pagedLod.setDatabasePath("");
	traverse(pagedLod);
}