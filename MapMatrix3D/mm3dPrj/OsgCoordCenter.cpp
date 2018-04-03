#include "stdafx.h"
#include "OsgCoordCenter.h"
#include "qcomm.h"

COsgCoordCenter::COsgCoordCenter()
{
}


COsgCoordCenter::~COsgCoordCenter()
{
}

bool COsgCoordCenter::InitByTwoImg(CoreObject& core, CString lImg, CString rImg)
{
	Init(core, "", lImg);
	//look for stereo ID	
	return SwitchToTwoImg(lImg, rImg);
}


bool COsgCoordCenter::SwitchToTwoImg(CString lImg, CString rImg)
{
	//fill temporary structure
	//find image
	CString strID[2];
	strID[0] = lImg;
	strID[1] = rImg;

	IMAGE image[2];
	for (int i = 0; i<coreobj.iStripNum; i++)
	{
		for (int j = 0; j<coreobj.strip[i].iImageNum; j++)
		{
			if (coreobj.strip[i].image[j].strImageID.CompareNoCase(strID[0]) == 0)
			{
				m_iStripNum0 = i;
				m_iImageNum0 = j;
				image[0] = coreobj.strip[i].image[j];
			}
			if (coreobj.strip[i].image[j].strImageID.CompareNoCase(strID[1]) == 0)
			{
				m_iStripNum1 = i;
				m_iImageNum1 = j;
				image[1] = coreobj.strip[i].image[j];
			}
		}
	}
	//should add code to detect when loading failed!!!!!!
	//fill internal structure
	m_structSPT[0] = image[0].ipara;
	m_structSPT[1] = image[1].ipara;
	m_structIP[0] = image[0].ipoint;
	m_structIP[1] = image[1].ipoint;

	m_structIOP[0] = image[0].iop;
	m_structIOP[1] = image[1].iop;

	m_structAOP[0] = image[0].aop;
	m_structAOP[1] = image[1].aop;

	return true;
}
