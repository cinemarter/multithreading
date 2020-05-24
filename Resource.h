//class to keep the resource which will be used by Consumer and Producer objects
#ifndef RESOURCE_H
#define RESOURCE_H

#include <mutex>
#include <vector>
#include <condition_variable>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

template <class T>
class ResourceBase
{
public:
	virtual bool readSum(T *el) = 0;
	virtual bool writeElement(T elem) = 0;
};

template <class T>
class ResourceVector : public ResourceBase <T>
{
public:

	ResourceVector(int maxSize = 1000, int chunkSize = 10)
		: _maxSize(maxSize)
		, _chunkSize(chunkSize)
		, _chunkStart(0)
		, _timeOut(500)
	{
		_vecRes.reserve(maxSize);//to avoid reallocation
	}
	virtual bool readSum(T* sum)
	{
		if(_chunkStart >= _maxSize) //nothing else to read. Vector is full
			return false;
		auto end = _chunkStart + _chunkSize;
		if (end > _maxSize) 
			end = _maxSize;
		std::unique_lock<std::mutex> lck(_mutex);
		//the predicate is to avoid waiting for missed notify calls
		if (_cond.wait_for(lck, _timeOut, [this, end]() {return _vecRes.size() >= end; }))
		{
			*sum = 0;
			for (; _chunkStart < end; ++_chunkStart)
			{
				*sum += _vecRes[_chunkStart];
			}
		}
		else
			return false;
		return true;
	}
	virtual bool writeElement(T elem)
	{
		if(_vecRes.size() == _maxSize)//vector is full
			return false;
		std::unique_lock<std::mutex> lck(_mutex);
		_vecRes.emplace_back(elem);
		if(_vecRes.size() % _chunkSize == 0)
		{
			//chunk is ready!
			_cond.notify_one();
		}
		return true;
	}
	
private:

	//data members
	std::vector<T>					_vecRes;
	const size_t					_maxSize;
	const size_t					_chunkSize;
	size_t							_chunkStart;
	//synchronization members
	std::mutex						_mutex;
	std::condition_variable			_cond;
	const std::chrono::milliseconds	_timeOut;
};

#endif // !RESOURCE_H

