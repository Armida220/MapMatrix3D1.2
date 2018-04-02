#include "stdafx.h"
#include "OverrideRealtimeVectLayer.h"



vector<GrBuffer*> COverrideRealtimeVectLayer::getRenderBufs()
{
	vector<GrBuffer*> vecBufs;

	GrElement *p = m_eleListTempObjsForemost.head;
	while (p)
	{
		GrBuffer* buf = static_cast<GrBuffer*> (p->pBuf);

		if (buf)
		{
			vecBufs.push_back(buf);
		}

		p = p->next;
	}

	p = m_eleListTempObjsUppermost.head;
	while (p)
	{
		GrBuffer* buf = static_cast<GrBuffer*> (p->pBuf);

		if (buf)
		{
			vecBufs.push_back(buf);
		}

		p = p->next;
	}

	return vecBufs;
}