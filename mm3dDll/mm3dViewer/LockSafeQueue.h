#pragma once
#include "ThreadSafeQueue.h"
#include <queue>

namespace mm3dView
{

	/**
	* @brief ����������Ϣ��ջ
	*/
	struct CEmptyQueue : std::exception
	{
	};


	/**
	* @brief �����̰߳�ȫ���Ķ���
	*/
	template<typename T>
	class CLockSafeQueue : public CThreadSafeQueue<T>
	{
	public:
		CLockSafeQueue(){}
		virtual ~CLockSafeQueue(){}
		/**
		* @brief ���ƹ��캯��,��Ҫ��������
		*/
		CLockSafeQueue(const CLockSafeQueue& other);

		/**
		* @brief ����м�������
		*/
		virtual void push(T new_value);

		/**
		* @brief �����м�������, ���ؼ���ָ���ֹй¶
		*/
		virtual std::shared_ptr<T> pop();

		/**
		* @brief �����м�������, �������ã����ط�ʽ������������ͬ
		*/
		virtual void pop(T& value);

		/**
		* @brief �����Ƿ�Ϊ��
		*/
		virtual bool empty();

	private:
		/**
		* @brief data �������ݵ�ջ
		*/
		std::queue<T> data;

		/**
		* @brief m �̱߳����ӵ���
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