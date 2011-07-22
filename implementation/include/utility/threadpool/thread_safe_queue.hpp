#ifndef SF_THREAD_SAFE_QUEUE_HPP
#define SF_THREAD_SAFE_QUEUE_HPP

#include <list>
#include <boost/thread.hpp>

namespace sf
{
namespace utility
{

/*
 *  class ThreadSafeQueue is a wrapper around an std::list
 *  which provides thread-safe queue operations on the
 *  underlying list
 *
 *  # Implementing a Thread-Safe Queue using Condition Variables
 *     @ http://www.justsoftwaresolutions.co.uk/
 */

template <class T>
class ThreadSafeQueue
{
public:
	void push(const T &data)
	{
		boost::mutex::scoped_lock sl(mutex);
		list.push_back(data);
		sl.unlock();
		notEmpty.notify_one();
	}

	bool empty() const
	{
		boost::mutex::scoped_lock sl(mutex);
		return list.empty();
	}

	size_t size() const
	{
		boost::mutex::scoped_lock sl(mutex);
		return list.size();
	}

	void pop(T &popped)
	{
		boost::mutex::scoped_lock sl(mutex);
		
		while(list.empty())
			notEmpty.wait(sl);
		popped = list.front();
		list.pop_front();
	}
	
	void clear()
	{
		boost::mutex::scoped_lock sl(mutex);
		list.clear();
	}

private:
	std::list<T> list;

	//mutex is mutable so it can be modified in
	//const member functions and const queues
	mutable boost::mutex mutex;
	
	boost::condition_variable notEmpty;
};

}
}

#endif