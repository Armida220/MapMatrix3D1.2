#pragma once
#include "OsgbView.h"

#define WM_LEFT_VIEW			WM_USER+3129
#define WM_TOP_VIEW				WM_USER+3130
#define WM_RIGHT_VIEW			WM_USER+3131
#define WM_PRE_VIEW				WM_USER+3132
#define WM_PROMPT_CATCH			WM_USER+3133
#define WM_BACK					WM_USER+3134
#define WM_PROMPT_OPERA			WM_USER+3135
#define WM_PAN_XY				WM_USER+3136
#define WM_ZOOM_TO				WM_USER+3137
#define WM_SHOW_IMG				WM_USER+3138
#define WM_CHANGE_CURPT			WM_USER+3139
#define WM_LEFTBTDOUBLECLICK		WM_USER+3140

struct stPoint
{
public:
	stPoint(double x, double y, double z)
		: mX(x),
		mY(y),
		mZ(z)
	{
	}

	double mX;
	double mY;
	double mZ;
};

class CMsgSender
{
public:
	CMsgSender(CView* view);
	~CMsgSender();

	/**
	* @brief sendMsgLfClk 发送鼠标左击事件到vector view中的消息
	* @param x y z 三维点坐标
	*/
	void sendMsgLfClk(double x, double y, double z);

	/**
	* @brief sendMsgMouseMove 发送鼠标移动消息
	* @param x y z 三维点坐标
	*/
	void sendMsgMouseMove(double x, double y, double z);

	/**
	* @brief sendMsgRightClk 发送鼠标右击消息
	* @param x y z 三维点坐标
	*/
	void sendMsgRightClk(double x, double y, double z);

	/**
	* @brief sendMsgPromptLock 发送通知提示锁定消息
	* @param bLock 是否锁定
	*/
	void sendMsgPromptLock(bool bLock);

	/**
	* @brief sendMsgPromptCatch 发送开启内角点捕捉
	* @param bCatch 是否捕捉
	*/
	void sendMsgPromptCatch(bool bCatch);

	/**
	* @brief sendMsgBkspace 发送回退消息
	*/
	void sendMsgBkspace();

	/**
	* @brief sendMsgPromptOpera 发送提示消息
	* @param bShiftOpera 是否shift操作
	*/
	void sendMsgPromptOpera(bool bShiftOpera);

	/**
	* @brief sendMsgPan 发送移动消息
	* @param centerX 移动到中心x
	* @param centerY 移动到中心y
	* @param centerZ 移动到中心z
	*/
	void sendMsgPan(double centerX, double centerY, double centerZ);

	/**
	* @brief sendMsgZoom 发送缩放消息
	* @param centerX 移动到中心x
	* @param centerY 移动到中心y
	* @param scale 尺度
	*/
	void sendMsgZoom(double centerX, double centerY, double scale);

	/**
	* @brief sendMsgChangeCurpt 发送消息修改矢量视图当前点
	*/
	void sendMsgChangeCurPt();

	/**
	* @brief sendMsgAdjustFakePt 发送消息调整假点点位
	*/
	void sendMsgAdjustFakePt();

	/**
	* @brief sendMsgShowImg 发送消息显示影像
	* @param x y z 三维点坐标
	*/
	void sendMsgShowImg(double x, double y, double z);

	/**
	* @brief sendMsgDoubleClick 发送消息双击
	* @param x y z 三维点坐标
	*/
	void sendMsgDoubleClick(double x, double y, double z, int bt);

	/**
	* @brief sendMsgChangeAbsOriDlg 发送消息更新绝对定向对话框
	* @param relaX relaY relaZ 三维点坐标
	*/
	void sendMsgChangeAbsOriDlg(double relaX, double relaY, double relaZ);

	/**
	* @brief sendMsgFetchOriData 发送消息获取定向数据
	*/
	void sendMsgFetchOriData();

private:
	CView* pView;
};

