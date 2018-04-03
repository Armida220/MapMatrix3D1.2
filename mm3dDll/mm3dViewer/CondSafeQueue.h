#pragma once
#include "ThreadSafeQueue.h"
#include <condition_variable>

namespace mm3dView
{
	const int MAX_ELEMENT_NUM = 100;
	/**
	* @brief CThreadSafeQueue �������������������̰߳�ȫջ
	* @brief �����˵����������ϲ���Ҫ����ļ����ȴ����ٶ�Ҫ����
	*/
	template<typename T>
	class CCondSafeQueue : public CThreadSafeQueue<T>
	{
	private:
		/**
		* @brief mut �̰߳�ȫ������
		*/
		mutable std::mutex mut;

		/**
		* @brief data_queue �������ݶ���
		*/
		std::queue<T> data_queue;

		/**
		* @brief data_queue ��������
		*/
		std::condition_variable data_cond;

	public:
		CCondSafeQueue(){}

		/**
		* @brief ���ƹ��캯��
		*/
		CCondSafeQueue(const CCondSafeQueue& other);

		/**
		* @brief push ����м�������
		*/
		virtual void push(T new_value);

		/**
		* @brief �����м�������, ���ؼ���ָ���ֹй¶
		*/
		virtual void pop(T& value); // 2

		/**
		* @brief �����м�������, �������ã����ط�ʽ������������ͬ
		*/
		virtual std::shared_ptr<T> pop(); // 3

		/**
		* @brief �����Ƿ�Ϊ��
		*/
		virtual bool empty();

		/**
		* @brief ���з�æ��������
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
