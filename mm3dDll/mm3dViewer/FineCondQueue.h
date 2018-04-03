#pragma once
#include "ThreadSafeQueue.h"

namespace mm3dView
{

	/**
	* @brief CFineCondQueue 利用自己实现的容器队列
	* @brief 创建细粒度更高的锁，从而实现并发
	*/
	template<typename T>
	class CFineCondQueue : public CThreadSafeQueue<T>
	{
	public:
		CFineCondQueue() : head(new node), tail(head.get())
		{}

		virtual ~CFineCondQueue() {}

		/**
		* @brief 复制构造函数,需要加锁处理
		*/
		CFineCondQueue(const CFineCondQueue& other) = delete;

		CFineCondQueue& operator=(const CFineCondQueue& other) = delete;

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
		* @brief 条件变量
		*/
		std::condition_variable data_cond;

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
			std::unique_ptr<node> old_head = std::move(head);
			head = std::move(old_head->next);
			return old_head;
		}

		/**
		* @brief 等待数据
		*/
		std::unique_lock<std::mutex> wait_for_data() // 2
		{
			std::unique_lock<std::mutex> head_lock(head_mutex);
			data_cond.wait(head_lock, [&]{return head.get() != get_tail(); });
			return std::move(head_lock); // 3
		}

		/**
		* @brief 出队列操作
		*/
		std::unique_ptr<node> wait_pop_head()
		{
			std::unique_lock<std::mutex> head_lock(wait_for_data()); // 4
			return pop_head();
		}

		/**
		* @brief 出队列操作
		* @param value 返回值
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