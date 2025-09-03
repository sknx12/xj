#pragma once
#include "Counter.h"
#include "WeakPtr.h"

template<typename T>
class SharePtr {
private:
    T* _ptr;          // 资源指针
    Counter<T>* cnt;  // 计数对象指针

public:
    // 默认构造：空指针
    SharePtr() : _ptr(nullptr), cnt(nullptr) {}

    // 原始指针构造（explicit 禁止隐式转换）
    explicit SharePtr(T* p) {
      
       
        if (p) {
            cnt = new Counter<T>(p);
            _ptr = p;
        }
        else {
            cnt = nullptr;
            _ptr = nullptr;
        }
    }

    // 从 WeakPtr 构造（供 WeakPtr::lock() 使用）
    SharePtr(const WeakPtr<T>& wp) : _ptr(nullptr), cnt(nullptr) {
        // 仅当强引用>0时，才持有资源（避免访问已释放资源）
        if (wp.cnt && wp.cnt->s > 0) {
            cnt = wp.cnt;
            _ptr = wp._ptr;
            cnt->s++; // 强引用计数+1
        }
    }

    // 拷贝构造：共享计数，强引用+1
    SharePtr(const SharePtr<T>& sp) : _ptr(sp._ptr), cnt(sp.cnt) {
        if (cnt) cnt->s++;
    }

    // 移动构造：转移所有权（不增加计数）
    SharePtr(SharePtr<T>&& sp) noexcept : _ptr(sp._ptr), cnt(sp.cnt) {
        // 原对象置空，避免析构时重复释放
        sp._ptr = nullptr;
        sp.cnt = nullptr;
    }

    // 析构：释放强引用，计数为0时删除计数对象
    ~SharePtr() {
        if (cnt) {
            cnt->s--;
            // 强引用和弱引用都为0时，才删除计数对象（避免内存泄漏）
            if (cnt->s == 0&&cnt->w==0 ) {
                delete cnt;
            }
            // 置空当前指针，避免野指针
            cnt = nullptr;
            _ptr = nullptr;
        }
    }

    // 拷贝赋值：先释放当前，再共享新计数
    SharePtr<T>& operator=(const SharePtr<T>& sp) {
        if (this != &sp) {
            // 释放当前强引用
            if (cnt) {
                cnt->s--;
                if (cnt->s == 0 && cnt->w == 0) {
                    delete cnt;
                }
            }
            // 共享新计数
            _ptr = sp._ptr;
            cnt = sp.cnt;
            if (cnt) cnt->s++;
        }
        return *this;
    }

    // 移动赋值：转移所有权（不增加计数）
    SharePtr<T>& operator=(SharePtr<T>&& sp) noexcept {
        if (this != &sp) {
            // 释放当前强引用
            if (cnt) {
                cnt->s--;
                if (cnt->s == 0 && cnt->w == 0) {
                    delete cnt;
                }
            }
            // 转移所有权
            _ptr = sp._ptr;
            cnt = sp.cnt;
            // 原对象置空，避免析构时重复释放
            sp._ptr = nullptr;
            sp.cnt = nullptr;
        }
        return *this;
    }

    // 解引用运算符
    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }

    // 获取原始资源指针
    T* get() const { return _ptr; }

    // （可选）供测试调试：获取强引用计数
    int getShareCount() const {
        return cnt ? cnt->s : 0;
    }
    
    void reset(T* p = nullptr) {
        // 1. 释放旧资源（原有逻辑保留）
        if (cnt) {
            cnt->s--;
            if (cnt->s == 0 && cnt->w == 0) {
                delete cnt;
            }
            cnt = nullptr;
            _ptr = nullptr;
        }
        // 2. 绑定新资源（重点检查此处）
        if (p) {
            cnt = new Counter<T>(p);  // 创建新计数对象
            _ptr = p;  // 确保_ptr指向新资源
           
            std::cout << "[reset] 绑定新资源，id=" << _ptr->id << std::endl;
        }
        else {
            cnt = nullptr;
            _ptr = nullptr;
        }
    }
    bool operator==(std::nullptr_t) const {
        return _ptr == nullptr;
    }

    // 2. 与另一个 SharePtr 比较：判断是否指向同一资源
    bool operator==(const SharePtr<T>& other) const {
        // 若两者都为空，或指向同一原始资源，则相等
        return (_ptr == other._ptr) && (cnt == other.cnt);
    }

    // 3. （可选）添加 != 运算符，与 == 逻辑相反
    bool operator!=(std::nullptr_t) const {
        return !(*this == nullptr);
    }

    bool operator!=(const SharePtr<T>& other) const {
        return !(*this == other);
    }
    // 友元：允许 WeakPtr 访问私有成员
    friend class WeakPtr<T>;
};