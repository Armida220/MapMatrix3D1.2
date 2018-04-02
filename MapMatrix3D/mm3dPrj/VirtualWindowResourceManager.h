#pragma once
#include <vector>
#include <string>
#include <thread>
#include <map>
#include "threadSafeQueueT.h"
#include "Ori.h"

class CVirtualWindowResourceManager
{
	CHwndRenderTarget* m_RenderTarget;
	std::vector<CD2DSolidColorBrush*> m_SolidBrush;
	std::vector<CD2DRadialGradientBrush*> m_RadialGradientBrush;
	std::vector<CD2DTextFormat*> m_TextFormat;
	std::vector<COri> m_vecImgOri;
	std::thread * m_thread;
	std::map<std::string, CD2DBitmap * > m_img_bmp;
	void removeBMP(const std::string & str);
	threadSafeQueueT<std::string> loadImageQueue;
	void loadImageThreadFun();
public:
	CVirtualWindowResourceManager();
	~CVirtualWindowResourceManager();

	void Init(CHwndRenderTarget* pRenderTarget);
	enum SolidBrushID
	{
		CHECKBOX_CHECK       = 0,
		CHECKBOX_NOCHECK     ,
		CHECKBOX_BUTTON      ,
		CHECKBOX_THINGFRAME  ,
		CHECKBOX_MOUSEINFRAME,
		IMG_BACKGROUND       ,
		IMG_CROSS            ,
		IMG_TEXT
	};

	CD2DBrush * GetSolidBrush(SolidBrushID id)
	{
		return m_SolidBrush[id];
	}
	enum RadialGradientID
	{
		RG_DEFAULE = 0
	};
	CD2DBrush * GetRadialGradientBrush(const CD2DPointF & center,
		RadialGradientID id = RadialGradientID::RG_DEFAULE)
	{
		m_RadialGradientBrush[id]->SetCenter(center);
		return m_RadialGradientBrush[id];
	}
	enum TextFormatID
	{
		TF_DEFAULE = 0
	};
	CD2DTextFormat * GetTextFormat(TextFormatID id = TextFormatID::TF_DEFAULE)
	{
		return m_TextFormat[id];
	}

	void setVecOri(const std::vector<COri> & vecImgOri);
	CD2DBitmap * getImageByIndex(int i, COri & ori);
};

