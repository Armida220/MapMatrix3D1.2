#pragma once
#include <exception>
#include <queue>
#include <mutex>
#include <memory>

namespace mm3dView
{
	/**
	* @brief CThreadSafeQueue 基于锁的线程安全栈
	*/
	template<typename T>
	class CThreadSafeQueue
	{
	public:
		CThreadSafeQueue() {}
		virtual ~CThreadSafeQueue() {}

		/**
		* @brief 去掉赋值函数
		*/
		CThreadSafeQueue& operator=(const CThreadSafeQueue&) = delete;

		/**
		* @brief 入队列加锁保护
		*/
		virtual void push(T new_value) {}

		/**
		* @brief 出队列加锁保护, 返回引用，返回方式与上述有所不同
		*/
		virtual void pop(T& value) {}

		/**
		* @brief 队列是否为空
		*/
		virtual bool empty() { return false; }

		/**
		* @brief 队列繁忙时候请等待
		*/
		virtual bool isNeedToWait() { return false; }
	};


}
