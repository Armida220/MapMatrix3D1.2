#include "stdafx.h"
#include "VirtualWindowResourceManager.h"
#include <io.h>
#include <set>
using namespace std;

void CVirtualWindowResourceManager::removeBMP(const std::string & str)
{
	auto iter = m_img_bmp.find(str);
	if (iter != m_img_bmp.end())
	{
		if (iter->second) delete iter->second;
		m_img_bmp.erase(iter);
	}
}

void CVirtualWindowResourceManager::loadImageThreadFun()
{

	while (true)
	{
		string * pstr =  loadImageQueue.waitPop();
		if (!pstr) break;
		string str = *pstr;
		delete pstr;
		auto iter = m_img_bmp.find(str);
		if (iter != m_img_bmp.end()) continue; 
		if (_access(str.c_str(), 0) != 0)
		{
			m_img_bmp.insert(std::pair<std::string, CD2DBitmap*>(str, nullptr));
			continue;
		}
		else
		{
			
			CString strPath;
			strPath = str.c_str();
			CD2DBitmap * pBitmap = new CD2DBitmap(m_RenderTarget, strPath, CD2DSizeU(0,0), FALSE);
			
			m_img_bmp.insert(std::pair<std::string, CD2DBitmap*>(str, pBitmap));
		}
	}
}

CVirtualWindowResourceManager::CVirtualWindowResourceManager() :m_RenderTarget(nullptr), m_thread(nullptr)
{
}



CVirtualWindowResourceManager::~CVirtualWindowResourceManager()
{
	if (m_thread)
	{
		loadImageQueue.push(nullptr);
		m_thread->join();
		delete m_thread;
		m_thread = 0;
	}
	while (!m_img_bmp.empty())
	{
		removeBMP(m_img_bmp.begin()->first);
	}
}

void CVirtualWindowResourceManager::Init(CHwndRenderTarget* pRenderTarget)
{
	typedef struct ENUM_COLOR
	{
		SolidBrushID id;
		COLORREF color;
	} ENUM_COLOR;

	ENUM_COLOR ecs[] = {
		{ SolidBrushID::CHECKBOX_CHECK, RGB(5, 170, 222) },
		{ SolidBrushID::CHECKBOX_NOCHECK, RGB(205, 205, 205) },
		{ SolidBrushID::CHECKBOX_BUTTON, RGB(251, 251, 251) },
		{ SolidBrushID::CHECKBOX_THINGFRAME, RGB(82, 82, 82) },
		{ SolidBrushID::CHECKBOX_MOUSEINFRAME, RGB(2, 68, 90) },
		{ SolidBrushID::IMG_BACKGROUND, RGB(251, 251, 251) },
		{ SolidBrushID::IMG_CROSS, RGB(255, 0, 0) },
		{ SolidBrushID::IMG_TEXT, RGB(51, 255, 255) },
	};
	int solidBrushSize = sizeof(ecs) / sizeof(ecs[0]);
	m_RenderTarget = pRenderTarget;
	m_SolidBrush.clear();
	m_SolidBrush.resize(solidBrushSize, nullptr);
	for (int i = 0; i < solidBrushSize; i ++)
	{
		ENUM_COLOR & ec = ecs[i];
		m_SolidBrush[ec.id] = new CD2DSolidColorBrush(pRenderTarget, ec.color);
	}
	
	m_RadialGradientBrush.clear();
	m_RadialGradientBrush.resize(1, nullptr);
	D2D1_GRADIENT_STOP gradientStops2[] = {
		{ 1.f, { 0.1f, 0.1f, 0.1f, 1.f } },
		{ 0.0f, { 0.8f, 0.8f, 0.8f, 1.f } },
	};
	m_RadialGradientBrush[0] = new CD2DRadialGradientBrush(pRenderTarget,
		gradientStops2, ARRAYSIZE(gradientStops2),
		D2D1::RadialGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, 0), 50, 10));
	
	m_TextFormat.clear();
	m_TextFormat.resize(1, nullptr);
	m_TextFormat[0] = new CD2DTextFormat(pRenderTarget, _T("Verdana"), 18);
	m_TextFormat[0]->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_TextFormat[0]->Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void CVirtualWindowResourceManager::setVecOri(const std::vector<COri> & vecImgOri)
{
	if (!m_img_bmp.empty())
	{
		set<string> newSet;
		set<string> removeSet;
		for (const COri & o : vecImgOri)
		{
			newSet.insert(o.imgName);
		}
		for (auto & iter : m_img_bmp)
		{
			if (newSet.find(iter.first) != newSet.end())
			{
				removeSet.insert(iter.first);
			}
		}
		for (const string & img : removeSet)
		{
			removeBMP(img);
		}
	}
	m_vecImgOri = vecImgOri;

}

CD2DBitmap * CVirtualWindowResourceManager::getImageByIndex(int i, COri & ori)
{
	if (i >= m_vecImgOri.size() || i < 0) { ori.imgName = "Null"; return nullptr; }
	ori.imgName = m_vecImgOri[i].imgName;
	ori.imgX = m_vecImgOri[i].imgX;
	ori.imgY = m_vecImgOri[i].imgY;
	auto iter = m_img_bmp.find(ori.imgName);
	if (iter != m_img_bmp.end()) { 
		if (!iter->second)
		{
			ori.imgName = "Can't open " + ori.getFileName(ori.imgName);
		}
		return iter->second; 
	}
	if (!m_thread) m_thread = new std::thread(&CVirtualWindowResourceManager::loadImageThreadFun, this);
	loadImageQueue.push(new string(ori.imgName));
	ori.imgName = ori.getFileName(ori.imgName);
	return nullptr;
}
