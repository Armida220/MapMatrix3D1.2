	#pragma once
#include "ICallBack.h"


namespace osgCall
{
	/**
	* @brief CallBack 回调类
	*/
	class CallBack : public ICallBack
	{
	public:
		CallBack(const Fun &funLfClk, const Fun &funMouseMv,
			const Fun &funRightClk, const Fun4 &funPromptLock, 
			const Fun4 &funPromptCatch, const Fun2 &funBack,
			const Fun4 &funPromptOpera, const Fun &funPan, 
			const Fun &funZoom, const Fun &funShowImg, const Fun6 & funDBL, 
			const Fun &funChangeAbsOriDlg, const Fun2 &funFetchOriData);

		virtual ~CallBack();

		/**
		* @brief callLfClk 回调鼠标左击
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callLfClk(double x, double y, double z);

		/**
		* @brief callMouseMv 回调鼠标移动
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callMouseMv(double x, double y, double z);

		/**
		* @brief callRightClk 回调鼠标右击
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callRightClk(double x, double y, double z);

		/**
		* @brief promptLock 提示锁定
		* @param bLock 是否已经锁定了
		*/
		virtual void promptLock(bool bLock);

		/**
		* @brief promptCatchCorner 提示开启内角点捕捉
		* @param bCatch 是否已经开启内角点捕捉
		*/
		virtual void promptCatch(bool bCatch);

		/**
		* @brief callBkspace 回调backspace按键
		*/
		virtual void callBkspace();

		/**
		* @brief promptOpera 提示是否以shift方式进行操作
		* @param bShiftOpera 是否以shift方式进行操作
		*/
		virtual void promptOpera(bool bShiftOpera);

		/**
		* @brief callPan 回调拖动模型
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callPan(double x, double y, double z);

		/**
		* @brief callZoom 回调缩放模型
		* @param x x方向坐标
		* @param y y方向坐标
		* @param scale 尺度
		*/
		virtual void callZoom(double x, double y, double scale);

		/**
		* @brief callShowImg 回调根据点显示影像
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callShowImg(double x, double y, double z);

		virtual void callDBC(double x, double y, double z, int button);
		
			/**
		* @brief callChangeAbsOriDlg 改变绝对定向刺点情况
		* @param ctrlPtNo 控制点标号
		* @param relaX x方向坐标
		* @param relaY y方向坐标
		* @param relaZ z方向坐标
		*/
		virtual void callChangeAbsOriDlg(double relaX, double relaY, double relaZ);

		/**
		* @brief callFetchOriData 获取定向数据
		*/
		virtual void callFetchOriData();
	private:
		Fun mFunLfClk;
		Fun mFunMouseMv;
		Fun mFunRightClk;
		Fun4 mFunPromptLock;
		Fun4 mFunPrompCatch;
		Fun2 mFunBack;
		Fun4 mFunPromptOpera;
		Fun mFunPan;
		Fun mFunZoom;
		Fun mFunShowImg;
		Fun6 mFunDBC;
		Fun mFunChangeAbsOriDlg;
		Fun2 mFunFetchOriData;
	};

}
