#include<iostream>
#include<thread>
#include "src/blockdeque.h"
mystd::BlockDeque<int> myque(1500);
int cnt = 1;

void producer() {
	while (true) {
		myque.push_back(cnt);
		std::cout << "���������ڲ�������ݣ� " << cnt++ << std::endl;
	}
}

void consumer() {
	while (true) {
		int num = -1000;
		myque.pop(num);
		std::cout << "������ȡ�������ݣ� " << num << std::endl;
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