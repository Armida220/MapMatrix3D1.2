#pragma once
#include "ThreadSafeQueue.h"
#include <atomic>

namespace mm3dView
{
	/**
	* @brief ���������̰߳�ȫ���У�ʹ����ԭ�Ӳ���
	* ��������ǵ������ߣ��������ߵĶ���
	*/
	template<typename T>
	class CAtomicSafeQueue : public CThreadSafeQueue<T>
	{
	public:
		/**
		* @brief ����м�������
		* @param new_value ����е�ֵ
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
		* @brief �����м�������, �������ã����ط�ʽ������������ͬ
		* @param value ���ص�ֵ
		*/
		virtual void pop(T &data)
		{
			node* old_head = pop_head();
			data = old_head->data;
		}

		/**
		* @brief �����м�������, ���ؼ���ָ���ֹй¶
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
		* @brief �����Ƿ�Ϊ��
		*/
		virtual bool empty()
		{
			return tail.load() == head.load();
		}

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
			node* next;

			/**
			* @brief ���캯��
			*/
			node() :
				next(nullptr)
			{}
		};

		/**
		* @brief head �����ͷ���
		*/
		std::atomic<node*> head;

		/**
		* @brief tail �����β�ڵ�
		*/
		std::atomic<node*> tail;

		/**
		* @brief pop_head ��ͷ��㵯��
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