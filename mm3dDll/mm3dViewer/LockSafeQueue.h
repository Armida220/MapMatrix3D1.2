#pragma once
#include "ThreadSafeQueue.h"
#include <queue>

namespace mm3dView
{

	/**
	* @brief 弹出错误信息的栈
	*/
	struct CEmptyQueue : std::exception
	{
	};


	/**
	* @brief 基于线程安全锁的队列
	*/
	template<typename T>
	class CLockSafeQueue : public CThreadSafeQueue<T>
	{
	public:
		CLockSafeQueue(){}
		virtual ~CLockSafeQueue(){}
		/**
		* @brief 复制构造函数,需要加锁处理
		*/
		CLockSafeQueue(const CLockSafeQueue& other);

		/**
		* @brief 入队列加锁保护
		*/
		virtual void push(T new_value);

		/**
		* @brief 出队列加锁保护, 返回计数指针防止泄露
		*/
		virtual std::shared_ptr<T> pop();

		/**
		* @brief 出队列加锁保护, 返回引用，返回方式与上述有所不同
		*/
		virtual void pop(T& value);

		/**
		* @brief 队列是否为空
		*/
		virtual bool empty();

	private:
		/**
		* @brief data 储存数据的栈
		*/
		std::queue<T> data;

		/**
		* @brief m 线程保护加的锁
		*/
		std::mutex m;

	};

	template<typename T>
	CLockSafeQueue<T>::CLockSafeQueue(const CLockSafeQueue& other)
	{
		std::lock_guard<mutex> lock(other.m);
		data = other.data;
	}

	template<typename T>
	void CLockSafeQueue<T>::push(T new_value)
	{
		std::lock_guard<mutex> lock(m);
		data.push(std::move(new_value)); // 1
	}


	template<typename T>
	std::shared_ptr<T> CLockSafeQueue<T>::pop()
	{
		std::lock_guard<mutex> lock(m);
		if (data.empty()) throw CEmptyQueue(); // 2
		std::shared_ptr<T> const res(
			std::make_shared<T>(std::move(data.front()))); // 3
		data.pop(); // 4
		return res;
	}

	template<typename T>
	void CLockSafeQueue<T>::pop(T& value)
	{
		std::lock_guard<mutex> lock(m);
		if (data.empty()) throw CEmptyQueue();
		value = std::move(data.front()); // 5
		data.pop(); // 6
	}

	template<typename T>
	bool CLockSafeQueue<T>::empty()
	{
		std::lock_guard<mutex> lock(m);
		return data.empty();
	}
}