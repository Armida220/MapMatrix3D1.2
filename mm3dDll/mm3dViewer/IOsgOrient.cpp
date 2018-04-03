#include "StdAfx.h"
#include "IOsgOrient.h"
#include "AbsoluteOrientationor.h"

IOsgOrient* IOsgOrientFactory::create()
{
	return new CAbsoluteOrientationor();
}