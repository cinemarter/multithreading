#ifndef CONSUMER_H
#define CONSUMER_H

#include <thread>
#include <iostream>
#include <memory>

template <class T>
class ResourceBase;

class WorkerBase
{
public:
	WorkerBase(std::shared_ptr<ResourceBase<int> > res);
	void startThread();
	void stopThread();
	virtual void run() = 0;

protected:
	std::thread									_workerThread;
	volatile  bool								_stopRequested;
	std::shared_ptr<ResourceBase<int> >			_res;
	bool										_isStarted;
};

class Consumer : public WorkerBase
{
public:
	Consumer(std::shared_ptr<ResourceBase<int> > res, std::ostream& log = std::cout);
	~Consumer();
private:
	virtual void run();
	std::ostream&	_log;
};

class RandProducer : public WorkerBase
{
public:
	RandProducer(std::shared_ptr<ResourceBase<int> > res, int maxNum = 1000);
private:
	virtual void run();
	const size_t _maxNum;
};

#endif // !CONSUMER_H

