#include<iostream>
#include"src/memory.h"

void main() {
	int* pint = new int(2);
	mystd::shared_ptr<int> ptr(pint);
	std::cout << ptr.get_count(); // 1
	mystd::shared_ptr<int> ptr2(ptr);
	std::cout << ptr.get_count();//2
	std::cout << ptr2.get_count();//2
	mystd::shared_ptr<int> ptr3(std::move(ptr));
	std::cout << ptr.get_count();//2
	std::cout << ptr3.get_count();//2
}