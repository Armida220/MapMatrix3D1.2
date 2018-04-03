#pragma once
#include <chrono>
#include <time.h>

namespace osgEvent
{
	class CTimeKeeper
	{
	public:
		CTimeKeeper();
		virtual ~CTimeKeeper();

		/**
		* @brief shift 进行状态切换
		*/
		virtual bool shift();

		/**
		* @brief getActivate 获得激活状态
		*/
		virtual bool getActivate();

		/**
		* @brief recordStart 记录开始时间
		*/
		virtual void recordStart();

		/**
		* @brief isEnoughTime 判断时间是否足够
		*/
		virtual bool isEnoughTime();

		void setActive(bool);

	protected:
		/**
		* @brief start 鼠标点击开始时间点
		*/
		std::chrono::system_clock::time_point start;

		/**
		* @brief end 释放结束时间点
		*/
		std::chrono::system_clock::time_point end;

		/**
		* @brief end 释放结束时间点
		*/
		bool bActivate;

	};



}

