// otus.lessons.05.01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
/*
#include <iostream>
#include <memory>
#include <vector>

#include <SDL2\SDL.h>
#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")

template<typename T>
class smart_ptr {
public:
	smart_ptr() {
		counter = new int{ 1 };
		ptr = new T{};
	}
	T * get() {
		return ptr;
	}
	~smart_ptr() {
		if (--*counter) {
			delete ptr;
			delete counter;
		}
	}
private:
	T * ptr;
	int *counter;
};

int test_main(int argc, char** argv)
{
	std::cout << "Hello World" << std::endl;

	smart_ptr<std::string> sp;
	auto p1 = std::make_unique<std::vector<int> >(std::vector<int>{ 1,2,3,4,5 });

	std::cout << "p:\n";
	for (const auto& v : *p1)
		std::cout << '\t' << v << '\n';

	auto init = []()->std::vector<int>* { return new std::vector<int>{ 1,2,3,4,5 }; };
	auto done = [](auto p) {delete p; };
	std::unique_ptr < std::vector<int>, decltype(done)> p2(init(), done);

	std::cout << "p2:\n";
	for (const auto& v : *p2)
		std::cout << '\t' << v << '\n';

	auto sp1 = std::make_shared<std::vector<int> >(std::vector<int>{1, 2, 3, 4, 5});
	{
		auto sp2 = sp1;
		std::cout << "sp2:\n";
		for (const auto& v : *sp2)
			std::cout << '\t' << v << '\n';
	}

	std::weak_ptr<int> weak;
	{
		auto ptr = std::make_shared<int>(42);
		weak = ptr;

		auto x = weak.lock();
	}
	auto x = weak.lock();

	getchar();
	return 0;
}

*/