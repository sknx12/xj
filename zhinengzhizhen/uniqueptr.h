#pragma once
#include<iostream>

template<typename T>
class uniqueptr {
private:
    T* ptr;

    // ���ÿ�������Ϳ�����ֵ����ռ����Ȩ��
    uniqueptr(const uniqueptr&) = delete;
    uniqueptr& operator=(const uniqueptr&) = delete;
public:
    // �������ͷ���Դ
    ~uniqueptr() {
        delete ptr;
    }
    // ԭʼָ�빹��
    explicit uniqueptr(T* p = nullptr) : ptr(p) {}

    // �ƶ����죺ת������Ȩ����ֵ������const��
    uniqueptr(uniqueptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr; // ԭ�����ÿգ�����˫���ͷ�
    }

    // �ƶ���ֵ��ת������Ȩ����ֵ������const��
    uniqueptr& operator=(uniqueptr&& other) noexcept {
        if (this != &other) {
            delete ptr;       // �ͷŵ�ǰ��Դ
            ptr = other.ptr;  // ת������Ȩ
            other.ptr = nullptr; // ԭ�����ÿ�
        }
        return *this;
    }

    // �����������
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }

    // ��ȡԭʼָ��
    T* get() const { return ptr; }

    // ���ã��ͷž���Դ��������Դ
    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }

    // ����ƴд��release���ͷ�����Ȩ����������Դ��
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // ����ת�����ж��Ƿ������Դ
    explicit operator bool() const {
        return ptr != nullptr;
    }
};