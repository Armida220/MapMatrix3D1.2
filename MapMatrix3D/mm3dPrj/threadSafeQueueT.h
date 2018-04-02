#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
template<class T>
class threadSafeQueueT
{
public:
	typedef T *              Ptr;
	typedef std::queue<Ptr>           Queue;
	typedef std::mutex Mutex;
	typedef std::condition_variable   ConditionVar;
	typedef std::lock_guard<Mutex>    MutexLockGuard;
	typedef std::unique_lock<Mutex>   MutexUniqueLock;
	size_t total_length;
	explicit threadSafeQueueT() :total_length(0) {}
	~threadSafeQueueT() {}

	threadSafeQueueT(const threadSafeQueueT&) = delete;
	threadSafeQueueT& operator=(const threadSafeQueueT&) = delete;

	bool empty() const
	{
		MutexLockGuard lk(mMutex);
		return mQueue.empty();
	}

	void waitPop(Ptr& value)
	{
		MutexUniqueLock lk(mMutex);
		mConVar.wait(lk, [this] {
			return !mQueue.empty();
		});
		value = mQueue.front();
		mQueue.pop();
	}

	Ptr waitPop()
	{
		MutexUniqueLock lk(mMutex);
		mConVar.wait(lk, [this] {
			return !mQueue.empty();
		});
		Ptr sp = mQueue.front();
		mQueue.pop();
		return sp;
	}

	bool tryPop(Ptr& value)
	{
		MutexLockGuard lk(mMutex);
		if (mQueue.empty())
			return false;
		value = mQueue.front();
		mQueue.pop();
		return true;
	}

	Ptr tryPop()
	{
		MutexLockGuard lk(mMutex);
		if (mQueue.empty())
			return nullptr;
		Ptr sp = mQueue.front();
		mQueue.pop();
		return sp;
	}

	void push(Ptr value)
	{
		MutexLockGuard lk(mMutex);
		mQueue.push(value);
		mConVar.notify_all();
	}

private:
	mutable Mutex             mMutex;
	ConditionVar              mConVar;
	Queue                     mQueue;
};


