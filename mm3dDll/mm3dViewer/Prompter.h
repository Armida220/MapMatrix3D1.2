#pragma once
#include <memory>
#include "EventHandler.h"

namespace osgCall
{
	class ICallBack;
}

namespace osgEvent
{
	/**
	* @brief ��ʾ��
	*/
	class CPrompter
	{
	public:
		CPrompter(std::shared_ptr<osgCall::ICallBack> pCall);
		virtual ~CPrompter();

		/**
		* @brief promptLock ��ʾ��ס�߳�
		* @param bLockHeight ��ʾ�����Ƿ������߶�
		*/
		virtual void promptLock(bool bLockHeight);

		/**
		* @brief promptCatch ��ʾ��׽
		* @param bCatch ��ʾ�����Ƿ�׽
		*/
		virtual void promptCatch(bool bCatch);

		/**
		* @brief promptOpera ��ʾ�����ַ�ʽ��ת
		* @param bShiftOpera 
		*/
		virtual void promptOpera(bool bShiftOpera);

	protected:
		/**
		* @brief iCall �¼��ص���
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;
	};




}
