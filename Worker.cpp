#include <mutex>
#include "Worker.h"
#include "Resource.h"

WorkerBase::WorkerBase(std::shared_ptr<ResourceBase<int> > res)
	: _res(res)
	, _stopRequested(false)
	, _isStarted(false)
{
}
void WorkerBase::startThread()
{
	if (!_isStarted)
	{
		_stopRequested = false;
		_workerThread = std::thread(&WorkerBase::run, this);
		_isStarted = true;
	}
}
void WorkerBase::stopThread()
{
	_stopRequested = true;
	if (_workerThread.joinable())
	{
		_workerThread.join();
		_isStarted = false;
	}

}


Consumer::Consumer(std::shared_ptr<ResourceBase<int> > res, std::ostream& log) 
	: WorkerBase(res)
	, _log(log)
{
}

Consumer::~Consumer()
{
}

void Consumer::run()
{
	while (!_stopRequested)
	{
		int sum;
		if(_res->readSum(&sum))
		{
			_log << sum << "\n";
		}
		else
		{
			_log << "Nothing else to read.\n";
			return;
		}
	}
		
}

RandProducer::RandProducer(std::shared_ptr<ResourceBase<int> > res, int maxNum)
	: WorkerBase(res)
	, _maxNum(maxNum)
{
	srand((unsigned)time(NULL));
}

void RandProducer::run()
{
	while (!_stopRequested)
	{
		if (!_res->writeElement(rand() % _maxNum + 1))
		{
			//vector is full. we can throw instead of return here. 
			return;
		}
	}
}
