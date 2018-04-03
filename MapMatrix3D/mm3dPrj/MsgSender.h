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
	* @brief sendMsgLfClk �����������¼���vector view�е���Ϣ
	* @param x y z ��ά������
	*/
	void sendMsgLfClk(double x, double y, double z);

	/**
	* @brief sendMsgMouseMove ��������ƶ���Ϣ
	* @param x y z ��ά������
	*/
	void sendMsgMouseMove(double x, double y, double z);

	/**
	* @brief sendMsgRightClk ��������һ���Ϣ
	* @param x y z ��ά������
	*/
	void sendMsgRightClk(double x, double y, double z);

	/**
	* @brief sendMsgPromptLock ����֪ͨ��ʾ������Ϣ
	* @param bLock �Ƿ�����
	*/
	void sendMsgPromptLock(bool bLock);

	/**
	* @brief sendMsgPromptCatch ���Ϳ����ڽǵ㲶׽
	* @param bCatch �Ƿ�׽
	*/
	void sendMsgPromptCatch(bool bCatch);

	/**
	* @brief sendMsgBkspace ���ͻ�����Ϣ
	*/
	void sendMsgBkspace();

	/**
	* @brief sendMsgPromptOpera ������ʾ��Ϣ
	* @param bShiftOpera �Ƿ�shift����
	*/
	void sendMsgPromptOpera(bool bShiftOpera);

	/**
	* @brief sendMsgPan �����ƶ���Ϣ
	* @param centerX �ƶ�������x
	* @param centerY �ƶ�������y
	* @param centerZ �ƶ�������z
	*/
	void sendMsgPan(double centerX, double centerY, double centerZ);

	/**
	* @brief sendMsgZoom ����������Ϣ
	* @param centerX �ƶ�������x
	* @param centerY �ƶ�������y
	* @param scale �߶�
	*/
	void sendMsgZoom(double centerX, double centerY, double scale);

	/**
	* @brief sendMsgChangeCurpt ������Ϣ�޸�ʸ����ͼ��ǰ��
	*/
	void sendMsgChangeCurPt();

	/**
	* @brief sendMsgAdjustFakePt ������Ϣ�����ٵ��λ
	*/
	void sendMsgAdjustFakePt();

	/**
	* @brief sendMsgShowImg ������Ϣ��ʾӰ��
	* @param x y z ��ά������
	*/
	void sendMsgShowImg(double x, double y, double z);

	/**
	* @brief sendMsgDoubleClick ������Ϣ˫��
	* @param x y z ��ά������
	*/
	void sendMsgDoubleClick(double x, double y, double z, int bt);

	/**
	* @brief sendMsgChangeAbsOriDlg ������Ϣ���¾��Զ���Ի���
	* @param relaX relaY relaZ ��ά������
	*/
	void sendMsgChangeAbsOriDlg(double relaX, double relaY, double relaZ);

	/**
	* @brief sendMsgFetchOriData ������Ϣ��ȡ��������
	*/
	void sendMsgFetchOriData();

private:
	CView* pView;
};

