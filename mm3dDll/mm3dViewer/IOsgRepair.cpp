#include "StdAfx.h"
#include "IOsgRepair.h"
#include "osgRepair.h"

using namespace std;

shared_ptr<IOsgRepair> IOsgRepairFactory::create()
{
	shared_ptr<IOsgRepair> temp(new osgRepair());
	return temp;
}