#pragma once

enum hint
{
	hc_SetOsgCrossPos = 55,
	hc_lfClk = 56,
	hc_rClk = 57,
	hc_mouseMv = 58,
	hc_keyDownDel = 59,
	hc_keyBkspace = 60,
	hc_osgLfClk = 61,
	hc_osgRClk = 62,
	hc_osgMouseMv = 63,
	hc_osgkeyDel = 64,
	hc_osgBkspace = 65,
	hc_driveToXY = 66,
	hc_selObjInArea = 67,
	hc_undo = 68,
	hc_osgSel = 69,
	hc_eventStart = 70,
	hc_viewAll = 71,
	hc_viewHideAll = 72,
	hc_viewLocal = 73,
	hc_viewExternal = 74,
	hc_zoomView = 75,
	hc_osgSymbol = 76,
	hc_UpdatesnapDraw = 77,
	hc_handMove = 78,
	hc_zoomScale = 79,
	hc_setCtrlPt = 80,
	hc_absOri = 81,
	hc_goto = 82,
	hc_showPrecion = 83,
	hc_showOriData = 84,
	hc_predictOriData = 85,
	hc_showFullOsgbView = 86,
	hc_showHalfOsgbView =87
};

#define WM_LOAD_SCENE                   20000
#define WM_RM_ALL_SCENE                 20001
#define WM_DRAW_PT                      20002
#define WM_DRAW_LINE                    20003
#define WM_DRAW_AREA                    20004
#define WM_ADD_PT                       20005
#define WM_ADD_LINE                     20006
#define WM_ADD_AREA                     20007
#define WM_OSG_MOUSE_MV                 20008
#define WM_SEL_RECT						20009
#define WM_DRAW_RECT					20010
#define WM_DEL_SEL						20011
#define WM_BKSPACE						20012
#define WM_UNDO2						20013
#define WM_LF_CLK						20014
#define WM_RIGHT_CLK					20015
#define WM_ACTI_DW_PT					20016
#define WM_ACTI_DW_LINE					20017
#define WM_ACTI_DW_POLY					20018
#define WM_ACTI_SEL_RECT				20019
#define WM_DRIVE						20020
#define WM_ZOOM							20021
#define ID_OPEN_OSGB					20023
#define ID_REMOVE_OSGB                  20024
#define WM_PROMPT_LOCK                  20025

