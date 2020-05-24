#include "Worker.h"
#include "Resource.h"
#include <thread>
#include <sstream>

void run_test();

int main()
{
	//testing 
	run_test();

	//usual run flow
	std::shared_ptr<ResourceBase<int> > res = std::make_shared<ResourceVector<int> >(1000,100);
	std::unique_ptr<RandProducer> prod = std::make_unique<RandProducer>(res);
	std::unique_ptr<Consumer> cons = std::make_unique<Consumer>(res);
	
	prod->startThread();
	cons->startThread();
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	prod->stopThread();
	cons->stopThread();
	return 0;
}

class TestProducer : public WorkerBase
{
public:
	TestProducer(std::shared_ptr<ResourceBase<int> > res, size_t elemCount = 100000) 
		: WorkerBase(res)
		, _elemCount(elemCount)
	{}
private:
	virtual void run()
	{
		size_t i = 0;
		while (!_stopRequested && i < _elemCount)
		{
			if (!_res->writeElement((++i)%3))
			{
				return;
			}
		}
	}
public:
	bool checkResult(std::stringstream& ss, size_t chunkSize)
	{
		std::stringstream goldOut;
		size_t sum = 0;
		size_t i = 1;
		for (; i <= _elemCount; ++i)
		{
			if (i % chunkSize == 1)
			{
				if (i != 1)
					goldOut << sum << "\n";
				sum = 0;
			}
			sum += i % 3;
		}
		goldOut << sum << "\n";
		goldOut << "Nothing else to read.\n";
		
		if (goldOut.str() != ss.str()) {
			return false;
		}
		return true;
	}
	const size_t _elemCount;
};

void run_test()
{
	std::stringstream ss;
	
	std::shared_ptr<ResourceBase<int> > res = std::make_shared<ResourceVector<int> >(100001, 10);
	std::unique_ptr<TestProducer> prod = std::make_unique<TestProducer>(res, 100001);
	std::unique_ptr<Consumer> cons = std::make_unique<Consumer>(res, ss);

	prod->startThread();
	prod->startThread();//testing starting thread twice
	cons->startThread();
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	prod->stopThread();
	cons->stopThread();

	if(prod->checkResult(ss, 10))
	{
		std::cout << "Test Passed!\n";
	}
	else
	{
		std::cout << "Test Failed!\n";
	}
}