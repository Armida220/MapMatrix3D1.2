#include "StdAfx.h"
#include "MFC_OSG.h"

using namespace mm3dView;

std::shared_ptr<IOsgMfcViewer> IOsgMfcViewerFactory::create(HWND hWnd)
{
	std::shared_ptr<IOsgMfcViewer> iViewer(new cOSG(hWnd));
	return iViewer;
}