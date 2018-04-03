#include "stdafx.h"
#include "IMm3dDLL.h"
#include "mm3dDll.h"

using namespace std;

shared_ptr<IMm3dDLL> IMm3dDLLFactory::create()
{
	shared_ptr<IMm3dDLL> iMM3dDll(new CMm3dDll());
	return iMM3dDll;
}