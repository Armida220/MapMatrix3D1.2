// EditDef.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_EDITDEF_H__INCLUDED_)
#define _EDITDEF_H__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"


// 光标状态
// 禁用模式：系统光标
#define CURSOR_DISABLE					0
// 通常模式：绘图测标（十字丝）和选择框都有
#define CURSOR_NORMAL					1
// 选择模式：只有选择框
#define CURSOR_SELECT					2
// 绘图模式：只有绘图测标
#define CURSOR_DRAW						3


// 选择模式
// 正常模式：鼠标点击时为单选，Ctrl+鼠标点击时为可以多选和去掉选择；
#define SELMODE_NORMAL					0
// 单选模式：总是只选中一个
#define SELMODE_SINGLE					1
// 多选模式：总是多选（与Ctrl没有关系）
#define SELMODE_MULTI					2
// 选择点的模式：并不选择任何对象，目的只在于获取鼠标点击的所在点
#define SELMODE_CLICK					3
// 选择雷达点模式:
#define SELMODE_LIDAR					4

#define SELMODE_POLYGON					5

//框选
#define SELMODE_RECT					6

// 选择状态标记
#define SELSTAT_NONESEL					0x0000  //空状态
#define SELSTAT_POINTSEL				0x0001  //单击选中
#define SELSTAT_DRAGSEL_START			0x0002	//开始拉框//选择集为空
#define SELSTAT_DRAGSEL					0x0004  //完成了框选
#define SELSTAT_DRAGSEL_RESTART			0x0008  //开始重启框选操作
#define SELSTAT_MULTISEL				0x0010  //当前处于多选状态(按住Ctrl等)

// 选择的对象过滤标志
#define SELFILTER_ALL					0xFFFFFFFF
#define SELFILTER_POINT					0x00000001
#define SELFILTER_CURVE					0x00000002
#define SELFILTER_SURFACE				0x00000004

// UI请求类型；用于CEditor::UIRequest的第一个参数；

//以下三个用于命令参数的界面显示
#define UIREQ_TYPE_LOADUIPARAM			1
#define UIREQ_TYPE_SHOWUIPARAM			2

#define UIREQ_TYPE_OUTPUTMSG			4
#define UIREQ_TYPE_OUTPUTMSG_OLDLINE	14
#define UIREQ_TYPE_STARTWAIT			5
#define UIREQ_TYPE_ENDWAIT				6

//以下两个用于命令参数的存储和读取
#define UIREQ_TYPE_LOADFILEPARAM		7
#define UIREQ_TYPE_SAVEFILEPARAM		8


MyNameSpaceBegin


enum HINTCODE
{
		hc_AddObject		= 1,
		hc_DelObject		= 2,
		hc_Refresh			= 3,
		hc_SetConstDragLine	= 4,
		hc_ClearDragLine	= 5,
 		hc_OpenSelector		= 6,
 		hc_CloseSelector	= 7,
		hc_SetCrossPos      = 8,
		hc_Attach_Accubox   = 10,
		hc_Detach_Accubox	= 11,
		hc_SetVariantDragLine	= 12,
		hc_AddConstDragLine	= 13,
		hc_SetMapArea		= 14,
		hc_SelChanged       = 15,
		hc_Update_Accubox	= 16,
		hc_UpdateOption		= 17,
		hc_DelAllObjects	= 18,
		hc_UpdateGrid		= 19,
		hc_UpdateViewType	= 20,
		hc_SetCursorType	= 21,
		hc_UpdateConstDrag	= 22,
		hc_SetViewArea		= 23,
		hc_UpdateOverlayBound = 24,
		hc_SetAnchorPoint	= 25,
		hc_AddGraph			= 26,
		hc_DelGraph			= 27,
		hc_UpdateAllObjects = 28,
		hc_UpdateVariantDrag= 29,
		hc_RotateAsStereo   = 30,
		hc_ModifyHeight		= 31,
		hc_SetRoadLine		= 32,
		hc_ManualLoadVect	= 33,
		hc_SetImagePosition = 34,
		hc_GetImagePosition = 35,
		hc_RestoreImagePosition = 36,
		hc_AddTopoPolygon   = 37,
		hc_AddVariantDragLine =38,
		hc_UpdateLayerDisplay = 39,
		hc_UpdateWindow		= 40,
		hc_UpdateLayerDisplayOrder =41,
		hc_LockXY = 42,
		hc_UpdateDataColor  =43,
		hc_AddSusPtColor  =44,
		hc_DelSusPtColor = 45,
		hc_AddPseudoPtColor  =46,
		hc_DelPseudoPtColor  = 47,
		hc_UpdateObjectDisplay = 48,
		hc_GetOverlayImgNames = 49,
		hc_RotateVectView = 50,
		hc_GetImageAdjust = 51,
		hc_UpdateRefFileText = 52,
		hc_UpdateAllObjects_Visible = 53,
		hc_UpdateAllObjects_VectorView = 54
};

enum UpdateDragType
{
	ud_ClearDrag = 0,
	ud_SetConstDrag = 1,
	ud_AddConstDrag = 2,
	ud_SetVariantDrag = 3,
	ud_SetStepDisplay = 6,
	ud_AddVariantDrag = 7,
	ud_Attach_Accubox = 8,
	ud_Detach_Accubox = 9,
	ud_SetCrossPos  = 10,
	ud_ModifyHeight = 11,
	ud_UpdateVariantDrag = 12,
	ud_UpdateConstDrag = 13
};

enum UpdateViewType
{
	uv_AllView = 0,
	uv_VectorView = 1,
	uv_StereoView = 2
};


MyNameSpaceEnd

#endif
