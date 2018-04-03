#include "StdAfx.h"
#include "IOsgDsmGene.h"
#include "osgDSMGene.h"
using namespace std;

shared_ptr<IOsgDsmGene> IOsgDsmGeneFactory::create()
{
	shared_ptr<IOsgDsmGene> temp(new osgDSMGene);
	return temp;
}