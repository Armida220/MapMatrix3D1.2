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
	* @brief 提示类
	*/
	class CPrompter
	{
	public:
		CPrompter(std::shared_ptr<osgCall::ICallBack> pCall);
		virtual ~CPrompter();

		/**
		* @brief promptLock 提示锁住高程
		* @param bLockHeight 提示界面是否锁定高度
		*/
		virtual void promptLock(bool bLockHeight);

		/**
		* @brief promptCatch 提示捕捉
		* @param bCatch 提示界面是否捕捉
		*/
		virtual void promptCatch(bool bCatch);

		/**
		* @brief promptOpera 提示是哪种方式旋转
		* @param bShiftOpera 
		*/
		virtual void promptOpera(bool bShiftOpera);

	protected:
		/**
		* @brief iCall 事件回调类
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;
	};




}
