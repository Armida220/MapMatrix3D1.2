#pragma once
#include "ThreadSafeQueue.h"

namespace mm3dView
{

	/**
	* @brief CFineLockQueue 利用自己实现的容器队列
	* @brief 创建细粒度更高的锁，从而实现并发
	*/
	template<typename T>
	class CFineLockQueue : public CThreadSafeQueue<T>
	{
	public:
		CFineLockQueue() : head(new node), tail(head.get())
		{}

		virtual ~CFineLockQueue() {}

		/**
		* @brief 复制构造函数,需要加锁处理
		*/
		CFineLockQueue(const CFineLockQueue& other) = delete;

		CFineLockQueue& operator=(const CFineLockQueue& other) = delete;

		/**
		* @brief 入队列加锁保护
		* @param new_value 入队列的值
		*/
		virtual void push(T new_value);

		/**
		* @brief 出队列加锁保护, 返回计数指针防止泄露
		*/
		virtual std::shared_ptr<T> pop();

		/**
		* @brief 出队列加锁保护, 返回引用，返回方式与上述有所不同
		* @param value 返回的值
		*/
		virtual void pop(T& value);

		/**
		* @brief 队列是否为空
		*/
		virtual bool empty();

	private:
		/**
		* @brief 链表的节点
		*/
		struct node
		{
			/**
			* @brief data 存储的数据
			*/
			T data;

			/**
			* @brief 链表的下一个节点指针
			*/
			std::unique_ptr<node> next;
		};

		/**
		* @brief 头结点锁
		*/
		std::mutex head_mutex;

		/**
		* @brief 头结点指针
		*/
		std::unique_ptr<node> head;

		/**
		* @brief 尾结点锁
		*/
		std::mutex tail_mutex;

		/**
		* @brief 尾结点指针
		*/
		node* tail;

		/**
		* @brief 获取尾节点指针
		*/
		node* get_tail()
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			return tail;
		}

		/**
		* @brief 弹出头结点
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