#pragma once
#include "ThreadSafeQueue.h"

namespace mm3dView
{

	/**
	* @brief CFineLockQueue �����Լ�ʵ�ֵ���������
	* @brief ����ϸ���ȸ��ߵ������Ӷ�ʵ�ֲ���
	*/
	template<typename T>
	class CFineLockQueue : public CThreadSafeQueue<T>
	{
	public:
		CFineLockQueue() : head(new node), tail(head.get())
		{}

		virtual ~CFineLockQueue() {}

		/**
		* @brief ���ƹ��캯��,��Ҫ��������
		*/
		CFineLockQueue(const CFineLockQueue& other) = delete;

		CFineLockQueue& operator=(const CFineLockQueue& other) = delete;

		/**
		* @brief ����м�������
		* @param new_value ����е�ֵ
		*/
		virtual void push(T new_value);

		/**
		* @brief �����м�������, ���ؼ���ָ���ֹй¶
		*/
		virtual std::shared_ptr<T> pop();

		/**
		* @brief �����м�������, �������ã����ط�ʽ������������ͬ
		* @param value ���ص�ֵ
		*/
		virtual void pop(T& value);

		/**
		* @brief �����Ƿ�Ϊ��
		*/
		virtual bool empty();

	private:
		/**
		* @brief ����Ľڵ�
		*/
		struct node
		{
			/**
			* @brief data �洢������
			*/
			T data;

			/**
			* @brief �������һ���ڵ�ָ��
			*/
			std::unique_ptr<node> next;
		};

		/**
		* @brief ͷ�����
		*/
		std::mutex head_mutex;

		/**
		* @brief ͷ���ָ��
		*/
		std::unique_ptr<node> head;

		/**
		* @brief β�����
		*/
		std::mutex tail_mutex;

		/**
		* @brief β���ָ��
		*/
		node* tail;

		/**
		* @brief ��ȡβ�ڵ�ָ��
		*/
		node* get_tail()
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			return tail;
		}

		/**
		* @brief ����ͷ���
		*/
		std::unique_ptr<node> pop_head()
		{
			std::lock_guard<std::mutex> head_lock(head_mutex);

			if (head.get() == get_tail())
			{
				return nullptr;
			}

			std::unique_ptr<node> old_head = std::move(head);
			head = std::move(old_head->next);
			return old_head;
		}

	};

	template<typename T>
	void CFineLockQueue<T>::push(T new_value)
	{
		std::unique_ptr<node> p(new node);
		node* new_tail = p.get();
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		tail->data = std::move(new_value);
		tail->next = std::move(p);
		tail = new_tail;
	}


	template<typename T>
	std::shared_ptr<T> CFineLockQueue<T>::pop()
	{
		std::unique_ptr<node> old_head = pop_head();

		if (old_head)
		{
			return std::make_shared<T>(std::move(old_head->data));
		}
		else
		{
			return std::shared_ptr<T>();
		}
	}

	template<typename T>
	void CFineLockQueue<T>::pop(T& value)
	{
		std::unique_ptr<node> old_head = pop_head();
		value = old_head->data;
	}

	template<typename T>
	bool CFineLockQueue<T>::empty()
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);

		return head.get() == get_tail();
	}

}