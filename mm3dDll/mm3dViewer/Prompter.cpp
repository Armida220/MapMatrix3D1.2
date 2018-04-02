#include "stdafx.h"
#include "Prompter.h"
#include "ICallBack.h"

using namespace osgEvent;
using namespace std;
using namespace osgCall;

CPrompter::CPrompter(shared_ptr<ICallBack> pCall)
	: iCall(pCall)
{
}


CPrompter::~CPrompter()
{
}

void CPrompter::promptLock(bool bLockHeight)
{
	iCall->promptLock(bLockHeight);
}


void CPrompter::promptCatch(bool bCatch)
{
	iCall->promptCatch(bCatch);
}

void CPrompter::promptOpera(bool bShiftOpera)
{
	iCall->promptOpera(bShiftOpera);
}