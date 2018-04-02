#pragma once
#include <exception>
#include <queue>
#include <mutex>
#include <memory>

namespace mm3dView
{
	/**
	* @brief CThreadSafeQueue ���������̰߳�ȫջ
	*/
	template<typename T>
	class CThreadSafeQueue
	{
	public:
		CThreadSafeQueue() {}
		virtual ~CThreadSafeQueue() {}

		/**
		* @brief ȥ����ֵ����
		*/
		CThreadSafeQueue& operator=(const CThreadSafeQueue&) = delete;

		/**
		* @brief ����м�������
		*/
		virtual void push(T new_value) {}

		/**
		* @brief �����м�������, �������ã����ط�ʽ������������ͬ
		*/
		virtual void pop(T& value) {}

		/**
		* @brief �����Ƿ�Ϊ��
		*/
		virtual bool empty() { return false; }

		/**
		* @brief ���з�æʱ����ȴ�
		*/
		virtual bool isNeedToWait() { return false; }
	};


}
