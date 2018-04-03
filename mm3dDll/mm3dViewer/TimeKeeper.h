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
		* @brief shift ����״̬�л�
		*/
		virtual bool shift();

		/**
		* @brief getActivate ��ü���״̬
		*/
		virtual bool getActivate();

		/**
		* @brief recordStart ��¼��ʼʱ��
		*/
		virtual void recordStart();

		/**
		* @brief isEnoughTime �ж�ʱ���Ƿ��㹻
		*/
		virtual bool isEnoughTime();

		void setActive(bool);

	protected:
		/**
		* @brief start �������ʼʱ���
		*/
		std::chrono::system_clock::time_point start;

		/**
		* @brief end �ͷŽ���ʱ���
		*/
		std::chrono::system_clock::time_point end;

		/**
		* @brief end �ͷŽ���ʱ���
		*/
		bool bActivate;

	};



}

