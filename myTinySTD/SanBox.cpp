#include<iostream>
#include<thread>
#include "src/blockdeque.h"
mystd::BlockDeque<int> myque(1500);
int cnt = 1;

void producer() {
	while (true) {
		myque.push_back(cnt);
		std::cout << "生产者向内部添加数据： " << cnt++ << std::endl;
	}
}

void consumer() {
	while (true) {
		int num = -1000;
		myque.pop(num);
		std::cout << "消费者取出了数据： " << num << std::endl;
	}
}

void main() {
	std::thread thd1[2], thd2[2];
	for (int i = 0; i < 2; i++) {
		thd1[i] = std::thread(producer);
		thd2[i] = std::thread(consumer);
		thd1[i].join();
		thd2[i].join();
	}
}