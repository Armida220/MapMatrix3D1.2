
#include "StdAfx.h"
#include "IOsgDomGenerator.h"
#include "osgDomGenerator.h"

using namespace std;

shared_ptr<IOsgDomGenerator> IOsgDomGeneratorFactory::create()
{
	shared_ptr<osgDomGenerator> temp(new osgDomGenerator());
	return temp;
}