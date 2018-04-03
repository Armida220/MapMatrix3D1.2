#pragma once
#include "ThreadSafeQueue.h"

namespace mm3dView
{

	/**
	* @brief CFineCondQueue �����Լ�ʵ�ֵ���������
	* @brief ����ϸ���ȸ��ߵ������Ӷ�ʵ�ֲ���
	*/
	template<typename T>
	class CFineCondQueue : public CThreadSafeQueue<T>
	{
	public:
		CFineCondQueue() : head(new node), tail(head.get())
		{}

		virtual ~CFineCondQueue() {}

		/**
		* @brief ���ƹ��캯��,��Ҫ��������
		*/
		CFineCondQueue(const CFineCondQueue& other) = delete;

		CFineCondQueue& operator=(const CFineCondQueue& other) = delete;

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
		* @brief ��������
		*/
		std::condition_variable data_cond;

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
			std::unique_ptr<node> old_head = std::move(head);
			head = std::move(old_head->next);
			return old_head;
		}

		/**
		* @brief �ȴ�����
		*/
		std::unique_lock<std::mutex> wait_for_data() // 2
		{
			std::unique_lock<std::mutex> head_lock(head_mutex);
			data_cond.wait(head_lock, [&]{return head.get() != get_tail(); });
			return std::move(head_lock); // 3
		}

		/**
		* @brief �����в���
		*/
		std::unique_ptr<node> wait_pop_head()
		{
			std::unique_lock<std::mutex> head_lock(wait_for_data()); // 4
			return pop_head();
		}

		/**
		* @brief �����в���
		* @param value ����ֵ
		*/
		std::unique_ptr<node> wait_pop_head(T& value)
		{
			std::unique_lock<std::mutex> head_lock(wait_for_data()); // 5
			value = std::move(head->data);
			return pop_head();
		}

	};

	template<typename T>
	void CFineCondQueue<T>::push(T new_value)
	{
		std::unique_ptr<node> p(new node);

		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			node* new_tail = p.get();
			tail->data = std::move(new_value);
			tail->next = std::move(p);
			tail = new_tail;
		}

		data_cond.notify_one();
	}


	template<typename T>
	std::shared_ptr<T> CFineCondQueue<T>::pop()
	{
		std::unique_ptr<node> old_head = wait_pop_head();

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
	void CFineCondQueue<T>::pop(T& value)
	{
		std::unique_ptr<node> old_head = wait_pop_head(value);
	}

	template<typename T>
	bool CFineCondQueue<T>::empty()
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);

		return head.get() == get_tail();
	}

}