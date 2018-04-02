#include "StdAfx.h"
#include "IOsgClip.h"
#include "OsgAccurateClip.h"

using namespace std;

shared_ptr<IOsgClip> IOsgClipFactory::create()
{
	shared_ptr<IOsgClip> pOsgClip(new COsgAccurateClip());
	return pOsgClip;
}