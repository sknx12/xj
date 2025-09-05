#include<iostream>
#include"vec.h"

using namespace std;
int main() {
	vect<int> v = { 1, 2, 3, 4, 5 };

	vect<int> v2 = v;
	v2.push_back(88);

	int e = 0;
	vect<int> v3 = vect<int>(8, 9);
	v3[2] = 5;
	v3.at(6) = 34;

	for (vect<int>::iterator it = v.begin(); it != v.end(); it++)
		std::cout << *it << " ";
	std::cout << std::endl;

	for (auto i : v2)
		std::cout << i << " ";
	std::cout << std::endl;

	for (size_t i = 0; i < v3.get_size(); i++)
		std::cout << v3[i] << " ";
	std::cout << std::endl;
	return 0;
}