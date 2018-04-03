#include "stdafx.h"
#include "TimeKeeper.h"

using namespace osgEvent;
using namespace std;
using namespace std::chrono;

CTimeKeeper::CTimeKeeper()
	:start(high_resolution_clock::now()),
	 end(high_resolution_clock::now()),
	 bActivate(true)
{
}


CTimeKeeper::~CTimeKeeper()
{
}

bool CTimeKeeper::shift()
{
	bActivate = !bActivate;
	return bActivate;
}

bool CTimeKeeper::getActivate()
{
	return bActivate;
}

void CTimeKeeper::recordStart()
{
	//记下开始时间
	start = high_resolution_clock::now();
}


bool CTimeKeeper::isEnoughTime()
{
	//记下结束时间
	end = high_resolution_clock::now();

	duration<double> time_span = duration_cast<duration<double>>(end - start);

	if (time_span.count() > 0.2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void osgEvent::CTimeKeeper::setActive(bool b)
{
	bActivate = b;
}
