#include <iostream>
using namespace std;
#include "threadpool.h"

using uLong = unsigned long long;

template<typename T>
class MyTask : public Task
{
public:
	MyTask(T begin, T end) :
		begin_(begin),
		end_(end)
	{}

	MyTask() = default;

	Any run()
	{
		std::cout << "tid:" << std::this_thread::get_id() << "begin!" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
		T sum = 0;
		for (T i = begin_; i <= end_; ++i)
			sum += i;
		std::cout << "tid:" << std::this_thread::get_id() << "end!" << std::endl;

		return sum;
	}

private:
	T begin_;
	T end_;
};


int main()
{
	{
		ThreadPool pool;
		pool.setMode(PoolMode::MODE_CACHED);
		pool.start(2);

		for(int i = 0;i<30;++i)
		{
			Result res1 = pool.submitTask(std::make_shared<MyTask<uLong>>(1, 100000000));
		}
		// Result res1 = pool.submitTask(std::make_shared<MyTask<uLong>>(1, 100000000));
		// Result res2 = pool.submitTask(std::make_shared<MyTask<uLong>>(100000001, 200000000));
		// pool.submitTask(std::make_shared<MyTask<uLong>>(100000001, 200000000));
		// pool.submitTask(std::make_shared<MyTask<uLong>>(100000001, 200000000));
		// pool.submitTask(std::make_shared<MyTask<uLong>>(100000001, 200000000));
		// pool.submitTask(std::make_shared<MyTask<uLong>>(100000001, 200000000));
 
		// uLong sum1 = res1.get().cast_<uLong>();
		// std::cout << sum1 << std::endl;
		while(1){}
	}

	std::cout << "Main over..." << std::endl;
	//getchar();

#if 0
	{
		ThreadPool pool;
		pool.setMode(PoolMode::MODE_CACHED);
		pool.start(4);

		Result res1 = pool.submitTask(std::make_shared<MyTask<uLong>>(1, 100000000));
		Result res2 = pool.submitTask(std::make_shared<MyTask<uLong>>(100000001, 200000000));
		Result res3 = pool.submitTask(std::make_shared<MyTask<uLong>>(200000001, 300000000));
		pool.submitTask(std::make_shared<MyTask<uLong>>(200000001, 300000000));


		pool.submitTask(std::make_shared<MyTask<uLong>>(200000001, 300000000));
		pool.submitTask(std::make_shared<MyTask<uLong>>(200000001, 300000000));


		uLong sum1 = res1.get().cast_<uLong>();
		uLong sum2 = res2.get().cast_<uLong>();
		uLong sum3 = res3.get().cast_<uLong>();

		cout << (sum1 + sum2 + sum3) << endl;

		//int sum = res.get().cast_<long>();


		cout << "------------------------------------------" << endl;
		uLong Sum = 0;
		for (uLong i = 1; i <= 300000000; ++i)
			Sum += i;
		cout << Sum << endl;
	}

	getchar();

#endif



	return 0;
}