#pragma once
#include<deque>
#include<mutex>
#include<assert.h>
#include<condition_variable>
#include<sys/timeb.h>

namespace mystd {

	template<class T>
	class BlockDeque {
	public:
		explicit BlockDeque(size_t MaxCapacity);
		~BlockDeque();

		void clear();
		void Close();
		bool empty();
		bool full();
		size_t size();
		size_t capacity();

		T front();
		T back();

		void push_front(const T& item);
		void push_back(const T& item);
		bool pop(T& item);
		bool pop(T& item, int timeout);
		void flush();
	private:
		std::deque<T> deq_;
		size_t capacity_;
		bool isClose_;
		std::mutex mtx_;
		std::condition_variable condProductor_;
		std::condition_variable condConsumer_;
	};

	template<class T>
	BlockDeque<T>::BlockDeque(size_t MaxCapacity) :capacity_(MaxCapacity), isClose_(false) {
		assert(MaxCapacity > 0);
	}

	template<class T>
	BlockDeque<T>::~BlockDeque() {
		Close();
	}

	template<class T>
	void BlockDeque<T>::clear() {
		std::lock_guard<std::mutex> locker(mtx_);
		deq_.clear();
	}

	template<class T>
	void BlockDeque<T>::Close() {
		{
			std::lock_guard<std::mutex> locker(mtx_);
			deq_.clear();
			isClose_ = true;
		}
		condProductor_.notify_all();
		condConsumer_.notify_all();
	}

	template<class T>
	bool BlockDeque<T>::empty() {
		std::lock_guard<std::mutex> locker(mtx_);
		return deq_.empty();
	}

	template<class T>
	bool BlockDeque<T>::full() {
		std::lock_guard<std::mutex> locker(mtx_);
		return deq_.size() >= capacity_;
	}

	template<class T>
	size_t BlockDeque<T>::size() {
		std::lock_guard<std::mutex> locker(mtx_);
		return deq_.size();
	}

	template<class T>
	size_t BlockDeque<T>::capacity() {
		return capacity_;
	}

	template<class T>
	T BlockDeque<T>::front() {
		std::lock_guard<std::mutex> locker(mtx_);
		return deq_.front();
	}

	template<class T>
	T BlockDeque<T>::back() {
		std::lock_guard<std::mutex> locker(mtx_);
		return deq_.back();
	}

	template<class T>
	void BlockDeque<T>::push_front(const T& item) {
		std::unique_lock<std::mutex> locker(mtx_);
		while (deq_.size() >= capacity_) {
			condProductor_.wait(locker);
		}
		deq_.push_front(item);
		condConsumer_.notify_one();
	}

	template<class T>
	void BlockDeque<T>::push_back(const T& item) {
		std::unique_lock<std::mutex> locker(mtx_);
		while (deq_.size() >= capacity_) {
			condProductor_.wait(locker);
		}
		deq_.push_back(item);
		condConsumer_.notify_one();
	}

	template<class T>
	bool BlockDeque<T>::pop(T& item) {
		std::unique_lock<std::mutex> locker(mtx_);
		while (deq_.empty()) {
			condConsumer_.wait(locker);
			if (isClose_ == true)
				return false;
		}
		item = deq_.front();
		deq_.pop_front();
		return true;
	}

	template<class T>
	bool BlockDeque<T>::pop(T& item, int timeout) {
		std::unique_lock<std::mutex> locker(mtx_);
		while (deq_.empty()) {
			if (condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) == std::cv_status::timeout)
				return false;
			if (isClose_ == true)
				return false;
		}
		item = deq_.front();
		deq_.pop_front();
		return true;
	}

	template<class T>
	void BlockDeque<T>::flush() {
		condConsumer_.notify_one();
	}

}