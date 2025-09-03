#pragma once
#include<iostream>

template<typename T>
class uniqueptr {
private:
    T* ptr;

    // 禁用拷贝构造和拷贝赋值（独占所有权）
    uniqueptr(const uniqueptr&) = delete;
    uniqueptr& operator=(const uniqueptr&) = delete;
public:
    // 析构：释放资源
    ~uniqueptr() {
        delete ptr;
    }
    // 原始指针构造
    explicit uniqueptr(T* p = nullptr) : ptr(p) {}

    // 移动构造：转移所有权（右值引用无const）
    uniqueptr(uniqueptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr; // 原对象置空，避免双重释放
    }

    // 移动赋值：转移所有权（右值引用无const）
    uniqueptr& operator=(uniqueptr&& other) noexcept {
        if (this != &other) {
            delete ptr;       // 释放当前资源
            ptr = other.ptr;  // 转移所有权
            other.ptr = nullptr; // 原对象置空
        }
        return *this;
    }

    // 解引用运算符
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }

    // 获取原始指针
    T* get() const { return ptr; }

    // 重置：释放旧资源，绑定新资源
    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }

    // 修正拼写：release（释放所有权，不销毁资源）
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // 布尔转换：判断是否持有资源
    explicit operator bool() const {
        return ptr != nullptr;
    }
};