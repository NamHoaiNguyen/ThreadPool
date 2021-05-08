#ifndef thread_pool_h
#define thread_pool_h

#include "assert.h"
#include <iostream>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#define MAX_THREADS	128


namespace tp {
	class ThreadPool {
		private:
			std::vector<std::thread> workers_;
			std::queue<std::function<void()>> tasks_;
			std::condition_variable cv_;
			std::mutex mutex_;
			bool stop;

		public:
			explicit ThreadPool(size_t threads);
			~ThreadPool();
			template<typename ...Args>
			decltype(auto) getThread(std::function<auto(Args... args)>);
			
			/*Enqueue a task. This function return an future object*/
			template<typename T, typename... Args>
			decltype(auto) enqueue(T func, Args... args);
//			auto enqueue(T func, Args... args) -> std::future<typename std::invoke_result<T(Args...)>::type>;

	};
}


namespace tp {
	ThreadPool::ThreadPool(size_t n_threads) : workers_(std::vector<std::thread>(n_threads)), stop(false)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		for (auto &worker : workers_) {
			worker = []()
			{
				std::unique_lock<std::mutex> queue_lock(mutex_);
				cv_.wait(queue_lock, [](){return !stop && !queue.empty()});
				
				std::move(tasks_.front())};
			}
	}	

	ThreadPool::~ThreadPool()
	{
		std::unique_lock<std::mutex> queue_lock(mutex_);
	
		for (auto &worker : workers_) {
			if (worker.joinable())
				worker.join();
		}
	}
	
	template<typename... Args>
	decltype(auto) ThreadPool::getThread(std::function<auto(Args... args)>)
	{
		
		std::lock_guard<std::mutex> lock(mutex_);
	}

	template<typename T, typename... Args>
	decltype(auto) ThreadPool::enqueue(T func, Args... args)
	{
	    /*--std=c++17*/
		using invoke_result_t = typename std::invoke_result<decltype(func(args...))>::type;
		/*--std=c++14*/
		//using invoke_result_t = typename std::result_of<decltype(func)(Args... args)>::type;
		std::packaged_task<T(Args...)> tsk(std::bind(std::forward<T>(func), std::forward<Args>(args)... ));
		auto task = std::make_shared<std::packaged_task<invoke_result_t>>(tsk);
		std::future<invoke_result_t> res = task->get_future();
		{
			std::lock_guard<std::mutex> queue_lock(mutex_);
		
			std::function<void()> wrapper = [&task](){(*task)();};
			tasks_.emplace(wrapper);
		}
		cv_.notify_one();

		return res;
	}
}


#endif
