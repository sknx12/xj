#pragma once
#include<memory>
#include<iostream>
#include<xmemory>
#include"Iterator.h"
#include <utility> 
template<typename T,typename _Alloc =std::allocator<T>>
class vect {
private:
	_Alloc use_alloctor;
	T* data;
	int size;
	int cap;
	void data_cap_grow(int size_) {
		if (cap) {
			if (cap < size_) {
				resize(std::max(2 * cap, size_));
			}
			return;
		}
		resize(size_);
	}
	void emplace_one(T&& elem) {
		int temp = size;
		data_cap_grow(size + 1);
		size = temp + 1;
		std::construct_at(data + size - 1, std::as_const(elem));
	}
	bool emplace_one_at_index(int index,T&& elem) {
		if (index >= size) {
			return false;
		}
		int temp = size;
		data_cap_grow(size + 1);
		size = temp + 1;
		for (int i = size-1; i > index; i--) {
			data[i] = std::move(data[i-1]);
		}
		std::destroy_at(data + index);
		std::construct_at(data + index, std::as_const(elem));
		return true;
	}
public:
	vect() :size(0), cap(16) {
		data = use_alloctor.allocate(16);
	}
	explicit vect(int size_) :size(size_), cap(size) {
		if (cap == 0) {
			data = nullptr;
			return;
		}
		data = use_alloctor.allocate(cap);
		for (int i = 0; i < size_; i++) {
			std::construct_at(data+i);
		}
	}
	vect(int size_, T& elem, int cap_) :size(size_), cap(cap_) {
		if (cap < size) {
			throw std::out_of_range("out of range");
		}
		if (cap == 0) {
			data = nullptr;
			return;
		}
		data = use_alloctor.allocate(cap);
		for (int i = 0; i < cap; i++) {
			std::construct_at(data + i,std::as_const(elem));
		}
	}
	vect(int size_,T&elem):vect(size_,elem,size_){}
	vect(int size_, T&& elem):vect(size_,elem,size_) {}
	vect(const vect<T>& other) :size(other.size), cap(other.cap) {
		if (cap == 0) {
			data = nullptr;
			return;
		}
		data = use_alloctor.allocate(cap);
		for (int i = 0; i < cap; i++) {
			std::construct_at(data + i,std::as_const(other.data[i]));
		}
	}
	explicit vect(const vect<T>&& other) noexcept :data(other.data), size(other.size), cap(other.cap) {
		other.data = nullptr;
		other.cap = 0;
		other.size = 0;
	}
	vect(std::initializer_list<T>&& list) :size(list.size()), cap(list.size()) {
		if (cap == 0) {
			data = nullptr;
			return;
		}
		data = use_alloctor.allocate(cap);
		int i = 0;
		for (auto it = list.begin(); it != list.end(); it++) {
			std::construct_at(data + i, std::move(*it));
			i++;
		}
	}
	~vect() {
		if (data) {
			for (int i = 0; i < cap; i++) {
				std::destroy_at(data + i);
			}
			use_alloctor.deallocate(data, cap);
			data = nullptr;
		}
	}
	int get_size() noexcept {
		return size;
	}
	int get_cap() noexcept {
		return cap;
	}
	T& at(int index) {
		if (index >= size) {
			throw std::out_of_range("out of range");
		}
		return data[index];
	}
	const T& at(int index) const {
		if (index >= size) {
			throw std::out_of_range("out of range");
		}
		return data[index];
	}
	T& operator[](int index) noexcept {
		return data[index];
	}
	const T& operator[](int index) const noexcept {
		return data[index];
	}
	T& back() {
		if (size == 0) {
			throw std::out_of_range("out of range");
		}
		return data[size - 1];
	}
	const T& back() const {
		if (size == 0) {
			throw std::out_of_range("out of range");
		}
		return data[size - 1];
	}
	T& front() {
		if (size == 0) {
			throw std::out_of_range("out of range");
		}
		return data[0];
	}
	const T& front() const {
		if (size == 0) {
			throw std::out_of_range("out of range");
		}
		return data[0];
	}
	void resize(int size_) {
		if (data) {
			if (size_ == 0) {
				for (int i = 0; i < size; i++) {
					std::destroy_at(data + i);
				}
				use_alloctor.deallocate(data, cap);
			}
			T* temp = data;
			data = use_alloctor.allocate(size_);
			for (int i = 0; i < size && i < size_; i++) {
				std::construct_at(data + i, temp[i]);
			}
			for (int i = 0; i < size; i++) {
				std::destroy_at(temp + i);
			}
			use_alloctor.deallocate(temp, cap);
			size = size_;
			cap = size_;
			return;
		}
		if (size_ == 0) {
			return;
		}
		data = use_alloctor.allocate(size_);
		size = size_;
		cap = size_;
		return;
	}
	void push_back(const  T& elem) {
		emplace_one(elem);
	}
	void push_back(T&& elem) {
		emplace_one(std::move(elem));
	}
	void pop_back() {
		if (size == 0) {
			return;
		}
		std::destroy_at(data + size - 1);
		size--;
	}
	bool erase(int index) {
		if (index >= size) {
			return false;
		}
		for (int i = index + 1; i < size; i++) {
			data[i - 1] = std::move(data[i]);
		}
		std::destroy_at(data + size - 1);
		size--;
		return true;
	}
	bool insert(int index, const T& elem) {
		return emplace_one_at_index(index, elem);
	}
	bool insert(int index, T&& elem) {
		return emplace_one_at_index(index, std::move(elem));
	}
	bool empty() {
		return size == 0;
	}
	void clear() {
		if (data) {
			for (int i = 0; i < size; i++) {
				std::destroy_at(data + i);
			}
			size = 0;
		}
	}
	void assign(int cnt, const T& elem) {
		clear();
		use_alloctor.deallocate(data, cap);
		cap = cnt;
		size = cnt;
		if (cap == 0) {
			return;
		}
		data= use_alloctor.allocate(cap);
		for (int i = 0; i < size; i++) {
			std::construct_at(data + i, std::as_const(elem));
		}

	}
	void assign(std::initializer_list<T>list) {
		clear();
		use_alloctor.deallocate(data, cap);
		cap = list.size();
		size = list.size();
		if (cap == 0) {
			return;
		}
		data = use_alloctor.allocate(cap);
		int i = 0;
		for (auto it = list.begin(); it != list.end(); it++) {
			std::construct_at(data + i, std::move(*it));
			i++;
		}
	}
	void shrink_to_fit() {
		resize(size);
	}
	using iterator = Iterator<T>;
	iterator begin() noexcept {
		if (size == 0) {
			return nullptr;
		}
		return &data[0];
	}
	iterator end() noexcept {
		if (size == 0) {
			return nullptr;
		}
		return &data[size];
	}
};

