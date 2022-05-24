#pragma once
#include<utility>

namespace mystd {

	class shared_count {
	public:
		shared_count() :count(1) {}
		void inc() { ++count; }
		int dec() { return --count; }
		int get() { return count; }
	private:
		int count;
	};

	template<class T>
	class shared_ptr {
	public:
		template<class U>
		friend class shared_ptr;

		explicit shared_ptr(T* ptr = nullptr) :raw_ptr(ptr) {
			if (raw_ptr) {
				count_ref = new shared_count();
			}
		}

		~shared_ptr() {
			if (raw_ptr && count_ref->dec() == 0) {
				delete raw_ptr;
				delete count_ref;
			}
		}

		shared_ptr(const shared_ptr& rhs) {
			raw_ptr = rhs.raw_ptr;
			if (raw_ptr) {
				count_ref = rhs.count_ref;
				count_ref->inc();
			}
		}

		template<class U>
		shared_ptr(const shared_ptr<U>& rhs) noexcept {
			raw_ptr = rhs.raw_ptr;
			if (raw_ptr) {
				count_ref = rhs.count_ref;
				count_ref->inc();
			}
		}

		template<class U>
		shared_ptr(shared_ptr<U>&& rhs) noexcept {
			raw_ptr = rhs.raw_ptr;
			if (raw_ptr) {
				count_ref = rhs.count_ref;
				rhs.raw_ptr = nullptr;
			}
		}

		void swap(shared_ptr& rhs) {
			using std::swap;
			swap(raw_ptr, rhs);
			swap(count_ref, rhs.count_ref);
		}

		shared_ptr& operator=(shared_ptr rhs) noexcept {
			rhs.swap(*this);
			return *this;
		}

		T& operator*() { return *raw_ptr; }
		T* operator->() { return raw_ptr; }
		operator bool() { return raw_ptr; }
		T* get() { return raw_ptr; }
		int get_count() { return count_ref->get(); }

	private:
		T* raw_ptr;
		shared_count* count_ref;
	};

}