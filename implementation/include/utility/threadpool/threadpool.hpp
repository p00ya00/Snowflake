#ifndef SF_THREADPOOL_HPP
#define SF_THREADPOOL_HPP

#include <vector>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <utility/threadpool/thread_safe_queue.hpp>

namespace sf
{
namespace utility
{

typedef boost::function<void()> nullaryFunctor;
typedef boost::shared_ptr<boost::thread> threadPtr;

template <class T = nullaryFunctor>
class Threadpool
{
public:
	Threadpool(int poolSize)
	{
		for(int i = 0; i < poolSize; ++i)
		{
			threadPtr t(new boost::thread(&Threadpool::run, this));
			wthreads.push_back(t);
		}
	}

	void schedule(T task)
	{
		queue.push(task);
	}

	void joinAll()
	{
		int size = wthreads.size();
		for(int i = 0; i < size; ++i)
			wthreads[i]->join();
	}

	void cancelAll()
	{
		int size = wthreads.size();
		for(int i = 0; i < size; ++i)
			wthreads[i]->interrupt();
	}

	size_t queueSize() const
	{
		return queue.size();
	}

	size_t clearJobs()
	{
		size_t jobsDeleted = queueSize();
		queue.clear();
		return jobsDeleted;
	}

	size_t poolSize() const
	{
		return wthreads.size();
	}

private:
	void run()
	{
		T task;
		
		while(1)
		{
			queue.pop(task);
			task();
		}
	}

	ThreadSafeQueue<T> queue;
	std::vector<threadPtr> wthreads;
};

}
}

#endif