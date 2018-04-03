#pragma once
#include "ThreadSafeQueue.h"
#include <atomic>

namespace mm3dView
{
	/**
	* @brief 基于无锁线程安全队列，使用了原子操作
	* 但是这个是单消费者，单生产者的队列
	*/
	template<typename T>
	class CAtomicSafeQueue : public CThreadSafeQueue<T>
	{
	public:
		/**
		* @brief 入队列加锁保护
		* @param new_value 入队列的值
		*/
		virtual void push(T new_value)
		{
			node* p = new node;
			node* const old_tail = tail.load();

			old_tail->data = new_value;
			old_tail->next = p;

			tail.store(p);
		}

		/**
		* @brief 出队列加锁保护, 返回引用，返回方式与上述有所不同
		* @param value 返回的值
		*/
		virtual void pop(T &data)
		{
			node* old_head = pop_head();
			data = old_head->data;
		}

		/**
		* @brief 出队列加锁保护, 返回计数指针防止泄露
		*/
		virtual std::shared_ptr<T> pop()
		{
			node* old_head = pop_head();

			if (!old_head)
			{
				return std::shared_ptr<T>();
			}

			std::shared_ptr<T> res = std::make_shared<T>(std::move(old_head->data)); // 2
			return res;
		}

		/**
		* @brief 队列是否为空
		*/
		virtual bool empty()
		{
			return tail.load() == head.load();
		}

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
			node* next;

			/**
			* @brief 构造函数
			*/
			node() :
				next(nullptr)
			{}
		};

		/**
		* @brief head 链表的头结点
		*/
		std::atomic<node*> head;

		/**
		* @brief tail 链表的尾节点
		*/
		std::atomic<node*> tail;

		/**
		* @brief pop_head 把头结点弹出
		*/
		node* pop_head()
		{
			node* const old_head = head.load();

			if (old_head == tail.load()) // 1
			{
				return nullptr;
			}

			head.store(old_head->next);
			return old_head;
		}

	public:
		CAtomicSafeQueue() :
			head(new node), tail(head.load())
		{}

		CAtomicSafeQueue(const CAtomicSafeQueue& other) = delete;

		CAtomicSafeQueue& operator=(const CAtomicSafeQueue& other) = delete;

		~CAtomicSafeQueue()
		{
			while (node* const old_head = head.load())
			{
				head.store(old_head->next);
				delete old_head;
			}
		}

	};

}