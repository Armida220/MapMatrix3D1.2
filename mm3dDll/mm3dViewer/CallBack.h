	#pragma once
#include "ICallBack.h"


namespace osgCall
{
	/**
	* @brief CallBack �ص���
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
		* @brief callLfClk �ص�������
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callLfClk(double x, double y, double z);

		/**
		* @brief callMouseMv �ص�����ƶ�
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callMouseMv(double x, double y, double z);

		/**
		* @brief callRightClk �ص�����һ�
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callRightClk(double x, double y, double z);

		/**
		* @brief promptLock ��ʾ����
		* @param bLock �Ƿ��Ѿ�������
		*/
		virtual void promptLock(bool bLock);

		/**
		* @brief promptCatchCorner ��ʾ�����ڽǵ㲶׽
		* @param bCatch �Ƿ��Ѿ������ڽǵ㲶׽
		*/
		virtual void promptCatch(bool bCatch);

		/**
		* @brief callBkspace �ص�backspace����
		*/
		virtual void callBkspace();

		/**
		* @brief promptOpera ��ʾ�Ƿ���shift��ʽ���в���
		* @param bShiftOpera �Ƿ���shift��ʽ���в���
		*/
		virtual void promptOpera(bool bShiftOpera);

		/**
		* @brief callPan �ص��϶�ģ��
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callPan(double x, double y, double z);

		/**
		* @brief callZoom �ص�����ģ��
		* @param x x��������
		* @param y y��������
		* @param scale �߶�
		*/
		virtual void callZoom(double x, double y, double scale);

		/**
		* @brief callShowImg �ص����ݵ���ʾӰ��
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callShowImg(double x, double y, double z);

		virtual void callDBC(double x, double y, double z, int button);
		
			/**
		* @brief callChangeAbsOriDlg �ı���Զ���̵����
		* @param ctrlPtNo ���Ƶ���
		* @param relaX x��������
		* @param relaY y��������
		* @param relaZ z��������
		*/
		virtual void callChangeAbsOriDlg(double relaX, double relaY, double relaZ);

		/**
		* @brief callFetchOriData ��ȡ��������
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
