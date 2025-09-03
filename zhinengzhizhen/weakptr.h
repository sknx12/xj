#pragma once
#include "Counter.h"

// 前向声明 SharePtr，解决循环依赖
template<typename T>
class SharePtr;

template<typename T>
class WeakPtr {
private:
    T* _ptr;          // 资源指针（不拥有所有权）
    Counter<T>* cnt;  // 计数对象指针（与 SharePtr 共享）

    // 释放弱引用：计数为0时删除计数对象
    void release() {
        if (cnt) {
            cnt->w--;
            // 强引用和弱引用都为0时，删除计数对象（避免内存泄漏）
            if (cnt->s == 0 && cnt->w == 0) {
                delete cnt;
            }
            // 置空当前指针，避免野指针
            cnt = nullptr;
            _ptr = nullptr;
        }
    }

public:
    // 默认构造：空指针
    WeakPtr() : _ptr(nullptr), cnt(nullptr) {}

    // 从 SharePtr 构造：弱引用+1
    WeakPtr(const SharePtr<T>& sp) : _ptr(sp._ptr), cnt(sp.cnt) {
        if (cnt) cnt->w++;
    }

    // 拷贝构造：弱引用+1
    WeakPtr(const WeakPtr<T>& wp) : _ptr(wp._ptr), cnt(wp.cnt) {
        if (cnt) cnt->w++;
    }

    // 析构：释放弱引用
    ~WeakPtr() { release(); }

    // 拷贝赋值：先释放当前，再共享新计数
    WeakPtr<T>& operator=(const WeakPtr<T>& wp) {
        if (this != &wp) {
            release();
            cnt = wp.cnt;
            _ptr = wp._ptr;
            if (cnt) cnt->w++;
        }
        return *this;
    }

    // 从 SharePtr 赋值：先释放当前，再弱引用+1
    WeakPtr<T>& operator=(const SharePtr<T>& sp) {
        release();
        cnt = sp.cnt;
        _ptr = sp._ptr;
        if (cnt) cnt->w++;
        return *this;
    }

    // 核心方法：获取强引用（资源存活则返回有效 SharePtr，否则返回空）
    SharePtr<T> lock() const {
        return SharePtr<T>(*this);
    }

    // 核心方法：判断资源是否过期（强引用为0则过期）
    bool expired() const {
        return cnt == nullptr || cnt->s == 0;
    }

    // 重置：手动释放弱引用
    void reset() { release(); }

    // （可选）供测试调试：获取弱引用计数
    int getWeakCount() const {
        return cnt ? cnt->w : 0;
    }

    // 友元：允许 SharePtr 访问私有成员
    friend class SharePtr<T>;
};