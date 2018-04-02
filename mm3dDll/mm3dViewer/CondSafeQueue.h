#pragma once
#include "ThreadSafeQueue.h"
#include <condition_variable>

namespace mm3dView
{
	const int MAX_ELEMENT_NUM = 100;
	/**
	* @brief CThreadSafeQueue 基于锁和条件变量的线程安全栈
	* @brief 相对来说比上面设计上不需要过多的加锁等待，速度要更快
	*/
	template<typename T>
	class CCondSafeQueue : public CThreadSafeQueue<T>
	{
	private:
		/**
		* @brief mut 线程安全锁变量
		*/
		mutable std::mutex mut;

		/**
		* @brief data_queue 储存数据队列
		*/
		std::queue<T> data_queue;

		/**
		* @brief data_queue 条件变量
		*/
		std::condition_variable data_cond;

	public:
		CCondSafeQueue(){}

		/**
		* @brief 复制构造函数
		*/
		CCondSafeQueue(const CCondSafeQueue& other);

		/**
		* @brief push 入队列加锁保护
		*/
		virtual void push(T new_value);

		/**
		* @brief 出队列加锁保护, 返回计数指针防止泄露
		*/
		virtual void pop(T& value); // 2

		/**
		* @brief 出队列加锁保护, 返回引用，返回方式与上述有所不同
		*/
		virtual std::shared_ptr<T> pop(); // 3

		/**
		* @brief 队列是否为空
		*/
		virtual bool empty();

		/**
		* @brief 队列繁忙进行阻塞
		*/
		virtual bool isNeedToWait();

	};

	template<typename T>
	CCondSafeQueue<T>::CCondSafeQueue(const CCondSafeQueue& other)
	{
		std::lock_guard<mutex> lock(other.m);
		data = other.data;
	}

	template<typename T>
	void CCondSafeQueue<T>::push(T new_value)
	{
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(std::move(new_value));
		data_cond.notify_one(); // 1

		if (data_queue.size() > MAX_ELEMENT_NUM)
		{

		}
	}

	template<typename T>
	void CCondSafeQueue<T>::pop(T& value) // 2
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		value = std::move(data_queue.front());
		data_queue.pop();
	}

	template<typename T>
	std::shared_ptr<T> CCondSafeQueue<T>::pop() // 3
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); }); // 4

		std::shared_ptr<T> res(
			std::make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return res;
	}

	template<typename T>
	bool CCondSafeQueue<T>::empty()
	{
		std::lock_guard<mutex> lock(mut);
		return data_queue.empty();
	}

	template<typename T>
	bool CCondSafeQueue<T>::isNeedToWait()
	{
		std::lock_guard<mutex> lock(mut);
		return data_queue.size() > MAX_ELEMENT_NUM;
	}
}
