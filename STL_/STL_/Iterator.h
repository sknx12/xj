#pragma once
template <typename T>
class Iterator {
private:
	T* ptr;
public:
	Iterator(T* p) noexcept :ptr(p) {}
	Iterator() noexcept :ptr(nullptr) {}
	Iterator(const Iterator<T>& other) noexcept :ptr(other.ptr) {}
	T& operator*() const {
		return *ptr;
	}
	void operator=(const Iterator<T>& other) {
		ptr = other.ptr;
	}
	bool operator==(const Iterator<T>& other) {
		return ptr == other.ptr;
	}
	bool operator!=(const Iterator<T>& other) {
		return ptr != other.ptr;
	}
	Iterator& operator++()noexcept {
		++ptr;
		return *this;
	}
	Iterator& operator++(int) {
		++ptr;
		return *this;
	}
	Iterator& operator--() noexcept {
		--ptr;
		return *this;
	}
	Iterator& operator--(int) {
		--ptr;
		return *this;
	}
};